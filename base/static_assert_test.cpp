// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 12/14/11
// Description: test for TOFT_STATIC_ASSERT

#include "toft/base/static_assert.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(StaticAssert, Test)
{
    TOFT_STATIC_ASSERT(1 == 1);
    TOFT_STATIC_ASSERT(1 == 1, "1 should be equal to 1");
}

TEST(StaticAssert, NoCompileTest)
{
#if 0 // uncomment to test
    TOFT_STATIC_ASSERT(false);
    TOFT_STATIC_ASSERT(1 == 2);
    TOFT_STATIC_ASSERT(1 == 2, "1 == 2");
#endif
}

} // namespace toft
