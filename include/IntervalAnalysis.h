//
// Created by edboy on 2020/7/1.
//

#ifndef CODEPUNK_INTERVALANALYSIS_H
#define CODEPUNK_INTERVALANALYSIS_H

#include <IntervalSolver.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/CFG.h>

#include <map>
#include <queue>
#include <numeric>

struct IntervalAnalysis {
    using Symbols = IntervalSymbols<const llvm::Value*>;

    std::map<const llvm::BasicBlock*, Symbols> dataMap;
    std::queue<const llvm::BasicBlock*> workList;

    explicit IntervalAnalysis(const llvm::Function* f) {
        for(const auto &bb : f->getBasicBlockList()) {
            dataMap.emplace(&bb, Symbols{});
            workList.push(&bb);
        }

        auto &entrySymbols = dataMap.at(&f->getEntryBlock());
        for(const auto& i : f->args()) {
            auto ty = i.getType();
            if(ty->isIntegerTy()) {
                entrySymbols.emplace((llvm::Value*)&i, Interval(
                        APSInt::getMinValue(ty->getIntegerBitWidth(), false),
                        APSInt::getMaxValue(ty->getIntegerBitWidth(), false)));
            }
        }
    }

    void analyze(int maxIteration = -1) {
        while(!workList.empty() && maxIteration != 0) {
            iterate();
            maxIteration--;
        }
    }

    void iterate() {
        auto bb = workList.front();
        workList.pop();

        auto oldSymbols = dataMap.at(bb);

        auto mergedSymbols = oldSymbols;
        if(bb->hasNPredecessorsOrMore(1)) {
            mergedSymbols = merge(std::vector<const llvm::BasicBlock *>{
                llvm::pred_begin(bb), llvm::pred_end(bb)}, bb);
        }

        auto newSymbols = transfer(bb, mergedSymbols);
        dataMap.at(bb) = newSymbols;

        if(newSymbols != oldSymbols) {
            for(auto succBb : llvm::successors(bb)) {
                workList.push(succBb);
            }
        }
    }

    Symbols merge(const std::vector<const llvm::BasicBlock*>& vec, const llvm::BasicBlock *to) {
        return std::accumulate(vec.begin(), vec.end(), Symbols{}, [this, to](
                const Symbols& symbols, const llvm::BasicBlock* bb) {
            auto bbSymbols = dataMap.at(bb), newSymbols = symbols | bbSymbols;
            auto term = bb->getTerminator();

            if(term->getOpcode() == llvm::Instruction::Br && term->getNumOperands() >= 3) {
                auto cond = term->getOperand(0), t = term->getOperand(1), f = term->getOperand(2);

                auto condVal = bbSymbols.at(cond);
                if(condVal.equals(APSInt(1, false))) {
                    newSymbols = symbols;
                }
                else if(condVal.length() != 0) {
                    if(auto cmpInst = llvm::dyn_cast<llvm::CmpInst>(cond)) {
                        auto l = cmpInst->getOperand(0), r = cmpInst->getOperand(1);
                        auto pred = cmpInstToBoolExpr<const llvm::Value*>(cmpInst->getPredicate());

                        if(pred != BoolExpr<const llvm::Value*>::Atomic) {
                            auto condExpr = std::make_shared<BinOp<const llvm::Value *>>(
                                    pred,
                                    std::make_shared<Atom<const llvm::Value *>>(l),
                                    std::make_shared<Atom<const llvm::Value *>>(r));

                            IntervalSolver<const llvm::Value *> solver{
                                    std::make_shared<IntervalSymbols<const llvm::Value *>>(bbSymbols), condExpr};

                            auto solvedSymbols = solver.solve(t != to);

                            if(auto lLoad = llvm::dyn_cast<llvm::Instruction>(l);
                                lLoad && lLoad->getOpcode() == llvm::Instruction::Load) {
                                auto lLFrom = lLoad->getOperand(0), lLTo = (llvm::Value*)lLoad;
                                solvedSymbols.at(lLFrom) = solvedSymbols.at(lLTo);
                            }
                            if(auto rLoad = llvm::dyn_cast<llvm::Instruction>(r);
                                    rLoad && rLoad->getOpcode() == llvm::Instruction::Load) {
                                auto rLFrom = rLoad->getOperand(0), rLTo = (llvm::Value*)rLoad;
                                solvedSymbols.at(rLFrom) = solvedSymbols.at(rLTo);
                            }

                            newSymbols = symbols | solvedSymbols;
                        }
                    }
                }
            }

            return newSymbols;
        });
    }

    template <typename T>
    static typename BoolExpr<T>::Opcode cmpInstToBoolExpr(llvm::CmpInst::Predicate p) {
        switch (p) {
            case llvm::CmpInst::ICMP_EQ:
                return BoolExpr<T>::EQ;
            case llvm::CmpInst::ICMP_NE:
                return BoolExpr<T>::NE;
            case llvm::CmpInst::ICMP_SLT:
                return BoolExpr<T>::LT;
            case llvm::CmpInst::ICMP_SLE:
                return BoolExpr<T>::LE;
            case llvm::CmpInst::ICMP_SGT:
                return BoolExpr<T>::GT;
            case llvm::CmpInst::ICMP_SGE:
                return BoolExpr<T>::GE;
            default:
                return BoolExpr<T>::Atomic;
        }
    }

    static Symbols transfer(const llvm::BasicBlock *bb, Symbols& symbols) {
        for(const auto &inst : bb->getInstList()) {
            switch (inst.getOpcode()) {
                case llvm::Instruction::Alloca: {
                    auto ty = inst.getType()->getPointerElementType();
                    if(ty->isIntegerTy()) {
                        symbols[&inst] = Interval(
                                APSInt::getMinValue(ty->getIntegerBitWidth(), false),
                                APSInt::getMaxValue(ty->getIntegerBitWidth(), false));
                    }
                    break;
                }
                case llvm::Instruction::Store: {
                    auto from = inst.getOperand(0), to = inst.getOperand(1);
                    auto ty = to->getType()->getPointerElementType();

                    if(!ty->isIntegerTy()) {
                        break;
                    }

                    symbols.at(to) = fromSymbolOrConstant(from, symbols);
                    break;
                }
                case llvm::Instruction::Load: {
                    auto from = inst.getOperand(0), to = (llvm::Value*)&inst;
                    auto ty = from->getType()->getPointerElementType();

                    if(!ty->isIntegerTy()) {
                        break;
                    }

                    symbols[to] = symbols.at(from);
                    break;
                }
                case llvm::Instruction::Add: {
                    doBinOp(inst, symbols, [](const Interval& a, const Interval& b) {
                        return a + b;
                    });
                    break;
                }
                case llvm::Instruction::Sub: {
                    doBinOp(inst, symbols, [](const Interval& a, const Interval& b) {
                        return a - b;
                    });
                    break;
                }
                case llvm::Instruction::Mul: {
                    doBinOp(inst, symbols, [](const Interval& a, const Interval& b) {
                        return a * b;
                    });
                    break;
                }
                case llvm::Instruction::SDiv: {
                    doBinOp(inst, symbols, [](const Interval& a, const Interval& b) {
                        return a / b;
                    });
                    break;
                }
                case llvm::Instruction::ICmp: {
                    const auto &cmpInst = (const llvm::CmpInst &)inst;

                    auto l = inst.getOperand(0), r = inst.getOperand(1);
                    auto lVal = fromSymbolOrConstant(l, symbols), rVal = fromSymbolOrConstant(r, symbols);

                    switch (cmpInst.getPredicate()) {
                        case llvm::CmpInst::ICMP_EQ:
                            symbols[&inst] = fromTernary(lVal == rVal);
                            break;
                        case llvm::CmpInst::ICMP_NE:
                            symbols[&inst] = fromTernary(lVal != rVal);
                            break;
                        case llvm::CmpInst::ICMP_SLT:
                            symbols[&inst] = fromTernary(lVal < rVal);
                            break;
                        case llvm::CmpInst::ICMP_SLE:
                            symbols[&inst] = fromTernary(lVal <= rVal);
                            break;
                        case llvm::CmpInst::ICMP_SGT:
                            symbols[&inst] = fromTernary(lVal > rVal);
                            break;
                        case llvm::CmpInst::ICMP_SGE:
                            symbols[&inst] = fromTernary(lVal >= rVal);
                            break;
                        default:
                            break;
                    }

                    break;
                }
                default:
                    break;
            }
        }

        return symbols;
    }

    static void doBinOp(const llvm::Instruction &inst, Symbols& symbols,
            const std::function<Interval(const Interval& a, const Interval& b)>& op) {
        auto l = inst.getOperand(0), r = inst.getOperand(1);
        auto ty = l->getType();

        if(!ty->isIntegerTy()) {
            return;
        }

        auto lVal = fromSymbolOrConstant(l, symbols), rVal = fromSymbolOrConstant(r, symbols);

        symbols[&inst] = op(lVal, rVal);
    }

    static Interval fromTernary(Ternary t) {
        APSInt zero(1, false), one(llvm::APInt(1, "1", 10), false);

        switch (t.v) {
            case Ternary::True:
                return {one, one};
            case Ternary::False:
                return {zero, zero};
            case Ternary::Unknown:
                return {one, zero};
        }

        return {};
    }

    static Interval fromSymbolOrConstant(const llvm::Value *v, Symbols &symbols) {
        if(auto c = llvm::dyn_cast<llvm::Constant>(v)) {
            symbols[v] = Interval{
                APSInt(c->getUniqueInteger(), false).extend(v->getType()->getIntegerBitWidth())};
        }

        return symbols.at(v);
    }
};

#endif //CODEPUNK_INTERVALANALYSIS_H
