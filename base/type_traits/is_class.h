// Copyright (c) 2013, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-02-28

// GLOBAL_NOLINT(whitespace/newline)
// GLOBAL_NOLINT(readability/braces)

#ifndef TOFT_BASE_TYPE_TRAITS_IS_CLASS_H
#define TOFT_BASE_TYPE_TRAITS_IS_CLASS_H
#pragma once

#include <tr1/type_traits>

namespace toft {
namespace type_traits {

template<typename _Tp>
struct __is_class_helper
{
private:
    template<typename _Up>
    static char __test(int _Up::*);
    template<typename>
    static int __test(...);

public:
    static const bool __value = sizeof(__test<_Tp>(0)) == 1;
};

// Extension.
template<typename _Tp>
struct is_class
: public std::tr1::integral_constant<bool, __is_class_helper<_Tp>::__value>
{ };

} // namespace type_traits
} // namespace toft

#endif // TOFT_BASE_TYPE_TRAITS_IS_CLASS_H
