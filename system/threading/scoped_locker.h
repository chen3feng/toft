// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_SYSTEM_THREADING_SCOPED_LOCKER_H
#define TOFT_SYSTEM_THREADING_SCOPED_LOCKER_H

// Scoped locker, is just like scoped pointer

#include "toft/base/uncopyable.h"

// GLOBAL_NOLINT(runtime/references)

namespace toft {

template <typename LockType>
class ScopedLocker : private Uncopyable
{
public:
    explicit ScopedLocker(LockType* lock)
        : m_lock(lock)
    {
        m_lock->Lock();
    }
    ~ScopedLocker()
    {
        m_lock->Unlock();
    }
private:
    LockType* m_lock;
};

template <typename LockType>
class ScopedTryLocker : private Uncopyable
{
public:
    explicit ScopedTryLocker(LockType* lock)
        : m_lock(lock)
    {
        m_locked = m_lock->TryLock();
    }
    ~ScopedTryLocker()
    {
        if (m_locked)
            m_lock->Unlock();
    }
    bool IsLocked() const
    {
        return m_locked;
    }
private:
    LockType* m_lock;
    bool m_locked;
};

template <typename LockType>
class ScopedReaderLocker : private Uncopyable
{
public:
    explicit ScopedReaderLocker(LockType* lock)
        : m_lock(lock)
    {
        m_lock->ReaderLock();
    }
    ~ScopedReaderLocker()
    {
        m_lock->ReaderUnlock();
    }
private:
    LockType* m_lock;
};

template <typename LockType>
class ScopedTryReaderLocker : private Uncopyable
{
public:
    explicit ScopedTryReaderLocker(LockType* lock)
        : m_lock(lock)
    {
        m_locked = m_lock->TryReaderLock();
    }
    ~ScopedTryReaderLocker()
    {
        if (m_locked)
            m_lock->ReaderUnlock();
    }
    bool IsLocked() const
    {
        return m_locked;
    }
private:
    LockType* m_lock;
    bool m_locked;
};

template <typename LockType>
class ScopedWriterLocker : private Uncopyable
{
public:
    explicit ScopedWriterLocker(LockType* lock) : m_lock(*lock)
    {
        m_lock.WriterLock();
    }
    ~ScopedWriterLocker()
    {
        m_lock.WriterUnlock();
    }
private:
    LockType& m_lock;
};

template <typename LockType>
class ScopedTryWriterLocker : private Uncopyable
{
public:
    explicit ScopedTryWriterLocker(LockType* lock)
        : m_lock(lock)
    {
        m_locked = m_lock->TryWriterLock();
    }
    ~ScopedTryWriterLocker()
    {
        if (m_locked)
            m_lock->WriterUnlock();
    }
    bool IsLocked() const
    {
        return m_locked;
    }
private:
    LockType* m_lock;
    bool m_locked;
};

} // namespace toft

#endif // TOFT_SYSTEM_THREADING_SCOPED_LOCKER_H
