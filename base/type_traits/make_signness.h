// Copyright (c) 2013, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-02-28

// GLOBAL_NOLINT(whitespace/newline)
// GLOBAL_NOLINT(readability/braces)
// GLOBAL_NOLINT(runtime/int)

#ifndef TOFT_BASE_TYPE_TRAITS_MAKE_SIGNNESS_H
#define TOFT_BASE_TYPE_TRAITS_MAKE_SIGNNESS_H
#pragma once

#include <tr1/type_traits>

#include "toft/base/type_traits/conditional.h"

namespace toft {
namespace type_traits {

// Utility for constructing identically cv-qualified types.
template<typename _Unqualified, bool _IsConst, bool _IsVol>
struct __cv_selector;

template<typename _Unqualified>
struct __cv_selector<_Unqualified, false, false>
{ typedef _Unqualified __type; };

template<typename _Unqualified>
struct __cv_selector<_Unqualified, false, true>
{ typedef volatile _Unqualified __type; };

template<typename _Unqualified>
struct __cv_selector<_Unqualified, true, false>
{ typedef const _Unqualified __type; };

template<typename _Unqualified>
struct __cv_selector<_Unqualified, true, true>
{ typedef const volatile _Unqualified __type; };

template<typename _Qualified, typename _Unqualified,
    bool _IsConst = std::tr1::is_const<_Qualified>::value,
    bool _IsVol = std::tr1::is_volatile<_Qualified>::value>
class __match_cv_qualifiers
{
    typedef __cv_selector<_Unqualified, _IsConst, _IsVol> __match;

public:
    typedef typename __match::__type __type;
};

// Utility for finding the unsigned versions of signed integral types.
template<typename _Tp>
struct __make_unsigned
{ typedef _Tp __type; };

template<>
struct __make_unsigned<char>
{ typedef unsigned char __type; };

template<>
struct __make_unsigned<signed char>
{ typedef unsigned char __type; };

template<>
struct __make_unsigned<short>
{ typedef unsigned short __type; };

template<>
struct __make_unsigned<int>
{ typedef unsigned int __type; };

template<>
struct __make_unsigned<long>
{ typedef unsigned long __type; };

template<>
struct __make_unsigned<long long>
{ typedef unsigned long long __type; };


// Select between integral and enum: not possible to be both.
template<typename _Tp,
    bool _IsInt = std::tr1::is_integral<_Tp>::value,
    bool _IsEnum = std::tr1::is_enum<_Tp>::value>
class __make_unsigned_selector;

template<typename _Tp>
    class __make_unsigned_selector<_Tp, true, false>
{
    typedef __make_unsigned<typename std::tr1::remove_cv<_Tp>::type> __unsignedt;
    typedef typename __unsignedt::__type __unsigned_type;
    typedef __match_cv_qualifiers<_Tp, __unsigned_type> __cv_unsigned;

public:
    typedef typename __cv_unsigned::__type __type;
};

template<typename _Tp>
class __make_unsigned_selector<_Tp, false, true>
{
    // With -fshort-enums, an enum may be as small as a char.
    typedef unsigned char __smallest;
    static const bool __b0 = sizeof(_Tp) <= sizeof(__smallest);
    static const bool __b1 = sizeof(_Tp) <= sizeof(unsigned short);
    static const bool __b2 = sizeof(_Tp) <= sizeof(unsigned int);
    typedef conditional<__b2, unsigned int, unsigned long> __cond2;
    typedef typename __cond2::type __cond2_type;
    typedef conditional<__b1, unsigned short, __cond2_type> __cond1;
    typedef typename __cond1::type __cond1_type;

public:
    typedef typename conditional<__b0, __smallest, __cond1_type>::type __type;
};

// Given an integral/enum type, return the corresponding unsigned
// integer type.
// Primary template.
/// make_unsigned
template<typename _Tp>
struct make_unsigned
{ typedef typename __make_unsigned_selector<_Tp>::__type type; };

// Integral, but don't define.
template<>
struct make_unsigned<bool>;


// Utility for finding the signed versions of unsigned integral types.
template<typename _Tp>
struct __make_signed
{ typedef _Tp __type; };

template<>
struct __make_signed<char>
{ typedef signed char __type; };

template<>
struct __make_signed<unsigned char>
{ typedef signed char __type; };

template<>
struct __make_signed<unsigned short>
{ typedef signed short __type; };

template<>
struct __make_signed<unsigned int>
{ typedef signed int __type; };

template<>
struct __make_signed<unsigned long>
{ typedef signed long __type; };

template<>
struct __make_signed<unsigned long long>
{ typedef signed long long __type; };


// Select between integral and enum: not possible to be both.
template<typename _Tp,
    bool _IsInt = std::tr1::is_integral<_Tp>::value,
    bool _IsEnum = std::tr1::is_enum<_Tp>::value>
class __make_signed_selector;

template<typename _Tp>
    class __make_signed_selector<_Tp, true, false>
{
    typedef __make_signed<typename std::tr1::remove_cv<_Tp>::type> __signedt;
    typedef typename __signedt::__type __signed_type;
    typedef __match_cv_qualifiers<_Tp, __signed_type> __cv_signed;

public:
    typedef typename __cv_signed::__type __type;
};

template<typename _Tp>
class __make_signed_selector<_Tp, false, true>
{
    // With -fshort-enums, an enum may be as small as a char.
    typedef signed char __smallest;
    static const bool __b0 = sizeof(_Tp) <= sizeof(__smallest);
    static const bool __b1 = sizeof(_Tp) <= sizeof(signed short);
    static const bool __b2 = sizeof(_Tp) <= sizeof(signed int);
    typedef conditional<__b2, signed int, signed long> __cond2;
    typedef typename __cond2::type __cond2_type;
    typedef conditional<__b1, signed short, __cond2_type> __cond1;
    typedef typename __cond1::type __cond1_type;

public:
    typedef typename conditional<__b0, __smallest, __cond1_type>::type __type;
};

// Given an integral/enum type, return the corresponding signed
// integer type.
// Primary template.
/// make_signed
template<typename _Tp>
struct make_signed
{ typedef typename __make_signed_selector<_Tp>::__type type; };

// Integral, but don't define.
template<>
struct make_signed<bool>;

} // namespace type_traits
} // namespace toft

#endif // TOFT_BASE_TYPE_TRAITS_MAKE_SIGNNESS_H
