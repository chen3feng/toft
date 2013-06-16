// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/base/closure.h"
#include "toft/base/shared_ptr.h"
#include "toft/cache/threadsafe_lru_cache.h"
#include "toft/system/threading/thread_pool.h"
#include "toft/system/time/clock.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

namespace toft {

typedef ThreadSafeLruCache<int, int> Cache;

TEST(ThreadSafeLruCacheTest, Normal) {
    Cache cache(5);
    cache.Put(2, new int(2));
    cache.Put(1, new int(1));
    cache.Put(3, new int(3));
    std::shared_ptr<int> value;
    EXPECT_TRUE(cache.Get(1, &value));
    EXPECT_EQ(1, *value);
    EXPECT_TRUE(cache.Get(3, &value));
    EXPECT_EQ(3, *value);
    EXPECT_TRUE(cache.Get(2, &value));
    EXPECT_EQ(2, *value);
    EXPECT_TRUE(cache.Get(1, &value));
    EXPECT_EQ(1, *value);
    EXPECT_TRUE(cache.Get(2, &value));
    EXPECT_EQ(2, *value);
    EXPECT_TRUE(cache.Get(3, &value));
    EXPECT_EQ(3, *value);
    EXPECT_FALSE(cache.Get(4, &value));
}

static void SetCache(Cache *cache, int multiplier) {
    for (int i = 2; i < 1000; ++i) {
        std::shared_ptr<int> value(new int(i * multiplier));
        cache->Put(i, value);
    }
}

static void SetThread(Cache *cache) {
    int64_t now = RealtimeClock.MicroSeconds();
    int multiplier = 1;
    while (RealtimeClock.MicroSeconds() - now < 500) {
        SetCache(cache, multiplier);
        ++multiplier;
    }
}

static void ReadThread(Cache *cache) {
    int64_t now = RealtimeClock.MicroSeconds();
    std::shared_ptr<int> value;
    while (RealtimeClock.MicroSeconds() - now < 500) {
        for (int i = 2; i < 1000; ++i) {
            bool r = cache->Get(i, &value);
            if (r)
                ASSERT_EQ(0, *value % i);
        }
    }
}

TEST(ThreadSafeLruCacheTest, MultiThread) {
    Cache cache(900);
    ThreadPool pool(7, 7);
    for (int i = 0; i < 2; ++i) {
        pool.AddTask(NewClosure(SetThread, &cache));
    }
    for (int i = 0; i < 5; ++i) {
        pool.AddTask(NewClosure(ReadThread, &cache));
    }
}

} // namespace toft
