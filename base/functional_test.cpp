// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/base/functional.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {

static int return42() { return 42; }
static int echo(int i) { return i; }


TEST(Function, Function)
{
    std::function<int ()> f = return42;
    EXPECT_EQ(42, f());
    std::function<int (int)> g = echo;
    EXPECT_EQ(42, g(42));
}

TEST(Function, Bind)
{
    std::function<int (int)> f = std::bind(echo, 42);
    EXPECT_EQ(42, f(42));
}


} // namespace toft
