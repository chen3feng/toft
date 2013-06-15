// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_SYSTEM_THREADING_THREAD_GROUP_H
#define TOFT_SYSTEM_THREADING_THREAD_GROUP_H
#pragma once

#include <vector>

#include "toft/base/functional.h"
#include "toft/base/uncopyable.h"
#include "toft/system/threading/thread.h"

namespace toft {

class ThreadGroup
{
    TOFT_DECLARE_UNCOPYABLE(ThreadGroup);
public:
    ThreadGroup();
    ThreadGroup(const std::function<void ()>& callback, size_t count);
    ~ThreadGroup();
    void Add(const std::function<void ()>& callback, size_t count = 1);
    void Start();
    void Join();
    size_t Size() const;
private:
    std::vector<Thread*> m_threads;
};

} // namespace toft

#endif // TOFT_SYSTEM_THREADING_THREAD_GROUP_H
