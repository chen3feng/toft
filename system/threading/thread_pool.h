// Copyright (C) 2013, The Toft Authors.
// Author: An Qin <anqin.qin@gmail.com>
//
// Description:

#ifndef TOFT_SYSTEM_THREADING_THREAD_POOL_H
#define TOFT_SYSTEM_THREADING_THREAD_POOL_H

#include <stdint.h>

#include "toft/base/closure.h"
#include "toft/base/functional.h"
#include "toft/base/intrusive_list.h"
#include "toft/system/threading/event.h"
#include "toft/system/threading/mutex.h"
#include "toft/system/threading/thread.h"

namespace toft {

class ThreadPool {
public:
    /// @param mun_threads number of threads, -1 means cpu number
    explicit ThreadPool(int num_threads = -1);
    ~ThreadPool();


    void AddTask(Closure<void ()>* callback);
    void AddTask(const std::function<void ()>& callback);

    void AddTask(Closure<void ()>* callback, int dispatch_key);
    void AddTask(const std::function<void ()>& callback, int dispatch_key);

    void WaitForIdle();
    void Terminate();

private:
    struct Task;
    struct ThreadContext;

    void AddTaskInternal(Closure<void ()>* callback,
                         const std::function<void ()>& function,
                         int dispatch_key);

    bool AnyTaskPending() const;
    void WorkRoutine(ThreadContext* thread);
    bool AnyThreadRunning() const;

private:
    ThreadContext* m_thread_contexts;
    size_t m_num_threads;
    size_t m_num_busy_threads;
    Mutex m_exit_lock;
    ConditionVariable m_exit_cond;
    bool m_exit;
};

} // namespace toft

#endif // TOFT_SYSTEM_THREADING_THREAD_POOL_H
