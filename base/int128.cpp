// Copyright 2004 Google Inc.
// All Rights Reserved.
//

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

#include "toft/base/int128.h"

#include <inttypes.h>
#include <iostream>

#include "thirdparty/glog/logging.h"

namespace toft {

const UInt128 kUInt128Max(UINT64_C(0xFFFFFFFFFFFFFFFF),
                          UINT64_C(0xFFFFFFFFFFFFFFFF));

// Shift operators.
UInt128 UInt128::operator<<(int amount) const {
    DCHECK_GE(amount, 0);

    // uint64_t shifts of >= 64 are undefined, so we will need some special-casing.
    if (amount < 64) {
        if (amount == 0) {
            return *this;
        }
        uint64_t new_hi = (hi_ << amount) | (lo_ >> (64 - amount));
        uint64_t new_lo = lo_ << amount;
        return UInt128(new_hi, new_lo);
    } else if (amount < 128) {
        return UInt128(lo_ << (amount - 64), 0);
    } else {
        return UInt128(0, 0);
    }
}

UInt128 UInt128::operator>>(int amount) const {
    DCHECK_GE(amount, 0);

    // uint64_t shifts of >= 64 are undefined, so we will need some special-casing.
    if (amount < 64) {
        if (amount == 0) {
            return *this;
        }
        uint64_t new_hi = hi_ >> amount;
        uint64_t new_lo = (lo_ >> amount) | (hi_ << (64 - amount));
        return UInt128(new_hi, new_lo);
    } else if (amount < 128) {
        return UInt128(0, hi_ >> (amount - 64));
    } else {
        return UInt128(0, 0);
    }
}

std::ostream& operator<<(std::ostream& o, const UInt128& b) {
    return (o << b.hi_ << "::" << b.lo_);
}

} // namespace toft

