// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 05/13/11
// Description:

#include "toft/system/threading/thread.h"
#include "toft/system/threading/this_thread.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {

static void ThreadCallback(int* p)
{
    ++*p;
}

TEST(Thread, Construct)
{
    int n = 0;
    Thread thread(std::bind(ThreadCallback, &n));
    thread.Join();
    EXPECT_EQ(1, n);
}

TEST(Thread, Start)
{
    int n = 0;
    Thread thread;
    thread.Start(std::bind(ThreadCallback, &n));
    thread.Join();
    EXPECT_EQ(1, n);
}

static void DoNothing()
{
}

TEST(Thread, Restart)
{
    Thread thread;
    for (int i = 0; i < 10; ++i)
    {
        thread.Start(DoNothing);
        int tid1 = thread.GetId();
        thread.Join();

        thread.Start(DoNothing);
        int tid2 = thread.GetId();
        EXPECT_NE(tid1, tid2);
        thread.Join();
    }
}

TEST(Thread, Reinitialize)
{
    Thread thread;
    thread.Start(DoNothing);
    thread.Join();
    thread.Start(DoNothing);
    thread.Join();
}

TEST(Thread, DuplicatedStartDeathTest)
{
    testing::FLAGS_gtest_death_test_style = "threadsafe";
    {
        Thread thread(DoNothing);
        EXPECT_DEATH(thread.Start(DoNothing), "Invalid argument");
    }
}

static void IsAliveTestThread(volatile const bool* stop)
{
    while (!*stop)
        ThisThread::Sleep(1);
    // ThisThread::Exit();
}

TEST(Thread, IsAlive)
{
    bool stop = false;
    Thread thread(std::bind(IsAliveTestThread, &stop));
    for (int i = 0; i < 1000; ++i)
    {
        if (!thread.IsAlive())
            ThisThread::Sleep(1);
    }
    stop = true;
    thread.Join();
    EXPECT_FALSE(thread.IsAlive());
}

TEST(Thread, Detach)
{
    for (int i = 0; i < 100; ++i)
    {
        Thread thread(std::bind(ThisThread::Sleep, 1));
        thread.Detach();
        ThisThread::Sleep(1);
    }
}

TEST(Thread, CreateDetach) {
    ThreadAttributes attributes;
    attributes.SetDetached(true);
    for (int i = 0; i < 100; ++i) {
        Thread thread(attributes, std::bind(ThisThread::Sleep, 1));
        ASSERT_FALSE(thread.IsJoinable());
    }
}

} // namespace toft
