// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/system/threading/rwlock.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(RwLock, Kind)
{
    RwLock lock1(RwLock::kKindPreferReader);
    RwLock lock2(RwLock::kKindPreferWriter);
    RwLock lock3(RwLock::kKindDefault);
}

TEST(RwLock, AccessDestructedCheckDeathTest)
{
    RwLock* plock;
    {
        RwLock lock;
        plock = &lock;
    }
    EXPECT_DEATH(plock->ReaderLock(), "Invalid argument");
    EXPECT_DEATH(plock->ReaderLock(), "Invalid argument");
    EXPECT_DEATH(plock->WriterLock(), "Invalid argument");
    EXPECT_DEATH(plock->TryReaderLock(), "Invalid argument");
    EXPECT_DEATH(plock->TryWriterLock(), "Invalid argument");
    EXPECT_DEATH(plock->Unlock(), "Invalid argument");
}

} // namespace toft
