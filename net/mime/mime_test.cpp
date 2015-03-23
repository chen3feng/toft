// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/net/mime/mime.h"
#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(MimeType, FromFileExtension)
{
    MimeType mt;
    ASSERT_TRUE(mt.FromFileExtension(".txt"));
    EXPECT_EQ("text/plain", mt.ToString());
    ASSERT_FALSE(mt.FromFileExtension("###"));
}


TEST(MimeType, Set)
{
    MimeType mt;
    EXPECT_TRUE(mt.Set("text/xml"));
    EXPECT_EQ("text/xml", mt.ToString());
    EXPECT_FALSE(mt.Set("invalid"));
}

TEST(MimeType, Match)
{
    MimeType mt;
    mt.Set("text/xml");
    EXPECT_TRUE(mt.Match("text/xml"));
    EXPECT_TRUE(mt.Match("*/xml"));
    EXPECT_TRUE(mt.Match("text/*"));
    EXPECT_FALSE(mt.Match("textxml"));
}

} // namespace toft
