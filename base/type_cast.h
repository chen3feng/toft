// Copyright (c) 2010, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_BASE_TYPE_CAST_H
#define TOFT_BASE_TYPE_CAST_H

# include <assert.h>
# include <typeinfo>

// 取自 glog
// Author: kenton@google.com (Kenton Varda) and others
//
// Contains basic types and utilities used by the rest of the library.

//
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

namespace toft {

/// 执行正确的类型转换的同时，不压制编译器的警告。
template<typename To, typename From>
inline To implicit_cast(From const &f) {
    return f;
}

///////////////////////////////////////////////////////////////////////////////
// 以下取自 boost/cast.hpp

/// 执行多态类型转换，如果转换失败，throw 异常。
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

/// 多态向下转型，debug 模式（未定义 NDEBUG）进行检查，否则等效于 static_cast
template <class Target, class Source>
inline Target polymorphic_downcast(Source* x)
{
    assert(dynamic_cast<Target>(x) == x); // NOLINT(runtime/rtti)
    return static_cast<Target>(x);
}

} // namespace toft

#endif // TOFT_BASE_TYPE_CAST_H

