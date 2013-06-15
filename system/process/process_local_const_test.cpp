// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-04-02

#include "toft/system/process/process_local_const.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(ProcessLocalConst, Value)
{
    static ProcessLocalConst<pid_t> s_pid(getpid);
    ASSERT_EQ(getpid(), s_pid.Value());
}

static void DeathIf(bool cond, const char* message)
{
    if (cond)
    {
        fprintf(stderr, "%s\n", message);
        abort();
    }
}

TEST(ProcessLocalConstDeathTest, DifferentInSubProcess)
{
    static ProcessLocalConst<pid_t> s_pid(getpid);
    int pid = getpid();
    EXPECT_DEATH(DeathIf(s_pid.Value() != pid, "different in subprocess"),
                 "different");
}

TEST(ProcessLocalConstDeathTest, SameInSubProcess)
{
    static ProcessLocalConst<pid_t> s_pid(getpid);
    EXPECT_DEATH(DeathIf(s_pid.Value() == getpid(), "same in subprocess"),
                 "same");
}

} // namespace toft
