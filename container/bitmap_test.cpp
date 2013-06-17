// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 06/23/11
// Description:

#include "toft/container/bitmap.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(Bitmap, GetSetClear)
{
    DynamicBitmap bm(1000);
    bm.SetAt(100);
    EXPECT_TRUE(bm.GetAt(100));
    bm.ClearAt(100);
    EXPECT_FALSE(bm.GetAt(100));

    bm.SetAt(100, true);
    EXPECT_TRUE(bm.GetAt(100));

    bm.SetAt(100, false);
    EXPECT_FALSE(bm.GetAt(100));
}

TEST(Bitmap, InitialValue)
{
    DynamicBitmap bm1(100, true);
    EXPECT_EQ(std::string(100, '1'), bm1.ToString());

    DynamicBitmap bm2(100, false);
    EXPECT_EQ(std::string(100, '0'), bm2.ToString());
}

TEST(Bitmap, FromString)
{
    DynamicBitmap bm("110");
    EXPECT_FALSE(bm.GetAt(0));
    EXPECT_TRUE(bm.GetAt(1));
    EXPECT_TRUE(bm.GetAt(2));
}

TEST(Bitmap, ToString)
{
    DynamicBitmap bm(100);
    std::string s(100, '0');
    EXPECT_EQ(s, bm.ToString());
}

TEST(Bitmap, All)
{
    DynamicBitmap bm(100);
    bm.SetAll();
    EXPECT_TRUE(bm.AllAreSet());
    bm.ClearAll();
    EXPECT_TRUE(bm.AllAreClear());
}

TEST(Bitmap, Shift)
{
    DynamicBitmap bm(5, true);
    bm.LeftShift(1);
    EXPECT_EQ("11110", bm.ToString());
    bm.RightShift(1);
    EXPECT_EQ("01111", bm.ToString());
}

TEST(Bitmap, FixedBitmap)
{
    FixedBitmap<100> bm(true);
    EXPECT_EQ(std::string(100, '1'), bm.ToString());
}

TEST(Bitmap, AllAreClearInRange)
{
    FixedBitmap<5> bm("10001");
    EXPECT_FALSE(bm.AllAreClearInRange(0, 5));
    EXPECT_FALSE(bm.AllAreClearInRange(0, 4));
    EXPECT_FALSE(bm.AllAreClearInRange(1, 5));
    EXPECT_TRUE(bm.AllAreClearInRange(1, 4));
    EXPECT_TRUE(bm.AllAreClearInRange(1, 3));
    EXPECT_TRUE(bm.AllAreClearInRange(2, 3));

    FixedBitmap<100> bm2(false);
    bm2.SetAt(50);
    bm2.SetAt(60);
    EXPECT_FALSE(bm2.AllAreClearInRange(0, 100));
    EXPECT_FALSE(bm2.AllAreClearInRange(50, 60));
    EXPECT_TRUE(bm2.AllAreClearInRange(0, 50));
    EXPECT_TRUE(bm2.AllAreClearInRange(51, 60));
    EXPECT_TRUE(bm2.AllAreClearInRange(61, 100));
}

TEST(BitmapDeathTest, Overflow)
{
    FixedBitmap<100> bm(false);
#ifndef NDEBUG
    EXPECT_DEBUG_DEATH(bm.GetAt(100), "");
    EXPECT_DEBUG_DEATH(bm.SetAt(100), "");
    EXPECT_DEBUG_DEATH(bm.SetAt(120), "");
    EXPECT_DEBUG_DEATH(bm.SetAt(-1), "");
#endif
}

TEST(Bitmap, IsSubsetOf)
{
    DynamicBitmap bm(10);
    DynamicBitmap bm2(100);
    DynamicBitmap bm3(100);
    EXPECT_FALSE(bm.IsSubsetOf(bm2));
    for (int i = 0; i < 20; ++i) {
        bm2.SetAt(i);
    }
    EXPECT_FALSE(bm2.IsSubsetOf(bm3));
    for (int i = 0; i < 100; ++i) {
        bm3.SetAt(i);
    }
    EXPECT_TRUE(bm2.IsSubsetOf(bm3));
}

TEST(Bitmap, BitwiseOperation)
{
    DynamicBitmap bm("110101");
    DynamicBitmap bm2("101011");
    bm.AndWith(bm2);
    EXPECT_EQ("100001", bm.ToString());
    bm.OrWith(bm2);
    EXPECT_EQ("101011", bm.ToString());
    bm.XorWith(bm2);
    EXPECT_EQ("000000", bm.ToString());
}

TEST(Bitmap, FindFirstAndNext)
{
    // All positions are set to 0
    DynamicBitmap bm(100);
    size_t result = 0;
    EXPECT_FALSE(bm.FindFirst(&result));
    EXPECT_EQ(0U, result);
    bm.SetAt(20);
    EXPECT_TRUE(bm.FindFirst(&result));
    EXPECT_EQ(20U, result);
    bm.SetAt(50);
    EXPECT_TRUE(bm.FindNext(20u, &result));
    EXPECT_EQ(50U, result);
    // Can't find another position set to 1
    EXPECT_FALSE(bm.FindNext(50u, &result));

    // Prev position out of bounds
    EXPECT_FALSE(bm.FindNext(100u, &result));
}

} // namespace toft
