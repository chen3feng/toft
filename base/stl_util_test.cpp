// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/base/stl_util.h"

#include <map>
#include <string>

#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(MapUtilTest, TestMap) {
    std::map<std::string, int> c;
    c["hello"] = 1;

    EXPECT_EQ(NULL, FindOrNull(c, "he"));
    EXPECT_TRUE(FindOrNull(c, "hello") != NULL);
    EXPECT_EQ(1, *FindOrNull(c, "hello"));

    EXPECT_FALSE(InsertIfNotPresent(&c, "hello", 2));
    EXPECT_FALSE(HasKey(c, "world"));
    EXPECT_TRUE(InsertIfNotPresent(&c, "world", 2));
    EXPECT_TRUE(HasKey(c, "world"));
    EXPECT_TRUE(FindOrNull(c, "world") != NULL);
    EXPECT_EQ(2, *FindOrNull(c, "world"));

    EXPECT_FALSE(HasKey(c, "china"));
    EXPECT_TRUE(InsertOrReplace(&c, "china", 3));
    EXPECT_TRUE(FindOrNull(c, "china") != NULL);
    EXPECT_EQ(3, *FindOrNull(c, "china"));
    EXPECT_FALSE(InsertOrReplace(&c, "china", 4));
    EXPECT_TRUE(FindOrNull(c, "china") != NULL);
    EXPECT_EQ(4, *FindOrNull(c, "china"));
}

TEST(MapUtilTest, TestPtrMap) {
    std::map<std::string, int*> ptrc;
    int val = 10;
    EXPECT_TRUE(InsertIfNotPresent(&ptrc, "a",  &val));
    EXPECT_TRUE(HasKey(ptrc, "a"));
    EXPECT_EQ(NULL, FindPtrOrNull(ptrc, "b"));
    EXPECT_TRUE(FindPtrOrNull(ptrc, "a") != NULL);
    EXPECT_EQ(val, *FindPtrOrNull(ptrc, "a"));
}

TEST(MapUtilTest, FindOrDefault) {
    std::map<std::string, int> c;
    c["hello"] = 1;
    EXPECT_EQ(1, FindOrDefault(c, "hello", 0));
    EXPECT_EQ(-1, FindOrDefault(c, "world", -1));
}

TEST(MapUtilTest, FindAndCopyTo) {
    std::map<std::string, int> c;
    c["hello"] = 1;
    int n = 0;
    EXPECT_TRUE(FindAndCopyTo(c, "hello", &n));
    EXPECT_EQ(1, n);
    EXPECT_FALSE(FindAndCopyTo(c, "world", &n));
}

TEST(MapUtilTest, HasKey) {
    std::map<std::string, int> c;
    c["hello"] = 1;
    EXPECT_TRUE(HasKey(c, "hello"));
    EXPECT_FALSE(HasKey(c, "world"));
}

TEST(MapUtilTest, ContainsKeyValuePair) {
    std::map<std::string, int> c;
    c["hello"] = 1;
    EXPECT_TRUE(ContainsKeyValuePair(c, "hello", 1));
    EXPECT_FALSE(ContainsKeyValuePair(c, "hello", 0));
    EXPECT_FALSE(ContainsKeyValuePair(c, "world", 1));
}

TEST(MapUtilTest, InsertOrReplace) {
    std::map<std::string, int> c;
    EXPECT_TRUE(InsertOrReplace(&c, "a",  1));
    EXPECT_EQ(1, c["a"]);
    EXPECT_FALSE(InsertOrReplace(&c, "a",  2));
    EXPECT_EQ(2, c["a"]);
}

TEST(MapUtilTest, InsertOrReplaceWithOldValue) {
    std::map<std::string, int> c;
    EXPECT_TRUE(InsertOrReplace(&c, "a",  1));
    int old = 0;
    EXPECT_FALSE(InsertOrReplace(&c, "a",  2, &old));
    EXPECT_EQ(2, c["a"]);
    EXPECT_EQ(1, old);
}

}  // namespace toft
