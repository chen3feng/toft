// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_SYSTEM_THREADING_SPINLOCK_H
#define TOFT_SYSTEM_THREADING_SPINLOCK_H

#include <errno.h>
#include <stdlib.h>

#include <pthread.h>

#include "toft/base/uncopyable.h"
#include "toft/system/threading/scoped_locker.h"

namespace toft {

// SpinLock is faster than mutex at some condition, but
// some time may be slower, be careful!
class SpinLock
{
    TOFT_DECLARE_UNCOPYABLE(SpinLock);
public:
    typedef ScopedLocker<SpinLock> Locker;
public:
    SpinLock();
    ~SpinLock();
    void Lock();
    bool TryLock();
    void Unlock();
private:
    pthread_spinlock_t m_lock;
    pid_t m_owner;
};

} // namespace toft

#endif // TOFT_SYSTEM_THREADING_SPINLOCK_H

