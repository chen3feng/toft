// Copyright (c) 2010, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_BASE_SHARED_PTR_H
#define TOFT_BASE_SHARED_PTR_H
#pragma once

#include "toft/base/cxx11.h"

#ifdef TOFT_CXX11_ENABLED

#include <memory>

#else

#include <tr1/memory>

namespace std {
using tr1::shared_ptr;
using tr1::weak_ptr;
using tr1::enable_shared_from_this;
using tr1::bad_weak_ptr;
}

#endif // TOFT_CXX11_ENABLED

#endif // TOFT_BASE_SHARED_PTR_H
