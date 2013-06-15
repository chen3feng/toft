// Copyright (c) 2011, The Toft Authors. All rights reserved.
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2010-06-18

#include "toft/system/threading/event.h"

#include <stdlib.h>

#include "toft/system/check_error.h"

namespace toft {

//////////////////////////////////////////////////////////////////////////////
// AutoResetEvent staff

AutoResetEvent::AutoResetEvent(bool init_state) :
    m_mutex(),
    m_cond(&m_mutex),
    m_signaled(init_state)
{
}

AutoResetEvent::~AutoResetEvent()
{
}

void AutoResetEvent::Wait()
{
    MutexLocker locker(&m_mutex);
    while (!m_signaled)
        m_cond.Wait();
    m_signaled = false;
}

bool AutoResetEvent::TimedWait(int64_t timeout)
{
    MutexLocker locker(&m_mutex);
    if (!m_signaled)
        m_cond.TimedWait(timeout);

    if (!m_signaled)
        return false;

    m_signaled = false;
    return true;
}

bool AutoResetEvent::TryWait()
{
    return TimedWait(0);
}

void AutoResetEvent::Set()
{
    MutexLocker locker(&m_mutex);
    m_signaled = true;
    m_cond.Signal();
}

void AutoResetEvent::Reset()
{
    MutexLocker locker(&m_mutex);
    m_signaled = false;
}

//////////////////////////////////////////////////////////////////////////////
// ManualResetEvent staff

ManualResetEvent::ManualResetEvent(bool init_state) :
    m_mutex(),
    m_cond(&m_mutex),
    m_signaled(init_state)
{
}

ManualResetEvent::~ManualResetEvent()
{
}

void ManualResetEvent::Wait()
{
    MutexLocker locker(&m_mutex);
    while (!m_signaled)
        m_cond.Wait();
}

bool ManualResetEvent::TimedWait(int64_t timeout)
{
    MutexLocker locker(&m_mutex);
    if (!m_signaled)
        m_cond.TimedWait(timeout);
    return m_signaled;
}

bool ManualResetEvent::TryWait()
{
    return TimedWait(0);
}

void ManualResetEvent::Set()
{
    MutexLocker locker(&m_mutex);
    m_signaled = true;
    m_cond.Broadcast();
}

void ManualResetEvent::Reset()
{
    MutexLocker locker(&m_mutex);
    m_signaled = false;
}

} // namespace toft

