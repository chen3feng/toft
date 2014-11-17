// Copyright (c) 2010, The Toft Authors. All rights reserved.
// Author: Chen Feng <chen3feng@gmail.com>

#ifndef TOFT_SYSTEM_INFO_INFO_H
#define TOFT_SYSTEM_INFO_INFO_H

// GLOBAL_NOLINT(runtime/int)

#include <string>

namespace toft {

unsigned int GetLogicalCpuNumber();
unsigned long long GetPhysicalMemorySize();
unsigned long long GetTotalPhysicalMemorySize();
bool GetOsKernelInfo(std::string* kernerl_info);
bool GetMachineArchitecture(std::string* arch_info);
std::string GetUserName();

} // namespace toft

#endif // TOFT_SYSTEM_INFO_INFO_H
