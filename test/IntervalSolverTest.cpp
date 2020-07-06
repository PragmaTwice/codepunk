//
// Created by edboy on 2020/7/6.
//

#include <gtest/gtest.h>
#include <IntervalSolver.h>

using std::make_shared;
using std::string;

using SIS = IntervalSolver<string>;
using SBE = BoolExpr<string>;
using SBO = BinOp<string>;
using SAO = AndOp<string>;
using SOO = OrOp<string>;
using SNO = NotOp<string>;
using SA = Atom<string>;

TEST(IntervalSolver, Solve) {
    {
        SIS solver{make_shared<SIS::Symbols>(SIS::Symbols{
                {"10", Interval(APInt(32, 10))},
                {"a",  Interval(APInt(32, 0), APInt(32, 100))}
        }), make_shared<SBO>(SBO::LT, make_shared<SA>("a"), make_shared<SA>("10"))};

        auto res = solver.solve(true);
        ASSERT_TRUE(res["a"].equals(Interval(APInt(32, 0), APInt(32, 9))));
        ASSERT_TRUE(res["10"].equals(Interval(APInt(32, 10))));
    }

    {
        SIS solver{make_shared<SIS::Symbols>(SIS::Symbols{
                {"10", Interval(APInt(32, 10))},
                {"5",  Interval(APInt(32, 5))},
                {"a",  Interval(APInt(32, 0), APInt(32, 100))}
        }), make_shared<SAO>(
                make_shared<SBO>(SBO::GT, make_shared<SA>("a"), make_shared<SA>("5")),
                make_shared<SBO>(SBO::LT, make_shared<SA>("a"), make_shared<SA>("10")))};

        auto res = solver.solve(true);
        ASSERT_TRUE(res["a"].equals(Interval(APInt(32, 6), APInt(32, 9))));
        ASSERT_TRUE(res["10"].equals(Interval(APInt(32, 10))));
        ASSERT_TRUE(res["5"].equals(Interval(APInt(32, 5))));
    }

    {
        SIS solver{make_shared<SIS::Symbols>(SIS::Symbols{
                {"10", Interval(APInt(32, 10))},
                {"5",  Interval(APInt(32, 5))},
                {"a",  Interval(APInt(32, 0), APInt(32, 100))}
        }), make_shared<SOO>(
                make_shared<SBO>(SBO::EQ, make_shared<SA>("a"), make_shared<SA>("5")),
                make_shared<SBO>(SBO::EQ, make_shared<SA>("a"), make_shared<SA>("10")))};

        auto res = solver.solve(true);
        ASSERT_TRUE(res["a"].equals(Interval(APInt(32, 5), APInt(32, 10))));
        ASSERT_TRUE(res["10"].equals(Interval(APInt(32, 10))));
        ASSERT_TRUE(res["5"].equals(Interval(APInt(32, 5))));
    }

    // TODO: fix a && b, a || b
}

TEST(IntervalSolver, Eval) {
    {
        SIS solver{make_shared<SIS::Symbols>(SIS::Symbols{
                {"10", Interval(APInt(32, 10))},
                {"a",  Interval(APInt(32, 0), APInt(32, 100))}
        }), make_shared<SBO>(SBO::LT, make_shared<SA>("a"), make_shared<SA>("10"))};

        ASSERT_TRUE(solver.eval().equals(Ternary::Unknown));
    }

    {
        SIS solver{make_shared<SIS::Symbols>(SIS::Symbols{
                {"10", Interval(APInt(32, 10))},
                {"a",  Interval(APInt(32, 0), APInt(32, 5))}
        }), make_shared<SBO>(SBO::LT, make_shared<SA>("a"), make_shared<SA>("10"))};

        ASSERT_TRUE(solver.eval().equals(Ternary::True));
    }

    {
        SIS solver{make_shared<SIS::Symbols>(SIS::Symbols{
                {"10", Interval(APInt(32, 10))},
                {"a",  Interval(APInt(32, 99), APInt(32, 100))}
        }), make_shared<SBO>(SBO::LT, make_shared<SA>("a"), make_shared<SA>("10"))};

        ASSERT_TRUE(solver.eval().equals(Ternary::False));
    }

    {
        SIS solver{make_shared<SIS::Symbols>(SIS::Symbols{
                {"10", Interval(APInt(32, 10))},
                {"5",  Interval(APInt(32, 5))},
                {"a",  Interval(APInt(32, 7), APInt(32, 9))}
        }), make_shared<SAO>(
                make_shared<SBO>(SBO::GT, make_shared<SA>("a"), make_shared<SA>("5")),
                make_shared<SBO>(SBO::LT, make_shared<SA>("a"), make_shared<SA>("10")))};

        ASSERT_TRUE(solver.eval().equals(Ternary::True));
    }

    {
        SIS solver{make_shared<SIS::Symbols>(SIS::Symbols{
                {"10", Interval(APInt(32, 10))},
                {"5",  Interval(APInt(32, 5))},
                {"a",  Interval(APInt(32, 7), APInt(32, 10))}
        }), make_shared<SAO>(
                make_shared<SBO>(SBO::GT, make_shared<SA>("a"), make_shared<SA>("5")),
                make_shared<SBO>(SBO::LT, make_shared<SA>("a"), make_shared<SA>("10")))};

        ASSERT_TRUE(solver.eval().equals(Ternary::Unknown));
    }

    {
        SIS solver{make_shared<SIS::Symbols>(SIS::Symbols{
                {"10", Interval(APInt(32, 10))},
                {"5",  Interval(APInt(32, 5))},
                {"a",  Interval(APInt(32, 1), APInt(32, 5))}
        }), make_shared<SAO>(
                make_shared<SBO>(SBO::GT, make_shared<SA>("a"), make_shared<SA>("5")),
                make_shared<SBO>(SBO::LT, make_shared<SA>("a"), make_shared<SA>("10")))};

        ASSERT_TRUE(solver.eval().equals(Ternary::False));
    }
}