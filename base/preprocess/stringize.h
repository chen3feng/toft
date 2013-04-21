// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-03-02
// Description:

#ifndef TOFT_BASE_PREPROCESS_STRINGIZE_H
#define TOFT_BASE_PREPROCESS_STRINGIZE_H
#pragma once

/// Converts the parameter X to a string after macro replacement
/// on X has been performed.
/// example: TOFT_PP_STRINGIZE(UCHAR_MAX) -> "255"
#define TOFT_PP_STRINGIZE(X) TOFT_PP_DO_STRINGIZE(X)
#define TOFT_PP_DO_STRINGIZE(X) #X

#endif // TOFT_BASE_PREPROCESS_STRINGIZE_H
