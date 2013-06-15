// Copyright (c) 2010, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_SYSTEM_MEMORY_UNALIGNED_GENERIC_H
#define TOFT_SYSTEM_MEMORY_UNALIGNED_GENERIC_H

// generic solution, using memcpy

#include <string.h>

#include "toft/base/type_cast.h"
#include "toft/system/memory/unaligned/check_direct_include.h"

namespace toft {

template <typename T>
T GetUnaligned(const void* p)
{
    T t;
    memcpy(&t, p, sizeof(t));
    return t;
}

template <typename T, typename U>
void PutUnaligned(void* p, const U& value)
{
    T t = implicit_cast<T>(value);
    memcpy(p, &t, sizeof(t));
}

} // namespace toft

#endif // TOFT_SYSTEM_MEMORY_UNALIGNED_GENERIC_H
