// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 04/30/11
// Description: test string concat

#include "toft/base/string/concat.h"
#include "toft/base/string/format.h"

#include "thirdparty/gtest/gtest.h"
#include "thirdparty/perftools/profiler.h"
#include "thirdparty/protobuf/text_format.h"

namespace toft {

namespace {

enum NamedEnum {
    N_FIRST,
    N_SECOND
};

// Anonymous enum
enum {
    A_FIRST,
    A_SECOND
};

} // namespace

TEST(StringConcat, Enum)
{
    // local enum
    enum {
        L_FIRST,
        L_SECOND
    };

    std::string s;
    StringAppend(&s, N_FIRST);
    EXPECT_EQ("0", s);
    StringAppend(&s, A_FIRST);
    EXPECT_EQ("00", s);
    StringAppend(&s, L_FIRST);
    EXPECT_EQ("000", s);
    EXPECT_EQ("0", StringConcat(N_FIRST));

    // The following code can't compile because anonymous and local enums
    // has no linkage
#if 0
    EXPECT_EQ("0", StringConcat(A_FIRST));
    EXPECT_EQ("0", StringConcat(L_FIRST));
#endif
}

TEST(StringConcat, Append)
{
    std::string s = "hello";
    StringAppend(&s, "world");
    EXPECT_EQ("helloworld", s);
}

TEST(StringConcat, Concat)
{
    EXPECT_EQ("12580", StringConcat(12580));
    EXPECT_EQ("helloworld", StringConcat("hello", "world"));
    EXPECT_EQ("hello,world", StringConcat("hello", ",", "world"));
    EXPECT_EQ("xxx1024", StringConcat("xxx", 1024));
    EXPECT_EQ("xxx,1024", StringConcat("xxx", ",", 1024));
    EXPECT_EQ("xxx1024-1024", StringConcat("xxx", 1024, -1024));
    EXPECT_EQ("xxx1", StringConcat("xxx", static_cast<uint16_t>(1)));
    EXPECT_EQ("012345678910ABCDEFGHIJKLMNOPQRSTU",
              StringConcat(static_cast<short>(0), static_cast<unsigned short>(1),
                           2, 3U, 4L, 5ULL, 6LL, 7ULL, 8.0f, 9.0, 10.0L,
                           "A", "B", "C", "D", "E", 'F', 'G', 'H', 'I', 'J',
                           'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
                           'U'));
}

TEST(StringConcat, ConcatTo)
{
    std::string s = "hello";
    StringConcatTo(&s, "world");
    EXPECT_EQ("world", s);
    StringConcatTo(&s, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                   "A", "B", "C", "D", "E", 'F', 'G', 'H', 'I', 'J',
                   'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
                   'U', 'V');
    EXPECT_EQ("0123456789ABCDEFGHIJKLMNOPQRSTUV", s);
}

///////////////////////////////////////////////////////////////////////////////
// ExtensionTest

class StringConcatExtensionTest : public testing::Test {};

struct SupportOss
{
    int a;
    int b;
};

struct CustomType
{
    int a;
    long b;
};

template <typename A, typename B>
struct GenericType
{
    A a;
    B b;
};

std::ostream& operator<<(std::ostream& oss, const SupportOss& o)
{
    return oss << "(" << o.a << ", " << o.b << ")";
}

void StringAppend(std::string* str, const CustomType& o)
{
    StringAppend(str, "(", o.a, ", ", o.b, ")");
}

template <typename A, typename B>
void StringAppend(std::string* str, const GenericType<A, B>& o)
{
    StringAppend(str, "(", o.a, ", ", o.b, ")");
}

TEST_F(StringConcatExtensionTest, SupportOss)
{
    SupportOss o = {1, 2};
    EXPECT_EQ("(1, 2)", StringConcat(o));
}

TEST_F(StringConcatExtensionTest, CustomType)
{
    CustomType o = {3, 4};
    EXPECT_EQ("(3, 4)", StringConcat(o));
}

TEST_F(StringConcatExtensionTest, GenericType)
{
    GenericType<int, long> o = {3, 4};
    EXPECT_EQ("(3, 4)", StringConcat(o));
}

///////////////////////////////////////////////////////////////////////////////
// PerformanceTest

class PerformanceTest : public testing::Test {};

const int kTestLoopCount = 500000;

TEST_F(PerformanceTest, Concat)
{
    std::string s;
    for (int i = 0; i < kTestLoopCount; ++i)
        s = StringConcat(i, i, i, i, i, i, i, i, i, i);
}

TEST_F(PerformanceTest, ConcatTo)
{
    std::string s;
    for (int i = 0; i < kTestLoopCount; ++i)
        StringConcatTo(&s, i, i, i, i, i, i, i, i, i, i);
}

TEST_F(PerformanceTest, Print)
{
    std::string s;
    for (int i = 0; i < kTestLoopCount; ++i)
        StringPrintTo(&s, "%d%d%d%d%d%d%d%d%d%d", i, i, i, i, i, i, i, i, i, i);
}

TEST_F(PerformanceTest, StringStream)
{
    std::string s;
    for (int i = 0; i < kTestLoopCount; ++i)
    {
        std::ostringstream oss;
        oss << i << i << i << i << i << i << i << i << i << i;
        s = oss.str();
    }
}

TEST_F(PerformanceTest, StringConcatTo)
{
    std::string result;
    for (int i = 0; i < kTestLoopCount; ++i)
    {
        StringConcatTo(&result,
                       "hello", ",", "world",
                       100000000,
                       200000000,
                       300000000,
                       400000000,
                       500000000,
                       "\xFF\x95\x27",
                       "ABCD",
                       "ABCD",
                       1, 2, 3);
    }
}

} // namespace toft
