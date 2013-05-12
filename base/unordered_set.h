// Copyright (c) 2013, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_BASE_UNORDERED_SET_H
#define TOFT_BASE_UNORDERED_SET_H
#pragma once

#ifdef TOFT_CXX11_ENABLED

// Using std::unordered_*
#include <unordered_set>

#else // Non-C++11

#include <tr1/unordered_set>

namespace std {
using ::std::tr1::unordered_set;
using ::std::tr1::unordered_multiset;
} // namespace std

#endif // TOFT_CXX11_ENABLED

#endif // TOFT_BASE_UNORDERED_SET_H
