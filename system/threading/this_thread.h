// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 05/31/11
// Description: current thread scoped attributes and operations

#ifndef TOFT_SYSTEM_THREADING_THIS_THREAD_H
#define TOFT_SYSTEM_THREADING_THIS_THREAD_H
#pragma once

#include <stdint.h>

#include "toft/system/threading/thread_types.h"

namespace toft {

/// thread scoped attribute and operations of current thread
class ThisThread
{
    ThisThread();
    ~ThisThread();
public:
    static void Exit();
    static void Yield();
    static void Sleep(int64_t time_in_ms);
    static int GetLastErrorCode();
    static ThreadHandleType GetHandle();
    static int GetId();
    static bool IsMain();
};

} // namespace toft

#endif // TOFT_SYSTEM_THREADING_THIS_THREAD_H
