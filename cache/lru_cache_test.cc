// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/cache/lru_cache.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(LruCacheTest, Normal) {
    LruCache<int, int> cache(5);
    cache.Put(2, new int(2));
    std::shared_ptr<int> ptr(new int(1));
    cache.Put(1, ptr);
    cache.Put(3, std::shared_ptr<int>(new int(3)));
    EXPECT_EQ(1, *cache.Get(1).get());
    EXPECT_EQ(3, *cache.Get(3).get());
    EXPECT_EQ(2, *cache.Get(2).get());
    EXPECT_EQ(1, *cache.Get(1).get());
    EXPECT_EQ(2, *cache.Get(2).get());
    EXPECT_EQ(3, *cache.Get(3).get());
    EXPECT_EQ(NULL, cache.Get(4).get());
}

TEST(LruCacheTest, SameKey) {
    LruCache<int, int> cache(4);
    cache.Put(1, new int(1));
    cache.Put(1, new int(2));
    cache.Put(3, new int(3));
    EXPECT_EQ(2, *cache.Get(1).get());
    EXPECT_EQ(3, *cache.Get(3).get());
    EXPECT_EQ(2, *cache.Get(1).get());
}

TEST(LruCacheTest, Overflow) {
    LruCache<int, int> cache(3);
    cache.Put(1, new int(1));
    cache.Put(2, new int(2));
    cache.Put(2, new int(1));
    cache.Put(3, new int(3));
    cache.Put(4, new int(4));
    EXPECT_EQ(1, *cache.Get(2).get());
    EXPECT_EQ(3, *cache.Get(3).get());
    EXPECT_EQ(NULL, cache.Get(1).get());
    EXPECT_EQ(4, *cache.Get(4).get());
}

}  // namespace toft
