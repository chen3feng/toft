// Copyright (c) 2012, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2012-06-21

#include "toft/system/threading/this_thread.h"
#include "toft/system/threading/thread.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {

static void IsMainThreadTestThread(bool* b)
{
    *b = ThisThread::IsMain();
}

TEST(ThisThread, GetId)
{
    EXPECT_GT(ThisThread::GetId(), 0);
}

TEST(ThisThread, GetHandle)
{
#if defined _WIN32 || defined __linux__
    EXPECT_TRUE(ThisThread::GetHandle() != 0); // NOLINT(readability/check)
#endif
}

TEST(ThisThread, Exit)
{
    // EXPECT_EXIT(ThisThread::Exit(), ::testing::ExitedWithCode(0), "");
}

TEST(ThisThread, IsMainThread)
{
    EXPECT_TRUE(ThisThread::IsMain());
    bool is_main = true;
    Thread thread;
    EXPECT_TRUE(thread.TryStart(std::bind(IsMainThreadTestThread, &is_main)));
    thread.Join();
    EXPECT_FALSE(is_main);
}

TEST(ThisThread, Yield)
{
    ThisThread::Yield();
}

} // namespace toft
