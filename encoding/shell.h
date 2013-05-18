// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-03-30
// Description: Shell quote, escape, split, join...

#ifndef TOFT_ENCODING_SHELL_H
#define TOFT_ENCODING_SHELL_H
#pragma once

#include <string>
#include <vector>

namespace toft {

// Whether a string must be quoted to ensure safe.
bool ShellIsQuoteNeed(const std::string& str);

// Quote a string as shell way.
std::string ShellQuote(const std::string& str, char quote_char = '"');

// Quote a string as shell way if needed, otherwise keep unchanged.
std::string ShellQuoteIfNeed(const std::string& str, char quote_char = '"');

// Unquote a shell as shell way.
bool ShellUnquote(const std::string& str, std::string* result);

// Escape a string as shell way.
std::string ShellEscape(const std::string& str);

// Unescape a ShellEscaped string.
bool ShellUnescape(const std::string& str, std::string* result);

// Split command line into segments.
bool SplitCommandLine(const std::string& command,
                      std::vector<std::string>* args);

// Join command line segments into string.
std::string JoinCommandLine(const std::vector<std::string>& args);

} // namespace toft

#endif // TOFT_ENCODING_SHELL_H
