// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/base/string/compare.h"

#include <ctype.h>
#include <limits.h>

namespace toft {

int memcasecmp(const void *vs1, const void *vs2, size_t n)
{
    size_t i;
    const unsigned char *s1 = static_cast<const unsigned char*>(vs1);
    const unsigned char *s2 = static_cast<const unsigned char*>(vs2);
    for (i = 0; i < n; i++)
    {
        unsigned char u1 = s1[i];
        unsigned char u2 = s2[i];
        int U1 = toupper(u1);
        int U2 = toupper(u2);
        int diff = (UCHAR_MAX <= INT_MAX ? U1 - U2
                    : U1 < U2 ? -1 : U2 < U1);
        if (diff)
            return diff;
    }
    return 0;
}

} // namespace toft
