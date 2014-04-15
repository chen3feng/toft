// Copyright (c) 2012, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 03/05/12
// Description:

#include "toft/net/http/response.h"
#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(HttpMessage, ParseResponse)
{
    HttpResponse response;
    ASSERT_TRUE(response.ParseHeaders("HTTP/1.1 200 OK\r\n\r\n"));
    ASSERT_TRUE(response.ParseHeaders("HTTP/1.0 404 Not Found\r\n\r\n"));

    static const char response_text[] =
        "HTTP/1.1 200 OK\r\n"
        "Date: Mon, 16 Jan 2012 06:41:37 GMT\r\n"
        "Server: Apache/2.2.21 (Unix)\r\n"
        "Last-Modified: Wed, 11 Jan 2012 09:15:13 GMT\r\n"
        "Accept-Ranges: bytes\r\n"
        "Content-Length: 45\r\n"
        "Keep-Alive: timeout=5, max=100\r\n"
        "Connection: Keep-Alive\r\n"
        "Content-Type: text/html\r\n"
        "\r\n";

    EXPECT_EQ(sizeof(response_text) - 1, response.ParseHeaders(response_text));
    EXPECT_EQ(8U, response.Headers().Count());
    EXPECT_EQ(response_text, response.HeadersToString());
}

TEST(HttpMessage, NonstrictParseResponse)
{
    // This message is not valid for RPC2616, but is accepted by browsers
    // (tested on IE, FF, Chrome), so, we should compatible this case.
    static const char response_text[] =
        "HTTP/1.1 200 OK\r\n"
        "Date: Mon, 16 Jan 2012 07:12:41 GMT\r\n"
        "Server: Microsoft-IIS/6.0\r\n"
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 615\r\n"
        "\r\n";
    HttpResponse response;
    EXPECT_EQ(sizeof(response_text) - 1, response.ParseHeaders(response_text));
    EXPECT_EQ(4U, response.Headers().Count());

    static const char corrected_response_text[] =
        "HTTP/1.1 200 OK\r\n"
        "Date: Mon, 16 Jan 2012 07:12:41 GMT\r\n"
        "Server: Microsoft-IIS/6.0\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 615\r\n"
        "\r\n";
    EXPECT_EQ(corrected_response_text, response.HeadersToString());
}

TEST(HttpMessage, NonstrictParseResponse2)
{
    // http://easy-infonow.com/plasma-tv-lg-42pj350/
    static const char response_text[] =
        "HTTP/1.1 508 unused\r\n"
        "Date: Mon, 16 Jan 2012 08:47:48 GMT\r\n"
        "Server: Apache/2.2.21 (Unix) mod_ssl/2.2.21 OpenSSL/0.9.8e-fips-rhel5\r\n"
        "Content-Type: text/html\r\n"
        "\r\n";
    HttpResponse response;
    EXPECT_EQ(sizeof(response_text) - 1, response.ParseHeaders(response_text));
}

TEST(HttpResponse, ReasonPhrase)
{
    EXPECT_STREQ("OK",
                 HttpResponse::StatusCodeToReasonPhrase(HttpResponse::Status_OK));
    EXPECT_STREQ("Not Found", HttpResponse::StatusCodeToReasonPhrase(
            HttpResponse::Status_NotFound));
    EXPECT_STREQ(NULL, HttpResponse::StatusCodeToReasonPhrase(
            static_cast<HttpResponse::StatusCode>(1000)));
}

TEST(HttpResponse, Swap)
{
    HttpResponse req1;
    req1.SetVersion(1, 0);
    req1.SetHeader("A", "1");
    req1.SetBody("hello");

    HttpResponse req2;
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

TEST(HttpResponse, FillWithHtmlPage)
{
    HttpResponse req;
    req.FillWithHtmlPage(HttpResponse::Status_NotFound);
    static const char expected[] =
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "<html>\n"
        "<head>\n"
        "</title>HTTP 404 Not Found</title>\n"
        "</head>\n"
        "<body>Nothing matches the given URI</body>\n"
        "</html>\n";
    EXPECT_EQ(expected, req.ToString());
}

TEST(HttpResponse, GetContentLength)
{
    HttpResponse response;
    EXPECT_EQ(-1, response.GetContentLength());
    response.AddHeader("Content-Length", "100");
    EXPECT_EQ(100, response.GetContentLength());
    response.SetHeader("Content-Length", "-100");
    EXPECT_EQ(-1, response.GetContentLength());
}

TEST(HttpResponse, IsKeepAlive)
{
    HttpResponse response;
    response.SetVersion(1, 0);
    EXPECT_FALSE(response.IsKeepAlive());
    response.SetVersion(0, 9);
    EXPECT_FALSE(response.IsKeepAlive());
    response.SetVersion(HttpVersion());
    EXPECT_FALSE(response.IsKeepAlive());

    response.SetVersion(1, 1);
    EXPECT_TRUE(response.IsKeepAlive());

    response.AddHeader("Connection", "closed");
    EXPECT_FALSE(response.IsKeepAlive());

    response.SetHeader("Connection", "keep-alive");
    EXPECT_TRUE(response.IsKeepAlive());
}

TEST(HttpResponse, HeadersToString)
{
    HttpResponse response;
    response.SetStatus(HttpResponse::Status_OK);
    response.AddHeader("Content-Length", "100");
    response.AddHeader("Connection", "keep-alive");
    std::string headers = response.HeadersToString();
    EXPECT_EQ("HTTP/1.1 200 OK\r\nContent-Length: 100\r\nConnection: keep-alive\r\n\r\n", headers);
}

} // namespace toft

