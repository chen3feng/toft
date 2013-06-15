// Copyright (c) 2013, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-02-28

#include "toft/base/type_traits.h"

#include "thirdparty/gtest/gtest.h"

TEST(TypeTraits, IsVoid)
{
    EXPECT_TRUE(std::is_void<void>::value);
    EXPECT_FALSE(std::is_void<int>::value);
}

TEST(TypeTraits, IsIntgral)
{
    EXPECT_TRUE(std::is_integral<char>::value);
    EXPECT_TRUE(std::is_integral<unsigned char>::value);
    EXPECT_FALSE(std::is_integral<float>::value);
}

TEST(TypeTraits, IsArithmetic)
{
    EXPECT_TRUE(std::is_arithmetic<char>::value);
    EXPECT_TRUE(std::is_arithmetic<signed char>::value);
    EXPECT_TRUE(std::is_arithmetic<float>::value);
    EXPECT_FALSE(std::is_arithmetic<void*>::value);
}

TEST(TypeTraits, IsSigned)
{
    EXPECT_TRUE(std::is_signed<char>::value);
    EXPECT_TRUE(std::is_signed<int>::value);
    EXPECT_FALSE(std::is_signed<unsigned int>::value);
}

TEST(TypeTraits, MakeSigned)
{
    EXPECT_TRUE((std::is_same<std::make_signed<char>::type, signed char>::value));
    EXPECT_TRUE((std::is_same<std::make_signed<int>::type, int>::value));
    EXPECT_TRUE((std::is_same<std::make_signed<unsigned int>::type, int>::value));
}

TEST(TypeTraits, MakeUnsigned)
{
    EXPECT_TRUE((std::is_same<std::make_unsigned<char>::type, unsigned char>::value));
    EXPECT_TRUE((std::is_same<std::make_unsigned<int>::type, unsigned int>::value));
}

