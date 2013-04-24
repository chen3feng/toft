// Copyright (c) 2012, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/storage/path/path.h"
#include <errno.h>
#include <stdlib.h>
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
    const int MAX_PATH_LEN = 4096;
    char resolved[MAX_PATH_LEN];
    resolved[0] = '\0';
#ifdef _WIN32
    _fullpath(resolved, filepath.c_str(), MAX_PATH_LEN);
#else
    if (realpath(filepath.c_str(), resolved) == NULL)
    {
        // realpath check the existance of the result path, just ignore any
        // error except EINVAL.
        // TODO(phongchen): rewrite by std::string operation.
        if (errno == EINVAL)
            return "";
    }
#endif
    return resolved;
}

// Normalize path, eliminating double slashes, etc.
std::string Path::Normalize(const std::string& path)
{
    if (path.empty())
        return ".";

    int initial_slashes = path[0] == '/';
    // POSIX allows one or two initial slashes, but treats three or more
    // as single slash.
    if (initial_slashes and
        StringStartsWith(path, "//") and not StringStartsWith(path, "///"))
    {
        initial_slashes = 2;
    }

    std::vector<std::string> comps;
    SplitStringKeepEmpty(path, "/", &comps);

    std::vector<std::string> new_comps;
    for (std::vector<std::string>::iterator i = comps.begin(); i != comps.end(); ++i)
    {
        const std::string& comp = *i;
        if (comp.empty() or comp == ".")
            continue;
        if (comp != ".." or (not initial_slashes and new_comps.empty()) or
            (not new_comps.empty() and new_comps.back() == ".."))
            new_comps.push_back(comp);
        else if (not new_comps.empty())
            new_comps.pop_back();
    }

    std::swap(comps, new_comps);
    std::string new_path = JoinStrings(comps, "/");
    if (initial_slashes)
        new_path = std::string(initial_slashes, '/') + new_path;

    return new_path.empty() ? "." : new_path;
}

} // namespace toft

