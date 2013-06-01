// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/encoding/ascii.h"
#include <ctype.h>
#include <limits.h>
#include <locale.h>
#include "thirdparty/gtest/gtest.h"

namespace toft {

class AsciiTest : public testing::Test {
protected:
    static void SetUpTestCase() {
        setlocale(LC_ALL, "C");
    }
};

#define ASCII_TEST_CTYPE_FUNCTION_EQUIVALENCE(type, New, std) \
TEST_F(AsciiTest, New) \
{ \
    for (int c = 0; c <= UCHAR_MAX; ++c) \
        EXPECT_EQ(static_cast<type>(std(c)), Ascii::New(c)) \
            << c << "(" << (isprint(c) ? static_cast<char>(c): ' ') << ")"; \
}

ASCII_TEST_CTYPE_FUNCTION_EQUIVALENCE(bool, IsLower, islower)
ASCII_TEST_CTYPE_FUNCTION_EQUIVALENCE(bool, IsUpper, isupper)
ASCII_TEST_CTYPE_FUNCTION_EQUIVALENCE(bool, IsAlpha, isalpha)
ASCII_TEST_CTYPE_FUNCTION_EQUIVALENCE(bool, IsDigit, isdigit)
ASCII_TEST_CTYPE_FUNCTION_EQUIVALENCE(bool, IsAlphaNumber, isalnum)
ASCII_TEST_CTYPE_FUNCTION_EQUIVALENCE(bool, IsHexDigit, isxdigit)

#ifdef __GNUC__ // windows has no function named 'isblank'
ASCII_TEST_CTYPE_FUNCTION_EQUIVALENCE(bool, IsBlank, isblank)
#endif

ASCII_TEST_CTYPE_FUNCTION_EQUIVALENCE(bool, IsSpace, isspace)
ASCII_TEST_CTYPE_FUNCTION_EQUIVALENCE(bool, IsPunct, ispunct)
ASCII_TEST_CTYPE_FUNCTION_EQUIVALENCE(bool, IsControl, iscntrl)
ASCII_TEST_CTYPE_FUNCTION_EQUIVALENCE(bool, IsGraph, isgraph)
ASCII_TEST_CTYPE_FUNCTION_EQUIVALENCE(bool, IsPrint, isprint)
ASCII_TEST_CTYPE_FUNCTION_EQUIVALENCE(char, ToUpper, toupper)
ASCII_TEST_CTYPE_FUNCTION_EQUIVALENCE(char, ToLower, tolower)
ASCII_TEST_CTYPE_FUNCTION_EQUIVALENCE(char, ToAscii, toascii)

} // namespace toft
