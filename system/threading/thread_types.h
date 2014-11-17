// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 05/31/11

#ifndef TOFT_SYSTEM_THREADING_THREAD_TYPES_H
#define TOFT_SYSTEM_THREADING_THREAD_TYPES_H
#pragma once

#include <pthread.h>
#include <string>

namespace toft {

typedef pthread_t   ThreadHandleType;

class BaseThread;

/// ThreadAttribute represent thread attribute.
/// Usage:
/// ThreadAttribute()
///     .SetName("ThreadPoolThread")
///     .SetStackSize(64 * 1024)
class ThreadAttributes {
    friend class BaseThread;
public:
    ThreadAttributes();
    ~ThreadAttributes();
    ThreadAttributes& SetName(const std::string& name);
    ThreadAttributes& SetStackSize(size_t size);
    ThreadAttributes& SetDetached(bool detached);
    ThreadAttributes& SetPriority(int priority);
    bool IsDetached() const;
private:
    std::string m_name;
    pthread_attr_t m_attr;
};

} // namespace toft

#endif // TOFT_SYSTEM_THREADING_THREAD_TYPES_H
