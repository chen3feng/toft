// Copyright 2011, The Toft Authors.
// Author: Yongsong Liu <lyscsu@gmail.com>

#ifndef TOFT_NET_HTTP_TIME_H
#define TOFT_NET_HTTP_TIME_H

#include <time.h>
#include <string>

namespace toft {

bool ParseHttpTime(const char* str, time_t* time);
inline bool ParseHttpTime(const std::string& str, time_t* time)
{
    return ParseHttpTime(str.c_str(), time);
}

size_t FormatHttpTime(time_t time, char* str, size_t str_length);
bool FormatHttpTime(time_t time, std::string* str);

} // namespace toft

#endif // TOFT_NET_HTTP_TIME_H
