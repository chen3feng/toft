// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_BASE_ENSURE_LINKING_WITH_H
#define TOFT_BASE_ENSURE_LINKING_WITH_H
#pragma once

namespace toft {

// Used for header files with whole inline/template implementation.
// Then make sure all code depends on it must add its library to the deps.
//
// If your design your library to be headers only, without a corresponding
// library, you will have no chance to add implementation depends without
// impacting your user.
//
// So, we strongly suggest you not writing header only libraries. To enforce
// the user must depends on your library, we introduce the following macros
// to make you achive this easily.
//
// Usage:
// template <typename T>
// class Foo : private ENSURE_LINKING_WITH(Foo) {
//     ...
// };
//
// or:
// void Foo() {
//     ASSERT_LINKING_WITH(Foo);
// }
//
// If the user forget to link with the library foo, the linker will issue
// following error message:
// undefined reference to `toft::ensure_linking_with_the_library_contains_Foo_::definition()
// to prompt the user.
//
// There is no any runtime overhead for release building.

//////////////////////////////////////////////////////////////////////////////
// Implementation details.
#ifdef NDEBUG

#define TOFT_DECLARE_ENSURE_LINKING_WITH_IMPL(name) \
class ensure_linking_with_the_library_contains_##name##_ { \
protected: \
    ensure_linking_with_the_library_contains_##name##_() {} \
    ~ensure_linking_with_the_library_contains_##name##_() {} \
public: \
    static void Assert() {} \
};

#define TOFT_DEFINE_ENSURE_LINKING_WITH_IMPL(name)

#else // DEBUG

#define TOFT_DECLARE_ENSURE_LINKING_WITH_IMPL(name) \
class ensure_linking_with_the_library_contains_##name##_ { \
protected: \
    ensure_linking_with_the_library_contains_##name##_() { Assert(); } \
    ~ensure_linking_with_the_library_contains_##name##_() {} \
public: \
    static void Assert() { definition(); } \
private: \
    static void definition(); \
};

#define TOFT_DEFINE_ENSURE_LINKING_WITH_IMPL(name) \
void ensure_linking_with_the_library_contains_##name##_::definition() {}

#endif // NDEBUG

//////////////////////////////////////////////////////////////////////////////
// Interfaces

// Put this into you '.h' file before the interface definition.
#define TOFT_DECLARE_ENSURE_LINKING_WITH(name) TOFT_DECLARE_ENSURE_LINKING_WITH_IMPL(name)

// Put this into you '.cpp' file.
#define TOFT_DEFINE_ENSURE_LINKING_WITH(name) TOFT_DEFINE_ENSURE_LINKING_WITH_IMPL(name)

// Usage 1:
// As a private base class:
// class Foo : private TOFT_ENSURE_LINKING_WITH(Foo) {
#define TOFT_ENSURE_LINKING_WITH(name) ensure_linking_with_the_library_contains_##name##_

// Usage 2:
// Call in a function:
// void Foo() {
//      TOFT_ASSERT_LINKING_WITH(Foo);
// }
#define TOFT_ASSERT_LINKING_WITH(name) TOFT_ENSURE_LINKING_WITH(name)::Assert()

} // namespace toft

#endif // TOFT_BASE_ENSURE_LINKING_WITH_H
