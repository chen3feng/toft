// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 12/14/11
// Description: test for preprocess.h

#include "toft/base/preprocess.h"
#include "toft/base/preprocess_test_helper.h"

#include "thirdparty/gtest/gtest.h"

TEST(Preprocess, Stringize)
{
    EXPECT_STREQ("ABC", TOFT_PP_STRINGIZE(ABC));
}

TEST(Preprocess, Join)
{
    EXPECT_EQ(12, TOFT_PP_JOIN(1, 2));
}

TEST(Preprocess, DisallowInHeader)
{
    TOFT_PP_DISALLOW_IN_HEADER_FILE();
}

TEST(Preprocess, VaNargs)
{
    EXPECT_EQ(0, TOFT_PP_N_ARGS());
    EXPECT_EQ(1, TOFT_PP_N_ARGS(a));
    EXPECT_EQ(2, TOFT_PP_N_ARGS(a, b));
    EXPECT_EQ(3, TOFT_PP_N_ARGS(a, b, c));
    EXPECT_EQ(4, TOFT_PP_N_ARGS(a, b, c, d));
    EXPECT_EQ(5, TOFT_PP_N_ARGS(a, b, c, d, e));
    EXPECT_EQ(6, TOFT_PP_N_ARGS(a, b, c, d, e, f));
    EXPECT_EQ(7, TOFT_PP_N_ARGS(a, b, c, d, e, f, g));
    EXPECT_EQ(8, TOFT_PP_N_ARGS(a, b, c, d, e, f, g, h));
    EXPECT_EQ(9, TOFT_PP_N_ARGS(a, b, c, d, e, f, g, h, i));
    EXPECT_EQ(10, TOFT_PP_N_ARGS(a, b, c, d, e, f, g, h, i, j));
    EXPECT_EQ(11, TOFT_PP_N_ARGS(a, b, c, d, e, f, g, h, i, j, k));
    EXPECT_EQ(12, TOFT_PP_N_ARGS(a, b, c, d, e, f, g, h, i, j, k, l));
    EXPECT_EQ(13, TOFT_PP_N_ARGS(a, b, c, d, e, f, g, h, i, j, k, l, m));
    EXPECT_EQ(14, TOFT_PP_N_ARGS(a, b, c, d, e, f, g, h, i, j, k, l, m, n));
    EXPECT_EQ(15, TOFT_PP_N_ARGS(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o));
}

TEST(Preprocess, Varargs)
{
    EXPECT_EQ("a", TOFT_PP_FOR_EACH_ARGS(TOFT_PP_STRINGIZE, a));
    EXPECT_EQ("ab", TOFT_PP_FOR_EACH_ARGS(TOFT_PP_STRINGIZE, a, b));
    EXPECT_EQ("abc", TOFT_PP_FOR_EACH_ARGS(TOFT_PP_STRINGIZE, a, b, c));
    EXPECT_EQ("abcd", TOFT_PP_FOR_EACH_ARGS(TOFT_PP_STRINGIZE, a, b, c, d));
    EXPECT_EQ("abcde",
              TOFT_PP_FOR_EACH_ARGS(TOFT_PP_STRINGIZE, a, b, c, d, e));
    EXPECT_EQ("abcdef",
              TOFT_PP_FOR_EACH_ARGS(TOFT_PP_STRINGIZE, a, b, c, d, e, f));
    EXPECT_EQ("abcdefg",
              TOFT_PP_FOR_EACH_ARGS(TOFT_PP_STRINGIZE, a, b, c, d, e, f, g));
    EXPECT_EQ("abcdefgh",
              TOFT_PP_FOR_EACH_ARGS(
                  TOFT_PP_STRINGIZE, a, b, c, d, e, f, g, h));
    EXPECT_EQ("abcdefghi",
              TOFT_PP_FOR_EACH_ARGS(
                  TOFT_PP_STRINGIZE, a, b, c, d, e, f, g, h, i));
    EXPECT_EQ("abcdefghij",
              TOFT_PP_FOR_EACH_ARGS(
                  TOFT_PP_STRINGIZE, a, b, c, d, e, f, g, h, i, j));
    EXPECT_EQ("abcdefghijk",
              TOFT_PP_FOR_EACH_ARGS(TOFT_PP_STRINGIZE,
                  a, b, c, d, e, f, g, h, i, j, k));
    EXPECT_EQ("abcdefghijkl",
              TOFT_PP_FOR_EACH_ARGS(
                  TOFT_PP_STRINGIZE, a, b, c, d, e, f, g, h, i, j, k, l));
    EXPECT_EQ("abcdefghijklm",
              TOFT_PP_FOR_EACH_ARGS(
                  TOFT_PP_STRINGIZE, a, b, c, d, e, f, g, h, i, j, k, l, m));
    EXPECT_EQ("abcdefghijklmn",
              TOFT_PP_FOR_EACH_ARGS(
                  TOFT_PP_STRINGIZE,
                  a, b, c, d, e, f, g, h, i, j, k, l, m, n));
    EXPECT_EQ("abcdefghijklmno",
              TOFT_PP_FOR_EACH_ARGS(
                  TOFT_PP_STRINGIZE,
                  a, b, c, d, e, f, g, h, i, j, k, l, m, n, o));
}

#define DEFINE_METHOD(cmd, name) (cmd, name)

#define EXPAND_METHOD_(cmd, name) int name() { return cmd; }
#define EXPAND_METHOD(x) EXPAND_METHOD_ x

#define DEFINE_SERVICE(name, ...) \
    class name { \
    public: \
        TOFT_PP_FOR_EACH_ARGS(EXPAND_METHOD, __VA_ARGS__) \
    };

DEFINE_SERVICE(TestService,
    DEFINE_METHOD(1, Echo),
    DEFINE_METHOD(2, Inc)
)

TEST(Preprocess, VaForEach)
{
    TestService service;
    EXPECT_EQ(1, service.Echo());
    EXPECT_EQ(2, service.Inc());
}
