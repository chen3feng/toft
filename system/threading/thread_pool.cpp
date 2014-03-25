// Copyright (C) 2013, The Toft Authors.
// Author: An Qin <anqin.qin@gmail.com>
//
// Description:

#include "toft/system/threading/thread_pool.h"

#include "thirdparty/glog/logging.h"

#include "toft/system/threading/this_thread.h"

namespace toft {

ThreadPool::ThreadPool(int32_t min_thread_num,
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

ThreadPool::~ThreadPool() {
    Terminate();
}

void ThreadPool::Terminate(bool is_wait) {
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

    MutexLocker locker(&m_mutex_thread);
    while (!m_idle_threads.empty()) {
        ThreadNode* thread_node = &m_idle_threads.front();
        m_idle_threads.pop_front();
        thread_node->Join();
        delete thread_node;
    }
}

void ThreadPool::AddTask(Closure<void ()>* callback) {
    AddTaskInternal(false, callback, NULL);
}

void ThreadPool::AddTask(std::function<void ()> callback) {
    AddTaskInternal(false, NULL, callback);
}

void ThreadPool::AddPriorityTask(Closure<void ()>* callback) {
    AddTaskInternal(true, callback, NULL);
}

void ThreadPool::AddPriorityTask(std::function<void ()> callback) {
    AddTaskInternal(true, NULL, callback);
}

void ThreadPool::AddTaskInternal(bool is_priority,
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

bool ThreadPool::NeedNewThread() {
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

void ThreadPool::Task::Run() const {
    if (callback) {
        callback->Run();
    } else if (function) {
        function();
    }
}

void ThreadPool::ThreadRunner() {
    m_cur_busy_thread_num++;

    TaskNode* task_node = NULL;
    while (!m_exited) {
        task_node = PickPendingTask();
        if (task_node) {
            task_node->Run();
            AddCompleteTask(task_node);
        } else {
            --m_cur_busy_thread_num;
            bool got = m_event_new_task.TimedWait(m_idle_timeout);
            ++m_cur_busy_thread_num;
            if (!got && m_pending_tasks.empty() &&
                m_cur_thread_num > m_min_thread_num) {
                break;
            }
        }
    }
}

void ThreadPool::ThreadRuntine(ThreadNode* thread_node) {
    ThreadRunner();

    MutexLocker locker(&m_mutex_thread);
    m_working_threads.erase(thread_node);
    m_idle_threads.push_back(thread_node);
    m_cur_busy_thread_num--;
    m_cur_thread_num--;

    if (m_cur_thread_num == 0) {
        m_event_relase_all_task.Set();
    }
}

void ThreadPool::AddPendingTask(TaskNode* task_node, bool is_priority) {
    MutexLocker locker(&m_mutex_task);
    if (is_priority) {
        m_pending_tasks.push_front(task_node);
    } else {
        m_pending_tasks.push_back(task_node);
    }
    m_pending_task_num++;
}

void ThreadPool::AddCompleteTask(TaskNode* task_node) {
    MutexLocker locker(&m_mutex_task);
    m_completed_tasks.push_back(task_node);
}

void ThreadPool::ReleaseAllCompleteTasks() {
    MutexLocker locker(&m_mutex_task);
    while (!m_completed_tasks.empty()) {
        TaskNode* task_node = &m_completed_tasks.front();
        m_completed_tasks.pop_front();
        delete task_node;
    }
}

void ThreadPool::ReleaseAllPendingTasks() {
    MutexLocker locker(&m_mutex_task);
    while (!m_pending_tasks.empty()) {
        TaskNode* task_node = &m_pending_tasks.front();
        m_pending_tasks.pop_front();
        m_pending_task_num--;
        delete task_node->callback;
        delete task_node;
    }
}

ThreadPool::TaskNode* ThreadPool::PickPendingTask() {
    TaskNode* task_node = NULL;
    MutexLocker locker(&m_mutex_task);
    if (!m_pending_tasks.empty()) {
        task_node = &m_pending_tasks.front();
        m_pending_tasks.pop_front();
        m_pending_task_num--;
    }

    return task_node;
}

ThreadPool::TaskNode* ThreadPool::PickCompleteTask(bool is_new) {
    TaskNode* task_node = NULL;
    {
        MutexLocker locker(&m_mutex_task);
        if (!m_completed_tasks.empty()) {
            task_node = &m_completed_tasks.front();
            m_completed_tasks.pop_front();
        }
    }
    if (!task_node && is_new) {
        task_node = new TaskNode;
    }

    return task_node;
}

void ThreadPool::AddThreadNodeToList() {
    ThreadNode* thread_node = NULL;
    MutexLocker locker(&m_mutex_thread);

    if (!m_idle_threads.empty()) {
        thread_node = &m_idle_threads.front();
        m_idle_threads.pop_front();
        thread_node->Join();

        thread_node->~ThreadNode();
        new (thread_node) ThreadNode();
    } else {
        thread_node = new ThreadNode();
    }
    m_working_threads.push_back(thread_node);
    m_cur_thread_num++;
    if (m_stack_size > 0) {
        thread_node->SetStackSize(m_stack_size);
    }
    thread_node->Start(std::bind(&ThreadPool::ThreadRuntine, this, thread_node));
}

} // namespace toft
