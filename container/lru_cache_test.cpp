// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/container/lru_cache.h"

#include "toft/base/closure.h"
#include "toft/system/threading/thread_pool.h"
#include "toft/system/time/clock.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(LruCacheTest, Normal) {
    LruCache<int, int> cache(5);
    cache.Put(2, 2);
    cache.Put(1, 1);
    cache.Put(3, 3);
    EXPECT_EQ(1, cache.GetOrDefault(1));
    EXPECT_EQ(3, cache.GetOrDefault(3));
    EXPECT_EQ(2, cache.GetOrDefault(2));
    EXPECT_EQ(1, cache.GetOrDefault(1));
    EXPECT_EQ(2, cache.GetOrDefault(2));
    EXPECT_EQ(3, cache.GetOrDefault(3));
    EXPECT_FALSE(cache.HasKey(4));
}

TEST(LruCacheTest, SameKey) {
    LruCache<int, int> cache(4);
    cache.Put(1, 1);
    cache.Put(1, 2);
    cache.Put(3, 3);
    EXPECT_EQ(2, cache.GetOrDefault(1));
    EXPECT_EQ(3, cache.GetOrDefault(3));
    EXPECT_EQ(2, cache.GetOrDefault(1));
}

TEST(LruCacheTest, Size) {
    LruCache<int, int> cache(2);
    EXPECT_EQ(0U, cache.Size());
    EXPECT_EQ(2U, cache.Capacity());
    EXPECT_TRUE(cache.IsEmpty());
    EXPECT_FALSE(cache.IsFull());

    cache.Put(1, 1);
    EXPECT_EQ(1U, cache.Size());
    EXPECT_FALSE(cache.IsEmpty());
    EXPECT_FALSE(cache.IsFull());

    cache.Put(2, 2);
    EXPECT_EQ(2U, cache.Size());
    EXPECT_FALSE(cache.IsEmpty());
    EXPECT_TRUE(cache.IsFull());

    cache.Put(3, 3);
    EXPECT_EQ(2U, cache.Size());
    EXPECT_FALSE(cache.IsEmpty());
    EXPECT_TRUE(cache.IsFull());
}

TEST(LruCacheTest, HasKey) {
    LruCache<int, int> cache(2);
    cache.Put(1, 1);
    EXPECT_TRUE(cache.HasKey(1));
    EXPECT_FALSE(cache.HasKey(2));
}

TEST(LruCacheTest, Remove) {
    LruCache<int, int> cache(2);
    cache.Put(1, 1);
    EXPECT_TRUE(cache.Remove(1));
    EXPECT_EQ(0U, cache.Size());
    EXPECT_TRUE(cache.IsEmpty());
    EXPECT_FALSE(cache.Remove(2));
}

TEST(LruCacheTest, Overflow) {
    LruCache<int, int> cache(3);
    cache.Put(1, 1);
    cache.Put(2, 2);
    cache.Put(2, 1);
    cache.Put(3, 3);
    cache.Put(4, 4);
    EXPECT_EQ(1, cache.GetOrDefault(2));
    EXPECT_EQ(3, cache.GetOrDefault(3));
    EXPECT_EQ(0, cache.GetOrDefault(1));
    EXPECT_EQ(4, cache.GetOrDefault(4));
}

static void SetCache(LruCache<int, int> *cache, int multiplier) {
    for (int i = 2; i < 1000; ++i) {
        cache->Put(i, i * multiplier);
    }
}

static void SetThread(LruCache<int, int> *cache) {
    int64_t now = RealtimeClock.MicroSeconds();
    int multiplier = 1;
    while (RealtimeClock.MicroSeconds() - now < 500) {
        SetCache(cache, multiplier);
        ++multiplier;
    }
}

static void ReadThread(LruCache<int, int> *cache) {
    int64_t now = RealtimeClock.MicroSeconds();
    int value;
    while (RealtimeClock.MicroSeconds() - now < 500) {
        for (int i = 2; i < 1000; ++i) {
            bool r = cache->Get(i, &value);
            if (r)
                ASSERT_EQ(0, value % i);
        }
    }
}

TEST(ThreadSafeLruCacheTest, MultiThread) {
    LruCache<int, int> cache(900);
    ThreadPool pool(7);
    for (int i = 0; i < 2; ++i) {
        pool.AddTask(NewClosure(SetThread, &cache));
    }
    for (int i = 0; i < 5; ++i) {
        pool.AddTask(NewClosure(ReadThread, &cache));
    }
}

}  // namespace toft
