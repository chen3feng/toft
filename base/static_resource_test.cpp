// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 09/30/11
// Description: static resource test

#include "toft/base/static_resource.h"
#include "thirdparty/gtest/gtest.h"

namespace toft {

static const char RESOURCE_test[] = "hello";

TEST(StaticResource, Test)
{
    StringPiece resource = TOFT_STATIC_RESOURCE(test);
    EXPECT_STREQ(RESOURCE_test, resource.data());
}

} // namespace toft
