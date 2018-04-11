// Copyright 2013, For authors.
// Author: An Qin (anqin.qin@gmail.com)
// A test for URI Utils
#include "uri_utils.h"

#include <string>
#include <vector>
#include <map>

#include "gtest/gtest.h"

namespace toft {

TEST(UriUtilsTest, ExplodeTest) {
    std::vector<std::string> result;
    ASSERT_TRUE(UriUtils::Explode("/hdfs/szwg-ecomon?username=logging,password=abcd/user/logging", '/', &result));
    EXPECT_STREQ(result[0].c_str(), "hdfs");
    EXPECT_STREQ(result[1].c_str(), "szwg-ecomon?username=logging,password=abcd");
    EXPECT_STREQ(result[2].c_str(), "user");
    EXPECT_STREQ(result[3].c_str(), "logging");
    ASSERT_TRUE(UriUtils::Explode("hdfs/szwg-ecomon?username=logging,password=abcd", '/', &result));
    EXPECT_STREQ(result[0].c_str(), "hdfs");
    EXPECT_STREQ(result[1].c_str(), "szwg-ecomon?username=logging,password=abcd");

}

TEST(UriUtilsTest, ParseParamTest) {
    std::string main_str;
    std::map<std::string, std::string> params;
    ASSERT_TRUE(UriUtils::ParseParam("szwg-ecomon?username=logging,password=abcd",
            &main_str, &params));
    EXPECT_STREQ(main_str.c_str(), "szwg-ecomon");
    EXPECT_STREQ(params["username"].c_str(), "logging");
    EXPECT_STREQ(params["password"].c_str(), "abcd");

    // bad cases
    ASSERT_TRUE(UriUtils::ParseParam("szwg-ecomon",
            &main_str, &params));
    EXPECT_STREQ(main_str.c_str(), "szwg-ecomon");
    ASSERT_TRUE(UriUtils::ParseParam("szwg-ecomon?username=logging,password",
            &main_str, &params));
    EXPECT_STREQ(main_str.c_str(), "szwg-ecomon");
    EXPECT_STREQ(params["username"].c_str(), "logging");
    ASSERT_TRUE(UriUtils::ParseParam("szwg-ecomon?username=logging,",
            &main_str, &params));
    EXPECT_STREQ(main_str.c_str(), "szwg-ecomon");
    EXPECT_STREQ(params["username"].c_str(), "logging");
    ASSERT_TRUE(UriUtils::ParseParam("szwg-ecomon?",
            &main_str, &params));
    EXPECT_STREQ(main_str.c_str(), "szwg-ecomon");

    // empty username and password
    ASSERT_TRUE(UriUtils::ParseParam("szwg-ecomon?username=,password=",
            &main_str, &params));
    EXPECT_STREQ(main_str.c_str(), "szwg-ecomon");
    EXPECT_STREQ(params["username"].c_str(), "");
    EXPECT_STREQ(params["password"].c_str(), "");
}

TEST(UriUtilsTest, ShiftTest) {
    std::string result;
    ASSERT_TRUE(UriUtils::Shift("/hdfs/szwg-ecomon/user", &result, 1, '/'));
    EXPECT_STREQ(result.c_str(), "/szwg-ecomon/user");
    ASSERT_TRUE(UriUtils::Shift("/hdfs/szwg-ecomon/user", &result, 2, '/'));
    EXPECT_STREQ(result.c_str(), "/user");
}

}

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
