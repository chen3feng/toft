// Copyright (c) 2010, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Description: Byte order detect and convert

#ifndef TOFT_BASE_BYTE_ORDER_H
#define TOFT_BASE_BYTE_ORDER_H

// GLOBAL_NOLINT(runtime/int)

#include <stddef.h>

#ifdef __unix__
#include <byteswap.h>
#include <endian.h>
#include <arpa/inet.h>
#endif

// define __LITTLE_ENDIAN
#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN 1234
#endif

// define __BIG_ENDIAN
#ifndef __BIG_ENDIAN
#define __BIG_ENDIAN 4321
#endif

// define __BYTE_ORDER
#ifndef __BYTE_ORDER
#if defined(_M_IX86) || defined(_M_X64) || defined(__i386__) || defined(__x86_64__)
#define __BYTE_ORDER __LITTLE_ENDIAN
#else
#error unknown byte order
#endif // ARCH
#endif // __BYTE_ORDER

#define TOFT_LITTLE_ENDIAN __LITTLE_ENDIAN
#define TOFT_BIG_ENDIAN __BIG_ENDIAN
#define TOFT_BYTE_ORDER __BYTE_ORDER

namespace toft {
namespace internal {

template <size_t Size> struct ByteOrderSwapperBase {};

template <>
struct ByteOrderSwapperBase<1> {
    template <typename T> static T Swap(T value) { return value; }
    template <typename T> static T hton(T value) { return value; }
    template <typename T> static T ntoh(T value) { return value; }
};

template <>
struct ByteOrderSwapperBase<2> {
    template <typename T> static T Swap(T value) { return bswap_16(value); }
    template <typename T> static T hton(T value) { return htons(value); }
    template <typename T> static T ntoh(T value) { return ntohs(value); }
};

template <>
struct ByteOrderSwapperBase<4> {
    template <typename T> static T Swap(T value) { return bswap_32(value); }
    template <typename T> static T hton(T value) { return htonl(value); }
    template <typename T> static T ntoh(T value) { return ntohl(value); }
};

template <>
struct ByteOrderSwapperBase<8> {
    template <typename T> static T Swap(T value) { return bswap_64(value); }
#if TOFT_BYTE_ORDER == TOFT_BIG_ENDIAN
    template <typename T> static T hton(T value) { return value; }
    template <typename T> static T ntoh(T value) { return value; }
#elif TOFT_BYTE_ORDER == TOFT_LITTLE_ENDIAN
    template <typename T> static T hton(T value) { return Swap(value); }
    template <typename T> static T ntoh(T value) { return Swap(value); }
#endif
};

template <size_t Size>
struct ByteOrderSwapper : public ByteOrderSwapperBase<Size> {
    using ByteOrderSwapperBase<Size>::Swap;
#if TOFT_BYTE_ORDER == TOFT_BIG_ENDIAN
    template <typename T> static T ToBig(T value) { return value; }
    template <typename T> static T FromBig(T value) { return value; }
    template <typename T> static T ToLittle(T value) { return Swap(value); }
    template <typename T> static T FromLittle(T value) { return Swap(value); }
#elif TOFT_BYTE_ORDER == TOFT_LITTLE_ENDIAN
    template <typename T> static T ToBig(T value) { return Swap(value); }
    template <typename T> static T FromBig(T value) { return Swap(value); }
    template <typename T> static T ToLittle(T value) { return value; }
    template <typename T> static T FromLittle(T value) { return value; }
#endif
};

} // namespace internal

// Byte order conversion.
struct ByteOrder
{
private:
    ByteOrder();
    ~ByteOrder();

public:
    static bool IsBigEndian()
    {
        return TOFT_BYTE_ORDER == TOFT_BIG_ENDIAN;
    }

    static bool IsLittleEndian()
    {
        return TOFT_BYTE_ORDER == TOFT_LITTLE_ENDIAN;
    }

    // Return the conversion result. The ExplicitT must be given explicitly.
    template <typename ExplicitT, typename T>
    static ExplicitT Swap(T value) {
        return internal::ByteOrderSwapper<sizeof(ExplicitT)>::Swap(value);
    }

    // Inplace conversion.
    template <typename T>
    static void Swap(T* value)
    {
        *value = Swap<T>(*value);
    }

    /////////////////////////////////////////////////////////////////////////
    // NetOrder <-> LocalOrder conversion.

    // Return the conversion result. The ExplicitT must be given explicitly.
    template <typename ExplicitT, typename T>
    static ExplicitT ToNet(T value) {
        return internal::ByteOrderSwapper<sizeof(ExplicitT)>::hton(value);
    }

    // Inplace conversion.
    template <typename T>
    static void ToNet(T* value)
    {
        *value = ToNet<T>(*value);
    }

    // Return the conversion result. The ExplicitT must be given explicitly.
    template <typename ExplicitT, typename T>
    static ExplicitT FromNet(T value) {
        return internal::ByteOrderSwapper<sizeof(ExplicitT)>::ntoh(value);
    }

    // Inplace conversion.
    template <typename T>
    static void FromNet(T* value)
    {
        *value = FromNet<T>(*value);
    }

    /////////////////////////////////////////////////////////////////////////
    // BigEndian <-> LocalOrder conversion.

    // Return the conversion result. The ExplicitT must be given explicitly.
    template <typename ExplicitT, typename T>
    static ExplicitT ToBigEndian(T value) {
        return internal::ByteOrderSwapper<sizeof(ExplicitT)>::ToBig(value);
    }

    // Inplace conversion.
    template <typename T>
    static void ToBigEndian(T* value)
    {
        *value = ToBigEndian<T>(*value);
    }

    // Return the conversion result. The ExplicitT must be given explicitly.
    template <typename ExplicitT, typename T>
    static ExplicitT FromBigEndian(T value) {
        return internal::ByteOrderSwapper<sizeof(ExplicitT)>::FromBig(value);
    }

    // Inplace conversion.
    template <typename T>
    static void FromBigEndian(T* value)
    {
        *value = FromBigEndian<T>(*value);
    }

    /////////////////////////////////////////////////////////////////////////
    // LittleEndian <-> LocalOrder conversion.

    // Return the conversion result. The ExplicitT must be given explicitly.
    template <typename ExplicitT, typename T>
    static ExplicitT ToLittleEndian(T value) {
        return internal::ByteOrderSwapper<sizeof(ExplicitT)>::ToLittle(value);
    }

    // Inplace conversion.
    template <typename T>
    static void ToLittleEndian(T* value)
    {
        *value = ToLittleEndian<T>(*value);
    }

    // Return the conversion result. The ExplicitT must be given explicitly.
    template <typename ExplicitT, typename T>
    static ExplicitT FromLittleEndian(T value) {
        return internal::ByteOrderSwapper<sizeof(ExplicitT)>::FromLittle(value);
    }

    // Inplace conversion.
    template <typename T>
    static void FromLittleEndian(T* value)
    {
        *value = FromLittleEndian<T>(*value);
    }
};

} // namespace toft

#endif // TOFT_BASE_BYTE_ORDER_H

