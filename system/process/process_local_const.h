// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_SYSTEM_PROCESS_PROCESS_LOCAL_CONST_H
#define TOFT_SYSTEM_PROCESS_PROCESS_LOCAL_CONST_H
#pragma once

#include <unistd.h>

#include "toft/base/linking_assert.h"
#include "toft/system/memory/barrier.h"

namespace toft {

TOFT_DECLARE_LINKING_ASSERT(ProcessLocalConst);

// Fork aware process local storage.
// Exampls:
//
// ThisProcess::StartTime() {
//     // Yes, should be static
//     static ProcessLocalConst<time_t> s_start_time(GetStartTime);
//     return s_start_time.Value();
// }
//
// Cache the result of the first call of GetStartTime().
// But if the process forked, update it again.
//
template <typename T>
class ProcessLocalConst : private TOFT_LINKING_ASSERT_BASE(ProcessLocalConst) {
    typedef T (*GetFunction)();
public:
    explicit ProcessLocalConst(GetFunction get) :
#ifdef __unix__
        m_get(get), m_pid(getpid()),
#endif
        m_value(get()) {}

    const T& Value() const {
#ifdef __unix__
        int pid = getpid();
        if (pid != m_pid) {
            m_value = m_get();
            MemoryWriteBarrier();
            m_pid = pid;
        }
        MemoryReadBarrier();
#endif
        return m_value;
    }
private:
#ifdef __unix__
    mutable GetFunction m_get;
    mutable pid_t m_pid;
#endif
    mutable T m_value;
};

} // namespace toft

#endif // TOFT_SYSTEM_PROCESS_PROCESS_LOCAL_CONST_H
