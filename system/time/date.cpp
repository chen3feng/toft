// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/system/time/date.h"

#include <time.h>
#include <algorithm>
#include <stdexcept>

namespace toft {

static int const kDaysInMonth[] = {
    // 1   2   3   4   5   6   7   8   9   10  11  12
    0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

Date::Date(int year, int month, int day) {
    if (!Set(year, month, day))
        throw std::runtime_error("Invalid date");
}

void Date::AdjustMonthLimit() {
    if (m_month == 2 && m_day > 28) {
        int days = DaysInMonth(m_year, m_month);
        if (m_day > days)
            m_day = days;
    }
}

bool Date::Set(int year, int month, int day) {
    if (!IsValid(year, month, day))
        return false;
    m_year = year;
    m_month = month;
    m_day = day;
    return true;
}

bool Date::SetYear(int year) {
    m_year = year;
    AdjustMonthLimit();
    return true;
}

bool Date::SetMonth(int month) {
    if (month <= 0 || m_month > 12)
        return false;
    m_month = month;
    AdjustMonthLimit();
    return true;
}

bool Date::SetDay(int day) {
    if (day <= 0 || day > 31)
        return false;
    if (day > DaysInMonth(m_year, m_month))
        return false;
    m_day = day;
    return true;
}

Date& Date::AddYears(int years) {
    SetYear(m_year + years);
    return *this;
}

Date& Date::AddMonths(int months) {
    if (months == 0)
        return *this;

    if (months > 0) {
        int years = months / 12;
        months %= 12;
        m_year += years;
        m_month += months;
        if (m_month > 12) {
            ++m_year;
            m_month -= 12;
        }
    } else {
        months = -months;
        int years = months / 12;
        months %= 12;
        m_year -= years;
        m_month -= months;
        if (m_month < 1) {
            --m_year;
            m_month += 12;
        }
    }

    AdjustMonthLimit();
    return *this;
}

Date& Date::AddDays(int days) {
    if (days == 0)
        return *this;

    if (days > 0) {
        for (;;) { // Add years one by one.
            int days_to_next_year = DaysToNextYear();
            if (days < days_to_next_year)
                break;
            AddYears(1);
            days -= days_to_next_year;
        }
        for (;;) { // Add months one by one.
            int days_to_next_month = DaysToNextMonth();
            if (days < days_to_next_month)
                break;
            AddMonths(1);
            days -= days_to_next_month;
        }
        m_day = m_day + days;
        int days_in_month = DaysInMonth(m_year, m_month);
        if (m_day > days_in_month) {
            // Month overflow, adjust to next month.
            m_day -= days_in_month;
            AddMonths(1);
        }
    } else {
        days = -days;
        for (;;) {
            int days_to_previous_year = DaysToPreviousYear();
            if (days < days_to_previous_year)
                break;
            AddYears(-1);
            days -= days_to_previous_year;
        }
        for (;;) {
            int days_to_previous_month = DaysToPreviousMonth();
            if (days < days_to_previous_month)
                break;
            AddMonths(-1);
            days -= days_to_previous_month;
        }
        m_day = m_day - days;
        if (m_day < 1) {
            int days_in_prev_month = DaysInPreviousMonth(m_year, m_month);
            m_day += days_in_prev_month;
            AddMonths(-1);
        }
    }

    return *this;
}

Date& Date::operator+=(int days) {
    AddDays(days);
    return *this;
}

Date& Date::operator-=(int days) {
    AddDays(-days);
    return *this;
}

Date& Date::operator++() {
    ++m_day;
    if (m_day > DaysInMonth(m_year, m_month)) {
        m_day = 1;
        ++m_month;
        if (m_month > 12) {
            ++m_year;
            m_month = 1;
        }
    }
    return *this;
}

Date& Date::operator--() {
    --m_day;
    if (m_day == 0) {
        --m_month;
        if (m_month == 0) {
            m_month = 12;
            --m_year;
        }
        m_day = DaysInMonth(m_year, m_month);
    }
    return *this;
}

Date Date::operator++(int) { // NOLINT(readability/function)
    Date org(*this);
    ++*this;
    return org;
}

Date Date::operator--(int) { // NOLINT(readability/function)
    Date org(*this);
    --*this;
    return org;
}

std::string Date::ToString() const {
    struct tm tm = {};
    tm.tm_year = m_year - 1900;
    tm.tm_mon = m_month - 1;
    tm.tm_mday = m_day;
    char buf[16];
    return std::string(buf, strftime(buf, sizeof(buf), "%F", &tm));
}

#pragma GCC diagnostic ignored "-Wformat-nonliteral"

std::string Date::Format(const std::string& format) const {
    struct tm tm = {};
    tm.tm_year = m_year - 1900;
    tm.tm_mon = m_month - 1;
    tm.tm_mday = m_day;
    char buf[4096];
    size_t n = strftime(buf, sizeof(buf), format.c_str(), &tm);
    return std::string(buf, n);
}

bool Date::Parse(const std::string& src, const std::string& format) {
    struct tm tm = {};
    if (!strptime(src.c_str(), format.c_str(), &tm))
        return false;
    m_year = tm.tm_year + 1900;
    m_month = tm.tm_mon + 1;
    m_day = tm.tm_mday;
    return true;
}

bool Date::Parse(const std::string& src) {
    return Parse(src, "%F"); // || Parse(src, "%Y%m%D");
}

Date Date::Today() {
    struct tm* tm;
    struct tm tms;
    time_t t = time(NULL);
#if _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _BSD_SOURCE || _SVID_SOURCE || _POSIX_SOURCE
    tm = localtime_r(&t, &tms);
#else
    tm = localtime(&t); // NOLINT(runtime/threadsafe_fn)
#endif
    return Date(tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
}

Date Date::UtcToday() {
    struct tm* tm;
    struct tm tms;
    time_t t = time(NULL);
#if _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _BSD_SOURCE || _SVID_SOURCE || _POSIX_SOURCE
    tm = gmtime_r(&t, &tms);
#else
    tm = gmtime(&t); // NOLINT(runtime/threadsafe_fn)
#endif
    return Date(tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
}

bool Date::IsLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

int Date::DaysInMonth(int year, int month) {
    int days = kDaysInMonth[month];
    if (month == 2 && IsLeapYear(year))
        ++days;
    return days;
}

int Date::DaysInPreviousMonth(int year, int month) {
    return month > 1 ? DaysInMonth(year, month - 1) : DaysInMonth(year - 1, 12);
}

int Date::DaysInNextMonth(int year, int month) {
    return month < 12 ? DaysInMonth(year, month + 1) : DaysInMonth(year + 1, 1);
}

bool Date::IsValid(int year, int month, int day) {
    if (month < 1 || month > 12)
        return false;
    if (day < 1 || day > DaysInMonth(year, month))
        return false;
    return true;
}

int Date::Compare(const Date& lhs, const Date& rhs) {
    if (lhs.m_year > rhs.m_year)
        return 1;
    if (lhs.m_year < rhs.m_year)
        return -1;
    if (lhs.m_month > rhs.m_month)
        return 1;
    if (lhs.m_month < rhs.m_month)
        return -1;
    if (lhs.m_day > rhs.m_day)
        return 1;
    if (lhs.m_day < rhs.m_day)
        return -1;
    return 0;
}

Date Date::NextDay() const {
    Date result(*this);
    return ++result;
}

Date Date::NextMonth() const {
    Date result(*this);
    return result.AddMonths(1);
}

Date Date::NextYear() const {
    Date result(*this);
    return result.AddYears(1);
}

int Date::DaysToNextYear() const {
    if (m_month > 2 || (m_month == 2 && m_day > 28)) {
        return 365 + IsLeapYear(m_year + 1);
    } else {
        return 365 + IsLeapYear(m_year);
    }
}

int Date::DaysToNextMonth() const {
    int this_month_days = DaysInMonth(m_year, m_month);
    int next_month_days = DaysInNextMonth(m_year, m_month);
    if (m_day <= next_month_days)
        return this_month_days;
    return next_month_days + this_month_days - m_day;
}

int Date::DaysToPreviousYear() const {
    if (m_month < 2 || (m_month == 2 && m_day < 28)) {
        return 365 + IsLeapYear(m_year - 1);
    } else {
        return 365 + IsLeapYear(m_year);
    }
}

int Date::DaysToPreviousMonth() const {
    int prev_month_days = DaysInPreviousMonth(m_year, m_month);
    return std::max(static_cast<int>(m_day), prev_month_days);
}

int Date::DaysTo(const Date& to) const {
    if (to < *this)
        return to.DaysTo(*this);
    int days = 0;
    Date from(*this);
    for (;;) {
        Date next_year = from.NextYear();
        if (next_year > to)
            break;
        days += from.DaysToNextYear();
        from = next_year;
    }
    for (;;) {
        Date next_month = from.NextMonth();
        if (next_month > to)
            break;
        days += from.DaysToNextMonth();
        from = next_month;
    }
    while (from < to) {
        ++from;
        ++days;
    }
    return days;
}

} // namespace toft
