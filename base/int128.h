// Copyright 2004 Google Inc.
// All Rights Reserved.
//

#ifndef TOFT_BASE_INT128_H
#define TOFT_BASE_INT128_H
#pragma once

#include <stdint.h>
#include <iosfwd>

namespace toft {

// An unsigned 128-bit integer type. Thread-compatible.
class UInt128 {
public:
    UInt128();  // Sets to 0, but don't trust on this behavior.
    UInt128(uint64_t top, uint64_t bottom);
#ifndef SWIG
    UInt128(int bottom); // NOLINT(runtime/explicit)
    UInt128(uint32_t bottom);   // Top 96 bits = 0 NOLINT(runtime/explicit)
#endif
    UInt128(uint64_t bottom);   // hi_ = 0, NOLINT(runtime/explicit)
    UInt128(const UInt128 &val);

    void Initialize(uint64_t top, uint64_t bottom);

    bool operator==(const UInt128& b) const;
    bool operator!=(const UInt128& b) const;
    UInt128& operator=(const UInt128& b);

    bool operator<(const UInt128& b) const;
    bool operator>(const UInt128& b) const;
    bool operator<=(const UInt128& b) const;
    bool operator>=(const UInt128& b) const;

    // Logical operators.
    UInt128 operator~() const;
    UInt128 operator|(const UInt128& b) const;
    UInt128 operator&(const UInt128& b) const;
    UInt128 operator^(const UInt128& b) const;

    // Shift operators.
    UInt128 operator<<(int amount) const;
    UInt128 operator>>(int amount) const;

    // Arithmetic operators.
    // TODO(xxx): multiplication, division, etc.
    UInt128 operator+(const UInt128& b) const;
    UInt128 operator-(const UInt128& b) const;
    UInt128 operator+=(const UInt128& b);
    UInt128 operator-=(const UInt128& b);
    UInt128 operator++(int);
    UInt128 operator--(int);
    UInt128 operator++();
    UInt128 operator--();

    friend uint64_t UInt128Low64(const UInt128& v);
    friend uint64_t UInt128High64(const UInt128& v);

    friend std::ostream& operator<<(std::ostream& o, const UInt128& b);

private:
    // Little-endian memory order optimizations can benefit from
    // having lo_ first, hi_ last.
    // See util/endian/endian.h and Load128/Store128 for storing a UInt128.
    uint64_t        lo_;
    uint64_t        hi_;

    // Not implemented, just declared for catching automatic type conversions.
    UInt128(uint8_t v);  // NOLINT(runtime/explicit)
    UInt128(uint16_t v); // NOLINT(runtime/explicit)
    UInt128(float v);    // NOLINT(runtime/explicit)
    UInt128(double v);   // NOLINT(runtime/explicit)
};

extern const UInt128 kUInt128Max;

// allow UInt128 to be logged
extern std::ostream& operator<<(std::ostream& o, const UInt128& b);

// Methods to access low and high pieces of 128-bit value.
// Defined externally from UInt128 to facilitate conversion
// to native 128-bit types when compilers support them.
inline uint64_t UInt128Low64(const UInt128& v) { return v.lo_; }
inline uint64_t UInt128High64(const UInt128& v) { return v.hi_; }

// TODO(xxx): perhaps it would be nice to have int128, a signed 128-bit type?

// --------------------------------------------------------------------------
//                      Implementation details follow
// --------------------------------------------------------------------------
inline bool UInt128::operator==(const UInt128& b) const {
    return (lo_ == b.lo_) && (hi_ == b.hi_);
}
inline bool UInt128::operator!=(const UInt128& b) const {
    return !(*this == b);
}
inline UInt128& UInt128::operator=(const UInt128& b) {
    lo_ = b.lo_;
    hi_ = b.hi_;
    return *this;
}

inline UInt128::UInt128(): lo_(0), hi_(0) { }
inline UInt128::UInt128(uint64_t top, uint64_t bottom) : lo_(bottom), hi_(top) { }
inline UInt128::UInt128(const UInt128 &v) : lo_(v.lo_), hi_(v.hi_) { }
inline UInt128::UInt128(uint64_t bottom) : lo_(bottom), hi_(0) { }
#ifndef SWIG
inline UInt128::UInt128(uint32_t bottom) : lo_(bottom), hi_(0) { }
inline UInt128::UInt128(int bottom) : lo_(bottom), hi_(0) {
    if (bottom < 0) {
        --hi_;
    }
}
#endif
inline void UInt128::Initialize(uint64_t top, uint64_t bottom) {
    hi_ = top;
    lo_ = bottom;
}

// Comparison operators.

#define CMP128(op)                                              \
    inline bool UInt128::operator op(const UInt128& b) const {      \
        return (hi_ == b.hi_) ? (lo_ op b.lo_) : (hi_ op b.hi_);      \
    }

CMP128( < )
CMP128( > )
CMP128( >= )
CMP128( <= )

#undef CMP128

    // Logical operators.

    inline UInt128 UInt128::operator~() const {
        return UInt128(~hi_, ~lo_);
    }

#define LOGIC128(op)                                             \
    inline UInt128 UInt128::operator op(const UInt128& b) const {    \
        return UInt128(hi_ op b.hi_, lo_ op b.lo_);                    \
    }

LOGIC128(|)
LOGIC128(&)
LOGIC128(^)

#undef LOGIC128

inline UInt128 UInt128::operator+(const UInt128& b) const {
    return UInt128(*this) += b;
}

inline UInt128 UInt128::operator-(const UInt128& b) const {
    return UInt128(*this) -= b;
}

inline UInt128 UInt128::operator+=(const UInt128& b) {
    hi_ += b.hi_;
    lo_ += b.lo_;
    if (lo_ < b.lo_)
        ++hi_;
    return *this;
}

inline UInt128 UInt128::operator-=(const UInt128& b) {
    hi_ -= b.hi_;
    if (b.lo_ > lo_)
        --hi_;
    lo_ -= b.lo_;
    return *this;
}

inline UInt128 UInt128::operator++(int) {
    UInt128 tmp(*this);
    *this += 1;
    return tmp;
}

inline UInt128 UInt128::operator--(int) {
    UInt128 tmp(*this);
    *this -= 1;
    return tmp;
}

inline UInt128 UInt128::operator++() {
    *this += 1;
    return *this;
}

inline UInt128 UInt128::operator--() {
    *this -= 1;
    return *this;
}

} // namespace toft

#endif // TOFT_BASE_INT128_H
