//
// Created by edboy on 2020/6/29.
//

#ifndef CODEPUNK_INTERVAL_H
#define CODEPUNK_INTERVAL_H

#include "Ternary.h"

#include <llvm/ADT/APSInt.h>
#include <llvm/Support/raw_ostream.h>

#include <algorithm>


using llvm::APInt;
using llvm::APSInt;

template <typename T>
struct IntervalSolver;

struct Interval {
private:
    APSInt l, r;

    template <typename T>
    friend struct IntervalSolver;

public:
    Interval() = default;

    Interval(APSInt l, APSInt r) : l(std::move(l)), r(std::move(r)) {}
    Interval(const APInt &l, const APInt &r, bool isUnsigned = false)
        : l(l, isUnsigned), r(r, isUnsigned) {}

    explicit Interval(const APSInt& c) : l(c), r(c) {}

    [[nodiscard]] bool isValid() const {
        return l <= r;
    }

    [[nodiscard]] const APSInt &getLeft() const {
        return l;
    }

    [[nodiscard]] const APSInt &getRight() const {
        return r;
    }

    [[nodiscard]] bool contains(const APSInt& v) const {
        return l <= v && v <= r;
    }

    [[nodiscard]] bool contains(const Interval& v) const {
        return l <= v.l && v.r <= r;
    }

    [[nodiscard]] bool equals(const Interval& v) const {
        return l == v.l && r == v.r;
    }

    [[nodiscard]] bool equals(const APSInt& v) const {
        return l == v && r == v;
    }

    [[nodiscard]] bool overlaps(const Interval& v) const {
        return r >= v.l && l <= v.r;
    }

    [[nodiscard]] bool isConstant() const {
        return l == r;
    }

    APSInt length() const {
        return r - l;
    }

    friend Interval operator&(const Interval& a, const Interval& b) {
        return {std::max(a.l, b.l), std::min(a.r, b.r)};
    }

    friend Interval operator|(const Interval& a, const Interval& b) {
        return {std::min(a.l, b.l), std::max(a.r, b.r)};
    }

    friend Interval operator+(const Interval& a, const Interval& b) {
        return {a.l + b.l, a.r + b.r};
    }

    friend Interval operator-(const Interval& a, const Interval& b) {
        return {a.l - b.r, a.r - b.l};
    }

    friend Interval operator*(const Interval& a, const Interval& b) {
        auto list = {a.l*b.l, a.l*b.r, a.r*b.l, a.r*b.r};
        return {std::min(list), std::max(list)};
    }

    friend Interval operator/(const Interval& a, const Interval& b) {
        auto list = {a.l/b.l, a.l/b.r, a.r/b.l, a.r/b.r};
        return {std::min(list), std::max(list)};
    }

    friend Ternary operator==(const Interval& a, const Interval& b) {
        if(a.isConstant() && b.isConstant()) {
            return Ternary{ a.l == b.l };
        }

        if(!a.overlaps(b)) {
            return Ternary::False;
        }

        return {};
    }

    friend Ternary operator!=(const Interval& a, const Interval& b) {
        return !(a == b);
    }

    friend Ternary operator<(const Interval& a, const Interval& b) {
        if(a.r < b.l) return Ternary::True;
        if(a.l >= b.r) return Ternary::False;

        return {};
    }

    friend Ternary operator<=(const Interval& a, const Interval& b) {
        if(a.r <= b.l) return Ternary::True;
        if(a.l > b.r) return Ternary::False;

        return {};
    }

    friend Ternary operator>(const Interval& a, const Interval& b) {
        return !(a <= b);
    }

    friend Ternary operator>=(const Interval& a, const Interval& b) {
        return !(a < b);
    }

    friend llvm::raw_ostream &operator<<(llvm::raw_ostream& o, const Interval& v) {
        return o << "[" << v.l << "," << v.r << "]";
    }

};

#endif //CODEPUNK_INTERVAL_H
