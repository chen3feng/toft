// Copyright (c) 2013, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-02-28

#ifndef TOFT_BASE_TYPE_TRAITS_H
#define TOFT_BASE_TYPE_TRAITS_H
#pragma once

#include "toft/config.h"

#ifdef _STD_HAS_TYPE_TRAITS
#include "toft/base/type_traits/std.h"
#elif defined _TR1_HAS_TYPE_TRAITS
#include "toft/base/type_traits/tr1.h"
#else
#include "toft/base/type_traits/missing.h"
#endif
    
#undef TOFT_HAS_STD_TYPE_TRAITS
#undef TOFT_HAS_STD_TR1_TYPE_TRAITS

#endif // TOFT_BASE_TYPE_TRAITS_H
