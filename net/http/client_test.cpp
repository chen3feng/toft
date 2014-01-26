// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: DongPing HUANG <hdping99@gmail.com>
// Created: 11/05/11
// Description:

#include "toft/net/http/client.h"
#include <iostream>
#include "thirdparty/gtest/gtest.h"

namespace toft {

class HttpClientTest : public testing::Test
{
protected:
    HttpClientTest() : m_server_address("http://www.qq.com/")
    {
    }
    std::string m_server_address;
};

TEST_F(HttpClientTest, GetWithBadURI)
{
    HttpClient client;
    HttpResponse response;
    HttpClient::ErrorCode error;
    std::string bad_uri = "http://-www.qq.com";
    EXPECT_FALSE(client.Get(bad_uri, &response, &error));
    EXPECT_EQ(HttpClient::ERROR_INVALID_URI_ADDRESS, error);
}

TEST_F(HttpClientTest, GetWithBadProxy)
{
    HttpClient client;
    HttpResponse response;
    HttpClient::ErrorCode error;
    client.SetProxy("http://-proxy.tencent.com:8080");
    EXPECT_FALSE(client.Get(m_server_address + "/hello.txt", &response, &error));
    EXPECT_EQ(HttpClient::ERROR_INVALID_PROXY_ADDRESS, error);
}

TEST_F(HttpClientTest, GetWithoutProxy)
{
    HttpClient client;
    HttpResponse response;
    HttpClient::Options options;
    options.AddHeader("Content-Type", "text/plain");
    client.SetUserAgent("TestDownloader");
    HttpClient::ErrorCode error;
    EXPECT_TRUE(client.Get(m_server_address + "/robots.txt", &response, &error));
    EXPECT_EQ(HttpClient::SUCCESS, error);
    EXPECT_EQ(HttpResponse::Status_OK, response.Status());
    EXPECT_FALSE(response.Body().empty());

    EXPECT_FALSE(client.Get(m_server_address + "/world", &response, &error));
    EXPECT_EQ(HttpClient::ERROR_HTTP_STATUS_CODE, error);
    EXPECT_EQ(HttpResponse::Status_NotFound, response.Status());

    EXPECT_FALSE(client.Get("http://127.0.0.1:8/world", &response, &error));
    EXPECT_EQ(HttpClient::ERROR_FAIL_TO_CONNECT_SERVER, error);
    EXPECT_STREQ("Failed to connect to server", client.GetErrorMessage(error));

    // FIXME(chen3feng) : On some env, below cases will fail.
    //  EXPECT_FALSE(client.Get("http://non-exist-domaon.test/world", &response, &error));
    //  EXPECT_EQ(HttpClient::ERROR_FAIL_TO_RESOLVE_ADDRESS, error);
    //  EXPECT_STREQ("Failed to resolve address", client.GetErrorMessage(error));

    // not supported scheme
    EXPECT_FALSE(client.Get("ftp://127.0.0.1/hello.txt", &response, &error));

    // Receive with small response length
    HttpClient client2;
    options.SetMaxResponseLength(3);
    EXPECT_FALSE(client2.Get(m_server_address + "/hello.txt", options, &response, &error));
    // room is not enough to store a complete http response header
    EXPECT_NE(error, HttpClient::SUCCESS);
}

TEST_F(HttpClientTest, Post)
{
    HttpClient client;
    HttpResponse response;
    HttpClient::Options options;
    HttpClient::ErrorCode error;
    std::string data = "Post Content";
    EXPECT_FALSE(client.Post(m_server_address + "/aaa/bbb",
                             data,
                             options,
                             &response,
                             &error));
    EXPECT_NE(error, HttpClient::SUCCESS);
    EXPECT_EQ(HttpResponse::Status_NotFound, response.Status());
}

TEST_F(HttpClientTest, Put)
{
    HttpClient client;
    HttpResponse response;
    HttpClient::Options options;
    HttpClient::ErrorCode error;
    std::string data = "Put Content";
    EXPECT_TRUE(client.Put(m_server_address + "/Put",
                           data,
                           options,
                           &response,
                           &error));
    EXPECT_EQ(HttpClient::SUCCESS, error);
    // www.qq.com always return 200.
    EXPECT_EQ(HttpResponse::Status_OK, response.Status());
}

TEST_F(HttpClientTest, Delete)
{
    HttpClient client;
    HttpResponse response;
    HttpClient::Options options;
    HttpClient::ErrorCode error;
    EXPECT_TRUE(client.Delete(m_server_address + "/Delete",
                              options,
                              &response,
                              &error));
    EXPECT_EQ(HttpClient::SUCCESS, error);
    EXPECT_EQ(HttpResponse::Status_OK, response.Status());
}

} // namespace toft

