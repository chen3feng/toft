// Copyright (c) 2009, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

// GLOBAL_NOLINT(readability/function)
// GLOBAL_NOLINT(runtime/int)

#include "toft/system/atomic/atomic.h"

#include <stdint.h>
#include <stdio.h>

#include "thirdparty/gtest/gtest.h"

namespace toft {

template <typename T>
struct type_tester
{
    static void run(const char* type)
    {
        Atomic<T> n = 1;
        ASSERT_EQ(n, (T)1);

        T t = (n = 2);
        ASSERT_EQ(n, (T)2);
        ASSERT_EQ(t, (T)2);

        n = 1;
        t = (n += 1);
        ASSERT_EQ((T)2, n);
        ASSERT_EQ((T)2, t);

        n = 1;
        t = (n -= 1);
        ASSERT_EQ((T)0, n);
        ASSERT_EQ((T)0, t);

        n = 1;
        t = ++n;
        ASSERT_EQ((T)2, n);
        ASSERT_EQ((T)2, t);

        n = 1;
        t = --n;
        ASSERT_EQ((T)0, n);
        ASSERT_EQ((T)0, t);

        n = 1;
        t = n++;
        ASSERT_EQ(n, (T)2);
        ASSERT_EQ(t, (T)1);

        n = 1;
        t = n--;
        ASSERT_EQ((T)0, n);
        ASSERT_EQ((T)1, t);

        n = 1;
        t = (n &= 1);
        ASSERT_EQ((T)1, n);
        ASSERT_EQ((T)1, t);

        t = (n &= 2);
        ASSERT_EQ((T)0, n);
        ASSERT_EQ((T)0, t);

        t = (n |= 1);
        ASSERT_EQ((T)1, n);
        ASSERT_EQ((T)1, t);

        t = (n |= 2);
        ASSERT_EQ((T)3, n);
        ASSERT_EQ((T)3, t);

        t = (n |= 3);
        ASSERT_EQ((T)3, n);

        t = (n ^= 3);
        ASSERT_EQ((T)0, n);

        t = n.Exchange(4);
        ASSERT_EQ((T)0, t);
        ASSERT_EQ((T)4, n);

        ASSERT_TRUE(n.CompareExchange(4, 5, &t));
        ASSERT_EQ((T)4, t);
        ASSERT_EQ((T)5, n);

        ASSERT_FALSE(n.CompareExchange(4, 6, &t));
        ASSERT_EQ((T)5, t);
        ASSERT_EQ((T)5, n);
    }
};

template <>
struct type_tester<bool>
{
    static void run(const char* type)
    {
        Atomic<bool> n = true;
        ASSERT_TRUE(n);

        bool t = (n = false);
        ASSERT_FALSE(n);
        ASSERT_FALSE(t);

        t = (n = true);
        ASSERT_TRUE(n);
        ASSERT_TRUE(t);

        n = false;
        t = n.Exchange(true);
        ASSERT_FALSE(t);
        ASSERT_TRUE(n);

        n = true;
        ASSERT_TRUE(n.CompareExchange(true, false, &t));
        ASSERT_TRUE(t);
        ASSERT_FALSE(n);

        n = false;
        ASSERT_FALSE(n.CompareExchange(true, false, &t));
        ASSERT_FALSE(t);
        ASSERT_FALSE(n);
    }
};

#define TEST_TYPE(type) do { type_tester<type>::run(#type); } while (0)

TEST(Atomic, BasicTypes)
{
#if (_MSC_VER && _M_X86) || __GNUC__
    TEST_TYPE(bool);
    TEST_TYPE(char);
    TEST_TYPE(signed char);
    TEST_TYPE(unsigned char);

    TEST_TYPE(wchar_t);

    TEST_TYPE(short);
    TEST_TYPE(unsigned short);
#endif

    TEST_TYPE(int);
    TEST_TYPE(unsigned int);

    TEST_TYPE(long);
    TEST_TYPE(unsigned long);

    TEST_TYPE(long long);
    TEST_TYPE(unsigned long long);
}

static long long g_n;
TEST(Atomic, Global64)
{
    EXPECT_EQ(1LL, AtomicIncrement(&g_n));
}

#if defined __unix__
#include <pthread.h>
#include <sys/time.h>
inline long long time_stamp()
{
    timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000LL + tv.tv_usec / 1000;
}
#else
#error unsupported platform
#endif

volatile int N;

TEST(Atomic, Speed)
{
    long long t0, t1;
    int test_count = 10000000;

#if defined _WIN32
    CRITICAL_SECTION cs;
    InitializeCriticalSection(&cs);
    t0 = time_stamp();

    for (int i = 0; i < test_count; ++i)
    {
        EnterCriticalSection(&cs);
        ++N;
        LeaveCriticalSection(&cs);
    }

#elif __unix__
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    t0 = time_stamp();
    for (int i = 0; i < test_count; ++i)
    {
        pthread_mutex_lock(&mutex);
        ++N;
        pthread_mutex_unlock(&mutex);
    }
#endif
    t1 = time_stamp();
    printf("mutex time: %lldms\n", t1 - t0);

    t0 = t1;
    N = 0;

    for (int i = 0; i < test_count; ++i)
    {
        AtomicIncrement(&N);
    }
    t1 = time_stamp();
    printf("atomic time: %lldms\n", t1 - t0);

    t0 = t1;
    N = 0;

    for (int i = 0; i < test_count; ++i)
    {
        ++N;
    }
    t1 = time_stamp();
    printf("unprotected time: %lldms\n", t1 - t0);
}

template <typename T>
class RWLocked
{
    static const size_t PointerBits =
        sizeof(void*) == 8 ? 48 : 32; // NOLINT(runtime/sizeof)

private:
    union Internal
    {
        struct
        {
            uintptr_t ptr : PointerBits;
            int ref : 64 - PointerBits;
        };
        unsigned long long as_uint64;
    } m_internal;
    typedef char CheckSize[sizeof(Internal) == 8 ? 1 : -1];

public:
    RWLocked(T* value) // NOLINT(runtime/explicit)
    {
        m_internal.ptr = (uintptr_t)value;
        m_internal.ref = 0;
    }

    T* Acquire()
    {
        Internal to_compare = m_internal;
        Internal to_exchange;

        do
        {
            to_exchange.ptr = to_compare.ptr;
            to_exchange.ref = to_compare.ref + 1;
        } while (!AtomicCompareExchange(&m_internal.as_uint64,
                                        to_compare.as_uint64,
                                        to_exchange.as_uint64,
                                        &to_compare.as_uint64));

        return reinterpret_cast<T*>(to_compare.ptr);
    }

    void Release()
    {
        Internal to_exchange;
        Internal to_compare = m_internal;

        do
        {
            to_exchange.ptr = to_compare.ptr;
            to_exchange.ref = to_compare.ref - 1;
        } while (!AtomicCompareExchange(&m_internal.as_uint64,
                                        to_compare.as_uint64,
                                        to_exchange.as_uint64,
                                        &to_compare.as_uint64));
    }

    T* Update(T* new_value)
    {
        Internal to_compare = m_internal;
        Internal to_exchange;
        to_exchange.ptr = (uintptr_t)new_value;
        to_exchange.ref = 0;
        do { } while (!AtomicCompareExchange(&m_internal.as_uint64,
                                             to_compare.as_uint64,
                                             to_exchange.as_uint64,
                                             &to_compare.as_uint64));
        return reinterpret_cast<T*>(to_compare.ptr);
    }

private:
    RWLocked(const RWLocked&);
    RWLocked& operator=(const RWLocked&);
};

TEST(Test, RWLocked)
{
    const int test_count = 10000000;

    RWLocked<int> rwlocked_int(new int(6));
    int *p = rwlocked_int.Acquire();
    ASSERT_EQ(6, *p);
    ++*p;
    rwlocked_int.Release();

    p = rwlocked_int.Acquire();
    ASSERT_EQ(7, *p);
    rwlocked_int.Release();

    long long t0, t1;
    t0 = time_stamp();
    for (int i = 0; i < test_count; ++i)
    {
        rwlocked_int.Acquire();
        rwlocked_int.Release();
    }
    t1 = time_stamp();
    printf("RWLocked time: %lldms\n", t1 - t0);

    delete rwlocked_int.Update(NULL);

#if _WIN32
    CRITICAL_SECTION cs;
    InitializeCriticalSection(&cs);
#else
    pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
#endif
    t0 = time_stamp();
    for (int i = 0; i < test_count; ++i)
    {
#if _WIN32
        EnterCriticalSection(&cs);
        LeaveCriticalSection(&cs);
#else
        pthread_rwlock_rdlock(&rwlock);
        pthread_rwlock_unlock(&rwlock);
#endif
    }
    t1 = time_stamp();
    printf("sys lock time: %lldms\n", t1 - t0);
}

TEST(Atomic, Pointer)
{
    Atomic<int*> p;
    Atomic<int*> q(NULL);
    p = 0;
    int *pp = p;
    pp = p.Value();
    ASSERT_EQ(0U, (uintptr_t)p.Value());
    int n;
    pp = p.Exchange(&n);
    ASSERT_EQ((uintptr_t)&n, (uintptr_t)p.Value());
    ASSERT_EQ(0U, (uintptr_t)pp);
}

TEST(Atomic, Function)
{
    int n = 0;
    EXPECT_EQ(1, AtomicAdd(&n, 1));
    EXPECT_EQ(1, n);

    n = 0;
    EXPECT_EQ(-1, AtomicSub(&n, 1));
    EXPECT_EQ(-1, n);

    n = 0;
    EXPECT_EQ(0, AtomicAnd(&n, 1));
    EXPECT_EQ(0, n);

    n = 0;
    EXPECT_EQ(1, AtomicOr(&n, 1));
    EXPECT_EQ(1, n);

    n = 0;
    EXPECT_EQ(1, AtomicXor(&n, 1));
    EXPECT_EQ(1, n);

    n = 0;
    EXPECT_EQ(0, AtomicExchangeAdd(&n, 1));
    EXPECT_EQ(1, n);

    n = 0;
    EXPECT_EQ(0, AtomicExchangeSub(&n, 1));
    EXPECT_EQ(-1, n);

    n = 0;
    EXPECT_EQ(0, AtomicExchangeAnd(&n, 1));
    EXPECT_EQ(0, n);

    n = 0;
    EXPECT_EQ(0, AtomicExchangeOr(&n, 1));
    EXPECT_EQ(1, n);

    n = 0;
    EXPECT_EQ(0, AtomicExchangeXor(&n, 1));
    EXPECT_EQ(1, n);
}

} // namespace toft
