// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 05/05/11
// Description: binary version information

#ifndef TOFT_BASE_BINARY_VERSION_H
#define TOFT_BASE_BINARY_VERSION_H
#pragma once

extern "C" {
namespace binary_version {
extern const int kSvnInfoCount;
extern const char * const kSvnInfo[];
extern const char kBuildType[];
extern const char kBuildTime[];
extern const char kBuilderName[];
extern const char kHostName[];
extern const char kCompiler[];
}
}

namespace toft {

// Set version information to gflags, which make --version print usable
// text.
void SetupBinaryVersion();

} // namespace toft


#endif // TOFT_BASE_BINARY_VERSION_H
