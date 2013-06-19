// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/system/time/clock.h"

#include <time.h>
#include <sys/time.h>

namespace toft {

class SystemClock : public Clock {
public:
    explicit SystemClock(int id) : m_id(id) {}
    virtual int64_t StartTime() {
        return 19700101000000L;
    }
    virtual int64_t MicroSeconds() {
        timespec ts;
        if (clock_gettime(m_id, &ts) < 0)
            return -1;
        return ts.tv_sec * 1000000LL + ts.tv_nsec / 1000;
    }
    virtual int64_t MilliSeconds() {
        return MicroSeconds() / 1000;
    }
    virtual int64_t Seconds() {
        return MicroSeconds() / 1000000;
    }
    virtual bool Set(int64_t microseconds) {
        timespec ts = {microseconds / 1000000, microseconds % 1000000 * 1000};
        return clock_settime(m_id, &ts) == 0;
    }
private:
    clockid_t m_id;
};

class RtcClock : public SystemClock {
public:
    RtcClock() : SystemClock(CLOCK_REALTIME) {}
    virtual int64_t MicroSeconds() {
        timeval tv;
        if (gettimeofday(&tv, NULL) != 0)
            return -1;
        return tv.tv_sec * 1000000LL + tv.tv_usec;
    }
    virtual int64_t Seconds() {
        return time(NULL);
    }
};

static RtcClock s_rtc_clock;
static SystemClock s_mono_clock(CLOCK_MONOTONIC);
static SystemClock s_process_clock(CLOCK_PROCESS_CPUTIME_ID);
static SystemClock s_thread_clock(CLOCK_PROCESS_CPUTIME_ID);

Clock& RealtimeClock = s_rtc_clock;
Clock& MonotonicClock = s_mono_clock;
Clock& ProcessCpuTimeClock = s_process_clock;
Clock& ThreadCpuTimeClock = s_thread_clock;

} // namespace toft

