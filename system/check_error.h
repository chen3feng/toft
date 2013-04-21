// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 06/23/11

#ifndef TOFT_SYSTEM_CHECK_ERROR_H
#define TOFT_SYSTEM_CHECK_ERROR_H
#pragma once

namespace toft {

void ReportErrnoError(const char* function_name, int error);
inline void CheckErrnoError(const char* function_name, int error)
{
    if (error)
        ReportErrnoError(function_name, error);
}

void ReportPosixError(const char* function_name);
inline void CheckPosixError(const char* function_name, int result)
{
    if (result < 0)
        ReportPosixError(function_name);
}

bool ReportPosixTimedError(const char* function_name);
inline bool CheckPosixTimedError(const char* function_name, int result)
{
    if (result < 0)
        return ReportPosixTimedError(function_name);
    return true;
}

bool ReportPthreadTimedError(const char* function_name, int error);
inline bool CheckPthreadTimedError(const char* function_name, int error)
{
    if (error)
        return ReportPthreadTimedError(function_name, error);
    return true;
}

bool ReportPthreadTryLockError(const char* function_name, int error);
inline bool CheckPthreadTryLockError(const char* function_name, int error)
{
    if (error)
        return ReportPthreadTryLockError(function_name, error);
    return true;
}

} // namespace toft

#define TOFT_CHECK_ERRNO_ERROR(expr) \
    CheckErrnoError(__PRETTY_FUNCTION__, (expr))

#define TOFT_CHECK_POSIX_ERROR(expr) \
    CheckPosixError(__PRETTY_FUNCTION__, (expr))

#define TOFT_CHECK_POSIX_TIMED_ERROR(expr) \
    CheckPosixTimedError(__PRETTY_FUNCTION__, (expr))

#define TOFT_CHECK_PTHREAD_ERROR(expr) \
    TOFT_CHECK_ERRNO_ERROR((expr))

#define TOFT_CHECK_PTHREAD_TIMED_ERROR(expr) \
    CheckPthreadTimedError(__PRETTY_FUNCTION__, (expr))

#define TOFT_CHECK_PTHREAD_TRYLOCK_ERROR(expr) \
    CheckPthreadTryLockError(__PRETTY_FUNCTION__, (expr))

#endif // TOFT_SYSTEM_CHECK_ERROR_H
