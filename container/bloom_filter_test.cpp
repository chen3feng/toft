// Copyright (c) 2011, The Toft Authors. All rights reserved.
// Author: Chen Feng <chen3feng@gmail.com>

#include "toft/container/bloom_filter.h"

#include "toft/system/memory/unaligned.h"
#include "toft/system/time/clock.h"

#include "thirdparty/gtest/gtest.h"

#define TEST_URL_PREFIX \
"https://www.qq.com/news/2010-09-21/deucation/advanced/oversea/united_states/" \
"california/university_of_california_berkeley/home/";

namespace toft {

TEST(BloomFilter, FalsePositiveRate)
{
    const int capacity = 100000;
    int total_conflicts = 0;
    BloomFilter bloom_filter(capacity, 0.001);
    for (int i = 0; i < capacity; ++i)
    {
        if (!bloom_filter.InsertUnique(&i, sizeof(i)))
        {
            ++total_conflicts;
        }
    }
    std::cout << "total_conflicts: " << total_conflicts << "\n";
    if (total_conflicts > 0) {
        EXPECT_GT(capacity/total_conflicts, 1000) << total_conflicts;
    }
}

TEST(BloomFilter, Bitmap)
{
    const int capacity = 1000000;
    BloomFilter bloom_filter(capacity, 0.001);

    // a long key
    char url[256] = TEST_URL_PREFIX;
    size_t fixed_length = strlen(url);

    for (int i = 0; i < capacity; ++i)
    {
        PutUnaligned<int>(url + fixed_length, i);
        bloom_filter.Insert(url, fixed_length + sizeof(i));
    }

    for (int i = 0; i < capacity; ++i)
    {
        PutUnaligned<int>(url + fixed_length, i);
        EXPECT_TRUE(bloom_filter.MayContain(url, fixed_length + sizeof(i)));
    }

    std::string bitmap(reinterpret_cast<const char*>(bloom_filter.GetBitmap()),
                       bloom_filter.MemorySize());
    BloomFilter bloom_filter2(
        const_cast<void*>(static_cast<const void*>(bitmap.data())),
        bitmap.size(),
        bloom_filter.HashNumber()
    );

    for (int i = 0; i < capacity; ++i)
    {
        PutUnaligned<int>(url + fixed_length, i);
        EXPECT_TRUE(bloom_filter2.MayContain(url, fixed_length + sizeof(i)));
    }
}

/*
TEST(BloomFilter, Correction)
{
    const int capacity = 100000;
    BloomFilter bloom_filter(capacity, 0.001);
    for (int i = 0; i < capacity; ++i)
    {
        bool may_contain = bloom_filter.MayContain(&i, sizeof(i));
        ASSERT_NE(may_contain, bloom_filter.InsertUnique(&i, sizeof(i))) << i;
        ASSERT_TRUE(bloom_filter.MayContain(&i, sizeof(i))) << i;
    }
}
*/

TEST(BloomFilter, InsertPerformace)
{
    const int capacity = 1000000;
    BloomFilter bloom_filter(capacity, 0.001);

    // a long key
    char url[256] = TEST_URL_PREFIX;
    size_t fixed_length = strlen(url);

    for (int i = 0; i < capacity; ++i)
    {
        PutUnaligned<int>(url + fixed_length, i);
        bloom_filter.Insert(url, fixed_length + sizeof(i));
    }
}

TEST(BloomFilter, Performace)
{
    const int capacity = 1000000;
    BloomFilter bloom_filter(capacity, 0.001);
    printf(
        "memory size: %llu, hash number: %u\n",
        (unsigned long long) bloom_filter.MemorySize(),  // NOLINT(runtime/int)
        bloom_filter.HashNumber()
    );

    // a long key
    char url[256] = TEST_URL_PREFIX;
    size_t fixed_length = strlen(url);

    int conflict_count = 0;
    int first_conflict = 0;
    uint64_t start_time = RealtimeClock.MilliSeconds();
    for (int i = 0; i < capacity; ++i)
    {
        PutUnaligned<int>(url + fixed_length, i);
        if (!bloom_filter.InsertUnique(url, fixed_length + sizeof(i)))
        {
            ++conflict_count;
            if (conflict_count == 1)
                first_conflict = i;
        }
    }
    uint64_t end_time = RealtimeClock.MilliSeconds();

    printf("capacity=%d\nfirst conflict=%d\nconflict_count=%d\ntime=%d\n",
           capacity, first_conflict, conflict_count,
           static_cast<int>(end_time - start_time));
}

} // namespace toft
