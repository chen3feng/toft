// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 05/31/11

#include "toft/system/threading/this_thread.h"

#include <errno.h>
#include <string.h>

#include <pthread.h>
#include <syscall.h>
#include <unistd.h>

namespace toft {

ThreadHandleType ThisThread::GetHandle()
{
    return ::pthread_self();
}

int ThisThread::GetId()
{
    static __thread pid_t tid = 0;
    if (tid == 0)
        tid = syscall(SYS_gettid);
    return tid;
}

void ThisThread::Exit()
{
    pthread_exit(NULL);
}

void ThisThread::Sleep(int64_t time_in_ms)
{
    if (time_in_ms >= 0)
    {
        timespec ts = { time_in_ms / 1000, (time_in_ms % 1000) * 1000000 };
        nanosleep(&ts, &ts);
    }
    else
    {
        pause();
    }
}

void ThisThread::Yield()
{
    sched_yield();
}

int ThisThread::GetLastErrorCode()
{
    return errno;
}

bool ThisThread::IsMain()
{
    return ThisThread::GetId() == getpid();
}

} // namespace toft

