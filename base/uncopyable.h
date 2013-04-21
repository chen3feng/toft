// Copyright (c) 2010, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_BASE_UNCOPYABLE_H
#define TOFT_BASE_UNCOPYABLE_H

//  Private copy constructor and copy assignment ensure classes derived from
//  class Uncopyable cannot be copied.

//  Contributed by Dave Abrahams

namespace toft {

/// the private base class way
namespace uncopyable_details  // protection from unintended ADL
{
class Uncopyable
{
protected:
    Uncopyable() {}
    ~Uncopyable() {}
private:  // emphasize the following members are private
    Uncopyable(const Uncopyable&);
    const Uncopyable& operator=(const Uncopyable&);
};
} // namespace uncopyable_details

typedef uncopyable_details::Uncopyable Uncopyable;

/*

Usage:

class Foo : private Uncopyable
{
};

*/

/// The macro way
#define DECLARE_UNCOPYABLE(Class) \
private: \
    Class(const Class&); \
    Class& operator=(const Class&)

/*

Usage:

class Foo {
    DECLARE_UNCOPYABLE(Foo);
public:
    Foo();
    ~Foo();
};

*/

} // namespace toft

#endif // TOFT_BASE_UNCOPYABLE_H
