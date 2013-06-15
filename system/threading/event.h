// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_SYSTEM_THREADING_EVENT_H
#define TOFT_SYSTEM_THREADING_EVENT_H

#include <stdint.h>

#include "toft/base/uncopyable.h"
#include "toft/system/threading/condition_variable.h"
#include "toft/system/threading/mutex.h"

namespace toft {

// The AutoResetEvent class represents a local waitable event that resets
// automatically when signaled, after releasing a single waiting thread.
//
// AutoResetEvent allows threads to communicate with each other by signaling.
// Typically, you use this class when threads need exclusive access to a resource.
//
// Important
// There is no guarantee that every call to the Set method will release a thread.
// If two calls are too close together, so that the second call occurs before a
// thread has been released, only one thread is released. It is as if the second
// call did not happen. Also, if Set is called when there are no threads waiting
// and the AutoResetEvent is already signaled, the call has no effect.
//
// If you want to release a thread after each call, Semaphore is a good choice.
class AutoResetEvent
{
    TOFT_DECLARE_UNCOPYABLE(AutoResetEvent);
public:
    // init_state: initial state is signaled or non-signaled
    explicit AutoResetEvent(bool init_state = false);

    ~AutoResetEvent();

    // Calling thread waits for a signal by calling Wait on the AutoResetEvent.
    // If the event is in the non-signaled state, the thread blocks, waiting for
    // the thread that currently controls the resource to signal that the
    // resource is available by calling Set.
    //
    // If a thread calls Wait while the AutoResetEvent is in the signaled state,
    // the thread does not block. The AutoResetEvent releases the thread
    // immediately and returns to the non-signaled state.
    void Wait();

    // Wait with timout, in milliseconds.
    // return true if success, false if timeout
    bool TimedWait(int64_t timeout);

    // Try to wait the event.
    // return true immediately if signaled, else return false
    bool TryWait();

    // Signals the event to release a waiting thread.
    // The event remains signaled until a single waiting thread is released,
    // and then automatically returns to the non-signaled state. If no threads
    // are waiting, the state remains signaled indefinitely.
    void Set();

    // Sets the state of the event to nonsignaled, causing threads to block.
    void Reset();

private:
    Mutex m_mutex;
    ConditionVariable m_cond;
    bool m_signaled;
};

// This class represents a local waitable event object that must be reset
// manually after it is signaled.
//
// ManualResetEvent allows threads to communicate with each other by signaling.
// Typically, this communication concerns a task which one thread must complete
// before other threads can proceed.
//
// The object remains signaled until its Reset method is called. Any number of
// waiting threads, or threads that wait on the event after it has been signaled,
// can be released while the object's state is signaled.
class ManualResetEvent
{
    TOFT_DECLARE_UNCOPYABLE(ManualResetEvent);
public:
    // init_state: initial state is signaled or non-signaled
    explicit ManualResetEvent(bool init_state = false);
    ~ManualResetEvent();

    // Blocks the current thread until the current WaitHandle receives a signal.
    void Wait();

    // Wait with timout, in milliseconds.
    // return true if success, false if timeout
    bool TimedWait(int64_t timeout);

    // Try to wait the event.
    bool TryWait();

    // Sets the state of the event to signaled, allowing one or more waiting
    // threads to proceed.
    void Set();

    // Sets the state of the event to nonsignaled, causing threads to block.
    void Reset();

private:
    Mutex m_mutex;
    ConditionVariable m_cond;
    bool m_signaled;
};

} // namespace toft

#endif // TOFT_SYSTEM_THREADING_EVENT_H
