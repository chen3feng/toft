// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/system/threading/semaphore.h"
#include "thirdparty/gtest/gtest.h"

namespace toft {

template <typename T>
class SemaphoreTest : public testing::Test
{
protected:
    SemaphoreTest() : m_semaphore(2) {}
    T m_semaphore;
};

template <>
class SemaphoreTest<NamedSemaphore> : public testing::Test
{
protected:
    SemaphoreTest() : m_semaphore("semaphoretest", 2) {}
    ~SemaphoreTest() { NamedSemaphore::Unlink("semaphoretest"); }
    NamedSemaphore m_semaphore;
};

typedef testing::Types<Semaphore, NamedSemaphore> SemaphoreTestTypes;
TYPED_TEST_CASE(SemaphoreTest, SemaphoreTestTypes);

TYPED_TEST(SemaphoreTest, Init)
{
    ASSERT_EQ(this->m_semaphore.GetValue(), 2U);
    this->m_semaphore.Acquire();
    ASSERT_EQ(this->m_semaphore.GetValue(), 1U);
    this->m_semaphore.Acquire();
    ASSERT_EQ(this->m_semaphore.GetValue(), 0U);
    ASSERT_FALSE(this->m_semaphore.TryAcquire());
    ASSERT_EQ(this->m_semaphore.GetValue(), 0U);
}

TYPED_TEST(SemaphoreTest, AcquireAndRelease)
{
    ASSERT_EQ(this->m_semaphore.GetValue(), 2U);
    this->m_semaphore.Acquire();
    ASSERT_EQ(this->m_semaphore.GetValue(), 1U);
    this->m_semaphore.Release();
    ASSERT_EQ(this->m_semaphore.GetValue(), 2U);
}

TYPED_TEST(SemaphoreTest, TryAcquire)
{
    ASSERT_EQ(this->m_semaphore.GetValue(), 2U);
    ASSERT_TRUE(this->m_semaphore.TryAcquire());
    ASSERT_EQ(this->m_semaphore.GetValue(), 1U);
    ASSERT_TRUE(this->m_semaphore.TryAcquire());
    ASSERT_EQ(this->m_semaphore.GetValue(), 0U);
    ASSERT_FALSE(this->m_semaphore.TryAcquire());
    ASSERT_EQ(this->m_semaphore.GetValue(), 0U);
    this->m_semaphore.Release();
    ASSERT_EQ(this->m_semaphore.GetValue(), 1U);
    ASSERT_TRUE(this->m_semaphore.TryAcquire());
}

TYPED_TEST(SemaphoreTest, TimedAcquire)
{
    this->m_semaphore.Acquire();
    this->m_semaphore.Acquire();
    ASSERT_FALSE(this->m_semaphore.TryAcquire());
    time_t t0 = time(NULL);
    ASSERT_FALSE(this->m_semaphore.TimedAcquire(1100));
    time_t t1 = time(NULL);
    ASSERT_GT(t1, t0);
}

TEST(NamedSemaphore, DefaultCtorDeathTest)
{
    NamedSemaphore semaphore;
    EXPECT_DEATH(semaphore.Acquire(), "");
}

TEST(NamedSemaphore, OpenCreateClose)
{
    std::string sem_name = "testsem1";
    NamedSemaphore::Unlink(sem_name);
    NamedSemaphore semaphore;
    EXPECT_FALSE(semaphore.Close());
    EXPECT_FALSE(semaphore.Open(sem_name));
    EXPECT_TRUE(semaphore.Create(sem_name, 1));
    EXPECT_TRUE(semaphore.Close());
    EXPECT_TRUE(semaphore.Open(sem_name));
    EXPECT_TRUE(semaphore.Close());
    NamedSemaphore::Unlink(sem_name);
    EXPECT_FALSE(semaphore.Open(sem_name));
    EXPECT_TRUE(semaphore.OpenOrCreate(sem_name, 1));
    NamedSemaphore::Unlink(sem_name);
}

} // namespace toft
