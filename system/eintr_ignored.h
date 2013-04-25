// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 06/23/11
// Description: Define a helper macro to handle EINTR

#ifndef TOFT_SYSTEM_EINTR_IGNORED_H
#define TOFT_SYSTEM_EINTR_IGNORED_H
#pragma once

#include <errno.h>

/// @define TOFT_EINTR_IGNORED(expr)
/// Ignore EINTR for system call, for example,
/// EINTR_IGNORED(write(fd, buf, size)) will retry if EINTR occured

#ifdef __unix__
#define TOFT_EINTR_IGNORED(expr) \
    ({ \
        __typeof__(expr) eintr_ignored_result; \
        do { \
            eintr_ignored_result = (expr); \
        } while (eintr_ignored_result < 0 && errno == EINTR); \
        eintr_ignored_result; \
    })
#else
#define TOFT_EINTR_IGNORED(expr) (expr)
#endif

#endif // TOFT_SYSTEM_EINTR_IGNORED_H
