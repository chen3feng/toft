// Copyright (c) 2011, The Toft Authors. All rights reserved.
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/system/threading/mutex.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(Mutex, Lock)
{
    Mutex mutex;
    mutex.Lock();
    mutex.AssertLocked();
    mutex.Unlock();
}

TEST(Mutex, Locker)
{
    Mutex mutex;
    {
        MutexLocker locker(&mutex);
        mutex.AssertLocked();
    }
}

template <typename Type>
static void DestroyWithLockHolding()
{
    Type mutex;
    mutex.Lock();
}

TEST(MutexDeathTest, DestroyCheck)
{
    testing::FLAGS_gtest_death_test_style = "threadsafe";
#ifndef NDEBUG
    // Checking of destry a locked mutex is only enabled in debug mode.
    EXPECT_DEATH(DestroyWithLockHolding<AdaptiveMutex>(), "");
    EXPECT_DEATH(DestroyWithLockHolding<Mutex>(), "");
    EXPECT_DEATH(DestroyWithLockHolding<RecursiveMutex>(), "");
#endif
}

template <typename Type>
void RecursiveLocking()
{
    Type mutex;
    mutex.Lock();
    mutex.Lock();
    mutex.Unlock();
    mutex.Unlock();
}


TEST(MutexDeathTest, SelfDeadLock)
{
    testing::FLAGS_gtest_death_test_style = "threadsafe";
#ifndef NDEBUG
    EXPECT_DEATH(RecursiveLocking<Mutex>(), "");
    EXPECT_DEATH(RecursiveLocking<AdaptiveMutex>(), "");
#endif
    RecursiveLocking<RecursiveMutex>();
}

TEST(Mutex, LockerWithException)
{
    Mutex mutex;
    try
    {
        MutexLocker locker(&mutex);
        mutex.AssertLocked();
        throw 0;
    }
    catch (...)
    {
        // ignore
    }
}

} // namespace toft
