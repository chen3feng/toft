// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include <sstream>

#include "toft/base/binary_version.h"

#include "thirdparty/gflags/gflags.h"

namespace toft {

// This function can't be in binary_version.cpp,
// otherwise, compiler optmizer will use kSvnInfoCount as 0 at compile time.
// and then generate empty version info.
static std::string MakeVersionInfo()
{
    using namespace binary_version;

    std::ostringstream oss;
    oss << "\n"; // Open a new line in gflags --version output.

    if (kSvnInfoCount > 0)
    {
        oss << "Sources:\n"
            << "----------------------------------------------------------\n";
        for (int i = 0; i < kSvnInfoCount; ++i)
            oss << kSvnInfo[i];
        oss << "----------------------------------------------------------\n";
    }

    oss << "BuildTime: " << kBuildTime << "\n"
        << "BuildType: " << kBuildType << "\n"
        << "BuilderName: " << kBuilderName << "\n"
        << "HostName: " << kHostName << "\n"
        << "Compiler: " << kCompiler << "\n";

    return oss.str();
}

void SetupBinaryVersion()
{
    google::SetVersionString(MakeVersionInfo());
}

} // namespace toft

