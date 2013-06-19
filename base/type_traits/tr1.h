// Copyright (c) 2013, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-02-28

#ifndef TOFT_BASE_TYPE_TRAITS_TR1_H
#define TOFT_BASE_TYPE_TRAITS_TR1_H
#pragma once

#include <limits.h>
#include <tr1/type_traits>

#include "toft/base/type_traits/add_missing.h"
#include "toft/base/type_traits/is_class.h"

namespace std {
using tr1::add_const;
using tr1::add_cv;
using tr1::add_pointer;
using tr1::add_reference;
using tr1::add_volatile;
using tr1::aligned_storage;
using tr1::alignment_of;
using tr1::extent;
using tr1::has_nothrow_assign;
using tr1::has_nothrow_constructor;
using tr1::has_nothrow_copy;
using tr1::has_trivial_assign;
using tr1::has_trivial_constructor;
using tr1::has_trivial_copy;
using tr1::has_trivial_destructor;
using tr1::has_virtual_destructor;
using tr1::integral_constant;
using tr1::is_abstract;
using tr1::is_arithmetic;
using tr1::is_array;
using tr1::is_base_of;
// using tr1::is_class;
using tr1::is_compound;
using tr1::is_const;
using tr1::is_convertible;
using tr1::is_empty;
using tr1::is_enum;
using tr1::is_floating_point;
using tr1::is_function;
using tr1::is_fundamental;
using tr1::is_integral;
using tr1::is_member_function_pointer;
using tr1::is_member_object_pointer;
using tr1::is_member_pointer;
using tr1::is_object;
using tr1::is_pod;
using tr1::is_pointer;
using tr1::is_polymorphic;
using tr1::is_reference;
using tr1::is_same;
using tr1::is_scalar;
// using tr1::is_signed;
using tr1::is_union;
using tr1::is_unsigned;
using tr1::is_void;
using tr1::is_volatile;
using tr1::rank;
using tr1::remove_all_extents;
using tr1::remove_const;
using tr1::remove_cv;

using tr1::true_type;
using tr1::false_type;

using ::toft::type_traits::enable_if;
using ::toft::type_traits::make_signed;
using ::toft::type_traits::make_unsigned;
using ::toft::type_traits::is_class;

#if CHAR_MIN < 0
// std::tr1::is_signed not consider char as signed type, this is different to
// C++11 standard, fixit.
template <typename T>
struct is_signed : integral_constant<
    bool,
    tr1::is_signed<T>::value || tr1::is_same<typename remove_cv<T>::type,
                                             char>::value>
{
};
#else
using tr1::is_signed;
#endif

} // namespace std

#endif // TOFT_BASE_TYPE_TRAITS_TR1_H
