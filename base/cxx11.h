// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

// Define macro to check C++11

#ifndef TOFT_BASE_CXX11_H
#define TOFT_BASE_CXX11_H
#pragma once

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus == 201103L
#define TOFT_CXX11_ENABLED 1
#endif

#endif // TOFT_BASE_CXX11_H
