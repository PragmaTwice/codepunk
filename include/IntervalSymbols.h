//
// Created by edboy on 2020/7/1.
//

#ifndef CODEPUNK_INTERVALSYMBOLS_H
#define CODEPUNK_INTERVALSYMBOLS_H

#include <Interval.h>
#include <map>

template <typename Key>
struct IntervalSymbols : std::map<Key, Interval> {
    using std::map<Key, Interval>::map;

    template <typename F>
    static IntervalSymbols symbolOp(F op,
            const IntervalSymbols& a, const IntervalSymbols& b,
            bool addUniqueItem = true) {
        IntervalSymbols symbols;

        for(const auto &v : a) {
            if(auto iter = b.find(v.first); iter != b.end()) {
                symbols.emplace(v.first, op(v.second, iter->second));
            } else if(addUniqueItem) {
                symbols.emplace(v);
            }
        }

        for(const auto &v : b) {
            if(addUniqueItem && a.find(v.first) == a.end()) {
                symbols.emplace(v);
            }
        }

        return symbols;
    };

    friend IntervalSymbols operator|(const IntervalSymbols& a, const IntervalSymbols& b) {
        return symbolOp([](const Interval& a, const Interval& b) { return a | b; }, a, b);
    };

    friend IntervalSymbols operator&(const IntervalSymbols& a, const IntervalSymbols& b) {
        return symbolOp([](const Interval& a, const Interval& b) { return a & b; }, a, b, false);
    };
};

#endif //CODEPUNK_INTERVALSYMBOLS_H
