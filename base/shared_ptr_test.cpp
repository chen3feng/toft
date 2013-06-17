// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 04/15/2011

#include "toft/base/shared_ptr.h"

#include "thirdparty/gtest/gtest.h"

TEST(SharedPtr, Test)
{
    std::shared_ptr<int> p(new int());
    std::shared_ptr<int> q = p;
}

class Foo : public std::enable_shared_from_this<Foo>
{
};

TEST(SharedPtr, This)
{
    std::shared_ptr<Foo> p(new Foo());
    std::shared_ptr<Foo> q = p;
}
