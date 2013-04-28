// Copyright (c) 2010, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_BASE_UNCOPYABLE_H
#define TOFT_BASE_UNCOPYABLE_H

#include "toft/base/cxx11.h"

//  Private copy constructor and copy assignment ensure classes derived from
//  class Uncopyable cannot be copied.

//  Contributed by Dave Abrahams

namespace toft {

/// The macro way
#ifdef TOFT_CXX11_ENABLED
#define TOFT_DECLARE_UNCOPYABLE(Class) \
private: \
    Class(const Class&) = delete; \
    Class& operator=(const Class&) = delete
#else
#define TOFT_DECLARE_UNCOPYABLE(Class) \
private: \
    Class(const Class&); \
    Class& operator=(const Class&)
#endif

/*

Usage:

class Foo {
    TOFT_DECLARE_UNCOPYABLE(Foo);
public:
    Foo();
    ~Foo();
};

*/

/// The private base class way
namespace uncopyable_details  // protection from unintended ADL
{
class Uncopyable
{
    TOFT_DECLARE_UNCOPYABLE(Uncopyable);
protected:
    Uncopyable() {}
    ~Uncopyable() {}
};
} // namespace uncopyable_details

typedef uncopyable_details::Uncopyable Uncopyable;

/*

Usage:

class Foo : private Uncopyable
{
};

*/

} // namespace toft

#endif // TOFT_BASE_UNCOPYABLE_H
