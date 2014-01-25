// Copyright (c) 2012, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/storage/path/path.h"
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "toft/base/array_size.h"
#include "toft/base/string/algorithm.h"

namespace toft {

bool Path::IsSeparator(char ch)
{
#ifdef _WIN32
    return ch == '\\' || ch == '/';
#else
    return ch == '/';
#endif
}

std::string Path::GetBaseName(const std::string& filepath)
{
    std::string path = filepath;
    if (path.size() > 1 && path[path.size() - 1] == '/')
        path.resize(path.size() - 1);
    size_t last_slash = path.rfind('/');
    if (last_slash == std::string::npos)
        last_slash = 0;
    else
        ++last_slash;
    return path.substr(last_slash);
}

std::string Path::GetExtension(const std::string& filepath)
{
    size_t last_dot = filepath.rfind('.');
    if (last_dot == std::string::npos)
        return "";
    return filepath.substr(last_dot);
}

std::string Path::GetDirectory(const std::string& filepath)
{
    std::string path = filepath;
    if (path.size() > 1 && path[path.size() - 1] == '/')
        path.resize(path.size() - 1);
    size_t last_slash = path.rfind('/');
    if (last_slash == std::string::npos)
        return ".";
    if (last_slash == 0)
        return "/";

    return path.substr(0, last_slash);
}

std::string Path::ToAbsolute(const std::string& filepath)
{
    if (IsAbsolute(filepath))
        return Normalize(filepath);
    char cwd_buf[4096];
    std::string cwd = getcwd(cwd_buf, sizeof(cwd_buf));
    return Normalize(Join(cwd, filepath));
}

bool Path::IsAbsolute(const std::string& filepath)
{
    return !filepath.empty() && IsSeparator(filepath[0]);
}

// Normalize path, eliminating double slashes, etc.
std::string Path::Normalize(const std::string& path)
{
    if (path.empty())
        return ".";

    int initial_slashes = path[0] == '/';
    // POSIX allows one or two initial slashes, but treats three or more
    // as single slash.
    if (initial_slashes &&
        StringStartsWith(path, "//") && !StringStartsWith(path, "///"))
    {
        initial_slashes = 2;
    }

    std::vector<std::string> comps;
    SplitStringKeepEmpty(path, "/", &comps);

    std::vector<std::string> new_comps;
    for (std::vector<std::string>::iterator i = comps.begin(); i != comps.end(); ++i)
    {
        const std::string& comp = *i;
        if (comp.empty() || comp == ".")
            continue;
        if (comp != ".." || (!initial_slashes && new_comps.empty()) ||
            (!new_comps.empty() && new_comps.back() == ".."))
            new_comps.push_back(comp);
        else if (!new_comps.empty())
            new_comps.pop_back();
    }

    std::swap(comps, new_comps);
    std::string new_path = JoinStrings(comps, "/");
    if (initial_slashes)
        new_path = std::string(initial_slashes, '/') + new_path;

    return new_path.empty() ? "." : new_path;
}

std::string Path::Join(const std::string& p1, const std::string& p2)
{
    const std::string* paths[] = {&p1, &p2};
    return DoJoin(paths, TOFT_ARRAY_SIZE(paths));
}

std::string Path::Join(const std::string& p1, const std::string& p2,
                       const std::string& p3)
{
    const std::string* paths[] = {&p1, &p2, &p3};
    return DoJoin(paths, TOFT_ARRAY_SIZE(paths));
}


std::string Path::Join(const std::string& p1, const std::string& p2,
                       const std::string& p3, const std::string& p4)
{
    const std::string* paths[] = {&p1, &p2, &p3, &p4};
    return DoJoin(paths, TOFT_ARRAY_SIZE(paths));
}


std::string Path::Join(const std::string& p1, const std::string& p2,
                       const std::string& p3, const std::string& p4,
                       const std::string& p5)
{
    const std::string* paths[] = {&p1, &p2, &p3, &p4, &p5};
    return DoJoin(paths, TOFT_ARRAY_SIZE(paths));
}

std::string Path::Join(const std::string& p1, const std::string& p2,
                       const std::string& p3, const std::string& p4,
                       const std::string& p5, const std::string& p6)
{
    const std::string* paths[] = {&p1, &p2, &p3, &p4, &p5, &p6};
    return DoJoin(paths, TOFT_ARRAY_SIZE(paths));
}

std::string Path::DoJoin(const std::string** paths, size_t size)
{
    std::string result = *paths[0];
    for (size_t i = 1; i < size; ++i) {
        const std::string& path = *paths[i];
        if (!path.empty()) {
            if (path[0] == '/') {
                result = path;
            } else {
                if (!result.empty() && result[result.size() - 1] != '/')
                    result += '/';
                result += path;
            }
        }
    }
    return result;
}

} // namespace toft

