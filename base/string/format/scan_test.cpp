// Copyright (c) 2013, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-02-06

#include "toft/base/string/format/scan.h"

#include <math.h>
#include <stdint.h>

#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(Scan, Escape)
{
    int n = 0;
    ASSERT_EQ(1, StringScan("%1", "%%%d", &n));
    EXPECT_EQ(1, n);
}

TEST(Scan, Bool)
{
    bool b1, b2;

    ASSERT_EQ(2, StringScan("10", "%b%b", &b1, &b2));
    EXPECT_TRUE(b1);
    EXPECT_FALSE(b2);

    ASSERT_EQ(2, StringScan(" 1 0", "%b%b", &b1, &b2));
    EXPECT_TRUE(b1);
    EXPECT_FALSE(b2);

    ASSERT_EQ(1, StringScan("12", "%b%b", &b1, &b2));
    EXPECT_TRUE(b1);

    ASSERT_EQ(2, StringScan("truefalse", "%b%b", &b1, &b2));
    EXPECT_TRUE(b1);
    EXPECT_FALSE(b2);

    ASSERT_EQ(2, StringScan("true false", "%b%b", &b1, &b2));
    EXPECT_TRUE(b1);
    EXPECT_FALSE(b2);

    ASSERT_EQ(1, StringScan("true faster", "%b%b", &b1, &b2));
    EXPECT_TRUE(b1);

    ASSERT_EQ(2, StringScan("true0", "%v%v", &b1, &b2));
    EXPECT_TRUE(b1);
    EXPECT_FALSE(b2);
}

TEST(Scan, Int)
{
    int n = 0;
    unsigned int u = 0;
    ASSERT_EQ(1, StringScan("100", "%i", &n));
    EXPECT_EQ(100, n);

    ASSERT_EQ(1, StringScan("200", "%d", &n));
    EXPECT_EQ(200, n);

    ASSERT_EQ(1, StringScan("400", "%o", &n));
    EXPECT_EQ(0400, n);

    ASSERT_EQ(1, StringScan("500", "%o", &u));
    EXPECT_EQ(0500U, u);

    ASSERT_EQ(1, StringScan("600", "%x", &n));
    EXPECT_EQ(0x600, n);

    ASSERT_EQ(1, StringScan("600", "%x", &u));
    EXPECT_EQ(0x600U, u);
}

TEST(ScanDeathTest, IntInvalid)
{
    int n = 0;
    ASSERT_DEBUG_DEATH(StringScan("300", "%u", &n), "");
}

TEST(Scan, IntOverflow)
{
    int8_t i8 = 1;
    ASSERT_EQ(0, StringScan("129", "%i", &i8));
    EXPECT_EQ(0, StringScan("-129", "%i", &i8));
    EXPECT_EQ(1, i8);

    uint8_t u8 = 1;
    ASSERT_EQ(0, StringScan("256", "%i", &u8));
    EXPECT_EQ(0, StringScan("-1", "%i", &u8));
    EXPECT_EQ(1, u8);

    int16_t i16 = 1;
    ASSERT_EQ(0, StringScan("32768", "%i", &i16));
    EXPECT_EQ(0, StringScan("-32769", "%i", &i16));
    EXPECT_EQ(1, i16);

    uint16_t u16 = 1;
    ASSERT_EQ(0, StringScan("65536", "%i", &u16));
    EXPECT_EQ(0, StringScan("-1", "%i", &u16));
    EXPECT_EQ(1, u16);

    int32_t i32 = 1;
    ASSERT_EQ(0, StringScan("0xFFFFFFFF1", "%i", &i32));
    EXPECT_EQ(0, StringScan("-0xFFFFFFFF1", "%i", &i32));
    EXPECT_EQ(1, i32);

    uint32_t u32 = 1;
    ASSERT_EQ(0, StringScan("0xFFFFFFFF1", "%i", &u32));
    EXPECT_EQ(0, StringScan("-1", "%i", &u32));
    EXPECT_EQ(1U, u32);

    int64_t i64 = 1;
    ASSERT_EQ(0, StringScan("0xFFFFFFFFFFFFFFFF1", "%i", &i64));
    EXPECT_EQ(0, StringScan("-0xFFFFFFFFFFFFFFFF1", "%i", &i64));
    EXPECT_EQ(1LL, i64);

    uint64_t u64 = 1;
    ASSERT_EQ(0, StringScan("0xFFFFFFFFFFFFFFFF1", "%i", &u64));
    EXPECT_EQ(0, StringScan("-1", "%i", &u64));
    EXPECT_EQ(1ULL, u64);
}

template <typename T>
static void TestFloatCorrect(const char* format)
{
    T v = 0.0;
    ASSERT_EQ(1, StringScan("3.14", format, &v)) << format;
    EXPECT_FLOAT_EQ(3.14, v);
}

TEST(Scan, Float)
{
    TestFloatCorrect<float>("%f");
    TestFloatCorrect<float>("%g");
    TestFloatCorrect<float>("%e");
    TestFloatCorrect<float>("%E");
    TestFloatCorrect<float>("%a");
    TestFloatCorrect<float>("%v");

    TestFloatCorrect<double>("%f");
    TestFloatCorrect<double>("%g");
    TestFloatCorrect<double>("%e");
    TestFloatCorrect<double>("%E");
    TestFloatCorrect<double>("%a");
    TestFloatCorrect<double>("%v");

    TestFloatCorrect<double>("%lf");
    TestFloatCorrect<double>("%lg");
    TestFloatCorrect<double>("%le");
    TestFloatCorrect<double>("%lE");
    TestFloatCorrect<double>("%lv");
}

template <typename T>
static void TestFloatIncorrect(const char* format)
{
    T v = 0.0;
    ASSERT_DEBUG_DEATH(StringScan("3.14", format, &v), "");
}

TEST(ScanDeathTest, FloatInvalid)
{
    TestFloatIncorrect<float>("%lf");
    TestFloatIncorrect<float>("%lg");
    TestFloatIncorrect<float>("%le");
    TestFloatIncorrect<float>("%lE");
    TestFloatIncorrect<float>("%lv");
}

TEST(Scan, FloatOverflow)
{
    float f;
    EXPECT_EQ(1, StringScan("1e50", "%f", &f));
    EXPECT_EQ(1, isinf(f));
    EXPECT_EQ(1, StringScan("-1e50", "%f", &f));
    EXPECT_EQ(-1, isinf(f)); // Bypass bug of inff in glibc
    EXPECT_EQ(1, StringScan("1e-50", "%f", &f));
    EXPECT_FLOAT_EQ(0.0f, f);

    double d;
    EXPECT_EQ(1, StringScan("1e500", "%f", &d));
    EXPECT_EQ(1, isinf(d));
    EXPECT_EQ(1, StringScan("-1e500", "%f", &d));
    EXPECT_EQ(-1, isinf(d));
    EXPECT_EQ(1, StringScan("1e-500", "%f", &d));
    EXPECT_DOUBLE_EQ(0.0, d);

    long double ld;
    EXPECT_EQ(1, StringScan("1e5000", "%f", &ld));
    EXPECT_EQ(1, isinf(ld));
    EXPECT_EQ(1, StringScan("-1e5000", "%f", &ld));
    EXPECT_EQ(-1, isinf(ld));
    EXPECT_EQ(1, StringScan("1e-5000", "%f", &ld));
    EXPECT_DOUBLE_EQ(0.0, ld);
}

TEST(Scan, String)
{
    std::string s;
    ASSERT_EQ(1, StringScan("hello", "%s", &s));
    EXPECT_EQ("hello", s);
}

TEST(Scan, StringLeadingSpace)
{
    std::string s;
    ASSERT_EQ(1, StringScan(" hello", "%s", &s));
    EXPECT_EQ("hello", s);
}

TEST(Scan, StringTailSpace)
{
    std::string s;
    ASSERT_EQ(1, StringScan("hello ", "%s", &s));
    EXPECT_EQ("hello", s);
}

TEST(Scan, StringAroundSpace)
{
    std::string s;
    ASSERT_EQ(1, StringScan(" hello ", "%s", &s));
    EXPECT_EQ("hello", s);
}

TEST(Scan, StringEmpty)
{
    std::string s;
    ASSERT_EQ(0, StringScan("  ", "%s", &s));

    std::string s1 = "s1";
    ASSERT_EQ(1, StringScan("test  ", "%s%s", &s, &s1));
    EXPECT_EQ("test", s);
    EXPECT_EQ("s1", s1);
}

TEST(Scan, ScanSet)
{
    std::string s;
    int i;
    ASSERT_EQ(1, StringScan("abcdef123", "%[a-z]", &s));
    EXPECT_EQ("abcdef", s);

    ASSERT_EQ(1, StringScan("abcdef123", "%[a-z0-9]", &s));
    EXPECT_EQ("abcdef123", s);

    ASSERT_EQ(1, StringScan("   abcdef123", "%[ \t]", &s));
    EXPECT_EQ("   ", s);

    ASSERT_EQ(2, StringScan("abcdef123", "%[a-z]%d", &s, &i));
    EXPECT_EQ("abcdef", s);
    EXPECT_EQ(123, i);

    ASSERT_EQ(2, StringScan("abcdef123", "%[^0-9]%d", &s, &i));
    EXPECT_EQ("abcdef", s);
    EXPECT_EQ(123, i);
}

TEST(Scan, Mixed)
{
    std::string s;
    unsigned int u;
    int i;
    ASSERT_EQ(3, StringScan("key=100hello 200", "key=%d%s%d", &u, &s, &i));
    EXPECT_EQ("hello", s);
    EXPECT_EQ(100U, u);
    EXPECT_EQ(200, i);
}

TEST(Scan, Skip)
{
    int i;
    ASSERT_EQ(1, StringScan("1,2,3", "%*d,%*d,%d", &i));
    EXPECT_EQ(3, i);
}

TEST(Scan, Consumed)
{
    int i;
    ASSERT_EQ(1, StringScan("hello", "%*s%n", &i));
    EXPECT_EQ(5, i);
}

class PerformanceTest : public testing::Test {};

const int kLoopCount = 500000;
const char kTestString[] =
"1,2,4,8,16,32,64,128,256,512,1024,2048,4006,8192,16384,32768,65536";
#define TEST_FORMAT "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d"

TEST_F(PerformanceTest, StringScan)
{
    int n;
    for (int i = 0; i < kLoopCount; ++i) {
        StringScan(kTestString, TEST_FORMAT,
                   &n, &n, &n, &n, &n, &n, &n, &n,
                   &n, &n, &n, &n, &n, &n, &n, &n);
    }
}

TEST_F(PerformanceTest, Sscanf)
{
    int n;
    for (int i = 0; i < kLoopCount; ++i) {
        sscanf(kTestString, TEST_FORMAT, // NOLINT(runtime/printf)
               &n, &n, &n, &n, &n, &n, &n, &n,
               &n, &n, &n, &n, &n, &n, &n, &n);
    }
}

} // namespace toft
