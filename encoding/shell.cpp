// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-03-30

#include "toft/encoding/shell.h"

namespace toft {

bool ShellIsQuoteNeed(const std::string& str)
{
    return str.find_first_of("'\" \t&><#!") != std::string::npos;
}

std::string ShellQuote(const std::string& str, char quote_char)
{
    std::string result;
    result += quote_char;
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == quote_char)
            result += '\\';
        result += str[i];
    }
    result += quote_char;
    return result;
}

std::string ShellQuoteIfNeed(const std::string& str, char quote_char)
{
    if (ShellIsQuoteNeed(str))
        return ShellQuote(str, quote_char);
    return str;
}

bool ShellUnquote(const std::string& str, std::string* result)
{
    result->clear();

    std::string::const_iterator pos = str.begin();
    std::string::const_iterator end = str.end();
    char quote_char = '\0';

    while (pos < end) {
        char c = *pos++;
        switch (c) {
        case '"':
        case '\'':
            if (quote_char == c)
                quote_char = '\0';
            else if (quote_char == '\0')
                quote_char = c;
            else
                result->push_back(c);
            break;
        case '\\':
            if (quote_char && pos < end) {
                    char q = *pos;
                    if (q == '"' || q == '\'') {
                        result->push_back(q);
                        ++pos;
                    }
            } else {
                result->push_back(c);
            }
            break;
        default:
            result->push_back(c);
            break;
        }
    }
    return quote_char == '\0';
}

std::string ShellEscape(const std::string& str)
{
    std::string result;
    for (size_t i = 0; i < str.size(); ++i) {
        switch (str[i]) {
        case '\a':
            result += "\\a";
            break;
        case '\b':
            result += "\\b";
            break;
        case '\f':
            result += "\\f";
            break;
        case '\n':
            result += "\\n";
            break;
        case '\r':
            result += "\\r";
            break;
        case '\t':
            result += "\\t";
            break;
        case '\v':
            result += "\\v";
            break;
        case ' ':
        case '!':
        case '"':
        case '#':
        case '$':
        case '&':
        case '(':
        case ')':
        case '*':
        case ',':
        case ':':
        case ';':
        case '?':
        case '@':
        case '[':
        case '\\':
        case ']':
        case '`':
        case '{':
        case '}':
            result += '\\';
            // fall through
        default:
            result += str[i];
            break;
        }
    }
    return result;
}

static int DecodeHex(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    if (c >= 'a' && c <= 'a')
        return c - 'a' + 10;
    return -1;
}

static int UnescapeChar(std::string::const_iterator pos,
                        std::string::const_iterator end, char* c)
{
    if (pos >= end)
        return 0;

    std::string::const_iterator p = pos;
    char e = *p++;
    switch (e) {
    case 'a':
        *c = '\a';
        break;
    case 'b':
        *c = '\b';
        break;
    case 'e':
    case 'E':
        *c = '\033';
        break;
    case 'f':
        *c = '\f';
        break;
    case 'n':
        *c = '\n';
        break;
    case 'r':
        *c = '\r';
        break;
    case 't':
        *c = '\t';
        break;
    case 'v':
        *c = '\v';
        break;
    case '\\':
        *c = '\\';
        break;
    case 'x':
    case 'X':
        if (p < end) {
            int n = DecodeHex(*p);
            if (n > 0) {
                ++p;
                if (p < end) {
                    int d = DecodeHex(*p);
                    if (d > 0) {
                        n = (n << 4) + d;
                        ++p;
                    }
                }
                *c = n;
                break;
            }
        }
        break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
        {
            char n = e - '0';
            char d = *p;
            if (d >= '0' && d <= '9') {
                n = (n << 3) + (d - '0');
                d = *++p;
                if (d >= '0' && d <= '9')
                    n = (n << 3) + (d - '0');
            }
            *c = n;
        }
        break;
    default:
        p = pos;
        break;
    }
    return p - pos;
}

bool ShellUnescape(const std::string& str, std::string* result)
{
    std::string::const_iterator pos = str.begin();
    std::string::const_iterator end = str.end();
    while (pos < end) {
        char c = *pos++;
        if (c == '\\') {
            char e;
            int unescaped_size = UnescapeChar(pos, end, &e);
            if (unescaped_size > 0) {
                *result += e;
                pos += unescaped_size;
            } else {
                *result += *pos++;
            }
        } else { // Not '\\'
            *result += c;
        }
    }
    return true;
}

static void UnescapeCharToArg(std::string::const_iterator* pos,
                              std::string::const_iterator end,
                              bool in_quote, std::string* arg)
{
    char e;
    int len = UnescapeChar(*pos, end, &e);
    if (len > 0) {
        arg->push_back(e);
        *pos += len;
    } else {
        if (in_quote)
            arg->push_back('\\');
        arg->push_back(**pos);
        ++*pos;
    }
}

bool SplitCommandLine(const std::string& command,
                      std::vector<std::string>* args)
{
    args->clear();

    char state = '\0';
    std::string::const_iterator pos = command.begin();
    std::string::const_iterator end = command.end();
    std::string arg;
    while (pos < end) {
        char c = *pos++;
        switch (state) {
        case '\0':
        case 'a':
            switch (c) {
            case '"':
            case '\'':
                state = c;
                break;
            case '\\':
                UnescapeCharToArg(&pos, end, false, &arg);
                break;
            default:
                if (isspace(c)) {
                    if (state != '\0') {
                        args->push_back(arg);
                        arg.clear();
                    }
                    state = ' ';
                } else {
                    arg += c;
                    state = 'a';
                }
                break;
            }
            break;
        case '\'':
        case '"':
            if (state == c) {
                state = 'a';
            } else {
                if (c == '\\')
                    UnescapeCharToArg(&pos, end, true, &arg);
                else
                    arg += c;
            }
            break;
        case ' ':
            switch (c) {
            case '\'':
            case '"':
                state = c;
                break;
            case '\\':
                UnescapeCharToArg(&pos, end, false, &arg);
                state = 'a';
                break;
            default:
                if (!isspace(c)) {
                    state = 'a';
                    arg += c;
                }
            }
        }
    }

    switch (state) {
    case '"':
    case '\'':
        return false;
    case 'a':
        args->push_back(arg);
    }

    return true;
}

std::string JoinCommandLine(const std::vector<std::string>& args)
{
    std::string result;
    for (size_t i = 0; i < args.size(); ++i) {
        if (i != 0)
            result += ' ';
        const std::string& arg = args[i];
        result += ShellQuoteIfNeed(arg);
    }
    return result;
}

} // namespace toft
