// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/net/http/version.h"
#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(HttpVersion, DefaultCtor) {
    HttpVersion version;
    EXPECT_TRUE(version.IsEmpty());
    EXPECT_EQ(0, version.Major());
    EXPECT_EQ(0, version.Minor());
}

TEST(HttpVersion, Ctor) {
    HttpVersion version(1, 1);
    EXPECT_FALSE(version.IsEmpty());
    EXPECT_EQ(1, version.Major());
    EXPECT_EQ(1, version.Minor());
}

TEST(HttpVersion, Clear) {
    HttpVersion version(1, 1);
    EXPECT_FALSE(version.IsEmpty());
    version.Clear();
    EXPECT_TRUE(version.IsEmpty());
}

TEST(HttpVersion, Compare) {
    HttpVersion empty;
    HttpVersion v_0_9(0, 9);
    HttpVersion v_1_0(1, 0);
    HttpVersion v_1_1(1, 1);
    EXPECT_EQ(empty, empty);
    EXPECT_EQ(v_0_9, v_0_9);
    EXPECT_NE(v_0_9, empty);
    EXPECT_NE(v_0_9, v_1_0);
    EXPECT_LT(v_0_9, v_1_0);
    EXPECT_LT(v_1_0, v_1_1);
    EXPECT_LT(v_1_0, v_1_1);
    EXPECT_GT(v_1_0, v_0_9);
}

} // namespace toft
