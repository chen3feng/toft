// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 09/30/11
// Description: a handy resource convert macro

#ifndef TOFT_BASE_STATIC_RESOURCE_H
#define TOFT_BASE_STATIC_RESOURCE_H
#pragma once

#include "toft/base/string/string_piece.h"

/// @brief convert a static array to static resource StringPiece
/// @param name name of the resource
///
/// example:
/// StringPiece icon = TOFT_STATIC_RESOURCE(poppy_favicon_ico);
/// RegisterStaticResource("/favicon.ico", TOFT_STATIC_RESOURCE(poppy_favicon_ico))
///
#define TOFT_STATIC_RESOURCE(name) \
    StringPiece(RESOURCE_##name, sizeof(RESOURCE_##name))

#endif // TOFT_BASE_STATIC_RESOURCE_H
