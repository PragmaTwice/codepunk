//
// Created by edboy on 2020/6/30.
//

#ifndef CODEPUNK_TERNARY_H
#define CODEPUNK_TERNARY_H

#include <llvm/Support/raw_ostream.h>

struct Ternary {
    enum Type : char {Unknown, False, True} v;

    Ternary() : v(Unknown) {}
    Ternary(Type v) : v(v) {}

    explicit Ternary(bool b) : v(b ? True : False) {}

    friend llvm::raw_ostream &operator<<(llvm::raw_ostream& o, Ternary t) {
        return o << t.v;
    }

    explicit operator bool() const {
        return v == True;
    }

    friend Ternary operator!(Ternary t) {
        switch (t.v) {
            case True:
                return False;
            case False:
                return True;
            default:
                return Unknown;
        }
    }

    friend Ternary operator&&(Ternary a, Ternary b) {
        if(a.v == False || b.v == False) return False;
        if(a.v == Unknown || b.v == Unknown) return Unknown;

        return True;
    }

    friend Ternary operator||(Ternary a, Ternary b) {
        if(a.v == True || b.v == True) return True;
        if(a.v == Unknown || b.v == Unknown) return Unknown;

        return False;
    }
};

#endif //CODEPUNK_TERNARY_H
