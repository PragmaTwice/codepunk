//
// Created by edboy on 2020/6/30.
//

#ifndef CODEPUNK_INTERVALSOLVER_H
#define CODEPUNK_INTERVALSOLVER_H

#include <Interval.h>
#include <BoolExpr.h>

#include <map>

template <typename Key>
struct IntervalSolver {
    using Symbols = std::map<Key, Interval>;
    using Expr = BoolExpr<Key>;

    std::shared_ptr<Symbols> symbols;
    std::shared_ptr<Expr> expr;

    std::map<Key, Interval> solve(bool assume) {
        switch (expr->getOpcode()) {
            case Expr::Not:
                return IntervalSolver{symbols, std::static_pointer_cast<NotOp<Key>>(expr)->v}.solve(!assume);
            case Expr::Or: {
                auto a = IntervalSolver{symbols, std::static_pointer_cast<BinOp<Key>>(expr)->l}.solve(assume);
                auto b = IntervalSolver{symbols, std::static_pointer_cast<BinOp<Key>>(expr)->r}.solve(assume);

                return symbolOr(a, b);
            }
            case Expr::And: {
                auto a = IntervalSolver{symbols, std::static_pointer_cast<BinOp<Key>>(expr)->l}.solve(assume);
                auto b = IntervalSolver{symbols, std::static_pointer_cast<BinOp<Key>>(expr)->r}.solve(assume);

                return symbolAnd(a, b);
            }
            case Expr::LT: {
                const auto &lKey = std::static_pointer_cast<Atom<Key>>(std::static_pointer_cast<BinOp<Key>>(expr)->l)->v;
                const auto &rKey = std::static_pointer_cast<Atom<Key>>(std::static_pointer_cast<BinOp<Key>>(expr)->r)->v;

                const auto &lVal = (*symbols)[lKey], &rVal = (*symbols)[rKey];
                auto newSymbols = *symbols;

                if (assume) {
                    const auto& One = APSInt("1");
                    newSymbols[lKey] = Interval{lVal.l, std::min(lVal.r, rVal.r - One)};
                    newSymbols[rKey] = Interval{std::max(lVal.l + One, rVal.l), rVal.r};
                } else {
                    newSymbols[lKey] = Interval{std::max(lVal.l, rVal.l), lVal.r};
                    newSymbols[rKey] = Interval{rVal.l, std::min(lVal.r, rVal.r)};
                }

                return newSymbols;
            }
            case Expr::LE: {
                const auto &lKey = std::static_pointer_cast<Atom<Key>>(std::static_pointer_cast<BinOp<Key>>(expr)->l)->v;
                const auto &rKey = std::static_pointer_cast<Atom<Key>>(std::static_pointer_cast<BinOp<Key>>(expr)->r)->v;

                const auto &lVal = (*symbols)[lKey], &rVal = (*symbols)[rKey];
                auto newSymbols = *symbols;

                if (assume) {
                    newSymbols[lKey] = Interval{lVal.l, std::min(lVal.r, rVal.r)};
                    newSymbols[rKey] = Interval{std::max(lVal.l, rVal.l), rVal.r};
                } else {
                    const auto& One = APSInt("1");
                    newSymbols[lKey] = Interval{std::max(lVal.l, rVal.l + One), lVal.r};
                    newSymbols[rKey] = Interval{rVal.l, std::min(lVal.r - One, rVal.r)};
                }

                return newSymbols;
            }
            case Expr::EQ: {
                const auto &lKey = std::static_pointer_cast<Atom<Key>>(std::static_pointer_cast<BinOp<Key>>(expr)->l)->v;
                const auto &rKey = std::static_pointer_cast<Atom<Key>>(std::static_pointer_cast<BinOp<Key>>(expr)->r)->v;

                const auto &lVal = (*symbols)[lKey], &rVal = (*symbols)[rKey];
                auto newSymbols = *symbols;

                if (assume) {
                    auto res = lVal & rVal;
                    newSymbols[lKey] = res;
                    newSymbols[rKey] = res;
                } else {
                    const auto& One = APSInt("1");
                    if(lVal.isConstant()) {
                        if(lVal.l == rVal.l) newSymbols[rKey] = Interval{rVal.l + One, rVal.r};
                        if(lVal.l == rVal.r) newSymbols[rKey] = Interval{rVal.l, rVal.r - One};
                    }
                    if(rVal.isConstant()) {
                        if(lVal.l == rVal.l) newSymbols[lKey] = Interval{lVal.l + One, lVal.r};
                        if(lVal.r == rVal.l) newSymbols[lKey] = Interval{lVal.l, lVal.r - One};
                    }
                }

                return newSymbols;
            }
            case Expr::GT:
            case Expr::GE:
            case Expr::NE:
                return IntervalSolver{symbols,
                    std::make_shared<BinOp<Key>>(std::static_pointer_cast<BinOp<Key>>(expr)->reverse())
                }.solve(!assume);
            default:
                assert("unreachable");
                return {};
        }
    }

    template <typename F>
    static Symbols symbolOp(F op, const Symbols& a, const Symbols& b) {
        Symbols symbols;

        for(const auto &v : a) {
            symbols.emplace(v.first, op(v.second, b.at(v.first)));
        }

        return symbols;
    };

    static Symbols symbolOr(const Symbols& a, const Symbols& b) {
        return symbolOp([](const Interval& a, const Interval& b) { return a | b; }, a, b);
    };

    static Symbols symbolAnd(const Symbols& a, const Symbols& b) {
        return symbolOp([](const Interval& a, const Interval& b) { return a & b; }, a, b);
    };
};

#endif //CODEPUNK_INTERVALSOLVER_H
