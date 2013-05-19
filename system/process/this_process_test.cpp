// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Chen Feng <chen3feng@gmail.com>

#include "toft/system/process/this_process.h"
#include "toft/base/string/algorithm.h"
#include "toft/system/threading/this_thread.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(ThisProcess, BinaryPath)
{
    EXPECT_TRUE(StringEndsWith(ThisProcess::BinaryPath(), "/this_process_test"));
}

TEST(ThisProcess, BinaryName)
{
    EXPECT_EQ("this_process_test", ThisProcess::BinaryName());
}

TEST(ThisProcess, BinaryDirectory)
{
    EXPECT_TRUE(StringEndsWith(ThisProcess::BinaryDirectory(), "/process"));
}

TEST(ThisProcess, StartTime)
{
    time_t t = ThisProcess::StartTime();
    EXPECT_GT(t, 0);
    EXPECT_LE(t, time(NULL));
    ThisThread::Sleep(2000);
    ASSERT_EQ(t, ThisProcess::StartTime());
}

TEST(ThisProcess, ElapsedTime)
{
    time_t t = ThisProcess::ElapsedTime();
    EXPECT_GE(t, 0);
    EXPECT_LT(t, 100);
    ThisThread::Sleep(2000);
    EXPECT_GT(ThisProcess::ElapsedTime() - t, 1);
}

} // namespace toft

