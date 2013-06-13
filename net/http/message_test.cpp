// Copyright 2011, The Toft Authors.
// Xiaokang Liu <liuxk02@gmail.com>

#include "toft/net/http/message.h"

#include <algorithm>

#include "toft/net/http/request.h"
#include "toft/net/http/response.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(HttpMessage, HasHeader)
{
    HttpRequest req;
    EXPECT_FALSE(req.HasHeader("Test"));
    EXPECT_FALSE(req.RemoveHeader("Test"));
    req.AddHeader("Test", "1");
    EXPECT_TRUE(req.HasHeader("Test"));
    EXPECT_TRUE(req.RemoveHeader("Test"));
    EXPECT_FALSE(req.HasHeader("Test"));
}

TEST(HttpMessage, AddHeader)
{
    HttpRequest req;
    EXPECT_FALSE(req.HasHeader("Test"));
    req.AddHeader("Test", "1");
    EXPECT_TRUE(req.HasHeader("Test"));
}

TEST(HttpMessage, RemoveHeader)
{
    HttpRequest req;
    EXPECT_FALSE(req.HasHeader("Test"));
    EXPECT_FALSE(req.RemoveHeader("Test"));
    req.AddHeader("Test", "1");
    EXPECT_TRUE(req.HasHeader("Test"));
    EXPECT_TRUE(req.RemoveHeader("Test"));
    EXPECT_FALSE(req.HasHeader("Test"));
}

TEST(HttpMessage, ToString)
{
    HttpRequest request;
    request.SetMethod(HttpRequest::METHOD_GET);
    request.SetHeader("Hello", "World");
    request.SetBody("Kitty");
    std::string s;
    request.ToString(&s);
    EXPECT_EQ("GET / HTTP/1.1\r\nHello: World\r\n\r\nKitty", s);
}

TEST(HttpMessage, HeadersToString)
{
    HttpRequest request;
    request.SetMethod(HttpRequest::METHOD_GET);
    request.SetHeader("Hello", "World");
    request.SetBody("Kitty");
    std::string s;
    request.HeadersToString(&s);
    EXPECT_EQ("GET / HTTP/1.1\r\nHello: World\r\n\r\n", s);
}

TEST(HttpMessage, GetHeader)
{
    HttpRequest request;
    request.SetHeader("Hello", "World");
    std::string s;
    EXPECT_TRUE(request.GetHeader("Hello", &s));
    EXPECT_EQ("World", s);
    EXPECT_FALSE(request.GetHeader("Kitty", &s));
}

TEST(HttpMessage, SetHeaders)
{
    HttpRequest request;
    HttpHeaders headers;
    headers.Add("Hello", "World");
    headers.Add("Accept", "text/html");
    request.SetHeaders(headers);
    std::string s;
    EXPECT_TRUE(request.GetHeader("Hello", &s));
    EXPECT_EQ("World", s);
    EXPECT_TRUE(request.GetHeader("Accept", &s));
    EXPECT_EQ("text/html", s);
}

TEST(HttpMessage, GetMethodName)
{
    EXPECT_STREQ("HEAD", HttpRequest::GetMethodName(HttpRequest::METHOD_HEAD));
    EXPECT_STREQ("GET", HttpRequest::GetMethodName(HttpRequest::METHOD_GET));
    EXPECT_STREQ("POST", HttpRequest::GetMethodName(HttpRequest::METHOD_POST));
    EXPECT_STREQ("DELETE", HttpRequest::GetMethodName(HttpRequest::METHOD_DELETE));
    EXPECT_STREQ("OPTIONS", HttpRequest::GetMethodName(HttpRequest::METHOD_OPTIONS));
    EXPECT_STREQ("TRACE", HttpRequest::GetMethodName(HttpRequest::METHOD_TRACE));
    EXPECT_STREQ("CONNECT", HttpRequest::GetMethodName(HttpRequest::METHOD_CONNECT));
    EXPECT_TRUE(HttpRequest::GetMethodName(HttpRequest::METHOD_UNKNOWN) == NULL);
}

TEST(HttpMessage, GetMethodByName)
{
    EXPECT_EQ(HttpRequest::METHOD_HEAD, HttpRequest::GetMethodByName("HEAD"));
    EXPECT_EQ(HttpRequest::METHOD_GET, HttpRequest::GetMethodByName("GET"));
    EXPECT_EQ(HttpRequest::METHOD_POST, HttpRequest::GetMethodByName("POST"));
    EXPECT_EQ(HttpRequest::METHOD_DELETE, HttpRequest::GetMethodByName("DELETE"));
    EXPECT_EQ(HttpRequest::METHOD_OPTIONS, HttpRequest::GetMethodByName("OPTIONS"));
    EXPECT_EQ(HttpRequest::METHOD_TRACE, HttpRequest::GetMethodByName("TRACE"));
    EXPECT_EQ(HttpRequest::METHOD_CONNECT, HttpRequest::GetMethodByName("CONNECT"));
    EXPECT_EQ(HttpRequest::METHOD_UNKNOWN, HttpRequest::GetMethodByName("UNKNOWN"));
}

} // namespace toft
