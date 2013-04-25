// Copyright (c) 2012, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_STORAGE_PATH_PATH_H
#define TOFT_STORAGE_PATH_PATH_H

#include <string>
#include "toft/base/static_class.h"

namespace toft {

// Path operation of file system.
class Path {
    TOFT_DECLARE_STATIC_CLASS(Path);

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

    static bool IsAbsolute(const std::string& filepath);
    static bool IsRelative(const std::string& filepath) {
        return !IsAbsolute(filepath);
    }

    // Normalize a path, e.g. A//B, A/./B and A/foo/../B all become A/B.
    // Take the same behavior as os.path.normpath of python.
    static std::string Normalize(const std::string& filepath);

    // Join two or more pathname components, inserting '/' as needed.
    // If any component is an absolute path, all previous path components
    // will be discarded.
    static std::string Join(const std::string& p1, const std::string& p2);
    static std::string Join(const std::string& p1, const std::string& p2,
                            const std::string& p3);
    static std::string Join(const std::string& p1, const std::string& p2,
                            const std::string& p3, const std::string& p4);
    static std::string Join(const std::string& p1, const std::string& p2,
                            const std::string& p3, const std::string& p4,
                            const std::string& p5);
    static std::string Join(const std::string& p1, const std::string& p2,
                            const std::string& p3, const std::string& p4,
                            const std::string& p5, const std::string& p6);

private:
    static std::string DoJoin(const std::string** paths, size_t size);
};

} // namespace toft

#endif // TOFT_STORAGE_PATH_PATH_H
