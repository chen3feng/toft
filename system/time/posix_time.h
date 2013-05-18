// Copyright (c) 2010, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_SYSTEM_TIME_POSIX_TIME_H
#define TOFT_SYSTEM_TIME_POSIX_TIME_H

#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>

struct timespec;

namespace toft {

// for any timed* functions using absolute timespec
void RelativeMilliSecondsToAbsolute(
    int64_t relative_time_in_ms,
    timespec* ts
);

inline int64_t GetTimeInMs() {
    struct timeval now;
    if (gettimeofday(&now, NULL) < 0) {
        return -1;
    }
    gettimeofday(&now, NULL);
    return (int64_t) (now.tv_sec * 1000 + now.tv_usec / 1000);
}

inline int64_t GetTimeInUs() {
    struct timeval now;
    if (gettimeofday(&now, NULL) < 0) {
        return -1;
    }
    gettimeofday(&now, NULL);
    return (int64_t) (now.tv_sec * 1000 * 1000 + now.tv_usec);
}

inline int64_t GetTimeInNs() {
    struct timeval now;
    if (gettimeofday(&now, NULL) < 0) {
        return -1;
    }
    return (int64_t) now.tv_sec * 1000 * 1000 * 1000 + now.tv_usec * 1000;
}

} // namespace toft

#endif // TOFT_SYSTEM_TIME_POSIX_TIME_H
