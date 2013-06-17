// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 12/14/11
// Description: test for preprocess.h

#include "toft/base/preprocess.h"
#include "toft/base/preprocess_test_helper.h"

#include "thirdparty/gtest/gtest.h"

TEST(Preprocess, Stringize)
{
    EXPECT_STREQ("ABC", TOFT_PP_STRINGIZE(ABC));
}

TEST(Preprocess, Join)
{
    EXPECT_EQ(12, TOFT_PP_JOIN(1, 2));
}

TEST(Preprocess, DisallowInHeader)
{
    TOFT_PP_DISALLOW_IN_HEADER_FILE();
}
