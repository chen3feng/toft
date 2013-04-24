// Copyright (c) 2012, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_SYSTEM_IO_PATH_H
#define TOFT_SYSTEM_IO_PATH_H

#include <string>

namespace toft {

class Path {
public:
    static bool IsSeparator(char ch);

    // 'dir/name.ext' -> 'name.ext'
    static std::string GetBaseName(const std::string& filepath);

    // 'dir/name.ext' -> 'ext'
    static std::string GetExtension(const std::string& filepath);

    // 'dir/name.ext' -> 'dir'
    static std::string GetDirectory(const std::string& filepath);

    // 'dir/name.ext' -> '/data/dir/name.ext'
    static std::string ToAbsolute(const std::string& filepath);

    // Normalize a path, e.g. A//B, A/./B and A/foo/../B all become A/B.
    // Take the same behavior as os.path.normpath of python.
    static std::string Normalize(const std::string& filepath);
};

} // namespace toft

#endif // TOFT_SYSTEM_IO_PATH_H
