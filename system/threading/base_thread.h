// Copyright (c) 2011, The Toft Authors. All rights reserved.
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_SYSTEM_THREADING_BASE_THREAD_H
#define TOFT_SYSTEM_THREADING_BASE_THREAD_H

#include <string>

#include "toft/system/threading/this_thread.h"
#include "toft/system/threading/thread_types.h"

namespace toft {

class Thread;

// BaseThread class is designed to be used as base class,
// and override the Entry virtual method.
class BaseThread
{
    friend class Thread;

public:
    typedef ThreadHandleType HandleType;

protected:
    BaseThread();

public:
    virtual ~BaseThread();
    void SetStackSize(size_t size);

    // Start or TryStary Must be called to start a thread, or the thread will
    // not be created.
    // But can't be called again unless joined or detached.

    // Return false if failed
    bool TryStart();

    // If failed, abort the process.
    void Start();

    // Wait for thread termination
    // the thread must be started and not detached
    bool Join();

    // Whether the SendStopRequest or StopAndWaitForExit called
    bool IsStopRequested() const;

    // Make IsStopRequested return true
    void SendStopRequest();

    // call SendStopRequest and then Join
    bool StopAndWaitForExit();

    // Whether the thread is still alive
    bool IsAlive() const;

    // Is fitable to call Join?
    bool IsJoinable() const;

    // Get handle
    HandleType GetHandle() const;

    // Get numeric thread id
    int GetId() const;

protected:
    // This virtual function will be called before the thread exiting
    virtual void OnExit();

private:
    int DoStart();

    // BaseThread should not support Detach, because the running thread can't
    // control the memory of BaseThread object. So if it is detached and the
    // BaseThread destructed, the running thread will access the destructed
    // object, make it private.
    bool DoDetach();

    // The derived class must override this function as the thread entry point
    virtual void Entry() = 0;

    // Do some class shared initializing
    static void Cleanup(void* param);

private:
    static void* StaticEntry(void* inThread);

private:
    HandleType m_handle;
    int m_id;
    size_t m_stack_size;
    volatile bool m_stop_requested;
    volatile bool m_is_alive;
};

inline void BaseThread::SendStopRequest()
{
    m_stop_requested = true;
}

inline bool BaseThread::IsStopRequested() const
{
    return m_stop_requested;
}

inline ThreadHandleType BaseThread::GetHandle() const
{
    return m_handle;
}

} // namespace toft

#endif // TOFT_SYSTEM_THREADING_BASE_THREAD_H

