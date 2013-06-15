// Copyright (c) 2010, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/base/array_size.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(ArraySizeTest, Array)
{
    char a[2];
    ASSERT_EQ(sizeof(a), TOFT_ARRAY_SIZE(a));
}

TEST(ArraySizeTest, ConstArray)
{
    const char a[2] = {};
    ASSERT_EQ(sizeof(a), TOFT_ARRAY_SIZE(a));
}

TEST(ArraySizeTest, Scalable)
{
    // uncomment to test compile checking
    // char a; ASSERT_EQ(sizeof(a), TOFT_ARRAY_SIZE(a));
}

TEST(ArraySizeTest, Pointer)
{
    // uncomment to test compile checking
    // char* p; ASSERT_EQ(sizeof(p), TOFT_ARRAY_SIZE(p));
}

} // namespace toft
