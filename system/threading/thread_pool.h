// Copyright (C) 2013, The Toft Authors.
// Author: An Qin <anqin.qin@gmail.com>
//
// Description:

#ifndef TOFT_SYSTEM_THREADING_THREAD_POOL_H
#define TOFT_SYSTEM_THREADING_THREAD_POOL_H

#include <stdint.h>

#include "toft/base/closure.h"
#include "toft/base/intrusive_list.h"
#include "toft/system/atomic/atomic.h"
#include "toft/system/threading/event.h"
#include "toft/system/threading/mutex.h"
#include "toft/system/threading/thread.h"

namespace toft {

class ThreadPool {
public:
    explicit ThreadPool(int32_t min_thread_num = 0,
                        int32_t max_thread_num = -1,
                        int32_t idle_timeout_ms = 60000, /* in ms */
                        size_t stack_size = 0);
    ~ThreadPool();

    void Terminate(bool is_wait = true);

    void AddTask(Closure<void ()>* callback);
    void AddTask(std::function<void ()> callback);

    void AddPriorityTask(Closure<void ()>* callback);
    void AddPriorityTask(std::function<void ()> callback);

private:
    struct Task {
        Closure<void ()>* callback;
        std::function<void ()> function;

        void Run() const;
    };

    struct TaskNode : public Task {
        list_node link;
    };

    struct ThreadNode : public Thread {
        list_node link;
    };

    void AddTaskInternal(bool is_priority,
                         Closure<void ()>* callback,
                         std::function<void ()> function);

    bool NeedNewThread();
    void ThreadRunner();
    void ThreadRuntine(ThreadNode* thread_node);

    void AddPendingTask(TaskNode* task_node, bool is_priority = false);
    void AddCompleteTask(TaskNode* task_node);
    void ReleaseAllPendingTasks();
    void ReleaseAllCompleteTasks();

    TaskNode* PickPendingTask();
    TaskNode* PickCompleteTask(bool is_new = false);

    void AddThreadNodeToList();

private:
    mutable Mutex m_mutex_task;
    mutable Mutex m_mutex_thread;
    AutoResetEvent m_event_new_task;
    AutoResetEvent m_event_relase_all_task;

    intrusive_list<TaskNode> m_pending_tasks;
    intrusive_list<TaskNode> m_completed_tasks;
    intrusive_list<ThreadNode> m_working_threads;
    intrusive_list<ThreadNode> m_idle_threads;

    int32_t m_min_thread_num;
    int32_t m_max_thread_num;
    Atomic<int32_t> m_cur_thread_num;
    Atomic<int32_t> m_cur_busy_thread_num;
    int32_t m_pending_task_num;
    int32_t m_idle_timeout;
    size_t m_stack_size;
    Atomic<bool> m_exited;
};

} // namespace toft

#endif // TOFT_SYSTEM_THREADING_THREAD_POOL_H
