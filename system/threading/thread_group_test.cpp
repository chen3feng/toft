// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/system/threading/thread_group.h"
#include "toft/system/threading/mutex.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {

class ThreadGroupTest : public testing::Test
{
public:
    static const int  kCount = 100000;
public:
    ThreadGroupTest() : n(0)
    {
    }
    void TestThread()
    {
        for (;;)
        {
            Mutex::Locker locker(&mutex);
            if (++n >= kCount)
                return;
        }
    }
protected:
    int n;
    Mutex mutex;
};

const int ThreadGroupTest::kCount;

TEST_F(ThreadGroupTest, Test)
{
    ThreadGroup thread_group(std::bind(&ThreadGroupTest::TestThread, this), 4);
    thread_group.Join();
    EXPECT_GE(n, kCount);
    EXPECT_EQ(4U, thread_group.Size());
}

TEST_F(ThreadGroupTest, Add)
{
    ThreadGroup thread_group;
    thread_group.Add(std::bind(&ThreadGroupTest::TestThread, this), 3);
    EXPECT_EQ(3U, thread_group.Size());
    thread_group.Add(std::bind(&ThreadGroupTest::TestThread, this));
    EXPECT_EQ(4U, thread_group.Size());
    thread_group.Join();
    EXPECT_GE(n, kCount);
    EXPECT_EQ(4U, thread_group.Size());
}

} // namespace toft
