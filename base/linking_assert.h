// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_BASE_LINKING_ASSERT_H
#define TOFT_BASE_LINKING_ASSERT_H
#pragma once

namespace toft {

// Used for header files with whole inline/template implementation.
// Then make sure all code depends on it must add its library to the deps.
//
// If you design library to be headers only, without a corresponding cc_library,
// you will have no chance to add implementation depends without impacting
// your users.
//
// So, we strongly suggest you not writing header only libraries. To enforce
// the user must depends on your library, we introduce the following macros
// to make you achieve this easily.
//
// Usage:
// template <typename T>
// class Foo : private TOFT_LINKING_ASSERT_BASE(Foo) {
//     ...
// };
//
// or:
// void Foo() {
//     TOFT_LINKING_ASSERT(Foo);
// }
//
// If the user forget to link with the library foo, the linker will issue
// following error message:
// undefined reference to `please_link_with_the_library_contains_Foo_::definition()'
// to prompt the user.
//
// There is no any runtime overhead for release building.

//////////////////////////////////////////////////////////////////////////////
// Implementation details.
#ifdef NDEBUG

#define TOFT_DECLARE_LINKING_ASSERT_IMPL(name) \
class please_link_with_the_library_contains_##name##_ { \
protected: \
    please_link_with_the_library_contains_##name##_() {} \
    ~please_link_with_the_library_contains_##name##_() {} \
public: \
    static void Assert() {} \
};

#define TOFT_DEFINE_LINKING_ASSERT_IMPL(name)

#else // DEBUG

#define TOFT_DECLARE_LINKING_ASSERT_IMPL(name) \
class please_link_with_the_library_contains_##name##_ { \
protected: \
    please_link_with_the_library_contains_##name##_() { Assert(); } \
    ~please_link_with_the_library_contains_##name##_() {} \
public: \
    static void Assert() { definition(); } \
private: \
    static void definition(); \
};

#define TOFT_DEFINE_LINKING_ASSERT_IMPL(name) \
void please_link_with_the_library_contains_##name##_::definition() {}

#endif // NDEBUG

//////////////////////////////////////////////////////////////////////////////
// Interfaces

// Put this into you '.h' file before the interface definition.
#define TOFT_DECLARE_LINKING_ASSERT(name) TOFT_DECLARE_LINKING_ASSERT_IMPL(name)

// Put this into you '.cpp' file.
#define TOFT_DEFINE_LINKING_ASSERT(name) TOFT_DEFINE_LINKING_ASSERT_IMPL(name)

// Usage 1:
// As a private base class:
// class Foo : private TOFT_LINKING_ASSERT_BASE(Foo) {
#define TOFT_LINKING_ASSERT_BASE(name) please_link_with_the_library_contains_##name##_

// Usage 2:
// Call in a function:
// void Foo() {
//      TOFT_LINKING_ASSERT(Foo);
// }
#define TOFT_LINKING_ASSERT(name) TOFT_LINKING_ASSERT_BASE(name)::Assert()

} // namespace toft

#endif // TOFT_BASE_LINKING_ASSERT_H
