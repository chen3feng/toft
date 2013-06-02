// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_SYSTEM_TIME_DATE_H
#define TOFT_SYSTEM_TIME_DATE_H
#pragma once

#include <stdint.h>
#include <iosfwd>
#include <string>

namespace toft {

// Timezone neutral Gregorian calendar date.
class Date {
public:
    Date() : m_year(1970), m_month(1), m_day(1) {}
    Date(int year, int month, int day);

    // Attributes
    int Year() const { return m_year; }
    int Month() const { return m_month; }
    int Day() const { return m_day; }

    // Set methods
    bool Set(int year, int month, int day);
    bool SetYear(int year);
    bool SetMonth(int month);
    bool SetDay(int day);

    // Add/Sub
    Date& AddYears(int years);
    Date& AddMonths(int months);
    Date& AddDays(int days);
    Date& operator+=(int days);
    Date& operator-=(int days);
    Date& operator++();
    Date& operator--();
    Date operator++(int);
    Date operator--(int);

    // To string with format of 'yyyy-mm-dd'.
    std::string ToString() const;
    // To string with format same as strftime.
    std::string Format(const std::string& format) const;

    // Parse with format, see strftime.
    bool Parse(const std::string& src, const std::string& format);
    // Try parse from any possible format.
    bool Parse(const std::string& src);

    static Date Today(); // Current day of local time.
    static Date UtcToday(); // Current day of UTC.
    static int DaysInMonth(int year, int month); // How many days in the month.
    static bool IsLeapYear(int year); // Is a year leap year.
    static bool IsValid(int year, int month, int day); // Is a valid ymd.

    // Return >0 if lhs > rhs
    // Return 0 if lhs == rhs
    // Return <0 if lhs < rhs
    static int Compare(const Date& lhs, const Date& rhs);

    bool operator<(const Date& rhs) const { return Compare(*this, rhs) < 0; }
    bool operator>(const Date& rhs) const { return Compare(*this, rhs) > 0; }
    bool operator==(const Date& rhs) const { return Compare(*this, rhs) == 0; }
    bool operator<=(const Date& rhs) const { return Compare(*this, rhs) <= 0; }
    bool operator>=(const Date& rhs) const { return Compare(*this, rhs) >= 0; }
    bool operator!=(const Date& rhs) const { return Compare(*this, rhs) != 0; }

    Date NextDay() const; // Next day of *this.
    Date NextMonth() const; // Next month of *this.
    Date NextYear() const; // Next year of *this.

    int DaysTo(const Date& to) const; // Total day to the date 'to'
    int DaysToNextYear() const; // Total days to next year of this day.
    int DaysToNextMonth() const; // Total days to next month of this day.
    int DaysToPreviousYear() const; // Total days to previous year of this day.
    int DaysToPreviousMonth() const; // Total days to previous month of this day.

private:
    // Adjust day if overflow for month.
    void AdjustMonthLimit();
    // How many days in the previous month.
    static int DaysInPreviousMonth(int year, int month);
    // How many days in the next month.
    static int DaysInNextMonth(int year, int month);

private: // To save space, using small types.
    int16_t m_year;
    uint8_t m_month;
    uint8_t m_day;
};

template <typename Char, typename Traits>
std::basic_ostream<Char, Traits>&
operator<<(std::basic_ostream<Char, Traits>& os, Date date) {
    return os << date.ToString();
}

} // namespace toft

#endif // TOFT_SYSTEM_TIME_DATE_H
