// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/system/threading/spinlock.h"
#include "toft/system/check_error.h"
#include "toft/system/threading/this_thread.h"

namespace toft {

SpinLock::SpinLock()
{
    TOFT_CHECK_PTHREAD_ERROR(pthread_spin_init(&m_lock, 0));
    m_owner = 0;
}

SpinLock::~SpinLock()
{
    TOFT_CHECK_PTHREAD_ERROR(pthread_spin_destroy(&m_lock));
    m_owner = -1;
}

void SpinLock::Lock()
{
    TOFT_CHECK_PTHREAD_ERROR(pthread_spin_lock(&m_lock));
    m_owner = ThisThread::GetId();
}

bool SpinLock::TryLock()
{
    if (TOFT_CHECK_PTHREAD_TRYLOCK_ERROR(pthread_spin_trylock(&m_lock)))
    {
        m_owner = ThisThread::GetId();
        return true;
    }
    return false;
}

void SpinLock::Unlock()
{
    m_owner = 0;
    TOFT_CHECK_PTHREAD_ERROR(pthread_spin_unlock(&m_lock));
}

} // namespace toft
