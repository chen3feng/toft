// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/system/threading/rwlock.h"

#include <string.h>

namespace toft {

RwLock::RwLock()
{
    // Note: default rwlock is prefer reader
    TOFT_CHECK_PTHREAD_ERROR(pthread_rwlock_init(&m_lock, NULL));
}

RwLock::RwLock(Kind kind)
{
    pthread_rwlockattr_t attr;
    TOFT_CHECK_PTHREAD_ERROR(pthread_rwlockattr_init(&attr));
    TOFT_CHECK_PTHREAD_ERROR(pthread_rwlockattr_setkind_np(&attr, kind));
    TOFT_CHECK_PTHREAD_ERROR(pthread_rwlock_init(&m_lock, &attr));
    TOFT_CHECK_PTHREAD_ERROR(pthread_rwlockattr_destroy(&attr));
}

RwLock::~RwLock()
{
    TOFT_CHECK_PTHREAD_ERROR(pthread_rwlock_destroy(&m_lock));
    memset(&m_lock, 0xFF, sizeof(m_lock));
}

} // namespace toft
