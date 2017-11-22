// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
// Author: CHEN Feng <chen3feng@gmail.com>
// Created:  03/31/2011

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include "toft/base/string/number.h"

#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <algorithm>

#include "gtest/gtest.h"

namespace toft {

template <typename T>
class StringNumberTest : public ::testing::Test { };
TYPED_TEST_CASE_P(StringNumberTest);

TYPED_TEST_P(StringNumberTest, ParseNumberInvalid)
{
    const char* s = "hello";
    TypeParam n;
    EXPECT_FALSE(ParseNumber(s, &n));
    EXPECT_FALSE(ParseNumber(s, &n, NULL));

    char* endptr;
    EXPECT_FALSE(ParseNumber(s, &n, &endptr));
    EXPECT_EQ(s, endptr);
}

REGISTER_TYPED_TEST_CASE_P(StringNumberTest, ParseNumberInvalid);

typedef ::testing::Types<short, unsigned short, int, unsigned int,
                         long, unsigned long, long long, unsigned long long, float,
                         double, long double> NumericTypes;
INSTANTIATE_TYPED_TEST_CASE_P(NumericTypes, StringNumberTest, NumericTypes);

TEST(StringNumber, IntegerToStringBuffer)
{
    char buffer[1024];
    int32_t n1 = INT_MAX;
    int32_t n2 = -INT_MAX;
    int32_t n3 = 0;
    int32_t n4 = 100000;
    uint32_t n5 = 3147483647U;
    int32_t n6 = -123456789;

    int64_t s1 = LLONG_MAX;
    int64_t s2 = INT_MAX;
    int64_t s3 = 0;
    int64_t s4 = 1234567890123LL;
    int64_t s5 = 1000000000000LL;
    int64_t s6 = -1234567890034500LL;
    int64_t s7 = LLONG_MIN;

    ASSERT_STREQ("2147483647", IntegerToString(n1, buffer));
    ASSERT_STREQ("-2147483647", IntegerToString(n2, buffer));
    ASSERT_STREQ("0", IntegerToString(n3, buffer));
    ASSERT_STREQ("100000", IntegerToString(n4, buffer));
    ASSERT_STREQ("3147483647", IntegerToString(n5, buffer));
    ASSERT_STREQ("-123456789", IntegerToString(n6, buffer));

    ASSERT_STREQ("9223372036854775807", IntegerToString(s1, buffer));
    ASSERT_STREQ("2147483647", IntegerToString(s2, buffer));
    ASSERT_STREQ("0", IntegerToString(s3, buffer));
    ASSERT_STREQ("1234567890123", IntegerToString(s4, buffer));
    ASSERT_STREQ("1000000000000", IntegerToString(s5, buffer));
    ASSERT_STREQ("-1234567890034500", IntegerToString(s6, buffer));
    ASSERT_STREQ("-9223372036854775808", IntegerToString(s7, buffer));
}

TEST(StringNumber, IntegerToString)
{
    int32_t n1 = INT_MAX;
    int32_t n2 = -INT_MAX;
    int32_t n3 = 0;
    int32_t n4 = 100000;
    uint32_t n5 = 3147483647U;
    int32_t n6 = -123456789;

    int64_t s1 = LLONG_MAX;
    int64_t s2 = INT_MAX;
    int64_t s3 = 0;
    int64_t s4 = 1234567890123LL;
    int64_t s5 = 1000000000000LL;
    long long s6 = -1234567890034500LL;
    long long s7 = LLONG_MIN;

    ASSERT_EQ("2147483647", IntegerToString(n1));
    ASSERT_EQ("-2147483647", IntegerToString(n2));
    ASSERT_EQ("0", IntegerToString(n3));
    ASSERT_EQ("100000", IntegerToString(n4));
    ASSERT_EQ("3147483647", IntegerToString(n5));
    ASSERT_EQ("-123456789", IntegerToString(n6));

    ASSERT_EQ("9223372036854775807", IntegerToString(s1));
    ASSERT_EQ("2147483647", IntegerToString(s2));
    ASSERT_EQ("0", IntegerToString(s3));
    ASSERT_EQ("1234567890123", IntegerToString(s4));
    ASSERT_EQ("1000000000000", IntegerToString(s5));
    ASSERT_EQ("-1234567890034500", IntegerToString(s6));
    ASSERT_EQ("-9223372036854775808", IntegerToString(s7));
}

TEST(StringNumber, UIntToHexString)
{
    EXPECT_EQ("9527", UInt16ToHexString(0x9527));
    EXPECT_EQ("95279527", UInt32ToHexString(0x95279527));
    EXPECT_EQ("9527952795279527", UInt64ToHexString(0x9527952795279527ULL));
}

TEST(StringNumber, StringToNumber)
{
    int16_t i16;
    int32_t i32;
    int64_t i64;
    long long ll;
    unsigned long long ull;
    ASSERT_FALSE(StringToNumber("223372036854775807", &i32));
    ASSERT_TRUE(StringToNumber("223372036854775807", &i64));
    ASSERT_TRUE(StringToNumber("223372036854775807", &ll));
    ASSERT_TRUE(StringToNumber("223372036854775807", &ull));
    ASSERT_EQ(i64, 223372036854775807LL);
    ASSERT_EQ(ll, 223372036854775807LL);
    ASSERT_EQ(ull, 223372036854775807ULL);
    ASSERT_FALSE(StringToNumber("1147483647", &i16));
    ASSERT_TRUE(StringToNumber("1147483647", &i32));
    ASSERT_TRUE(StringToNumber("1147483647", &i64));
    ASSERT_EQ(i32, 1147483647);
    ASSERT_EQ(i64, 1147483647);

    uint32_t u32;
    ASSERT_TRUE(StringToNumber("1147483647", &u32));

    char buffer[1024];
    double d = 1.0003;

    ASSERT_STREQ(DoubleToString(d, buffer), "1.0003");
    d = std::numeric_limits<double>::infinity();
    ASSERT_STREQ(DoubleToString(d, buffer), "inf");
    d = -std::numeric_limits<double>::infinity();
    ASSERT_STREQ(DoubleToString(d, buffer), "-inf");
#ifdef __GNUC__ // divided by zero is not allowed in msvc
    d = NAN;
    ASSERT_STREQ(DoubleToString(d, buffer), "nan");
#endif

    float f = 1e+22;
    ASSERT_STREQ(FloatToString(f, buffer), "1e+22");
    f = 0.000325;
    ASSERT_STREQ(FloatToString(f, buffer), "0.000325");
    f = std::numeric_limits<double>::infinity();
    ASSERT_STREQ(FloatToString(f, buffer), "inf");
    f = -std::numeric_limits<double>::infinity();
    ASSERT_STREQ(FloatToString(f, buffer), "-inf");

#ifdef __GNUC__ // divided by zero is not allowed in msvc
    f = NAN;
    ASSERT_STREQ(FloatToString(f, buffer), "nan");

    f = INFINITY;
    ASSERT_STREQ(FloatToString(f, buffer), "inf");
#endif

    f = -std::numeric_limits<float>::infinity();
    ASSERT_STREQ(FloatToString(f, buffer), "-inf");

    uint32_t i = 255;
    ASSERT_STREQ(UInt32ToHexString(i, buffer), "000000ff");

    std::string str = "1110.32505QQ";
    char* endptr;
    ASSERT_TRUE(ParseNumber(str.c_str(), &d, &endptr));
    ASSERT_TRUE(d == 1110.32505);
    ASSERT_FALSE(StringToNumber(str.c_str(), &d));

    ASSERT_TRUE(ParseNumber(str.c_str(), &f, &endptr));
    ASSERT_TRUE(f == 1110.32505f);
    ASSERT_FALSE(StringToNumber(str.c_str(), &f));

    ASSERT_TRUE(ParseNumber(str.c_str(), &i, &endptr));
    ASSERT_EQ(1110U, i);
    ASSERT_FALSE(StringToNumber(str.c_str(), &i));

    str = "1110.32505";
    d = 0;
    f = 0;
    i = 0;
    ASSERT_TRUE(StringToNumber(str.c_str(), &d));
    ASSERT_TRUE(d == 1110.32505);
    ASSERT_TRUE(StringToNumber(str.c_str(), &f));
    ASSERT_TRUE(f == 1110.32505f);
    ASSERT_FALSE(StringToNumber(str.c_str(), &i));
    str = "-1110";
    int32_t x;
    ASSERT_TRUE(StringToNumber(str.c_str(), &x));
    ASSERT_EQ(x, -1110);
}

// http://synesis.com.au/publications.html
const char* MatthewWilsonConvert(int value, char buf[])
{
    static char digits[19] =
    { '9', '8', '7', '6', '5', '4', '3', '2', '1',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
    static const char* zero = digits + 9;  // zero 指向 '0'
    // works for -2147483648 .. 2147483647
    int i = value;
    char* p = buf;
    do {
        int lsd = i % 10;  // lsd 可能小于 0
        i /= 10;           // 是向下取整还是向零取整？
        *p++ = zero[lsd];  // 下标可能为负
    } while (i != 0);
    if (value < 0) {
        *p++ = '-';
    }
    *p = '\0';
    std::reverse(buf, p);
    return p; // p - buf 即为整数长度
}

class StringNumberPerformanceTest : public testing::Test
{
protected:
    static const int kInt = 42576010;
    static const char kIntString[];
    static const int64_t kInt64 = 4257601042576010LL;
    static const char kInt64String[];
    static const double kDouble;
    static const char kDoubleString[];
private:
    void SetUp()
    {
        int n;
        sscanf(kIntString, "%d", &n); // NOLINT(runtime/printf)
        ASSERT_EQ(kInt, n);
        ASSERT_TRUE(StringToNumber(kIntString, &n));
        ASSERT_EQ(kInt, n);
    }
};

const int StringNumberPerformanceTest::kInt;
const int64_t StringNumberPerformanceTest::kInt64;
const char StringNumberPerformanceTest::kIntString[] = "42576010";
const char StringNumberPerformanceTest::kInt64String[] = "4257601042576010";
const double StringNumberPerformanceTest::kDouble = 1110.32505;
const char StringNumberPerformanceTest::kDoubleString[] = "1110.32505";

TEST_F(StringNumberPerformanceTest, SScanfInt)
{
    int n;
    for (int i = 0; i < 1000000; i++)
        sscanf(kIntString, "%d", &n); // NOLINT(runtime/printf)
}

TEST_F(StringNumberPerformanceTest, SScanfInt64)
{
    int64_t n;
    for (int i = 0; i < 1000000; i++)
        sscanf(kInt64String, "%" SCNd64, &n); // NOLINT(runtime/printf)
}

TEST_F(StringNumberPerformanceTest, StringToInt)
{
    int n;
    for (int i = 0; i < 1000000; i++)
        StringToNumber(kIntString, &n);
}

TEST_F(StringNumberPerformanceTest, StringToIn64)
{
    int64_t n;
    for (int i = 0; i < 1000000; i++)
        StringToNumber(kInt64String, &n);
}

TEST_F(StringNumberPerformanceTest, StringToDouble)
{
    double d;
    for (size_t i = 0; i < 100000; i++)
        StringToNumber(kDoubleString, &d);
}

TEST_F(StringNumberPerformanceTest, MatthewWilsonConvert)
{
    char buffer[16];
    for (int i = 0; i < 1000000; i++)
        MatthewWilsonConvert(kInt + i, buffer);
}

TEST_F(StringNumberPerformanceTest, IntToString)
{
    char buffer[16];
    for (size_t i = 0; i < 100000; i++)
        IntegerToString(kInt + i, buffer);
}

TEST_F(StringNumberPerformanceTest, Int64ToString)
{
    char buffer[16];
    for (size_t i = 0; i < 100000; i++)
        IntegerToString(kInt64 + i, buffer);
}

TEST_F(StringNumberPerformanceTest, DoubleToString)
{
    char buffer[16];
    double d = kDouble;
    for (size_t i = 0; i < 100000; i++)
        DoubleToString(d, buffer);
}

TEST(StringNumber, FormatMeasure)
{
    EXPECT_EQ("1", FormatMeasure(1));
    EXPECT_EQ("123", FormatMeasure(123));
    EXPECT_EQ("1.23k", FormatMeasure(1234));
    EXPECT_EQ("12.3k", FormatMeasure(12345));
    EXPECT_EQ("123k", FormatMeasure(123456));
    EXPECT_EQ("1.23M", FormatMeasure(1234567));
    EXPECT_EQ("12.3M", FormatMeasure(12345678));
    EXPECT_EQ("123M", FormatMeasure(123456789));

    EXPECT_EQ("1bps", FormatMeasure(1, "bps"));
    EXPECT_EQ("123bps", FormatMeasure(123, "bps"));
    EXPECT_EQ("1.23kbps", FormatMeasure(1234, "bps"));
    EXPECT_EQ("12.3kbps", FormatMeasure(12345, "bps"));
    EXPECT_EQ("123kbps", FormatMeasure(123456, "bps"));
    EXPECT_EQ("1.23Mbps", FormatMeasure(1234567, "bps"));
    EXPECT_EQ("12.3Mbps", FormatMeasure(12345678, "bps"));
    EXPECT_EQ("123Mbps", FormatMeasure(123456789, "bps"));
    EXPECT_EQ("0bps", FormatMeasure(0, "bps"));
    EXPECT_EQ("100 mF", FormatMeasure(0.1, " F"));
    EXPECT_EQ("12.3 mF", FormatMeasure(0.0123, " F"));
    EXPECT_EQ("1.23 mF", FormatMeasure(0.001234, " F"));
    EXPECT_EQ("123 uF", FormatMeasure(0.00012345, " F"));
    EXPECT_EQ("12.3 uF", FormatMeasure(0.0000123456, " F"));
    EXPECT_EQ("1.23 uF", FormatMeasure(0.000001234567, " F"));
    EXPECT_EQ("123 nF", FormatMeasure(0.00000012345678, " F"));
    EXPECT_EQ("12.3 nF", FormatMeasure(0.0000000123456789, " F"));
}

TEST(StringNumber, PhysicalConstantsMeasure)
{
    EXPECT_EQ("300 Mm/s", FormatMeasure(299792458, " m/s"));
    EXPECT_EQ("160 zC", FormatMeasure(1.60217733e-19, " C"));
    EXPECT_EQ("6.63e-34 J.s", FormatMeasure(6.6260755e-34, " J.s"));
    EXPECT_EQ("2.82 fm", FormatMeasure(2.81794092e-15, " m"));
    EXPECT_EQ("13.8 yJ/K", FormatMeasure(1.380658e-23, " J/K"));
}

TEST(StringNumber, FormatBinaryMeasure)
{
    EXPECT_EQ("1 B/s", FormatBinaryMeasure(1, " B/s"));
    EXPECT_EQ("123 B/s", FormatBinaryMeasure(123, " B/s"));
    EXPECT_EQ("1023 B/s", FormatBinaryMeasure(1023, " B/s"));
    EXPECT_EQ("1.21 KiB/s", FormatBinaryMeasure(1234, " B/s"));
    EXPECT_EQ("12.1 KiB/s", FormatBinaryMeasure(12345, " B/s"));
    EXPECT_EQ("121 KiB/s", FormatBinaryMeasure(123456, " B/s"));
    EXPECT_EQ("1.18 MiB/s", FormatBinaryMeasure(1234567, " B/s"));
    EXPECT_EQ("11.8 MiB/s", FormatBinaryMeasure(12345678, " B/s"));
    EXPECT_EQ("118 MiB/s", FormatBinaryMeasure(123456789, " B/s"));
    EXPECT_EQ("1.15 GiB/s", FormatBinaryMeasure(1234567890, " B/s"));
    EXPECT_EQ("11.5 GiB/s", FormatBinaryMeasure(12345678900ULL, " B/s"));
    EXPECT_EQ("115 GiB/s", FormatBinaryMeasure(123456789000ULL, " B/s"));
    EXPECT_EQ("1.12 TiB/s", FormatBinaryMeasure(1234567890000ULL, " B/s"));
}

TEST(StringNumber, MeasureUnderflow)
{
    EXPECT_EQ("1y", FormatMeasure(1e-24));
    EXPECT_EQ("1e-25", FormatMeasure(1e-25));
    EXPECT_EQ("1e-100", FormatMeasure(1e-100));
}

TEST(StringNumber, MeasureOverflow)
{
    EXPECT_EQ("1Y", FormatMeasure(1e24));
    EXPECT_EQ("100Y", FormatMeasure(1e26));
    EXPECT_EQ("1e+27", FormatMeasure(1e27));
    EXPECT_EQ("1e+100", FormatMeasure(1e100));
}

TEST(StringNumber, MeasureNanAndInf)
{
    EXPECT_EQ("inf", FormatMeasure(INFINITY));
    EXPECT_EQ("inf US$", FormatMeasure(INFINITY, " US$"));
    EXPECT_EQ("infUS$", FormatMeasure(INFINITY, "US$"));
    EXPECT_EQ("nan", FormatMeasure(NAN));
    EXPECT_EQ("nan X", FormatMeasure(NAN, " X"));
}

} // namespace toft
