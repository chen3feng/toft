// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/system/threading/thread_group.h"

#include <assert.h>

#include "toft/base/scoped_ptr.h"

namespace toft {

ThreadGroup::ThreadGroup()
{
}

ThreadGroup::ThreadGroup(const std::function<void ()>& callback, size_t count)
{
    Add(callback, count);
}

ThreadGroup::~ThreadGroup()
{
    for (size_t i = 0; i < m_threads.size(); ++i)
    {
        if (m_threads[i]->IsJoinable())
            m_threads[i]->Join();
        delete m_threads[i];
    }
    m_threads.clear();
}

void ThreadGroup::Add(const std::function<void ()>& callback, size_t count)
{
    for (size_t i = 0; i < count; ++i)
    {
        scoped_ptr<Thread> thread(new Thread(callback));
        m_threads.push_back(thread.get());
        thread.release();
    }
}

void ThreadGroup::Join()
{
    // TODO(chen3feng): using pthread_barrier
    for (size_t i = 0; i < m_threads.size(); ++i)
        m_threads[i]->Join();
}

size_t ThreadGroup::Size() const
{
    return m_threads.size();
}

} // namespace toft
