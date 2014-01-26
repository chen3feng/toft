// Copyright (c) 2012, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 03/05/12
// Description:

#include "toft/net/http/request.h"
#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(HttpMessage, ParseRequest)
{
    static const char request_text[] =
        "GET / HTTP/1.1\r\n"
        "Accept-Language: zh-cn,zh-hk,zh-tw,en-us\r\n"
        "User-Agent: Sosospider+(+http://help.soso.com/webspider.htm)\r\n"
        "Accept-Encoding: gzip\r\n"
        "Connection: Keep-Alive\r\n"
        "Host: 10.6.222.133\r\n"
        "\r\n";
    HttpRequest request;
    ASSERT_EQ(sizeof(request_text) - 1, request.ParseHeaders(request_text));
    ASSERT_EQ("GET / HTTP/1.1", request.StartLine());
}

TEST(HttpRequest, Swap)
{
    HttpRequest req1;
    req1.SetVersion(1, 0);
    req1.SetHeader("A", "1");
    req1.SetBody("hello");

    HttpRequest req2;
    req2.SetVersion(1, 1);
    req2.SetHeader("B", "1");
    req2.SetBody("world");

    std::swap(req1, req2);

    EXPECT_EQ(HttpVersion(1, 1), req1.Version());
    EXPECT_FALSE(req1.HasHeader("A"));
    EXPECT_TRUE(req1.HasHeader("B"));
    EXPECT_EQ("world", req1.Body());

    EXPECT_EQ(HttpVersion(1, 0), req2.Version());
    EXPECT_FALSE(req2.HasHeader("B"));
    EXPECT_TRUE(req2.HasHeader("A"));
    EXPECT_EQ("hello", req2.Body());
}

TEST(HttpRequest, HeadersToString)
{
    HttpRequest request;
    request.SetMethod(HttpRequest::METHOD_GET);
    request.SetUri("/index.html");
    request.SetHeader("A", "1");
    std::string headers = request.HeadersToString();
    EXPECT_EQ("GET /index.html HTTP/1.1\r\nA: 1\r\n\r\n", headers);
}

} // namespace toft

