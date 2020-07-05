//
// Created by edboy on 2020/7/5.
//

#include <Ternary.h>
#include <gtest/gtest.h>

TEST(Ternary, Construct) {
    ASSERT_TRUE(Ternary().equals(Ternary::Unknown));
    ASSERT_TRUE(Ternary(true).equals(Ternary::True));
    ASSERT_TRUE(Ternary(false).equals(Ternary::False));

    ASSERT_TRUE(Ternary(Ternary::Unknown).equals({}));
}

TEST(Ternary, EqOp) {
    ASSERT_TRUE((Ternary() == Ternary()).equals({}));
    ASSERT_TRUE((Ternary() == Ternary(false)).equals({}));
    ASSERT_TRUE((Ternary() == Ternary(true)).equals({}));
    ASSERT_TRUE((Ternary(false) == Ternary()).equals({}));
    ASSERT_TRUE((Ternary(false) == Ternary(false)).equals(Ternary::True));
    ASSERT_TRUE((Ternary(false) == Ternary(true)).equals(Ternary::False));
    ASSERT_TRUE((Ternary(true) == Ternary()).equals({}));
    ASSERT_TRUE((Ternary(true) == Ternary(false)).equals(Ternary::False));
    ASSERT_TRUE((Ternary(true) == Ternary(true)).equals(Ternary::True));

    ASSERT_TRUE((Ternary() != Ternary()).equals({}));
    ASSERT_TRUE((Ternary() != Ternary(false)).equals({}));
    ASSERT_TRUE((Ternary() != Ternary(true)).equals({}));
    ASSERT_TRUE((Ternary(false) != Ternary()).equals({}));
    ASSERT_TRUE((Ternary(false) != Ternary(false)).equals(Ternary::False));
    ASSERT_TRUE((Ternary(false) != Ternary(true)).equals(Ternary::True));
    ASSERT_TRUE((Ternary(true) != Ternary()).equals({}));
    ASSERT_TRUE((Ternary(true) != Ternary(false)).equals(Ternary::True));
    ASSERT_TRUE((Ternary(true) != Ternary(true)).equals(Ternary::False));
}

TEST(Ternary, LogicOp) {
    ASSERT_TRUE((!Ternary()).equals({}));
    ASSERT_TRUE((!Ternary(false)).equals(Ternary::True));
    ASSERT_TRUE((!Ternary(true)).equals(Ternary::False));

    ASSERT_TRUE((Ternary() || Ternary()).equals({}));
    ASSERT_TRUE((Ternary() || Ternary(false)).equals({}));
    ASSERT_TRUE((Ternary() || Ternary(true)).equals(Ternary::True));
    ASSERT_TRUE((Ternary(false) || Ternary()).equals({}));
    ASSERT_TRUE((Ternary(false) || Ternary(false)).equals(Ternary::False));
    ASSERT_TRUE((Ternary(false) || Ternary(true)).equals(Ternary::True));
    ASSERT_TRUE((Ternary(true) || Ternary()).equals(Ternary::True));
    ASSERT_TRUE((Ternary(true) || Ternary(false)).equals(Ternary::True));
    ASSERT_TRUE((Ternary(true) || Ternary(true)).equals(Ternary::True));

    ASSERT_TRUE((Ternary() && Ternary()).equals({}));
    ASSERT_TRUE((Ternary() && Ternary(false)).equals(Ternary::False));
    ASSERT_TRUE((Ternary() && Ternary(true)).equals({}));
    ASSERT_TRUE((Ternary(false) && Ternary()).equals(Ternary::False));
    ASSERT_TRUE((Ternary(false) && Ternary(false)).equals(Ternary::False));
    ASSERT_TRUE((Ternary(false) && Ternary(true)).equals(Ternary::False));
    ASSERT_TRUE((Ternary(true) && Ternary()).equals({}));
    ASSERT_TRUE((Ternary(true) && Ternary(false)).equals(Ternary::False));
    ASSERT_TRUE((Ternary(true) && Ternary(true)).equals(Ternary::True));
}
