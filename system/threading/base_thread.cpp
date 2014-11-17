// Copyright (c) 2011, The Toft Authors.
// All rights reserved.

#include "toft/system/threading/base_thread.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "toft/system/atomic/atomic.h"
#include "toft/system/check_error.h"

namespace toft {


BaseThread::BaseThread():
    m_handle(),
    m_id(-1),
    m_is_alive(false)
{
}

BaseThread::BaseThread(const ThreadAttributes& attributes) :
    m_attributes(attributes),
    m_handle(),
    m_id(-1),
    m_is_alive(false) {
}

BaseThread::~BaseThread()
{
    // What we're trying to do is allow the thread we want to delete to complete
    // running. So we wait for it to stop.
    if (IsJoinable())
        Join();
}

int BaseThread::DoStart()
{
    // Start can only be used on a new born or a started but already joined
    // or a detached Thread object. In other words, not associated to any
    // system thread, both alive and dead.
    if (IsJoinable())
    {
        // If crash here, means the thread is still alive or finished but not
        // joined.
        TOFT_CHECK_ERRNO_ERROR(EINVAL);
    }

    m_handle = ThreadHandleType();
    m_id = 0;

    const pthread_attr_t &attr = m_attributes.m_attr;
    int error = pthread_create(&m_handle, &attr, StaticEntry, this);
    if (error)
        m_id = -1;
    return error;
}

void BaseThread::Start()
{
    TOFT_CHECK_PTHREAD_ERROR(DoStart());
}

bool BaseThread::TryStart()
{
    int error = DoStart();
    if (error == 0)
        return true;
    if (error != EAGAIN)
        TOFT_CHECK_PTHREAD_ERROR(error);
    return false;
}

bool BaseThread::DoDetach()
{
    TOFT_CHECK_PTHREAD_ERROR(pthread_detach(m_handle));
    m_handle = HandleType();
    m_id = -1;
    return true;
}

bool BaseThread::Join()
{
    assert(IsJoinable());
    TOFT_CHECK_PTHREAD_ERROR(pthread_join(m_handle, NULL));
    m_handle = pthread_t();
    m_id = -1;
    return true;
}

int BaseThread::GetId() const
{
    if (m_id != 0)
        return m_id;

    // GetId is rarely used, so busy wait is more fitness
    while (AtomicGet(&m_id) == 0)
        ThisThread::Sleep(1);

    return m_id;
}

bool BaseThread::IsAlive() const
{
    return m_is_alive;
}

bool BaseThread::IsJoinable() const
{
    return !pthread_equal(m_handle, pthread_t());
}

// make sure execute before exit
void BaseThread::Cleanup(void* param)
{
    BaseThread* thread = static_cast<BaseThread*>(param);
    thread->m_is_alive = false;
    thread->OnExit();
}

void BaseThread::OnExit()
{
}

void* BaseThread::StaticEntry(void* param)
{
    BaseThread* base_thread = static_cast<BaseThread*>(param);
    bool detached = base_thread->m_attributes.IsDetached();
    if (!detached) {
        base_thread->m_is_alive = true;
        base_thread->m_id = ThisThread::GetId();
    }
    const std::string& name = base_thread->m_attributes.m_name;
    if (!name.empty()) {
        // Set thread name for easy debugging.
#if __GLIBC__ > 2 || __GLIBC__ == 2 && __GLIBC_MINOR__ >= 12
        pthread_setname_np(pthread_self(), name.c_str());
#else
        prctl(PR_SET_NAME, name.c_str(), 0, 0, 0);
#endif
    }
    pthread_cleanup_push(Cleanup, param);
    base_thread->Entry();
    base_thread->m_is_alive = false;
    pthread_cleanup_pop(true);

    return 0;
}

} // namespace toft
