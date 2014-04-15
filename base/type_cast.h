// Copyright (c) 2010, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_BASE_TYPE_CAST_H
#define TOFT_BASE_TYPE_CAST_H

#include <assert.h>
#include <typeinfo>
#include "toft/base/static_assert.h"

// Author: kenton@google.com (Kenton Varda) and others
//
// Contains basic types and utilities used by the rest of the library.

namespace toft {

// Use implicit_cast as a safe version of static_cast or const_cast
// for upcasting in the type hierarchy (i.e. casting a pointer to Foo
// to a pointer to SuperclassOfFoo or casting a pointer to Foo to
// a const pointer to Foo).
// When you use implicit_cast, the compiler checks that the cast is safe.
// Such explicit implicit_casts are necessary in surprisingly many
// situations where C++ demands an exact type match instead of an
// argument type convertable to a target type.
//
// The From type can be inferred, so the preferred syntax for using
// implicit_cast is the same as for static_cast etc.:
//
//   implicit_cast<ToType>(expr)
//
// implicit_cast would have been part of the C++ standard library,
// but the proposal was submitted too late.  It will probably make
// its way into the language in the future.
template<typename To, typename From>
inline To implicit_cast(From const &f) {
    return f;
}

///////////////////////////////////////////////////////////////////////////////
template <class Target, class Source>
inline Target polymorphic_cast(Source* x)
{
    if (x == 0)
        return 0;
    Target tmp = dynamic_cast<Target>(x); // NOLINT(runtime/rtti)
    if (tmp == 0)
        throw std::bad_cast();
    return tmp;
}

template <class Target, class Source>
inline Target polymorphic_downcast(Source* x)
{
    assert(dynamic_cast<Target>(x) == x); // NOLINT(runtime/rtti)
    return static_cast<Target>(x);
}

// Bypass compiler strict aliasing problem.
// such as
//   float f = ...;
//   int i = *(float*)&f;
// may cause compile-time waring or runtime error.
// See http://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html
// or man gcc(1), search -fstrict-aliasing for details.
//
// Usage:
//   auto i = bitwise_cast<uint32_t>(f);
template <typename ToType, typename FromType>
ToType bitwise_cast(FromType from) {
    TOFT_STATIC_ASSERT(sizeof(FromType) == sizeof(ToType));
    union {
        FromType from;
        ToType to;
    } u = { from };
    return u.to;
}

} // namespace toft

#endif // TOFT_BASE_TYPE_CAST_H

