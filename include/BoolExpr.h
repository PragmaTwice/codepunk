//
// Created by edboy on 2020/6/30.
//

#ifndef CODEPUNK_BOOLEXPR_H
#define CODEPUNK_BOOLEXPR_H

#include <memory>
#include <utility>
#include <string>

#include <llvm/Support/raw_ostream.h>

template <typename T>
struct BoolExpr {
    enum Opcode { Atomic, Not, Or, And, LT, GT, LE, GE, EQ, NE };

    [[nodiscard]] virtual Opcode getOpcode() const = 0;

    [[nodiscard]] bool isAtom() const {
        return getOpcode() == Atomic;
    }

    [[nodiscard]] bool isUnaryOp() const {
        return getOpcode() == Not;
    }

    [[nodiscard]] bool isBinaryOp() const {
        return getOpcode() > Not;
    }

    [[nodiscard]] bool isBinaryLogicOp() const {
        return getOpcode() > Not && getOpcode() <= And;
    }

    [[nodiscard]] bool isBinaryRelationOp() const {
        return getOpcode() > And && getOpcode() <= NE;
    }
};

template <typename V>
struct Atom : BoolExpr<V> {
    V v;

    explicit Atom(const V& v) : v(v) {}

    [[nodiscard]] typename BoolExpr<V>::Opcode getOpcode() const override {
        return Atom::Atomic;
    }
};

template <typename T>
struct NotOp : BoolExpr<T> {
    std::shared_ptr<BoolExpr<T>> v;

    explicit NotOp(const std::shared_ptr<BoolExpr<T>>& v) : v(v) {
        assert(!v->isAtom());
    }

    [[nodiscard]] typename BoolExpr<T>::Opcode getOpcode() const override {
        return NotOp::Not;
    }
};

template <typename T>
struct BinOp : BoolExpr<T> {
    using Opcode = typename BoolExpr<T>::Opcode;

    Opcode code;
    std::shared_ptr<BoolExpr<T>> l, r;

    BinOp(Opcode code,
        const std::shared_ptr<BoolExpr<T>>& l,
        const std::shared_ptr<BoolExpr<T>>& r
    ) : code(code), l(l), r(r) {
        assert(BoolExpr<T>::isBinaryOp());

        if(BoolExpr<T>::isBinaryLogicOp()) {
            assert(!l->isAtom() && !r->isAtom());
        } else if(BoolExpr<T>::isBinaryRelationOp()) {
            assert(l->isAtom() && r->isAtom());
        }
    }

    [[nodiscard]] Opcode getOpcode() const override {
        return code;
    }

    BinOp reverse() const {
        static Opcode revCode[] = {
                BinOp::Atomic, BinOp::Not, BinOp::And, BinOp::Or,
                BinOp::GE, BinOp::LE, BinOp::GT, BinOp::LT, BinOp::NE, BinOp::EQ
        };

        return {revCode[code], l, r};
    }
};

template <typename T>
struct OrOp : BinOp<T> {
    OrOp(const std::shared_ptr<BoolExpr<T>>& l, const std::shared_ptr<BoolExpr<T>>& r)
        : BinOp<T>(OrOp::Or, l, r) {}
};

template <typename T>
struct AndOp : BinOp<T> {
    AndOp(const std::shared_ptr<BoolExpr<T>>& l, const std::shared_ptr<BoolExpr<T>>& r)
        : BinOp<T>(AndOp::And, l, r) {}
};


#endif //CODEPUNK_BOOLEXPR_H
