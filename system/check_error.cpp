// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 06/23/11
// Description:

#include "toft/system/check_error.h"

#include <errno.h>
#include <string.h>

#include "thirdparty/glog/logging.h"
#include "thirdparty/glog/raw_logging.h"

namespace toft {

void ReportErrnoError(const char* function_name, int error)
{
    const char* msg = strerror(error);
    RAW_LOG(FATAL, "%s: Fatal error, %s", function_name, msg);
}

void ReportPosixError(const char* function_name)
{
    ReportErrnoError(function_name, errno);
}

bool ReportPosixTimedError(const char* function_name)
{
    int error = errno;
    if (error == ETIMEDOUT)
        return false;
    ReportErrnoError(function_name, error);
    return true;
}

bool ReportPthreadTimedError(const char* function_name, int error)
{
    if (error == ETIMEDOUT)
        return false;
    ReportErrnoError(function_name, error);
    return false;
}

bool ReportPthreadTryLockError(const char* function_name, int error)
{
    if (error == EBUSY || error == EAGAIN)
        return false;
    ReportErrnoError(function_name, error);
    return false;
}

} // namespace toft
