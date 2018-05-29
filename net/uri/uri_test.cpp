// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 05/11/11
// Description: test URI class

#include "toft/net/uri/uri.h"
#include "toft/base/array_size.h"
#include "toft/storage/file/file.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(URI, Parse)
{
    std::string uristr = "http://www.baidu.com/s?tn=monline_dg&bs=DVLOG&f=8&wd=glog+DVLOG#fragment";
    URI uri;
    ASSERT_TRUE(uri.Parse(uristr));
    EXPECT_EQ(uristr, uri.ToString());
    EXPECT_EQ("http", uri.Scheme());

    ASSERT_EQ("/s", uri.Path());
    ASSERT_EQ("www.baidu.com", uri.Host());
    ASSERT_FALSE(uri.HasPort());

    ASSERT_TRUE(uri.HasQuery());
    EXPECT_EQ("tn=monline_dg&bs=DVLOG&f=8&wd=glog+DVLOG", uri.Query());

    ASSERT_TRUE(uri.HasFragment());
    ASSERT_EQ("fragment", uri.Fragment());
}

TEST(URI, Set)
{
    URI uri;
    std::string user_info = "username:password";
    std::string scheme = "http";
    std::string host = "127.0.0.1";
    std::string port = "8080";
    std::string path = "/search";
    std::string query = "q=abc";
    std::string fragment = "fragment";
    uri.SetScheme(scheme);
    uri.SetUserInfo(user_info.data(), user_info.size());
    uri.SetHost(host);
    uri.SetPort(port);
    uri.SetPath(path);
    uri.SetQuery(query);
    uri.SetFragment(fragment);
    EXPECT_EQ("http://username:password@127.0.0.1:8080/search?q=abc#fragment", uri.ToString());
}

TEST(URI, Normalize)
{
    std::string uristr = "HtTp://WWW.baidu.Com/s?tn=monline_DG#fRGment";
    URI uri;
    ASSERT_TRUE(uri.Parse(uristr));
    EXPECT_TRUE(uri.Normalize());
    // just normalize scheme and host
    EXPECT_EQ("http://www.baidu.com/s?tn=monline_DG#fRGment", uri.ToString());
}

TEST(URI, ParseAuthority)
{
    std::string uristr = "http://username:password@127.0.0.1:8080/s?tn=monline_dg&bs=DVLOG";
    URI uri;
    ASSERT_TRUE(uri.Parse(uristr));
    EXPECT_EQ(uristr, uri.ToString());
    EXPECT_EQ("http", uri.Scheme());

    ASSERT_EQ("/s", uri.Path());
    ASSERT_TRUE(uri.HasAuthority());
    UriAuthority authority = uri.Authority();
    UriAuthority authority_empty;
    EXPECT_TRUE(authority.HasUserInfo());
    EXPECT_EQ("username:password", authority.UserInfo());
    authority.SetUserInfo("administrator", sizeof("adminitrator"));
    EXPECT_EQ("administrator", authority.UserInfo());
    EXPECT_EQ("127.0.0.1", authority.Host());
    authority.SetHost("10.12.22.200");
    EXPECT_TRUE(authority.HasPort());
    EXPECT_EQ("8080", authority.Port());
    authority.SetPort("8099");

    uri.SetAuthority(authority);
    EXPECT_EQ("http://administrator@10.12.22.200:8099/s?tn=monline_dg&bs=DVLOG", uri.ToString());

    EXPECT_FALSE(authority_empty.HasUserInfo());
    EXPECT_FALSE(authority_empty.HasPort());
    EXPECT_EQ("", authority_empty.Host());

    authority.Swap(&authority_empty);
    EXPECT_FALSE(authority.HasUserInfo());
    EXPECT_FALSE(authority.HasPort());
    EXPECT_EQ("", authority.Host());

    EXPECT_TRUE(authority_empty.HasUserInfo());
    EXPECT_EQ("administrator", authority_empty.UserInfo());
    EXPECT_TRUE(authority_empty.HasPort());
    EXPECT_EQ("8099", authority_empty.Port());
    EXPECT_EQ("10.12.22.200", authority_empty.Host());
}

TEST(URI, ParseRelative)
{
    const char* uristr = "/rpc?method=rpc_examples.EchoServer.Echo&format=json";
    URI uri;
    ASSERT_TRUE(uri.Parse(uristr));
    ASSERT_EQ("/rpc", uri.Path());
    ASSERT_TRUE(uri.HasQuery());
    ASSERT_EQ("method=rpc_examples.EchoServer.Echo&format=json", uri.Query());
}

TEST(URI, BadUrl)
{
    URI uri;
    ASSERT_FALSE(uri.Parse("http://-www.lianjiew.com/")); // leading -
    ASSERT_FALSE(uri.Parse("http://platform_info.py/")); // domain contains _
    ASSERT_FALSE(uri.Parse(" http://platform-info.py/")); // leading space

    std::vector<std::string> urls;
    ASSERT_TRUE(File::ReadLines("baduris.txt", &urls));
    for (size_t i = 0; i < urls.size(); ++i)
        EXPECT_FALSE(uri.Parse(urls[i])) << urls[i];
}

const char kSpecial[] = "&.<;/.\\~!@#$%^*()_|;':\"[]{}|,.<>/?+-=";

TEST(URI, Encode)
{
    std::string dest;
    std::string uri = "http://www.baidu.com/s?bs=";
    URI::Encode("http://www.baidu.com/s?bs=", &dest);
    EXPECT_EQ("http://www.baidu.com/s?bs=", dest);
    EXPECT_EQ("http://www.baidu.com/s?bs=",
              URI::Encode("http://www.baidu.com/s?bs="));
    URI::Encode(&uri);
    EXPECT_EQ("http://www.baidu.com/s?bs=", uri);
    EXPECT_EQ("&.%3C;/.%5C~!@#$%25%5E*()_%7C;':%22%5B%5D%7B%7D%7C,.%3C%3E/?+-=",
              URI::Encode(kSpecial));

    uri = "http://www.baidu.com/s?bs=";
    URI::EncodeComponent("http://www.baidu.com/s?bs=", &dest);
    EXPECT_EQ("http%3A%2F%2Fwww.baidu.com%2Fs%3Fbs%3D", dest);
    URI::EncodeComponent(&uri);
    EXPECT_EQ("http%3A%2F%2Fwww.baidu.com%2Fs%3Fbs%3D", uri);
    EXPECT_EQ("http%3A%2F%2Fwww.baidu.com%2Fs%3Fbs%3D",
              URI::EncodeComponent("http://www.baidu.com/s?bs="));
    EXPECT_TRUE(URI::Decode("http%3A%2F%2Fwww.baidu.com%2Fs%3Fbs%3D",
                            &dest));
    EXPECT_EQ("http://www.baidu.com/s?bs=", dest);
}

TEST(URI, Swap)
{
    std::string uristr = "http://www.baidu.com/s?tn=monline_dg&bs=DVLOG&f=8&wd=glog+DVLOG#fragment";
    URI uri1, uri2;
    EXPECT_TRUE(uri1.Parse(uristr));
    EXPECT_EQ(uristr, uri1.ToString());
    EXPECT_EQ("", uri2.ToString());
    uri1.Swap(&uri2);
    EXPECT_EQ(uristr, uri2.ToString());
    EXPECT_EQ("", uri1.ToString());
}

TEST(URI, WriteToBuffer)
{
    std::string uristr = "http://www.baidu.com/s?tn=monline_dg&bs=DVLOG&f=8&wd=glog+DVLOG#fragment";
    URI uri;
    EXPECT_TRUE(uri.Parse(uristr));
    char buffer[1024];
    size_t buffer_size = 1024;
    size_t result_size;
    EXPECT_TRUE(uri.WriteToBuffer(buffer, buffer_size, &result_size));
    EXPECT_EQ(uristr.size(), result_size);
    EXPECT_STREQ(uristr.data(), buffer);
}

static struct UriToAbsolute {
    const char* relative;
    const char* expected;
} merge_cases[] = {
    { "g",       "http://a/b/c/g" },
    { "./g",     "http://a/b/c/g" },
    { "g/",      "http://a/b/c/g/" },
    { "/g",      "http://a/g" },
    { "//g",     "http://g" },
    { "g?y",     "http://a/b/c/g?y" },
    { ";x"   ,   "http://a/b/c/;x"},
    { "g;x" ,    "http://a/b/c/g;x"},
    { "" ,       "http://a/b/c/d;p?q" },
    { ".",       "http://a/b/c/" },
    { "./",      "http://a/b/c/" },
    { "..",      "http://a/b/" },
    { "../",     "http://a/b/" },
    { "../g",    "http://a/b/g" },
    { "../../",  "http://a/" },
    { "../../g", "http://a/g" },
};

TEST(URI, ToAbsolute)
{
    URI base, relative;
    EXPECT_TRUE(base.Parse("http://a/b/c/d;p?q"));
    for (size_t i = 0; i < TOFT_ARRAY_SIZE(merge_cases); ++i) {
        EXPECT_TRUE(relative.Parse(merge_cases[i].relative)) << i;
        EXPECT_TRUE(relative.ToAbsolute(base)) << i;
        EXPECT_STREQ(merge_cases[i].expected, relative.ToString().data()) << i;
    }
}

class BatchTest : public testing::Test
{
public:
    virtual void SetUp()
    {
        ASSERT_TRUE(File::ReadLines("uris.txt", &urls));
    }
protected:
    std::vector<std::string> urls;
};

TEST_F(BatchTest, Test)
{
    URI uri;

    const int kLoopCount = 100;
    for (int i = 0; i < kLoopCount; ++i)
    {
        for (size_t j = 0; j < urls.size(); ++j)
        {
            bool parse_result = uri.Parse(urls[j]);
            EXPECT_TRUE(parse_result) << urls[j];
            if (parse_result) {
                EXPECT_EQ(urls[j], uri.ToString());
            }
        }
    }
}

} // namespace toft
