// Copyright (c) 2011, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 04/30/11
// Description: test string format functions

// GLOBAL_NOLINT(runtime/int)

#include "toft/base/string/format/print.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(StringPrint, NoARg)
{
    EXPECT_EQ("hello", StringPrint("hello"));
}

TEST(StringPrint, Escape)
{
    EXPECT_EQ("1%2%", StringPrint("1%%2%%"));
}

TEST(StringPrint, Bool)
{
    EXPECT_EQ("true, false", StringPrint("%v, %v", true, false));
}

TEST(StringPrint, Char)
{
    EXPECT_EQ("sb", StringPrint("%c%c", 's', 'b'));
    EXPECT_EQ("sb", StringPrint("%v%v", 's', 'b'));
}

TEST(StringPrint, String)
{
    EXPECT_EQ("hello, world", StringPrint("%s, %s", "hello", "world"));
    std::string hello = "hello";
    std::string world = "world";
    EXPECT_EQ("hello, world", StringPrint("%s, %s", hello, world));
    EXPECT_EQ("hello, world", StringPrint("%s, %s",
                                          std::string("hello"),
                                          std::string("world")));
}

TEST(StringPrint, Signed)
{
    signed char sc = 42;
    short s = 9527;
    int i = 95279527;
    long l = 95279527L;
    long long ll = 9527952795279527LL;

    EXPECT_EQ("42", StringPrint("%i", sc));

    EXPECT_EQ("42", StringPrint("%d", sc));
    EXPECT_EQ("9527", StringPrint("%d", s));
    EXPECT_EQ("95279527", StringPrint("%d", i));
    EXPECT_EQ("95279527", StringPrint("%d", l));
    EXPECT_EQ("9527952795279527", StringPrint("%d", ll));

    EXPECT_EQ("42", StringPrint("%hhd", sc));
    EXPECT_EQ("9527", StringPrint("%hd", s));
    EXPECT_EQ("95279527", StringPrint("%d", i));
    EXPECT_EQ("95279527", StringPrint("%ld", l));
    EXPECT_EQ("9527952795279527", StringPrint("%lld", ll));
}

TEST(StringPrint, Unsigned)
{
    unsigned char uc = 142;
    unsigned short us = 9527;
    unsigned int ui = 95279527U;
    unsigned long ul = 95279527UL;
    unsigned long long ull = 9527952795279527ULL;

    EXPECT_EQ("142", StringPrint("%d", uc));
    EXPECT_EQ("142", StringPrint("%i", uc));

    EXPECT_EQ("142", StringPrint("%u", uc));
    EXPECT_EQ("9527", StringPrint("%u", us));
    EXPECT_EQ("95279527", StringPrint("%u", ui));
    EXPECT_EQ("95279527", StringPrint("%u", ul));
    EXPECT_EQ("9527952795279527", StringPrint("%u", ull));

    EXPECT_EQ("142", StringPrint("%hhu", uc));
    EXPECT_EQ("9527", StringPrint("%hu", us));
    EXPECT_EQ("95279527", StringPrint("%u", ui));
    EXPECT_EQ("95279527", StringPrint("%lu", ul));
    EXPECT_EQ("9527952795279527", StringPrint("%llu", ull));
}

TEST(StringPrint, Hex)
{
    EXPECT_EQ("deadbeef", StringPrint("%x", 0xdeadbeef));
    EXPECT_EQ("DEADBEEF", StringPrint("%X", 0xDEADBEEF));
    EXPECT_EQ("64,c8,12c", StringPrint("%x,%x,%x", 100, 200, 300));
    EXPECT_EQ("64,C8,12C", StringPrint("%X,%X,%X", 100, 200, 300));
}

TEST(StringPrint, Oct)
{
    EXPECT_EQ("370", StringPrint("%o", 0370));
    EXPECT_EQ("371", StringPrint("%o", 0371));
    EXPECT_EQ("144,310,454", StringPrint("%o,%o,%o", 100, 200, 300));
}

TEST(StringPrint, Float)
{
    EXPECT_EQ("3.140000", StringPrint("%f", 3.14));
    EXPECT_EQ("3.14", StringPrint("%g", 3.14));
    EXPECT_EQ("3.140000e+12", StringPrint("%e", 3.14e12));
    EXPECT_EQ("3.14e+12", StringPrint("%g", 3.14e12));

    EXPECT_EQ("3.140000", StringPrint("%f", 3.14f));
    EXPECT_EQ("3.140000", StringPrint("%lf", 3.14));
    EXPECT_EQ("3.140000", StringPrint("%f", 3.14L));
    EXPECT_EQ("3.140000", StringPrint("%Lf", 3.14L));
}

TEST(StringPrint, Align)
{
    EXPECT_EQ("     100", StringPrint("%8d", 100));
    EXPECT_EQ("100     ", StringPrint("%-8d", 100));
}

TEST(StringPrint, Sign)
{
    EXPECT_EQ("-100", StringPrint("%+d", -100));
    EXPECT_EQ("+100", StringPrint("%+d", 100));
}

TEST(StringPrint, Space)
{
    EXPECT_EQ("-100", StringPrint("% d", -100));
    EXPECT_EQ(" 100", StringPrint("% d", 100));
}

TEST(StringPrint, Sharp)
{
    EXPECT_EQ("0x100", StringPrint("%#x", 0x100));
    EXPECT_EQ("0X100", StringPrint("%#X", 0x100));
    EXPECT_EQ("0370", StringPrint("%#o", 0370));
    EXPECT_EQ("0", StringPrint("%#x", 0));
    EXPECT_EQ("0", StringPrint("%#X", 0));
    EXPECT_EQ("0", StringPrint("%#o", 0));
}

TEST(StringPrint, Zero)
{
    EXPECT_EQ("-100", StringPrint("%0d", -100));
    EXPECT_EQ("100", StringPrint("%02d", 100));
    EXPECT_EQ("100", StringPrint("%03d", 100));
    EXPECT_EQ("00000100", StringPrint("%08d", 100));
    EXPECT_EQ("00000100", StringPrint("%08x", 0x100));
}

TEST(StringPrint, Width)
{
    EXPECT_EQ("0", StringPrint("%0d", 0));
    EXPECT_EQ("100", StringPrint("%2d", 100));
    EXPECT_EQ("100", StringPrint("%3d", 100));
    EXPECT_EQ("  100", StringPrint("%5d", 100));
    EXPECT_EQ("100", StringPrint("%*d", 2, 100));
    EXPECT_EQ("  100", StringPrint("%*d", 5, 100));
}

TEST(StringPrint, IntPrecision)
{
    EXPECT_EQ("", StringPrint("%.0d", 0));
    EXPECT_EQ("100", StringPrint("%.2d", 100));
    EXPECT_EQ("00100", StringPrint("%.5d", 100));
    EXPECT_EQ("100", StringPrint("%.*d", 2, 100));
    EXPECT_EQ("00100", StringPrint("%.*d", 5, 100));
}

TEST(StringPrint, FloatPrecision)
{
    EXPECT_EQ("3.14", StringPrint("%.2f", 3.1415926));
    EXPECT_EQ("3.1416", StringPrint("%.4f", 3.1415926));
    EXPECT_EQ("3.14", StringPrint("%.*f", 2, 3.1415926));
    EXPECT_EQ("3.14", StringPrint("%.3g", 3.1415926));
    EXPECT_EQ("3.14", StringPrint("%.*g", 3, 3.1415926));
}

TEST(StringPrint, StringPrecision)
{
    EXPECT_EQ("hel", StringPrint("%.3s", "hello"));
    EXPECT_EQ("hel", StringPrint("%.*s", 3, "hello"));
    EXPECT_EQ("hello", StringPrint("%.6s", "hello"));
    EXPECT_EQ("hello", StringPrint("%.*s", 6, "hello"));
}

TEST(StringPrint, StringPrintTo)
{
    const unsigned long lu = 99;
    std::string str;
    size_t length = StringPrintTo(&str, "sx%d%s%lu\n", 100, "hehe,", lu);
    EXPECT_EQ("sx100hehe,99\n", str);
    EXPECT_EQ(str.length(), length);
}

TEST(StringPrint, StringPrintAppend)
{
    const unsigned long lu = 99;
    std::string str = "hello";
    size_t org_length = str.length();
    size_t append_length = StringPrintAppend(&str, "sx%d%s%lu\n", 100, "hehe,", lu);
    EXPECT_EQ("hellosx100hehe,99\n", str);
    EXPECT_EQ(str.length() - org_length, append_length);
}

TEST(StringPrint, LongString)
{
    std::string a(1024, 'A');
    std::string b(1024, 'A');
    EXPECT_EQ(a + b, StringPrint("%s%s", a.c_str(), b.c_str()));
}

class PerformanceTest : public testing::Test {
};

const int kLoopCount = 1000000;

TEST_F(PerformanceTest, Print)
{
    std::string s;
    for (int i = 0; i < kLoopCount; ++i) {
        StringPrintTo(&s, "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                      i, i, i, i, i, i, i, i, i, i, i, i, i, i, i);
    }
}

TEST_F(PerformanceTest, Printf)
{
    char buf[1024];
    for (int i = 0; i < kLoopCount; ++i) {
        snprintf(buf, sizeof(buf), "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                    i, i, i, i, i, i, i, i, i, i, i, i, i, i, i);
    }
}

} // namespace toft
