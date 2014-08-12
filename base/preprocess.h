// Copyright (c) 2010, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_BASE_PREPROCESS_H
#define TOFT_BASE_PREPROCESS_H

#include "toft/base/preprocess/disallow_in_header.h"
#include "toft/base/preprocess/join.h"
#include "toft/base/preprocess/stringize.h"
#include "toft/base/preprocess/varargs.h"

/// prevent macro substitution for function-like macros
/// if macro 'min()' was defined:
/// 'int min()' whill be substituted, but
/// 'int min TOFT_PP_PREVENT_MACRO_SUBSTITUTION()' will not be substituted.
#define TOFT_PP_PREVENT_MACRO_SUBSTITUTION

#endif // TOFT_BASE_PREPROCESS_H

