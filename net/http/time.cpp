// Copyright 2011, The Toft Authors.
// Author: Yongsong Liu <lyscsu@gmail.com>

#include "toft/net/http/time.h"

#include <locale.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>
#include <sys/types.h>

namespace toft {

static locale_t GetCLocale()
{
    static locale_t c_locale = newlocale(LC_ALL_MASK, "C", 0);
    return c_locale;
}

static bool HttpDateToTm(const char* str, struct tm* tm)
{
    locale_t c_locate = GetCLocale();
    char* end;

    if ((end = strptime_l(str, "%c", tm, c_locate)) && *end == '\0')
        return true;

    // rfc1123-date
    if ((end = strptime_l(str, "%a, %d %b %Y %H:%M:%S GMT", tm, c_locate))
        && *end == '\0')
        return true;

    // rfc950-date
    if ((end = strptime_l(str, "%A, %d-%b-%y %H:%M:%S GMT", tm, c_locate))
        && *end == '\0')
        return true;

    return false;
}

bool ParseHttpTime(const char* str, time_t* time)
{
    struct tm tm = {};
    if (HttpDateToTm(str, &tm))
    {
        *time = timegm(&tm);
        return true;
    }
    return false;
}

size_t FormatHttpTime(time_t time, char* str, size_t str_length)
{
    locale_t c_locale = GetCLocale();
    struct tm tm = {};
    gmtime_r(&time, &tm);
    return strftime_l(str, str_length, "%a, %d %b %Y %H:%M:%S %Z", &tm, c_locale);
}

bool FormatHttpTime(time_t time, std::string* str)
{
    char buf[32];
    size_t length = FormatHttpTime(time, buf, sizeof(buf));
    if (length)
    {
        str->assign(buf, length);
        return true;
    }
    return false;
}

} // namespace toft
