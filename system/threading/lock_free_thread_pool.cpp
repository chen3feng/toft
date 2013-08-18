// Copyright (C) 2013, The Toft Authors.
// Author: An Qin <anqin.qin@gmail.com>
//
// Description:

#include "toft/system/threading/lock_free_thread_pool.h"

#include "thirdparty/glog/logging.h"

#include "toft/system/threading/this_thread.h"

namespace toft {

LockFreeThreadPool::LockFreeThreadPool(int32_t min_thread_num,
                                       int32_t max_thread_num,
                                       int32_t idle_timeout_ms,
                                       size_t stack_size)
    : m_min_thread_num(min_thread_num),
      m_max_thread_num(max_thread_num),
      m_cur_thread_num(0), m_cur_busy_thread_num(0),
      m_pending_task_num(0),
      m_idle_timeout(idle_timeout_ms),
      m_stack_size(stack_size),
      m_exited(false) {
    CHECK_GE(m_min_thread_num, 0);
    if (m_max_thread_num < 0) {
        m_max_thread_num = m_min_thread_num + 1;
    }

    if (m_max_thread_num < m_min_thread_num) {
        m_max_thread_num = m_min_thread_num;
    }
}

LockFreeThreadPool::~LockFreeThreadPool() {
    Terminate();
}

void LockFreeThreadPool::Terminate(bool is_wait) {
    if (!m_exited) {
        if (is_wait) {
            while (m_pending_task_num > 0) {
                ThisThread::Sleep(1);
            }
        }
        m_exited = true;
    }

    while (m_cur_thread_num > 0) {
        for (int32_t i = 0; i < m_cur_thread_num; ++i) {
            m_event_new_task.Set();
        }
        m_event_relase_all_task.TimedWait(m_idle_timeout);
    }

    ReleaseAllCompleteTasks();
    ReleaseAllPendingTasks();

    ThreadNode* thread_node = NULL;
    while (m_idle_threads.Dequeue(thread_node)) {
        CHECK(thread_node != NULL);
        thread_node->Join();
        delete thread_node;
        thread_node = NULL;
    }
}

void LockFreeThreadPool::AddTask(Closure<void ()>* callback) {
    AddTaskInternal(false, callback, NULL);
}

void LockFreeThreadPool::AddTask(std::function<void ()> callback) {
    AddTaskInternal(false, NULL, callback);
}

void LockFreeThreadPool::AddTaskInternal(bool is_priority,
                                         Closure<void ()>* callback,
                                         std::function<void ()> function) {
    CHECK(!m_exited);
    TaskNode* task_node = PickCompleteTask(true);
    task_node->callback = callback;
    task_node->function = function;

    AddPendingTask(task_node, is_priority);
    if (NeedNewThread()) {
        AddThreadNodeToList();
    }
    m_event_new_task.Set();
}

bool LockFreeThreadPool::NeedNewThread() {
    if (m_cur_thread_num >= m_max_thread_num) {
        return false;
    }
    if (m_cur_thread_num < m_min_thread_num) {
        return true;
    }

    if (m_cur_busy_thread_num == m_cur_thread_num) {
        return true;
    }

    return false;
}

void LockFreeThreadPool::Task::Run() const {
    if (callback) {
        callback->Run();
    } else if (function) {
        function();
    }
}

void LockFreeThreadPool::ThreadRunner() {
    m_cur_busy_thread_num++;

    TaskNode* task_node = NULL;
    while (!m_exited) {
        task_node = PickPendingTask();
        if (task_node) {
            task_node->Run();
            AddCompleteTask(task_node);
        } else {
            m_cur_busy_thread_num--;
            if (!m_event_new_task.TimedWait(m_idle_timeout)
                && m_cur_thread_num > m_min_thread_num) {
                break;
            }
            m_cur_busy_thread_num++;
        }
    }
}

void LockFreeThreadPool::ThreadRuntine(ThreadNode* thread_node) {
    ThreadRunner();

    m_idle_threads.Enqueue(thread_node);

    m_cur_busy_thread_num--;
    m_cur_thread_num--;

    if (m_cur_thread_num == 0) {
        m_event_relase_all_task.Set();
    }
}

void LockFreeThreadPool::AddPendingTask(TaskNode* task_node, bool is_priority) {
    m_pending_tasks.Enqueue(task_node);
    m_pending_task_num++;
}

void LockFreeThreadPool::AddCompleteTask(TaskNode* task_node) {
    m_completed_tasks.Enqueue(task_node);
}

void LockFreeThreadPool::ReleaseAllCompleteTasks() {
    TaskNode* task_node = NULL;
    while (m_completed_tasks.Dequeue(task_node)) {
        CHECK(task_node != NULL);
        delete task_node;
    }
}

void LockFreeThreadPool::ReleaseAllPendingTasks() {
    TaskNode* task_node = NULL;
    while (m_pending_tasks.Dequeue(task_node)) {
        CHECK(task_node != NULL);
        m_pending_task_num--;
        delete task_node->callback;
        delete task_node;
    }
}

LockFreeThreadPool::TaskNode* LockFreeThreadPool::PickPendingTask() {
    TaskNode* task_node = NULL;
    if (!m_pending_tasks.Dequeue(task_node)) {
        task_node = NULL;
    } else {
        m_pending_task_num--;
    }

    return task_node;
}

LockFreeThreadPool::TaskNode* LockFreeThreadPool::PickCompleteTask(bool is_new) {
    TaskNode* task_node = NULL;
    if (!m_completed_tasks.Dequeue(task_node)) {
        task_node = NULL;
    }

    if (!task_node && is_new) {
        task_node = new TaskNode;
    }

    return task_node;
}

void LockFreeThreadPool::AddThreadNodeToList() {
    ThreadNode* thread_node = NULL;
    if (m_idle_threads.Dequeue(thread_node)) {
        thread_node->Join();
        thread_node->~ThreadNode();
        new (thread_node) ThreadNode();
    } else {
        thread_node = new ThreadNode();
    }

    m_cur_thread_num++;
    if (m_stack_size > 0) {
        thread_node->SetStackSize(m_stack_size);
    }
    thread_node->Start(std::bind(&LockFreeThreadPool::ThreadRuntine, this, thread_node));
}

} // namespace toft
