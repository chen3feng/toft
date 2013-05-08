// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 05/23/11

#include "toft/base/binary_version.h"

extern "C" {
namespace binary_version {
__attribute__((weak)) extern const int kSvnInfoCount = 0;
__attribute__((weak)) extern const char * const kSvnInfo[] = {0};
__attribute__((weak)) extern const char kBuildType[] = "Unknown";
__attribute__((weak)) extern const char kBuildTime[] = "Unknown";
__attribute__((weak)) extern const char kBuilderName[] = "Unknown";
__attribute__((weak)) extern const char kHostName[] = "Unknown";
__attribute__((weak)) extern const char kCompiler[] = "Unknown";
}
}

