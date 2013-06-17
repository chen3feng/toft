// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 05/11/11

#include "toft/base/string/byte_set.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(ByteSet, Empty)
{
    ByteSet bs;
    EXPECT_FALSE(bs.Find('A'));
    bs.Insert('A');
    EXPECT_TRUE(bs.Find('A'));
}

} // namespace toft
