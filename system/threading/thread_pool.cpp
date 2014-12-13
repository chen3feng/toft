// Copyright (C) 2011, The Toft Authors.
// Authors: An Qin <anqin.qin@gmail.com>
//          CHEN Feng <chen3feng@gmail.com>
// Created: 2010-06-18
//
// Description:

#include "toft/system/threading/thread_pool.h"

#include "thirdparty/glog/logging.h"

#include "toft/base/scoped_ptr.h"
#include "toft/base/string/concat.h"
#include "toft/system/info/info.h"
#include "toft/system/threading/this_thread.h"

namespace toft {

struct ThreadPool::Task {
    Task(Closure<void()>* cb, const std::function<void ()>& f) :
        callback(cb), function(f)
    {
    }
    list_node link;
    Closure<void()>* callback;
    std::function<void ()> function;
};

struct ThreadPool::ThreadContext {
    typedef intrusive_list<ThreadPool::Task> TaskList;
    ThreadContext() : cond(&mutex), exit(false) {}
    scoped_ptr<Thread> thread;
    mutable Mutex mutex;
    ConditionVariable cond;
    TaskList pending_tasks;  // __attribute__((aligned(64)));
    TaskList free_tasks;  // __attribute__((aligned(64)));
    bool exit __attribute__((aligned(64)));

    bool GetPendingTask(TaskList* tasks);
} __attribute__((aligned(64)));  // Make cache alignment.

// Return whether should exit. Note even if return false, the task may be not
// empty because of remaining tasks before exit was set.
bool ThreadPool::ThreadContext::GetPendingTask(TaskList* tasks) {
    MutexLocker locker(&mutex);
    free_tasks.splice(*tasks);
    while (pending_tasks.empty() && !exit) {
        cond.Wait();
    }
    tasks->splice(pending_tasks);
    return !exit;
}

ThreadPool::ThreadPool(int num_threads):
    m_num_threads(0), m_num_busy_threads(0), m_exit_cond(&m_exit_lock), m_exit(false) {
    if (num_threads < 0)
        m_num_threads = GetLogicalCpuNumber();
    else if (num_threads == 0)
        m_num_threads = 1;
    else
        m_num_threads = num_threads;

    m_thread_contexts = new ThreadContext[m_num_threads];
    ThreadAttributes attr;
    for (size_t i = 0; i < m_num_threads; ++i) {
        attr.SetName(StringConcat("threadpool/", i));
        m_thread_contexts[i].thread.reset(
            new Thread(attr, std::bind(&ThreadPool::WorkRoutine, this, &m_thread_contexts[i])));
    }
    m_num_busy_threads = m_num_threads;
}


ThreadPool::~ThreadPool() {
    Terminate();
}

void ThreadPool::AddTaskInternal(
    Closure<void()>* callback,
    const std::function<void()>& function,
    int dispatch_key)
{
    DCHECK(!m_exit);
    ThreadContext& context = m_thread_contexts[dispatch_key % m_num_threads];
    {
        Task* task;
        MutexLocker locker(&context.mutex);
        if (!context.free_tasks.empty()) {
            task = &context.free_tasks.front();
            context.free_tasks.pop_front();
            task->callback = callback;
            task->function = function;
        } else {
            task = new Task(callback, function);
        }

        context.pending_tasks.push_back(task);
    }
    context.cond.Signal();
}

void ThreadPool::AddTask(Closure<void()>* callback) {
    // The memory address is random enough for load balance, but need
    // remove low alignment part. (The lowest 5 bits of allocated object
    // address are always 0 for 64 bit system).
    unsigned int dispatch_key = reinterpret_cast<uintptr_t>(callback) / 32;
    AddTask(callback, dispatch_key);
}

int rdtsc() { return 0; }

void ThreadPool::AddTask(Closure<void ()>* callback, int dispatch_key)
{
    AddTaskInternal(callback, NULL, dispatch_key);
}

void ThreadPool::AddTask(const std::function<void ()>& callback)
{
    AddTask(callback, rdtsc());
}

void ThreadPool::AddTask(const std::function<void ()>& callback, int dispatch_key)
{
    AddTaskInternal(NULL, callback, dispatch_key);
}

void ThreadPool::WorkRoutine(ThreadContext* context) {
    ThreadContext::TaskList tasks;
    for (;;) {
        bool continued = context->GetPendingTask(&tasks);
        ThreadContext::TaskList::iterator i;
        for (i = tasks.begin(); i != tasks.end(); ++i) {
            if (i->callback) {
                i->callback->Run();
                i->callback = NULL;
            } else {
                i->function();
                i->function = NULL;
            }
        }
        if (!continued)
            break;
    }

    // Clear free_tasks list
    // Exiting, so no other context will operate this list.
    context->free_tasks.splice(tasks);
    while (!context->free_tasks.empty()) {
        Task* task = &context->free_tasks.front();
        context->free_tasks.pop_front();
        delete task;
    }
    MutexLocker locker(&m_exit_lock);
    if (--m_num_busy_threads == 0)
        m_exit_cond.Signal();
}

bool ThreadPool::AnyTaskPending() const {
    for (size_t i = 0; i < m_num_threads; ++i) {
        MutexLocker locker(&m_thread_contexts[i].mutex);
        if (!m_thread_contexts[i].pending_tasks.empty())
            return true;
    }

    return false;
}

bool ThreadPool::AnyThreadRunning() const {
    for (size_t i = 0; i < m_num_threads; ++i) {
        if (m_thread_contexts[i].thread->IsAlive())
            return true;
    }
    return false;
}

void ThreadPool::Terminate() {
    MutexLocker lock(&m_exit_lock);
    if (m_exit)
        return;

    m_exit = true;
    for (size_t i = 0; i < m_num_threads; ++i) {
        MutexLocker locker(&m_thread_contexts[i].mutex);
        m_thread_contexts[i].exit = true;
        m_thread_contexts[i].cond.Signal();
    }
    while (m_num_busy_threads > 0)
        m_exit_cond.Wait();

    for (size_t i = 0; i < m_num_threads; ++i)
        m_thread_contexts[i].thread->Join();

    delete[] m_thread_contexts;
    m_thread_contexts = NULL;
    m_num_threads = 0;
}

void ThreadPool::WaitForIdle() {
    assert(!m_exit);
    while (AnyTaskPending()) {
        ThisThread::Sleep(1);
    }
}

} // namespace toft
