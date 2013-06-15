// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 05/05/11
// Description: test binary version

#include "toft/base/binary_version.h"

#include "thirdparty/gtest/gtest.h"

TEST(BinaryVersion, Test)
{
    EXPECT_STRNE("Unknown", binary_version::kBuilderName);
    EXPECT_STRNE("Unknown", binary_version::kBuildTime);
    EXPECT_STRNE("Unknown", binary_version::kHostName);
    EXPECT_STRNE("Unknown", binary_version::kCompiler);
}
