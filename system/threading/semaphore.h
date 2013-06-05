// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_SYSTEM_THREADING_SEMAPHORE_H
#define TOFT_SYSTEM_THREADING_SEMAPHORE_H

#include <errno.h>
#include <semaphore.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string>

#include "toft/base/uncopyable.h"
#include "toft/system/check_error.h"
#include "toft/system/eintr_ignored.h"
#include "toft/system/time/posix_time.h"

namespace toft {
namespace internal {

class SemaphoreOps
{
public:
    static void Acquire(sem_t* sem)
    {
        TOFT_CHECK_POSIX_ERROR(TOFT_EINTR_IGNORED(sem_wait(sem)));
    }

    static bool TryAcquire(sem_t* sem);
    static bool TimedAcquire(sem_t* sem, int64_t timeout);

    static void Release(sem_t* sem)
    {
        TOFT_CHECK_POSIX_ERROR(sem_post(sem));
    }

    static unsigned int GetValue(const sem_t* sem)
    {
        int value;
        TOFT_CHECK_POSIX_ERROR(sem_getvalue(const_cast<sem_t*>(sem), &value));
        return value;
    }
};

} // namespace internal

class Semaphore
{
    TOFT_DECLARE_UNCOPYABLE(Semaphore);

public:
    explicit Semaphore(unsigned int value)
    {
        TOFT_CHECK_POSIX_ERROR(sem_init(&m_sem, false, value));
    }

    ~Semaphore()
    {
        TOFT_CHECK_POSIX_ERROR(sem_destroy(&m_sem));
    }

    void Acquire()
    {
        internal::SemaphoreOps::Acquire(&m_sem);
    }

    bool TryAcquire()
    {
        return internal::SemaphoreOps::TryAcquire(&m_sem);
    }

    bool TimedAcquire(int64_t timeout) // in ms
    {
        return internal::SemaphoreOps::TimedAcquire(&m_sem, timeout);
    }

    void Release()
    {
        return internal::SemaphoreOps::Release(&m_sem);
    }

    // Usually get value is only used for debug propose,
    // be careful your design if you need it.
    unsigned int GetValue() const
    {
        return internal::SemaphoreOps::GetValue(&m_sem);
    }

private:
    sem_t m_sem;
};

/// named semaphone can be used for cross process sync
class NamedSemaphore
{
    TOFT_DECLARE_UNCOPYABLE(NamedSemaphore);

public:
    explicit NamedSemaphore() : m_sem(NULL) {}
    explicit NamedSemaphore(const std::string& name);
    explicit NamedSemaphore(const std::string& name,
                            unsigned int value,
                            mode_t mode = 0666);
    ~NamedSemaphore();

    bool Open(const std::string& name);
    bool Create(const std::string& name,
                unsigned int value,
                mode_t mode = 0666);
    bool OpenOrCreate(const std::string& name,
                      unsigned int value,
                      mode_t mode = 0666);

    bool Close();

    void Acquire()
    {
        internal::SemaphoreOps::Acquire(m_sem);
    }

    bool TryAcquire()
    {
        return internal::SemaphoreOps::TryAcquire(m_sem);
    }

    bool TimedAcquire(int timeout) // in ms
    {
        return internal::SemaphoreOps::TimedAcquire(m_sem, timeout);
    }

    void Release()
    {
        return internal::SemaphoreOps::Release(m_sem);
    }

    // Usually get value is only used for debug propose,
    // be careful your design if you need it.
    unsigned int GetValue() const
    {
        return internal::SemaphoreOps::GetValue(m_sem);
    }

    /// delete a named semaphone
    static bool Unlink(const std::string& name);

private:
    void CheckNull() const;

private:
    sem_t* m_sem;
};

} // namespace toft

#endif // TOFT_SYSTEM_THREADING_SEMAPHORE_H
