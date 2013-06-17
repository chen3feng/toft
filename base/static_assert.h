// Copyright (c) 2010, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_BASE_STATIC_ASSERT_H
#define TOFT_BASE_STATIC_ASSERT_H

#include "toft/base/cxx11.h"

#ifdef TOFT_CXX11_ENABLED

#define TOFT_STATIC_ASSERT(e, ...) static_assert(e, "" __VA_ARGS__)

#else

#include "toft/base/preprocess/join.h"

namespace toft {

template <bool x> struct static_assertion_failure;

template <> struct static_assertion_failure<true> {
    enum { value = 1 };
};

template<int x> struct static_assert_test {};

// Static assertions during compilation, Usage:
// TOFT_STATIC_ASSERT(sizeof(Foo) == 48, "Size of Foo must equal to 48");
#define TOFT_STATIC_ASSERT(e, ...) \
    typedef ::toft::static_assert_test < \
            sizeof(::toft::static_assertion_failure<static_cast<bool>(e)>)> \
            TOFT_PP_JOIN(static_assert_failed, __LINE__)

} // namespace toft

#endif // TOFT_CXX11_ENABLED

#endif // TOFT_BASE_STATIC_ASSERT_H

