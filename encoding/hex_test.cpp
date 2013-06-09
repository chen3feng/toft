// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/base/array_size.h"
#include "toft/encoding/hex.h"
#include "thirdparty/gtest/gtest.h"

namespace toft {

const char g_data[] = { '\x12', '\x34', '\x56', '\xFF' };

TEST(HexEncoding, Generic)
{
    char result[2 * TOFT_ARRAY_SIZE(g_data) + 1] = {};

    char end_char = result[2 * TOFT_ARRAY_SIZE(g_data)];
    Hex::Encode(g_data, g_data + TOFT_ARRAY_SIZE(g_data), result, true);
    EXPECT_EQ(end_char, result[2 * TOFT_ARRAY_SIZE(g_data)]);

    result[2 * TOFT_ARRAY_SIZE(g_data)] = '\0';
    EXPECT_STREQ("123456FF", result);
}

static bool IsEqual(const std::string& str, const std::vector<char>& v)
{
    return str.size() == v.size() &&
        std::equal(str.begin(), str.end(), v.begin());
}

TEST(HexEncoding, Buffer)
{
    char result[2 * TOFT_ARRAY_SIZE(g_data) + 1];
    EXPECT_STREQ("123456FF", Hex::EncodeToBuffer(g_data, TOFT_ARRAY_SIZE(g_data),
                                                 result, true));
}

TEST(HexEncoding, HexEncodeAppend)
{
    std::string result = "0x";
    EXPECT_EQ("0x123456FF", Hex::EncodeAppend(g_data, TOFT_ARRAY_SIZE(g_data),
                                              &result, true));

    std::vector<char> vector_result;
    vector_result.push_back('0');
    vector_result.push_back('x');
    Hex::EncodeAppend(g_data, TOFT_ARRAY_SIZE(g_data), &vector_result, true);
    EXPECT_TRUE(IsEqual(result, vector_result));
}

TEST(HexEncoding, HexEncodeTo)
{
    std::string result = "0x";
    EXPECT_EQ("123456FF", Hex::EncodeTo(g_data, TOFT_ARRAY_SIZE(g_data),
                                        &result, true));

    std::vector<char> vector_result;
    Hex::EncodeTo(g_data, TOFT_ARRAY_SIZE(g_data), &vector_result, true);
    EXPECT_TRUE(IsEqual(result, vector_result));
}

TEST(HexEncoding, Case)
{
    EXPECT_EQ("123456ff", Hex::EncodeAsString(g_data, TOFT_ARRAY_SIZE(g_data), false));
    EXPECT_EQ("123456FF", Hex::EncodeAsString(g_data, TOFT_ARRAY_SIZE(g_data), true));
}

} // namespace toft
