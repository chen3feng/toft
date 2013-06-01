// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/system/memory/barrier.h"
#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(MemoryBarrier, Test)
{
    MemoryBarrier();
    MemoryReadBarrier();
    MemoryWriteBarrier();
}

} // namespace toft
