// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//
// Author: CHEN Feng <phongchen@tencent.com>
// Created: 05/11/11

#include "common/base/string/byte_set.h"
#include "thirdparty/gtest/gtest.h"

// namespace common {

TEST(ByteSet, Empty)
{
    ByteSet bs;
    EXPECT_FALSE(bs.Find('A'));
    bs.Insert('A');
    EXPECT_TRUE(bs.Find('A'));
}

// } // namespace common
