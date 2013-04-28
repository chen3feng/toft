// Copyright (c) 2010, The Toft Authors.
// All rights reserved.
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/base/unique_ptr.h"
#include "thirdparty/gtest/gtest.h"

TEST(UniquePtr, New)
{
    int* a = new int(1);
    std::unique_ptr<int> p(a);
    EXPECT_EQ(1, *p);
    EXPECT_EQ(a, p.get());
    EXPECT_TRUE(static_cast<bool>(p));
    int* q = p.release();
    EXPECT_EQ(a, q);
    EXPECT_EQ(NULL, p.get());
    EXPECT_FALSE(p);
    p.reset(q);
}
