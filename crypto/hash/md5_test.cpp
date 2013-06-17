// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/crypto/hash/md5.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(MD5Test, UseClass) {
    std::string test_str = "qwertyuiopasdfghjklzxcvbnm";
    MD5 md5;
    md5.Update(test_str);
    UInt128 ret = md5.Final();
    UInt128 expect_value(18279382751233366757UL, 7523094284164231279UL);
    EXPECT_EQ(ret, expect_value);

    EXPECT_EQ(MD5::Digest(test_str), expect_value);
};

TEST(MD5Test, HexFinal) {
    std::string test_str = "qwertyuiopasdfghjklzxcvbnm";
    MD5 md5;
    md5.Update(test_str);
    std::string hex = md5.HexFinal();
    //  We got the result using python md5 module.
    std::string expected_str = "e5daaa90c369adfd156862d6df632ded";
    EXPECT_EQ(hex, expected_str);
    EXPECT_EQ(MD5::HexDigest(test_str), expected_str);
};

TEST(MD5Test, EmptyString) {
    std::string test_str = "";
    EXPECT_EQ(MD5::HexDigest(test_str), "d41d8cd98f00b204e9800998ecf8427e");
};

TEST(MD5Test, LittleChanged) {
    std::string test_str = "The quick brown fox jumps over the lazy dog";
    EXPECT_EQ(MD5::HexDigest(test_str), "9e107d9d372bb6826bd81d3542a419d6");

    std::string diff_str = "The quick brown fox jumps over the lazy cog";
    EXPECT_EQ(MD5::HexDigest(diff_str), "1055d3e698d289f2af8663725127bd4b");
};

} // namespace toft
