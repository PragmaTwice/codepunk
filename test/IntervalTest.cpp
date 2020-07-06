//
// Created by edboy on 2020/7/4.
//

#include <gtest/gtest.h>
#include <Interval.h>

const auto Zero = Interval(APSInt(32, false));
const auto One = Interval(APSInt(APInt(32, 1), false));
const auto ZeroToOne = Interval(
        APSInt(32, false),
        APSInt(APInt(32, 1), false)
);

TEST(Interval, Construct) {
    ASSERT_TRUE(Zero.equals(Interval(APSInt(APInt(32, 0), false))));
    ASSERT_TRUE(ZeroToOne.equals(Interval(
            APSInt(32, false),
            APSInt(APInt(32, 1), false)
    )));

    ASSERT_TRUE(Zero.isValid());
    ASSERT_TRUE(Interval().isValid());
    ASSERT_TRUE(ZeroToOne.isValid());
    ASSERT_FALSE(Interval(
            APSInt(APInt(32, 1), false),
            APSInt(32, false)
    ).isValid());
}

TEST(Interval, Copy) {
    Interval a = ZeroToOne;
    ASSERT_TRUE(a.equals(ZeroToOne));
    ASSERT_TRUE(a.isValid());

    a = ZeroToOne;
    ASSERT_TRUE(a.equals(ZeroToOne));
    ASSERT_TRUE(a.isValid());
}

TEST(Interval, Predicate) {
    ASSERT_FALSE(Zero.equals(ZeroToOne));
    ASSERT_FALSE(ZeroToOne.equals(Zero));

    ASSERT_TRUE(ZeroToOne.length() == APSInt(APInt(32, 1), false));
    ASSERT_FALSE(ZeroToOne.isConstant());

    ASSERT_TRUE(Zero.isConstant());
    ASSERT_TRUE(ZeroToOne.contains(Zero));
    ASSERT_TRUE(ZeroToOne.contains(APSInt(APInt(32, 1), false)));

    ASSERT_FALSE(ZeroToOne.contains(APSInt(APInt(32, 2), false)));
    ASSERT_FALSE(ZeroToOne.contains(APSInt(APInt(32, -1, true), false)));
    ASSERT_TRUE(Interval(APSInt("1"), APSInt("3")).contains(APSInt("2")));
    ASSERT_FALSE(Zero.contains(ZeroToOne));

    ASSERT_TRUE(Zero.length() == APSInt(32, false));
    ASSERT_TRUE(ZeroToOne.overlaps(Zero));
    ASSERT_TRUE(Zero.overlaps(ZeroToOne));
    ASSERT_FALSE(Zero.overlaps(One));
    ASSERT_TRUE(ZeroToOne.overlaps(Interval(APSInt(APInt(32, 1), false),
            APSInt(APInt(32, 2), false))));
    ASSERT_FALSE(ZeroToOne.overlaps(Interval(APSInt(APInt(32, 2), false),
            APSInt(APInt(32, 3), false))));
    ASSERT_TRUE(Interval(APSInt("0"), APSInt("2")).overlaps(Interval(APSInt("1"), APSInt("3"))));
}

TEST(Interval, SetOp) {
    ASSERT_TRUE(Zero.equals(Zero & Zero));
    ASSERT_TRUE(Zero.equals(Zero | Zero));
    ASSERT_TRUE(ZeroToOne.equals(ZeroToOne & ZeroToOne));
    ASSERT_TRUE(ZeroToOne.equals(ZeroToOne | ZeroToOne));
    ASSERT_TRUE(ZeroToOne.equals(Zero | ZeroToOne));
    ASSERT_TRUE(Zero.equals(ZeroToOne & Zero));

    ASSERT_TRUE(ZeroToOne.equals(
            Interval(APSInt(APInt(32, -1, true), false), APSInt(APInt(32, 1), false)) &
            Interval(APSInt(APInt(32, 0), false), APSInt(APInt(32, 3), false))));
    ASSERT_TRUE(ZeroToOne.equals(Zero | One));

    ASSERT_FALSE((Zero & One).isValid());
    ASSERT_TRUE(Interval(APSInt(APInt(32, -1, true), false), APSInt(APInt(32, 10), false)).equals(
            Interval(APSInt(APInt(32, -1, true), false), APSInt(APInt(32, 1), false)) |
            Interval(APSInt(APInt(32, 9), false), APSInt(APInt(32, 10), false))));
}

TEST(Interval, ArthmeticOp) {
    ASSERT_TRUE(Zero.equals(Zero + Zero));
    ASSERT_TRUE(ZeroToOne.equals(Zero + ZeroToOne));
    ASSERT_TRUE(ZeroToOne.equals(ZeroToOne + Zero));
    ASSERT_TRUE(Interval(APSInt(APInt(32, 1), false), APSInt(APInt(32, 2), false))
        .equals(One + ZeroToOne));
    ASSERT_TRUE(Interval(APSInt(APInt(32, 0), false), APSInt(APInt(32, 2), false))
        .equals(ZeroToOne + ZeroToOne));

    ASSERT_TRUE(Zero.equals(Zero - Zero));
    ASSERT_TRUE(Interval(APSInt(APInt(32, -1, true), false),
            APSInt(APInt(32, 0), false)).equals(Zero - ZeroToOne));
    ASSERT_TRUE(ZeroToOne.equals(ZeroToOne - Zero));
    ASSERT_TRUE(ZeroToOne.equals(One - ZeroToOne));
    ASSERT_TRUE(Interval(APSInt(APInt(32, -1, false), false), APSInt(APInt(32, 1), false))
                        .equals(ZeroToOne - ZeroToOne));

    // TODO: add tests for mul & div
}

TEST(Interval, OrderOp) {
    ASSERT_TRUE(Zero <= ZeroToOne);
    ASSERT_TRUE(ZeroToOne <= One);
    ASSERT_TRUE(Zero < One);
    ASSERT_TRUE((ZeroToOne <= ZeroToOne).equals(Ternary::Unknown));

    ASSERT_TRUE(Zero != One);
    ASSERT_TRUE((ZeroToOne == ZeroToOne).equals(Ternary::Unknown));
    ASSERT_TRUE(Zero == Zero);

    ASSERT_TRUE((ZeroToOne == Zero).equals(Ternary::Unknown));
    ASSERT_TRUE(Interval(APSInt(APInt(32, -1, false), false), APSInt(APInt(32, 0), false)) !=
        Interval(APSInt(APInt(32, 1, false), false), APSInt(APInt(32, 2), false)));

    ASSERT_TRUE(Interval(APSInt(APInt(32, -1, false), false), APSInt(APInt(32, 0), false)) <
                Interval(APSInt(APInt(32, 1, false), false), APSInt(APInt(32, 2), false)));
    ASSERT_TRUE(Interval(APSInt(APInt(32, -1, false), false), APSInt(APInt(32, 0), false)) <=
                Interval(APSInt(APInt(32, 1, false), false), APSInt(APInt(32, 2), false)));

    ASSERT_TRUE((Interval(APSInt(APInt(32, -1, false), false), APSInt(APInt(32, 1), false)) <
                Interval(APSInt(APInt(32, 1, false), false), APSInt(APInt(32, 2), false)))
                .equals(Ternary::Unknown));
    ASSERT_TRUE(Interval(APSInt(APInt(32, -1, false), false), APSInt(APInt(32, 1), false)) <=
                Interval(APSInt(APInt(32, 1, false), false), APSInt(APInt(32, 2), false)));

    ASSERT_TRUE((Interval(APSInt(APInt(32, -1, false), false), APSInt(APInt(32, 2), false)) <=
                 Interval(APSInt(APInt(32, 1, false), false), APSInt(APInt(32, 3), false)))
                 .equals(Ternary::Unknown));

}
