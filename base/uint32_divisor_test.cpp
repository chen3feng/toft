// Copyright (c) 2010, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/base/uint32_divisor.h"

#include <limits.h>

#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(UInt32Divisor, SetValue)
{
    UInt32Divisor divisor;
    EXPECT_FALSE(divisor.SetValue(0));
    EXPECT_TRUE(divisor.SetValue(2817));

    for (int i = 0; i < 32; ++i)
        EXPECT_TRUE(divisor.SetValue(1U << i));
    for (unsigned int i = 1; i < 4096; ++i)
        EXPECT_TRUE(divisor.SetValue(i)) << i;
    for (unsigned int i = 1; i < UINT_MAX - 1024; i += 1024)
        EXPECT_TRUE(divisor.SetValue(i)) << i;
}

TEST(UInt32Divisor, DivideAlgorithm1)
{
    UInt32Divisor divisor;
    EXPECT_TRUE(divisor.SetValue(2817));
    for (unsigned int i = 1; i < UINT_MAX - 256; i += 256)
    {
        ASSERT_EQ(i / 2817, divisor.Divide(i))
            << i << " shift=" << divisor.GetShift();
    }
}

TEST(UInt32Divisor, DividePowerOf2)
{
    UInt32Divisor divisor;
    EXPECT_TRUE(divisor.SetValue(2048));
    for (unsigned int i = 1; i < UINT_MAX - 256; i += 256)
    {
        ASSERT_EQ(i / 2048, divisor.Divide(i))
            << "i= " << i
            << " shift=" << divisor.GetShift();
    }
}

const unsigned int kTestDivisor = 10000000;
volatile unsigned int kVolatileTestDivisor = kTestDivisor;

TEST(UInt32Divisor, ConstDividePerformance)
{
    unsigned int total = 0;
    for (unsigned int i = 1; i < UINT_MAX - 256; i += 256)
        total += i / kTestDivisor;
    volatile unsigned t = total;
    (void) t;
}

TEST(UInt32Divisor, NormalDividePerformance)
{
    unsigned int divisor = kVolatileTestDivisor;
    unsigned int total = 0;
    for (unsigned int i = 1; i < UINT_MAX - 256; i += 256)
        total += i / divisor;
    volatile unsigned t = total;
    (void) t;
}

TEST(UInt32Divisor, OptimizedDividePerformance)
{
    UInt32Divisor divisor(kTestDivisor);
    unsigned int total = 0;
    for (unsigned int i = 1; i < UINT_MAX - 256; i+= 256)
        total += divisor.Divide(i);
    volatile unsigned t = total;
    (void) t;
}

} // namespace toft
