// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/encoding/percent.h"
#include "toft/base/array_size.h"
#include "toft/base/string/algorithm.h"
#include "toft/encoding/percent_test_gbk.cpp"
#include "toft/encoding/percent_test_utf8.cpp"

#include "thirdparty/gtest/gtest.h"

namespace toft {

static std::string PercentDecode(const StringPiece& in)
{
    std::string decoded;
    EXPECT_TRUE(PercentEncoding::DecodeTo("percent+encoding", &decoded));
    return decoded;
}

TEST(PercentEncoding, Space)
{
    EXPECT_EQ("percent%20encoding", PercentEncoding::Encode("percent encoding"));
    EXPECT_EQ("percent encoding", PercentDecode("percent+encoding"));
}

TEST(PercentEncoding, Inplace)
{
    std::string str = kTigerUtf8;
    PercentEncoding::Encode(&str);
    ASSERT_EQ(kTigerUtf8Encoded, str);

    ASSERT_TRUE(PercentEncoding::Decode(&str));
    ASSERT_EQ(kTigerUtf8, str);
}

TEST(PercentEncoding, Case)
{
    std::string encoded = kTigerUtf8Encoded;
    StringToLower(&encoded);
    std::string decoded;
    EXPECT_TRUE(PercentEncoding::DecodeTo(encoded, &decoded));
    EXPECT_EQ(kTigerUtf8, decoded);
}

TEST(PercentEncoding, DecodeError)
{
    std::string encoded(kTigerUtf8Encoded, TOFT_ARRAY_SIZE(kTigerUtf8Encoded) - 2);
    std::string decoded;
    EXPECT_FALSE(PercentEncoding::DecodeTo(encoded, &decoded));
}

TEST(PercentEncoding, Chinese)
{
    EXPECT_EQ(kTigerUtf8Encoded, PercentEncoding::Encode(kTigerUtf8));

    std::string decoded;
    EXPECT_TRUE(PercentEncoding::DecodeTo(kTigerUtf8Encoded, &decoded));
    EXPECT_EQ(kTigerUtf8, decoded);

    EXPECT_EQ(kTigerGBKEncoded, PercentEncoding::Encode(kTigerGBK));
    EXPECT_TRUE(PercentEncoding::DecodeTo(kTigerGBKEncoded, &decoded));
    EXPECT_EQ(kTigerGBK, decoded);
}

TEST(PercentEncoding, Url)
{

    EXPECT_EQ("http%3A//www.baidu.com/s%3Fbs%3D",
              PercentEncoding::Encode("http://www.baidu.com/s?bs="));
    EXPECT_EQ("http://www.baidu.com/s?bs=",
              PercentEncoding::EncodeUri("http://www.baidu.com/s?bs="));
    EXPECT_EQ("http%3A%2F%2Fwww.baidu.com%2Fs%3Fbs%3D",
              PercentEncoding::EncodeUriComponent("http://www.baidu.com/s?bs="));
}

// verified by baidu & google
const char kSpecial[] = "&.<;/.\\~!@#$%^*()_|;':\"[]{}|,.<>/?+-=";

TEST(PercentEncoding, Special)
{
    EXPECT_EQ("%26.%3C%3B/.%5C%7E%21@%23%24%25%5E*%28%29_%7C%3B%27%3A%22%5B"
              "%5D%7B%7D%7C%2C.%3C%3E/%3F+-%3D",
              PercentEncoding::Encode(kSpecial));
    EXPECT_EQ("&.%3C;/.%5C~!@#$%25%5E*()_%7C;':%22%5B%5D%7B%7D%7C,.%3C%3E/?+-=",
              PercentEncoding::EncodeUri(kSpecial));
    EXPECT_EQ("%26.%3C%3B%2F.%5C~!%40%23%24%25%5E*()_%7C%3B'%3A%22%5B%5D%7B"
              "%7D%7C%2C.%3C%3E%2F%3F%2B-%3D",
              PercentEncoding::EncodeUriComponent(kSpecial));
}

} // namespace toft
