// Copyright (c) 2011, The Toft Authors. All rights reserved.
// Author: CHEN Feng <chen3feng@gmail.com>

#undef NDEBUG
#include "toft/system/threading/mutex.h"
#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(Mutex, Lock)
{
    Mutex mutex;
    ASSERT_FALSE(mutex.IsLocked());
    mutex.Lock();
    ASSERT_TRUE(mutex.IsLocked());
    mutex.Unlock();
    ASSERT_FALSE(mutex.IsLocked());
}

TEST(Mutex, Locker)
{
    Mutex mutex;
    {
        ASSERT_FALSE(mutex.IsLocked());
        MutexLocker locker(mutex);
        ASSERT_TRUE(mutex.IsLocked());
    }
    ASSERT_FALSE(mutex.IsLocked());
}

template <typename Type>
void DestroyWithLockHolding()
{
    Type mutex;
    mutex.Lock();
}

TEST(MutexDeathTest, DestroyCheck)
{
    testing::FLAGS_gtest_death_test_style = "threadsafe";
    EXPECT_DEATH(DestroyWithLockHolding<AdaptiveMutex>(), "");
    EXPECT_DEATH(DestroyWithLockHolding<Mutex>(), "");
    EXPECT_DEATH(DestroyWithLockHolding<RecursiveMutex>(), "");
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
    EXPECT_DEATH(RecursiveLocking<Mutex>(), "");
    EXPECT_DEATH(RecursiveLocking<AdaptiveMutex>(), "");
    RecursiveLocking<RecursiveMutex>();
}

TEST(Mutex, LockerWithException)
{
    Mutex mutex;
    try
    {
        ASSERT_FALSE(mutex.IsLocked());
        MutexLocker locker(mutex);
        ASSERT_TRUE(mutex.IsLocked()) << "after locked constructed";
        throw 0;
    }
    catch (...)
    {
        // ignore
    }
    ASSERT_FALSE(mutex.IsLocked()) << "after exception thrown";
}

} // namespace toft
