// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/system/time/date.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(Date, Ctor) {
    EXPECT_EQ(Date(1970, 1, 1), Date());
    EXPECT_NO_THROW(Date(2000, 2, 29));
    EXPECT_NO_THROW(Date(2012, 2, 29));
}

TEST(Date, CtorInvalid) {
    EXPECT_ANY_THROW(Date(1970, 1, 32));
    EXPECT_ANY_THROW(Date(1970, 2, 31));
    EXPECT_ANY_THROW(Date(1970, 2, 29));
    EXPECT_ANY_THROW(Date(1970, 9, 31));
    EXPECT_ANY_THROW(Date(1970, -1, 31));
    EXPECT_ANY_THROW(Date(1970, -1, 31));
    EXPECT_ANY_THROW(Date(1970, 1, 0));
    EXPECT_ANY_THROW(Date(1900, 2, 29));
}

TEST(Date, Get) {
    Date date(1979, 12, 25);
    EXPECT_EQ(1979, date.Year());
    EXPECT_EQ(12, date.Month());
    EXPECT_EQ(25, date.Day());
}

TEST(Date, Set) {
    Date date;
    EXPECT_TRUE(date.Set(1979, 2, 21));
    EXPECT_TRUE(date.Set(1979, 3, 31));
    EXPECT_FALSE(date.Set(1979, 4, 41));
    EXPECT_TRUE(date.Set(2012, 2, 29));
    EXPECT_FALSE(date.Set(2013, 2, 29));
}

TEST(Date, SetYear) {
    Date date;
    ASSERT_TRUE(date.SetYear(2012));
    EXPECT_EQ(2012, date.Year());
}

TEST(Date, SetYearLeapYear) {
    Date date(2000, 2, 29);
    ASSERT_TRUE(date.SetYear(2012));
    EXPECT_EQ(29, date.Day());
    ASSERT_TRUE(date.SetYear(2011));
    EXPECT_EQ(28, date.Day());
}

TEST(Date, SetMonth) {
    Date date(1979, 3, 31);
    ASSERT_TRUE(date.SetMonth(2));
    EXPECT_EQ(1979, date.Year());
    EXPECT_EQ(2, date.Month());
    EXPECT_EQ(28, date.Day());
}

TEST(Date, SetMonthLeayPear) {
    Date date(2000, 1, 31);
    ASSERT_TRUE(date.SetMonth(2));
    EXPECT_EQ(2000, date.Year());
    EXPECT_EQ(2, date.Month());
    EXPECT_EQ(29, date.Day());
}

TEST(Date, SetDay) {
    Date date(1979, 2, 27);
    ASSERT_TRUE(date.SetDay(28));
    EXPECT_EQ(28, date.Day());
    ASSERT_FALSE(date.SetDay(29));
}

TEST(Date, SetDayLeapYear) {
    Date date(2000, 2, 27);
    ASSERT_TRUE(date.SetDay(29));
    EXPECT_EQ(29, date.Day());
}

TEST(Date, Compare) {
    Date small(1979, 2, 28);
    Date large(1980, 1, 2);
    EXPECT_LT(Date::Compare(small, large), 0);
    EXPECT_GT(Date::Compare(large, small), 0);
    EXPECT_EQ(Date::Compare(small, small), 0);
    EXPECT_EQ(small, small);
    EXPECT_EQ(large, large);
    EXPECT_NE(small, large);
    EXPECT_LT(small, large);
    EXPECT_LE(small, large);
    EXPECT_LE(small, small);
    EXPECT_GT(large, small);
    EXPECT_GE(large, small);
}

TEST(Date, AddMonths) {
    Date date(1970, 1, 1);
    date.AddMonths(1);
    EXPECT_EQ(Date(1970, 2, 1), date);
}

TEST(Date, SubMonths) {
    Date date(1970, 1, 1);
    date.AddMonths(-1);
    EXPECT_EQ(Date(1969, 12, 1), date);
}

TEST(Date, AddDays) {
    Date date(1970, 1, 31);
    EXPECT_EQ(Date(1970, 2, 2), Date(date).AddDays(2));
    EXPECT_EQ(Date(1970, 3, 2), Date(date).AddDays(30));
    EXPECT_EQ(Date(1970, 3, 1), Date(1970, 2, 28).AddDays(1));
}

TEST(Date, SubDays) {
    Date date(1970, 1, 31);
    date.AddDays(-1);
    EXPECT_EQ(Date(1970, 1, 30), date);
    ASSERT_TRUE(date.Set(1970, 1, 1));
    date.AddDays(-1);
    EXPECT_EQ(Date(1969, 12, 31), date);
}

TEST(Date, AddDaysVsIncrement) {
    Date from(1999, 1, 15);
    Date from2(from);
    Date to(2001, 3, 12);
    while (from < to) {
        ASSERT_EQ(++from, from2.AddDays(1));
    }
}

TEST(Date, SubDaysVsDecrement) {
    Date from(2001, 3, 12);
    Date from2(from);
    Date to(1999, 1, 15);
    while (from > to) {
        ASSERT_EQ(--from, from2.AddDays(-1));
    }
}

TEST(Date, DaysToNextMonth) {
    EXPECT_EQ(28, Date(2011, 2, 1).DaysToNextMonth());
    EXPECT_EQ(29, Date(2012, 2, 1).DaysToNextMonth());
    EXPECT_EQ(28, Date(2011, 1, 31).DaysToNextMonth());
    EXPECT_EQ(29, Date(2012, 1, 31).DaysToNextMonth());
}

TEST(Date, DaysToPreviousMonth) {
    EXPECT_EQ(28, Date(2011, 3, 1).DaysToPreviousMonth());
    EXPECT_EQ(31, Date(2012, 3, 31).DaysToPreviousMonth());
    EXPECT_EQ(28, Date(2011, 3, 28).DaysToPreviousMonth());
    EXPECT_EQ(29, Date(2012, 3, 27).DaysToPreviousMonth());
}

TEST(Date, DaysTo) {
    EXPECT_EQ(365, Date(1979, 2, 1).DaysTo(Date(1980, 2, 1)));
    EXPECT_EQ(366, Date(2000, 2, 1).DaysTo(Date(2001, 2, 1)));
    EXPECT_EQ(365, Date(2000, 3, 1).DaysTo(Date(2001, 3, 1)));
    EXPECT_EQ(12465, Date(1979, 4, 17).DaysTo(Date(2013, 6, 2)));
}

TEST(Date, ToString) {
    EXPECT_EQ("1979-02-21", Date(1979, 2, 21).ToString());
}

TEST(Date, Format) {
    EXPECT_EQ("Date is: 1979-02-21", Date(1979, 2, 21).Format("Date is: %F"));
}

TEST(Date, Parse) {
    Date date;
    ASSERT_TRUE(date.Parse("1979-02-12"));
    EXPECT_EQ(Date(1979, 2, 12), date);
    ASSERT_TRUE(date.Parse("Date is: 1979-02-21", "Date is: %F"));
    EXPECT_EQ(Date(1979, 2, 21), date);
}

TEST(Date, Iterate) {
    Date from(2011, 1, 15);
    Date to(2013, 3, 12);
    int days = from.DaysTo(to);
    int count = 0;
    while (from < to) {
        ++from;
        ++count;
    }
    ASSERT_EQ(count, days);
}

} // namespace toft
