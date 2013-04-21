// Copyright (c) 2010, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_SYSTEM_TIME_POSIX_TIME_H
#define TOFT_SYSTEM_TIME_POSIX_TIME_H

#include <stdint.h>

struct timespec;

namespace toft {

// for any timed* functions using absolute timespec
void RelativeMilliSecondsToAbsolute(
    int64_t relative_time_in_ms,
    timespec* ts
);

} // namespace toft

#endif // TOFT_SYSTEM_TIME_POSIX_TIME_H
