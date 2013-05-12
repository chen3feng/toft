// Copyright (c) 2013, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/base/unordered_map.h"
#include "toft/base/unordered_set.h"

#include "thirdparty/gtest/gtest.h"

TEST(Unordered, Set) {
    std::unordered_set<int> us;
    us.insert(1);
    EXPECT_NE(us.end(), us.find(1));
}

TEST(Unordered, Map) {
    std::unordered_map<int, int> um;
    um[1] = 1;
    EXPECT_EQ(1, um[1]);
}
