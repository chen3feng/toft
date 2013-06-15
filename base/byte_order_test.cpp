// Copyright (c) 2010, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/base/byte_order.h"

#include <stdint.h>

#include "thirdparty/gtest/gtest.h"

namespace toft {

const int8_t i8 = 0x12;
const uint8_t u8 = 0x12;
const int16_t i16 = 0x1234;
const uint16_t u16 = 0x1234;
const int32_t i32 = 0x12345678;
const uint32_t u32 = 0x12345678;
const int64_t i64 = 0x1234567890ABCDEFLL;
const uint64_t u64 = 0x1234567890ABCDEFULL;

const int8_t i8r = 0x12;
const uint8_t u8r = 0x12;
const int16_t i16r = 0x3412;
const uint16_t u16r = 0x3412;
const int32_t i32r = 0x78563412;
const uint32_t u32r = 0x78563412U;
const int64_t i64r = 0xEFCDAB9078563412LL;
const uint64_t u64r = 0xEFCDAB9078563412ULL;

TEST(ByteOrder, SwapInt16)
{
    int16_t x = i16;
    EXPECT_EQ(i16r, ByteOrder::Swap<int16_t>(x));
    ByteOrder::Swap(&x);
    EXPECT_EQ(i16r, x);
}

TEST(ByteOrder, SwapUInt6)
{
    uint16_t x = u16;
    EXPECT_EQ(u16r, ByteOrder::Swap<uint16_t>(x));
    ByteOrder::Swap(&x);
    EXPECT_EQ(u16r, x);
}

TEST(ByteOrder, SwapInt32)
{
    int32_t x = i32;
    EXPECT_EQ(i32r, ByteOrder::Swap<int32_t>(x));
    ByteOrder::Swap(&x);
    EXPECT_EQ(i32r, x);
}

TEST(ByteOrder, SwapUInt32)
{
    uint32_t x = u32;
    EXPECT_EQ(u32r, ByteOrder::Swap<uint32_t>(x));
    ByteOrder::Swap(&x);
    EXPECT_EQ(u32r, x);
}

TEST(ByteOrder, SwapInt64)
{
    int64_t x = i64;
    EXPECT_EQ(i64r, ByteOrder::Swap<int64_t>(x));
    ByteOrder::Swap(&x);
    EXPECT_EQ(i64r, x);
}

TEST(ByteOrder, SwapUInt64)
{
    uint64_t x = u64;
    EXPECT_EQ(u64r, ByteOrder::Swap<uint64_t>(x));
    ByteOrder::Swap(&x);
    EXPECT_EQ(u64r, x);
}

#undef htons
#undef htonl
#undef ntohs
#undef ntohl

TEST(ByteOrder, ToNet)
{
    ASSERT_EQ(i8, ByteOrder::ToNet<int8_t>(i8));
    ASSERT_EQ(u8, ByteOrder::ToNet<uint8_t>(u8));
    ASSERT_EQ(htons(i16), ByteOrder::ToNet<int16_t>(i16));
    ASSERT_EQ(htons(u16), ByteOrder::ToNet<uint16_t>(u16));
    ASSERT_EQ(static_cast<int32_t>(htonl(i32)), ByteOrder::ToNet<int32_t>(i32));
    ASSERT_EQ(htonl(u32), ByteOrder::ToNet<uint32_t>(u32));
#if TOFT_BYTE_ORDER == TOFT_BIG_ENDIAN
    ASSERT_EQ(i64, ByteOrder::ToNet<int64_t>(i64));
    ASSERT_EQ(u64, ByteOrder::ToNet<uint64_t>(u64));
#else
    ASSERT_EQ(i64r, ByteOrder::ToNet<int64_t>(i64));
    ASSERT_EQ(u64r, ByteOrder::ToNet<uint64_t>(u64));
#endif
}

TEST(ByteOrder, FromNet)
{
    ASSERT_EQ(i8, ByteOrder::FromNet<int8_t>(i8));
    ASSERT_EQ(u8, ByteOrder::FromNet<uint8_t>(u8));
    ASSERT_EQ(ntohs(i16), ByteOrder::FromNet<int16_t>(i16));
    ASSERT_EQ(ntohs(u16), ByteOrder::FromNet<uint16_t>(u16));
    ASSERT_EQ(static_cast<int32_t>(ntohl(i32)), ByteOrder::FromNet<int32_t>(i32));
    ASSERT_EQ(ntohl(u32), ByteOrder::FromNet<uint32_t>(u32));
#if TOFT_BYTE_ORDER == TOFT_BIG_ENDIAN
    ASSERT_EQ(i64, ByteOrder::FromNet<int64_t>(i64));
    ASSERT_EQ(u64, ByteOrder::FromNet<uint64_t>(u64));
#else
    ASSERT_EQ(i64r, ByteOrder::FromNet<int64_t>(i64));
    ASSERT_EQ(u64r, ByteOrder::FromNet<uint64_t>(u64));
#endif
}

#if TOFT_BYTE_ORDER == TOFT_LITTLE_ENDIAN
#define L(x) x
#define B(x) x##r
#else
#define L(x) x##r
#define B(x) x
#endif

TEST(ByteOrder, ToLittleEndian)
{
    ASSERT_EQ(L(i8), ByteOrder::ToLittleEndian<int8_t>(i8));
    ASSERT_EQ(L(u8), ByteOrder::ToLittleEndian<uint8_t>(u8));
    ASSERT_EQ(L(i16), ByteOrder::ToLittleEndian<int16_t>(i16));
    ASSERT_EQ(L(u16), ByteOrder::ToLittleEndian<uint16_t>(u16));
    ASSERT_EQ(L(i32), ByteOrder::ToLittleEndian<int32_t>(i32));
    ASSERT_EQ(L(u32), ByteOrder::ToLittleEndian<uint32_t>(u32));
    ASSERT_EQ(L(i64), ByteOrder::ToLittleEndian<int64_t>(i64));
    ASSERT_EQ(L(u64), ByteOrder::ToLittleEndian<uint64_t>(u64));
}

TEST(ByteOrder, ToBigEndian)
{
    ASSERT_EQ(B(i8), ByteOrder::ToBigEndian<int8_t>(i8));
    ASSERT_EQ(B(u8), ByteOrder::ToBigEndian<uint8_t>(u8));
    ASSERT_EQ(B(i16), ByteOrder::ToBigEndian<int16_t>(i16));
    ASSERT_EQ(B(u16), ByteOrder::ToBigEndian<uint16_t>(u16));
    ASSERT_EQ(B(i32), ByteOrder::ToBigEndian<int32_t>(i32));
    ASSERT_EQ(B(u32), ByteOrder::ToBigEndian<uint32_t>(u32));
    ASSERT_EQ(B(i64), ByteOrder::ToBigEndian<int64_t>(i64));
    ASSERT_EQ(B(u64), ByteOrder::ToBigEndian<uint64_t>(u64));
}

TEST(ByteOrder, FromLittleEndian)
{
    ASSERT_EQ(L(i8), ByteOrder::FromLittleEndian<int8_t>(i8));
    ASSERT_EQ(L(u8), ByteOrder::FromLittleEndian<uint8_t>(u8));
    ASSERT_EQ(L(i16), ByteOrder::FromLittleEndian<int16_t>(i16));
    ASSERT_EQ(L(u16), ByteOrder::FromLittleEndian<uint16_t>(u16));
    ASSERT_EQ(L(i32), ByteOrder::FromLittleEndian<int32_t>(i32));
    ASSERT_EQ(L(u32), ByteOrder::FromLittleEndian<uint32_t>(u32));
    ASSERT_EQ(L(i64), ByteOrder::FromLittleEndian<int64_t>(i64));
    ASSERT_EQ(L(u64), ByteOrder::FromLittleEndian<uint64_t>(u64));
}

TEST(ByteOrder, FromBigEndian)
{
    ASSERT_EQ(B(i8), ByteOrder::FromBigEndian<int8_t>(i8));
    ASSERT_EQ(B(u8), ByteOrder::FromBigEndian<uint8_t>(u8));
    ASSERT_EQ(B(i16), ByteOrder::FromBigEndian<int16_t>(i16));
    ASSERT_EQ(B(u16), ByteOrder::FromBigEndian<uint16_t>(u16));
    ASSERT_EQ(B(i32), ByteOrder::FromBigEndian<int32_t>(i32));
    ASSERT_EQ(B(u32), ByteOrder::FromBigEndian<uint32_t>(u32));
    ASSERT_EQ(B(i64), ByteOrder::FromBigEndian<int64_t>(i64));
    ASSERT_EQ(B(u64), ByteOrder::FromBigEndian<uint64_t>(u64));
}

} // namespace toft
