// Copyright (c) 2011, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: Dec 24, 2010

#ifndef TOFT_BASE_ARRAY_SIZE_H
#define TOFT_BASE_ARRAY_SIZE_H

#ifndef SWIG
#include <stddef.h>
#endif

#ifdef __cplusplus

namespace toft {

struct ArraySizeHelper
{
    template <size_t N>
    struct SizedType
    {
        char elements[N];
    };

    template <typename T, size_t N>
    static SizedType<N> Helper(const T (&a)[N]);
#ifdef __GNUC__ // gcc allow 0 sized array
    template <typename T>
    static SizedType<0> Helper(const T (&a)[0]);
#endif
};

} // namespace toft

/// @brief safe array_size
/// @param a array
/// @return number of elements of array
#define TOFT_ARRAY_SIZE(a) sizeof(::toft::ArraySizeHelper::Helper(a))

#else // __cplusplus

// XXX: from protobuf/stubs/common.h

// ===================================================================
// from google3/base/basictypes.h

// The GOOGLE_ARRAYSIZE(arr) macro returns the # of elements in an array arr.
// The expression is a compile-time constant, and therefore can be
// used in defining new arrays, for example.
//
// GOOGLE_ARRAYSIZE catches a few type errors.  If you see a compiler error
//
//   "warning: division by zero in ..."
//
// when using GOOGLE_ARRAYSIZE, you are (wrongfully) giving it a pointer.
// You should only use GOOGLE_ARRAYSIZE on statically allocated arrays.
//
// The following comments are on the implementation details, and can
// be ignored by the users.
//
// ARRAYSIZE(arr) works by inspecting sizeof(arr) (the # of bytes in
// the array) and sizeof(*(arr)) (the # of bytes in one array
// element).  If the former is divisible by the latter, perhaps arr is
// indeed an array, in which case the division result is the # of
// elements in the array.  Otherwise, arr cannot possibly be an array,
// and we generate a compiler error to prevent the code from
// compiling.
//
// Since the size of bool is implementation-defined, we need to cast
// !(sizeof(a) & sizeof(*(a))) to size_t in order to ensure the final
// result has type size_t.
//
// This macro is not perfect as it wrongfully accepts certain
// pointers, namely where the pointer size is divisible by the pointee
// size.  Since all our code has to go through a 32-bit compiler,
// where a pointer is 4 bytes, this means all pointers to a type whose
// size is 3 or greater than 4 will be (righteously) rejected.
//
// Kudos to Jorg Brown for this simple and elegant implementation.

#define TOFT_ARRAY_SIZE(a) \
    ((sizeof(a) / sizeof(*(a))) / (size_t)(!(sizeof(a) % sizeof(*(a)))))

#endif // __cplusplus

#endif // TOFT_BASE_ARRAY_SIZE_H

