// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/system/threading/mutex.h"
#include "toft/system/threading/spinlock.h"
#include "toft/system/threading/thread_group.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {

const int kLoopCount = 10000000;

TEST(MutexTest, Mutex)
{
    Mutex lock;
    for (int i = 0; i < kLoopCount; ++i)
    {
        Mutex::Locker locker(&lock);
    }
}

void TestThread(int* p, Mutex* mutex)
{
    for (;;)
    {
        Mutex::Locker locker(mutex);
        if (++(*p) >= kLoopCount)
            return;
    }
}

TEST(MutexTest, ThreadMutex)
{
    int n = 0;
    Mutex lock;
    ThreadGroup thread_group(std::bind(TestThread, &n, &lock), 4);
    thread_group.Join();
}

TEST(SpinLockTest, SpinLock)
{
    SpinLock lock;
    for (int i = 0; i < kLoopCount; ++i)
    {
        SpinLock::Locker locker(&lock);
    }
}

} // namespace toft
