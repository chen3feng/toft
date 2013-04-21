// Copyright (c) 2011, The Toft Authors. All rights reserved.
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/system/concurrency/condition_variable.h"
#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(ConditionVariable, Init)
{
    ConditionVariable cond;
}

TEST(ConditionVariable, Wait)
{
    ConditionVariable cond;
    cond.Signal();
}

TEST(ConditionVariable, Release)
{
}

} // namespace toft
