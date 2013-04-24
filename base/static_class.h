// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_BASE_STATIC_CLASS_H
#define TOFT_BASE_STATIC_CLASS_H
#pragma once

// Mark a class that all members a static.
#define TOFT_DECLARE_STATIC_CLASS(Name) \
    private: \
        Name(); \
        ~Name()

#endif // TOFT_BASE_STATIC_CLASS_H
