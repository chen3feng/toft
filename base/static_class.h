// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_BASE_STATIC_CLASS_H
#define TOFT_BASE_STATIC_CLASS_H
#pragma once

#include "toft/base/cxx11.h"

// TOFT_DECLARE_STATIC_CLASS Mark a class that all members a static.

#ifdef TOFT_CXX11_ENABLED
#define TOFT_DECLARE_STATIC_CLASS(Name) \
    private: \
        Name() = delete; \
        ~Name() = delete
#else
#define TOFT_DECLARE_STATIC_CLASS(Name) \
    private: \
        Name(); \
        ~Name()
#endif

#endif // TOFT_BASE_STATIC_CLASS_H
