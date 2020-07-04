//
// Created by edboy on 2020/7/4.
//

#include <gtest/gtest.h>
#include <Interval.h>

TEST(Interval, Construct) {
    ASSERT_EQ(Interval(), Interval(APSInt()));
    ASSERT_TRUE(Interval(
            APSInt(32, false),
            APSInt(llvm::APInt(32, 1), false)
    ).equals(Interval(
            APSInt(32, false),
            APSInt(llvm::APInt(32, 1), false)
    )));

    ASSERT_TRUE(Interval().isValid());
    ASSERT_TRUE(Interval(APSInt(32, false)).isValid());
    ASSERT_TRUE(Interval(
            APSInt(32, false),
            APSInt(llvm::APInt(32, 1), false)
    ).isValid());
    ASSERT_FALSE(Interval(
            APSInt(llvm::APInt(32, 1), false),
            APSInt(32, false)
    ).isValid());
}

TEST(Interval, Copy) {

}

TEST(Interval, Predicate) {

}

TEST(Interval, SetOp) {

}

TEST(Interval, ArthmeticOp) {

}

TEST(Interval, OrderOp) {

}
