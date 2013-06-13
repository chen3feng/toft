// Copyright 2011, The Toft Authors.
// Author: Yongsong Liu <lyscsu@gmail.com>

#include "toft/net/http/time.h"
#include "thirdparty/gtest/gtest.h"

namespace toft {

const time_t kEpochNumber = 0;
const std::string kEpochString1("Thu, 01 Jan 1970 00:00:00 GMT");
const std::string kEpochString2("Thursday, 01-Jan-70 00:00:00 GMT");
const std::string kEpochString3("Thu Jan  1 00:00:00 1970");

const time_t kNormalDateTimeNumber = 784111777;
const std::string kNormalDateTimeString1("Sun, 06 Nov 1994 08:49:37 GMT");
const std::string kNormalDateTimeString2("Sunday, 06-Nov-94 08:49:37 GMT");
const std::string kNormalDateTimeString3("Sun Nov  6 08:49:37 1994");

TEST(HttpTimeTest, Parse)
{
    time_t t;
    ASSERT_TRUE(ParseHttpTime(kEpochString1, &t));
    EXPECT_EQ(kEpochNumber, t);
    ASSERT_TRUE(ParseHttpTime(kEpochString2, &t));
    EXPECT_EQ(kEpochNumber, t);
    ASSERT_TRUE(ParseHttpTime(kEpochString3, &t));
    EXPECT_EQ(kEpochNumber, t);
    ASSERT_TRUE(ParseHttpTime(kNormalDateTimeString1, &t));
    EXPECT_EQ(kNormalDateTimeNumber, t);
    ASSERT_TRUE(ParseHttpTime(kNormalDateTimeString2, &t));
    EXPECT_EQ(kNormalDateTimeNumber, t);
    ASSERT_TRUE(ParseHttpTime(kNormalDateTimeString3, &t));
    EXPECT_EQ(kNormalDateTimeNumber, t);
}

TEST(HttpTimeTest, Format)
{
    std::string str;
    ASSERT_TRUE(FormatHttpTime(kEpochNumber, &str));
    EXPECT_EQ(kEpochString1, str);
    ASSERT_TRUE(FormatHttpTime(kNormalDateTimeNumber, &str));
    EXPECT_EQ(kNormalDateTimeString1, str);
}

} // namespace toft
