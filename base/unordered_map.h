// Copyright (c) 2013, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_BASE_UNORDERED_MAP_H
#define TOFT_BASE_UNORDERED_MAP_H
#pragma once

#ifdef TOFT_CXX11_ENABLED

// Using std::unorderd_*
#include <unordered_map>

#else // Non-C++11

#include <tr1/unordered_map>

namespace std {
using ::std::tr1::unordered_map;
using ::std::tr1::unordered_multimap;
} // namespace std

#endif // TOFT_CXX11_ENABLED

#endif // TOFT_BASE_UNORDERED_MAP_H
