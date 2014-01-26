// Copyright 2011, The Toft Authors.
// Xiaokang Liu <liuxk02@gmail.com>

#include "toft/net/http/headers.h"
#include "toft/net/http/message.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(HttpHeaders, Parse)
{
    HttpHeaders headers;
    int error;

    EXPECT_EQ(0U, headers.Parse("", &error));
    EXPECT_EQ(HttpMessage::ERROR_MESSAGE_NOT_COMPLETE, error);

    EXPECT_EQ(0U, headers.Parse("Host: 127.0.0.1", &error));
    EXPECT_EQ(HttpMessage::ERROR_MESSAGE_NOT_COMPLETE, error);

    EXPECT_EQ(0U, headers.Parse("Host: 127.0.0.1\r\n", &error));
    EXPECT_EQ(HttpMessage::ERROR_MESSAGE_NOT_COMPLETE, error);

    // Invalid, but accepted by all browsers
    EXPECT_EQ(8U, headers.Parse("Host\r\n\r\n", &error));
    EXPECT_EQ(0U, headers.Count());

    const char* text = "Host: 127.0.0.1\r\n\r\n";
    EXPECT_EQ(strlen(text), headers.Parse(text, &error));
    EXPECT_EQ(HttpMessage::SUCCESS, error);
    EXPECT_EQ(1U, headers.Count());

    text = "Host: 127.0.0.1\r\nContent-Type: text/xml\r\n\r\n";
    EXPECT_EQ(strlen(text), headers.Parse(text, &error));
    EXPECT_EQ(HttpMessage::SUCCESS, error);
    EXPECT_EQ(2U, headers.Count());

    EXPECT_EQ(1U, headers.Parse("\n"));
    EXPECT_EQ(2U, headers.Parse("\r\n"));

    EXPECT_EQ(2U, headers.Parse("\r\n\r\n"));
    EXPECT_EQ(HttpMessage::SUCCESS, error);
}

TEST(HttpHeaders, Count)
{
    HttpHeaders headers;

    EXPECT_EQ(0U, headers.Count());

    headers.Add("Hello", "World");
    EXPECT_EQ(1U, headers.Count());

    headers.Add("Great", "Wall");
    EXPECT_EQ(2U, headers.Count());

    std::pair<std::string, std::string> header;

    EXPECT_FALSE(headers.GetAt(-1, &header));
    EXPECT_FALSE(headers.GetAt(2, &header));

    EXPECT_TRUE(headers.GetAt(0, &header));
    EXPECT_EQ("Hello", header.first);
    EXPECT_EQ("World", header.second);

    EXPECT_TRUE(headers.GetAt(1, &header));
    EXPECT_EQ("Great", header.first);
    EXPECT_EQ("Wall", header.second);
}

TEST(HttpHeaders, ToString)
{
    HttpHeaders headers;
    EXPECT_EQ("", headers.ToString());

    headers.Add("Hello", "World");
    EXPECT_EQ("Hello: World\r\n", headers.ToString());

    headers.Add("Great", "Wall");
    EXPECT_EQ("Hello: World\r\nGreat: Wall\r\n", headers.ToString());
}

TEST(HttpHeaders, Get)
{
    HttpHeaders headers;
    std::string value;
    std::vector<std::string> header_values;

    EXPECT_FALSE(headers.Get("Hello", &value));
    headers.Add("Hello", "World");
    EXPECT_TRUE(headers.Get("Hello", &value));
    EXPECT_EQ("World", value);

    headers.Add("Hello", "China");
    EXPECT_TRUE(headers.Get("Hello", &header_values));
    EXPECT_EQ(2U, header_values.size());
    EXPECT_EQ("World", header_values[0]);
    EXPECT_EQ("China", header_values[1]);
}

} // namespace toft
