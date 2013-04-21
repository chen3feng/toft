// Copyright (c) 2012, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/system/memory/unaligned.h"

#include <inttypes.h>
#include <stdint.h>
#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(Unaligned, Get)
{
    unsigned char buffer[sizeof(uint64_t) + 1] = // NOLINT(runtime/sizeof)
    {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };

    ASSERT_EQ(0xFF, GetUnaligned<uint8_t>(buffer + 1));
    ASSERT_EQ(0xFFFF, GetUnaligned<uint16_t>(buffer + 1));
    ASSERT_EQ(0xFFFFFFFF, GetUnaligned<uint32_t>(buffer + 1));
    ASSERT_EQ(0xFFFFFFFFFFFFFFFFULL, GetUnaligned<uint64_t>(buffer + 1));
}

TEST(Unaligned, Put)
{
    char buffer[sizeof(uint64_t) + 1] = {}; // NOLINT(runtime/sizeof)

    PutUnaligned<uint8_t>(buffer + 1, 0xFF);
    ASSERT_EQ(0xFF, GetUnaligned<uint8_t>(buffer + 1));

    memset(buffer, 0, sizeof(buffer));
    PutUnaligned<uint16_t>(buffer + 1, 0xFFFF);
    ASSERT_EQ(0xFFFF, GetUnaligned<uint16_t>(buffer + 1));

    memset(buffer, 0, sizeof(buffer));
    PutUnaligned<uint32_t>(buffer + 1, 0xFFFFFFFF);
    ASSERT_EQ(0xFFFFFFFF, GetUnaligned<uint32_t>(buffer + 1));

    memset(buffer, 0, sizeof(buffer));
    PutUnaligned<uint64_t>(buffer + 1, 0xFFFFFFFFFFFFFFFFULL);
    ASSERT_EQ(0xFFFFFFFFFFFFFFFFULL, GetUnaligned<uint64_t>(buffer + 1));
}

TEST(Unaligned, Ptr)
{
    char buffer[sizeof(void*) + 1] = {}; // NOLINT(runtime/sizeof)
    const char* p = "hello";
    PutUnaligned<const char*>(buffer + 1, p);
    const char* q = GetUnaligned<const char*>(buffer + 1);
    ASSERT_EQ(p, q);
}

TEST(Unaligned, RoundUpPtr)
{
    const char* p = reinterpret_cast<const char*>(0x1000);
    ASSERT_EQ(p, RoundUpPtr(p, 4));
    ASSERT_EQ(p + 4, RoundUpPtr(p+1, 4));
    ASSERT_EQ(p + 4, RoundUpPtr(p+4, 4));
    ASSERT_EQ(p + 8, RoundUpPtr(p+5, 4));
}

TEST(Unaligned, RoundDownPtr)
{
    const char* p = reinterpret_cast<const char*>(0x1000);
    ASSERT_EQ(p, RoundDownPtr(p, 4));
    ASSERT_EQ(p, RoundDownPtr(p+1, 4));
    ASSERT_EQ(p + 4, RoundDownPtr(p+4, 4));
    ASSERT_EQ(p + 4, RoundDownPtr(p+5, 4));
}

} // namespace toft
