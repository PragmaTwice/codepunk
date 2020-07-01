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
                llvm::pred_begin(bb), llvm::pred_end(bb)});
        }

        auto newSymbols = transfer(bb, mergedSymbols);
        dataMap.at(bb) = newSymbols;

        if(newSymbols != oldSymbols) {
            for(auto succBb : llvm::successors(bb)) {
                workList.push(succBb);
            }
        }
    }

    Symbols merge(const std::vector<const llvm::BasicBlock*>& vec) {
        //TODO  using IntervalSolver
        return std::accumulate(vec.begin(), vec.end(), Symbols{}, [this](
                const Symbols& symbols, const llvm::BasicBlock* bb) {
            return symbols | dataMap.at(bb);
        });
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

                    APSInt zero("0"), one("1");
                    Interval True{one, one}, False{zero, zero}, Undefined{zero, one};

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
        APSInt zero("0"), one("1");

        switch (t.v) {
            case Ternary::True:
                return {one, one};
            case Ternary::False:
                return {zero, zero};
            case Ternary::Unknown:
                return {zero, one};
        }

        return {};
    }

    static Interval fromSymbolOrConstant(const llvm::Value *v, const Symbols &symbols) {
        if(auto c = llvm::dyn_cast<llvm::Constant>(v)) {
            return Interval{
                APSInt(c->getUniqueInteger(), false).extend(v->getType()->getIntegerBitWidth())};
        } else {
            return symbols.at(v);
        }
    }
};

#endif //CODEPUNK_INTERVALANALYSIS_H
