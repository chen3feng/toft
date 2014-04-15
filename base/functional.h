// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_BASE_FUNCTIONAL_H
#define TOFT_BASE_FUNCTIONAL_H
#pragma once

#include "toft/base/cxx11.h"

#ifdef TOFT_CXX11_ENABLED

#include <functional>

#else

#include <tr1/functional>

namespace std {
using tr1::function;
using tr1::bind;
using tr1::bad_function_call;
using tr1::ref;
using tr1::cref;
namespace placeholders = tr1::placeholders;
}

#endif // TOFT_CXX11_ENABLED

#endif // TOFT_BASE_FUNCTIONAL_H
