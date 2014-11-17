// Copyright (c) 2011, The Toft Authors. All rights reserved.
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2010-06-18

#ifndef TOFT_SYSTEM_THREADING_THREAD_H_INCLUDED
#define TOFT_SYSTEM_THREADING_THREAD_H_INCLUDED

#include "toft/base/functional.h"
#include "toft/base/uncopyable.h"
#include "toft/system/threading/thread_types.h"

namespace toft {

class Thread
{
    TOFT_DECLARE_UNCOPYABLE(Thread);
    class Impl;
public:
    // Default ctor, construct a invalid thread object, must call Initialize
    // befor Start.
    Thread();

    explicit Thread(const std::function<void ()>& function);
    explicit Thread(const ThreadAttributes& attr, const std::function<void ()>& function);

    virtual ~Thread();

    // Can only call before Start.
    void SetStackSize(size_t size);

    // A thread will not be started unless Start is called.
    // If failed, fatal error occured.
    void Start(const std::function<void ()>& function);

    // If failed because limitation, return false.
    bool TryStart(const std::function<void ()>& function);

    // Wait for a thread finish.
    // Only not detached thread call be joined.
    // If the thread is already stoped, return true.
    bool Join();

    // Detach the running thread from the Thread object.
    // After detached, can't operate the thread by the thread object.
    void Detach();

    void SendStopRequest();
    bool IsStopRequested() const;
    bool StopAndWaitForExit();

    // Retuen whether the thread is still alive.
    bool IsAlive() const;

    // Whether safe to call Join.
    bool IsJoinable() const;

    // Obtain system-spec thread handle
    ThreadHandleType GetHandle() const;

    // Return integral thread id.
    int GetId() const;

private:
    // Make the compile check misuse Thread as BaseThread
    // Note the prototype is different with BaseThread::Entry
    virtual void Entry() const {}

private:
    Impl* m_pimpl;
};

} // namespace toft

#endif // TOFT_SYSTEM_THREADING_THREAD_H_INCLUDED

