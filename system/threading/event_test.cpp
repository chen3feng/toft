// Copyright (c) 2011, The Toft Authors. All rights reserved.
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2010-06-18

#include "toft/system/threading/event.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(AutoResetEvent, SetAndWait)
{
    AutoResetEvent event;
    EXPECT_FALSE(event.TryWait());
    event.Set();
    EXPECT_TRUE(event.TryWait());
    EXPECT_FALSE(event.TryWait());
}

TEST(AutoResetEvent, TimedWait)
{
    AutoResetEvent event;
    EXPECT_FALSE(event.TimedWait(1));
    event.Set();
    EXPECT_TRUE(event.TimedWait(1));
    EXPECT_FALSE(event.TryWait());
}

TEST(AutoResetEvent, InitValue)
{
    AutoResetEvent event(true);
    EXPECT_TRUE(event.TryWait());
    EXPECT_FALSE(event.TryWait());
    event.Set();
    EXPECT_TRUE(event.TryWait());
}

TEST(AutoResetEvent, Set)
{
    AutoResetEvent event(false);
    event.Set();
    EXPECT_TRUE(event.TryWait());
}

TEST(AutoResetEvent, Reset)
{
    AutoResetEvent event(true);
    event.Reset();
    EXPECT_FALSE(event.TryWait());
}

TEST(ManualResetEvent, SetAndWait)
{
    ManualResetEvent event;
    EXPECT_FALSE(event.TryWait());
    event.Set();
    EXPECT_TRUE(event.TryWait());
    EXPECT_TRUE(event.TryWait());
    event.Reset();
    EXPECT_FALSE(event.TryWait());
}

TEST(ManualResetEvent, InitValue)
{
    ManualResetEvent event(true);
    EXPECT_TRUE(event.TryWait());
    EXPECT_TRUE(event.TryWait());
    event.Set();
    EXPECT_TRUE(event.TryWait());
    EXPECT_TRUE(event.TryWait());
}

TEST(ManualResetEvent, TimedWait)
{
    ManualResetEvent event(false);
    event.Set();
    EXPECT_TRUE(event.TimedWait(1));
    event.Reset();
    EXPECT_FALSE(event.TimedWait(1));
}

TEST(ManualResetEvent, Set)
{
    ManualResetEvent event(false);
    event.Set();
    event.Wait();
    EXPECT_TRUE(event.TryWait());
}

TEST(ManualResetEvent, Reset)
{
    ManualResetEvent event(true);
    event.Reset();
    EXPECT_FALSE(event.TryWait());
}

} // namespace toft
