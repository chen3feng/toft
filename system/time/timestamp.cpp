// Copyright 2013, Baidu Inc.
//
// Author: Qianqiong Zhang <zhangqianqiong02@baidu.com>


#include "toft/system/time/timestamp.h"

#include <sys/time.h>
#include <unistd.h>

namespace toft {

int64_t GetTimestampInUs() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t result = tv.tv_sec;
    result *= 1000000;
    result += tv.tv_usec;
    return result;
}

int64_t GetTimestampInMs() {
    int64_t timestamp = GetTimestampInUs();
    return timestamp / 1000;
}

} // namespace toft

