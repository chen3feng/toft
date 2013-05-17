// Copyright (c) 2010, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_TEXT_WILDCARD_H
#define TOFT_TEXT_WILDCARD_H

#include <string>

namespace toft {

// Wildcard match static class
struct Wildcard {
private:
    Wildcard();
    ~Wildcard();

public:
    enum MatchFlags
    {
        // Wildcards in string cannot match `/', match filename only
        // *.c not match dir/name.c
        MATCH_FILE_NAME_ONLY = 1<<0,

        // Don't treat the `\' character as escape
        NO_ESCAPE = 1<<1,

        // Not match `.' as the first character of string
        MATCH_PERIOD = 1<<2,

        // Match leading dir part
        MATCH_LEADING_DIR = 1<<3,

        // Ignore case
        // *.c match filename.C
        IGNORE_CASE = 1<<4,
    };

public:
    // Wildcard match
    // @param pattern wildcard string to be matched
    // @param string string to match
    // @param flags MatchFlags combination
    static bool Match(const char* pattern, const char* string, int flags = 0);

    static bool Match(
        const std::string& pattern,
        const std::string& string,
        int flags = 0)
    {
        return Match(pattern.c_str(), string.c_str(), flags);
    }

    static bool Match(
        const std::string& pattern,
        const char* string,
        int flags = 0)
    {
        return Match(pattern.c_str(), string, flags);
    }

    static bool Match(
        const char* pattern,
        const std::string& string,
        int flags = 0)
    {
        return Match(pattern, string.c_str(), flags);
    }
};

} // namespace toft

#endif // TOFT_TEXT_WILDCARD_H

