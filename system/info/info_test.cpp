// Copyright (c) 2012, The Toft Authors.
// All rights reserved.
//
// Author: Yongsong Liu <lyscsu@gmail.com>
// Created: 2012-07-10

#include "toft/system/info/info.h"
#include <stdio.h>
#include <iostream>
#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(SystemInformationTest, GetLogicalCpuNumber)
{
    unsigned int cpu_number = GetLogicalCpuNumber();
    EXPECT_GT(cpu_number, 0u);
}

TEST(SystemInformationTest, GetPhysicalMemorySize)
{
    unsigned long long physical_memory_size = GetPhysicalMemorySize();
    EXPECT_GT(physical_memory_size, 0u);
}

TEST(SystemInformationTest, GetTotalPhysicalMemorySize)
{
    FILE* fp = fopen("/proc/meminfo", "r");
    unsigned long long size;
    if (fscanf(fp, "MemTotal: %llu kB", &size) != 1) {
        size = 0;
    }
    fclose(fp);

    unsigned long long total_physical_memory_size = GetTotalPhysicalMemorySize();
    EXPECT_EQ(size*1024, total_physical_memory_size);
}

TEST(SystemInformationTest, GetOsKernelInfo)
{
    std::string kernel;
    EXPECT_TRUE(GetOsKernelInfo(&kernel));
    EXPECT_FALSE(kernel.empty());
}

TEST(SystemInformationTest, GetMachineArchitecture)
{
    std::string arch;
    EXPECT_TRUE(GetMachineArchitecture(&arch));
    EXPECT_FALSE(arch.empty());
}

TEST(SystemInformationTest, GetUserName)
{
    ASSERT_NE("", GetUserName());
}

} // namespace toft
