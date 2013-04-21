#ifndef TOFT_SYSTEM_TIME_CLOCK_H
#define TOFT_SYSTEM_TIME_CLOCK_H
#pragma once

#include <stdint.h>

namespace toft {

class Clock {
public:
    virtual ~Clock() {}
    // 19700101000000L
    virtual int64_t StartTime() = 0;

    virtual int64_t MicroSeconds() = 0;
    virtual int64_t MilliSeconds() = 0;
    virtual int64_t Seconds() = 0;

    virtual bool Set(int64_t microseconds) = 0;
};

extern Clock& RealtimeClock;

} // namespace toft

#endif // TOFT_SYSTEM_TIME_CLOCK_H
