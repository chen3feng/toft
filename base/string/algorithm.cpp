// Copyright (c) 2010, The TOFT Authors. All rights reserved.
/// @brief string algorithms
/// @author hsiaokangliu
/// @date 2010-11-25

#include "toft/base/string/algorithm.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <iterator>
#include <limits>

#include "toft/base/string/compare.h"

namespace toft {

bool StringStartsWithIgnoreCase(const StringPiece& str, const StringPiece& prefix)
{
    return str.size() >= prefix.size() &&
        memcasecmp(str.data(), prefix.data(), prefix.length()) == 0;
}

// Replace the first "old" pattern with the "new" pattern in a string
std::string ReplaceFirst(
    const StringPiece& s,
    const StringPiece& oldsub,
    const StringPiece& newsub)
{
    if (oldsub.empty())
        return s.as_string();

    std::string res;
    std::string::size_type pos = s.find(oldsub);
    if (pos == std::string::npos)
    {
        return s.as_string();
    }
    else
    {
        res.append(s.data(), pos);
        res.append(newsub.data(), newsub.size());
        res.append(s.data() + pos + oldsub.size(), s.length() - pos - oldsub.size());
    }
    return res;
}

// Replace all the "old" pattern with the "new" pattern in a string
std::string ReplaceAll(const StringPiece& s, const StringPiece& oldsub,
                       const StringPiece& newsub)
{
    if (oldsub.empty())
        return s.as_string();

    std::string res;
    std::string::size_type start_pos = 0;
    std::string::size_type pos;
    do {
        pos = s.find(oldsub, start_pos);
        if (pos == std::string::npos)
        {
            break;
        }
        res.append(s.data() + start_pos, pos - start_pos);
        res.append(newsub.data(), newsub.size());
        start_pos = pos + oldsub.size();
    } while (true);
    res.append(s.data() + start_pos, s.length() - start_pos);
    return res;
}

void ReplaceAll(std::string* s, const StringPiece& from, const StringPiece& to)
{
    size_t pos = 0;
    while ((pos = s->find(from.data(), pos, from.size())) != std::string::npos)
    {
        s->replace(pos, from.size(), to.data(), to.size());
        pos += to.size();
    }
}

size_t ReplaceAllChars(std::string* s, const StringPiece& from, char to)
{
    size_t num_replaced = 0;
    size_t length = s->length();
    for (size_t i = 0; i < length; ++i)
    {
        if (from.find((*s)[i]) != std::string::npos)
        {
            (*s)[i] = to;
            ++num_replaced;
        }
    }
    return num_replaced;
}

std::string ReplaceAllChars(const StringPiece& s, const StringPiece& from, char to)
{
    std::string result = s.as_string();
    ReplaceAllChars(&result, from, to);
    return result;
}

// Replaces any occurrence of the characters in 'remove' with the character 'replacewith'.
void StripString(std::string* s, const char* remove, char replacewith)
{
    const char * str_start = s->c_str();
    const char * str = str_start;
    for (str = strpbrk(str, remove); str != NULL; str = strpbrk(str + 1, remove))
    {
        (*s)[str - str_start] = replacewith;
    }
}

std::string StripString(const StringPiece& s, const char* remove, char replacewith)
{
    std::string res = s.as_string();
    StripString(&res, remove, replacewith);
    return res;
}

// This function merges a vector of string components
void JoinStrings(
    const std::vector<std::string>& components,
    const StringPiece& delim,
    std::string* result)
{
    size_t length = 0;

    for (std::vector<std::string>::const_iterator iter = components.begin();
         iter != components.end(); ++iter)
    {
        if (iter != components.begin())
        {
            length += delim.length();
        }
        length += iter->size();
    }
    result->reserve(length);
    return JoinStrings<std::vector<std::string>::const_iterator>(
        components.begin(), components.end(), delim, result);
}

std::string JoinStrings(const std::vector<std::string>& components, const StringPiece& delim)
{
    std::string result;
    JoinStrings(components, delim, &result);
    return result;
}

char* RemoveLineEnding(char* line)
{
    size_t length = strlen(line);
    while (length > 0 && (line[length - 1] == '\r' || line[length - 1] == '\n'))
        line[--length] = '\0';
    return line;
}

template <typename StringType>
void DoRemoveLineEnding(StringType* line)
{
    while (!line->empty())
    {
        char last = (*line)[line->length() - 1];
        if (last == '\r' || last == '\n')
            line->resize(line->length() - 1);
        else
            break;
    }
}

void RemoveLineEnding(std::string* line)
{
    DoRemoveLineEnding(line);
}

void RemoveLineEnding(StringPiece* line)
{
    DoRemoveLineEnding(line);
}

void RemoveContinuousBlank(std::string* str)
{
    bool first_blank = true;
    std::string::size_type end_pos = 0;
    std::string::size_type start_pos = 0;
    size_t len = str->length();

    for (start_pos = 0; start_pos != len; start_pos++)
    {
        if (str->at(start_pos) != ' ')
        {
            str->at(end_pos) = str->at(start_pos);
            end_pos++;
            first_blank = true;
        }
        else
        {
            if (first_blank)
            {
                str->at(end_pos) = str->at(start_pos);
                end_pos++;
                first_blank = false;
            }
        }
    }
    str->resize(end_pos);
}

std::string RemoveContinuousBlank(const StringPiece& str)
{
    std::string res = str.as_string();
    RemoveContinuousBlank(&res);
    return res;
}

std::string RemoveFirst(const StringPiece& s, const StringPiece& substr, bool fill_blank)
{
    return fill_blank ? ReplaceFirst(s, substr, " ") : ReplaceFirst(s, substr, "");
}

std::string RemoveAll(const StringPiece& s, const StringPiece& substr, bool fill_blank)
{
    return fill_blank ? ReplaceAll(s, substr, " ") :
           ReplaceAll(s, substr, "");
}

template <typename StringType, typename ITR>
static inline
void SplitStringToIteratorUsing(const StringPiece& full, const char* delim, ITR& result)
{
    // Optimize the common case where delim is a single character.
    if (delim[0] != '\0' && delim[1] == '\0')
    {
        char c = delim[0];
        const char* p = full.data();
        const char* end = p + full.size();
        while (p != end)
        {
            if (*p == c)
                ++p;
            else
            {
                const char* start = p;
                while (++p != end && *p != c) {}
                *result++ = StringType(start, p - start);
            }
        }
        return;
    }

    std::string::size_type begin_index, end_index;
    begin_index = full.find_first_not_of(delim);
    while (begin_index != std::string::npos)
    {
        end_index = full.find_first_of(delim, begin_index);
        if (end_index == std::string::npos)
        {
            *result++ = full.substr(begin_index).as_string();
            return;
        }
        *result++ = full.substr(begin_index, (end_index - begin_index)).as_string();
        begin_index = full.find_first_not_of(delim, end_index);
    }
}

// Split a string using a character delimiter.
void SplitStringByAnyOf(
    const StringPiece& full,
    const char* delim,
    std::vector<std::string>* result)
{
    result->clear();
    std::back_insert_iterator<std::vector<std::string> > it(*result);
    SplitStringToIteratorUsing<std::string>(full, delim, it);
}

template <typename StringType, typename ITR>
static inline
void SplitUsingStringDelimiterToIterator(const StringPiece& full,
                                         const char* delim,
                                         ITR& result)
{
    if (full.empty())
    {
        return;
    }
    if (delim[0] == '\0')
    {
        *result++ = full.as_string();
        return;
    }

    // Optimize the common case where delim is a single character.
    if (delim[1] == '\0')
    {
        SplitStringToIteratorUsing<StringType>(full, delim, result);
        return;
    }

    size_t delim_length = strlen(delim);
    for (size_t begin_index = 0; begin_index < full.size();)
    {
        size_t end_index = full.find(delim, begin_index);
        if (end_index == std::string::npos)
        {
            *result++ = full.substr(begin_index).as_string();
            return;
        }
        if (end_index > begin_index)
        {
            StringType value(full.data() + begin_index, end_index - begin_index);
            *result++ = value;
        }
        begin_index = end_index + delim_length;
    }
}

void SplitString(const StringPiece& full,
                 const char* delim,
                 std::vector<std::string>* result)
{
    result->clear();
    std::back_insert_iterator<std::vector<std::string> > it(*result);
    SplitUsingStringDelimiterToIterator<std::string>(full, delim, it);
}

void SplitString(const StringPiece& full,
                 const char* delim,
                 std::vector<StringPiece>* result) {
    result->clear();
    std::back_insert_iterator<std::vector<StringPiece> > it(*result);
    SplitUsingStringDelimiterToIterator<StringPiece>(full, delim, it);
}

void SplitStringToSet(const StringPiece& full,
                      const char* delim,
                      std::set<std::string>* result) {
    result->clear();
    std::insert_iterator<std::set<std::string> > it(*result, result->end());
    SplitUsingStringDelimiterToIterator<std::string>(full, delim, it);
}

void SplitStringByDelimiter(const StringPiece& full,
                            const char* delim,
                            std::vector<std::string>* result)
{
    return SplitString(full, delim, result);
}

/** 功能: 把一个字符串划分成多个字符串
 *  参数:
 *  输入参数 const StringPiece& full         主字符串
 *  输入参数 const StringPiece& delim     字符串分界符号
 *  输出参数 std::vector<std::string>& result 分解后的结果
 */
void SplitStringKeepEmpty(
    const StringPiece& full,
    char delim,
    std::vector<std::string>* result)
{
    result->clear();

    if (full.empty())
        return;

    size_t prev_pos = 0;
    size_t pos;
    std::string token;
    while ((pos = full.find(delim, prev_pos)) != std::string::npos)
    {
        token.assign(full.data() + prev_pos, pos - prev_pos);
        result->push_back(token);
        prev_pos = pos + 1;
    }

    token.assign(full.data() + prev_pos, full.length() - prev_pos);
    result->push_back(token);
}

void SplitStringKeepEmpty(
    const StringPiece& full,
    const StringPiece& delim,
    std::vector<std::string>* result)
{
    // 单个字符的分隔符转调字符版本的分割函数，要快一些
    if (delim.length() == 1)
    {
        SplitStringKeepEmpty(full, delim[0], result);
        return;
    }

    result->clear();

    if (full.empty() || delim.empty())
        return;

    size_t prev_pos = 0;
    size_t pos;
    std::string token;
    while ((pos = full.find(delim, prev_pos)) != std::string::npos)
    {
        token.assign(full.data() + prev_pos, pos - prev_pos);
        result->push_back(token);
        prev_pos = pos + delim.length();
    }

    token.assign(full.data() + prev_pos, full.length() - prev_pos);
    result->push_back(token);
}

template <typename StringType>
void DoSplitLines(
    const StringPiece& full,
    std::vector<StringType>* result,
    bool keep_line_endling
)
{
    result->clear();
    size_t prev_pos = 0;
    size_t pos;
    StringType token;
    while ((pos = full.find('\n', prev_pos)) != std::string::npos)
    {
        token.assign(full.data() + prev_pos, pos - prev_pos + 1);
        if (!keep_line_endling)
            RemoveLineEnding(&token);
        result->push_back(token);
        prev_pos = pos + 1;
    }
    if (prev_pos < full.size())
    {
        token.assign(full.data() + prev_pos, full.length() - prev_pos);
        if (!keep_line_endling)
            RemoveLineEnding(&token);
        result->push_back(token);
    }
}

void SplitLines(
    const StringPiece& full,
    std::vector<std::string>* result,
    bool keep_line_endling)
{
    DoSplitLines(full, result, keep_line_endling);
}

void SplitLines(
    const StringPiece& full,
    std::vector<StringPiece>* result,
    bool keep_line_endling)
{
    DoSplitLines(full, result, keep_line_endling);
}

void StringTrimLeft(std::string* str) {
    size_t start_pos = 0;
    size_t end_pos = str->length();
    while (start_pos != end_pos && isspace(str->at(start_pos)))
        start_pos++;
    *str = str->substr(start_pos);
}

std::string StringTrimLeft(const StringPiece& str)
{
    std::string res = str.as_string();
    StringTrimLeft(&res);
    return res;
}

void StringTrimRight(std::string* str)
{
    int end_pos = static_cast<int>(str->length()) - 1;
    while (end_pos >= 0 && isspace(str->at(end_pos)))
        end_pos--;
    *str = str->substr(0, end_pos + 1);
}

std::string StringTrimRight(const StringPiece& str)
{
    std::string res = str.as_string();
    StringTrimRight(&res);
    return res;
}

template <typename StringType>
static void DoStringTrim(StringType* str)
{
    size_t start_pos = 0;
    size_t end_pos = str->length();
    while (start_pos != end_pos && isspace((*str)[start_pos]))
        start_pos++;
    if (start_pos == end_pos)
    {
        str->clear();
        return;
    }
    end_pos--;
    while (isspace((*str)[end_pos])) // end_pos always >= 0
        end_pos--;
    *str = str->substr(start_pos, end_pos - start_pos + 1);
}

void StringTrim(std::string* str)
{
    DoStringTrim(str);
}

void StringTrim(StringPiece* str)
{
    DoStringTrim(str);
}

std::string StringTrim(const StringPiece& str)
{
    std::string res = str.as_string();
    StringTrim(&res);
    return res;
}

void StringTrimLeft(std::string* str, const StringPiece& trim_value)
{
    StringPiece piece(*str);
    size_t start_pos = piece.find_first_not_of(trim_value);
    if (start_pos != std::string::npos) {
        *str = str->substr(start_pos);
    } else {
        str->clear();
    }
}

std::string StringTrimLeft(const StringPiece& str, const StringPiece& trim_value)
{
    std::string res = str.as_string();
    StringTrimLeft(&res, trim_value);
    return res;
}

void StringTrimRight(std::string* str, const StringPiece& trim_value)
{
    StringPiece piece(*str);
    size_t end_pos = piece.find_last_not_of(trim_value);
    if (end_pos != std::string::npos) {
        *str = str->substr(0, end_pos + 1);
    } else {
        str->clear();
    }
}

std::string StringTrimRight(const StringPiece& str, const StringPiece& trim_value)
{
    std::string res = str.as_string();
    StringTrimRight(&res, trim_value);
    return res;
}

template <typename StringType>
static void DoStringTrim(StringType* str, const StringPiece& trim_value)
{
    StringPiece piece(*str);
    size_t start_pos = piece.find_first_not_of(trim_value);
    size_t end_pos = piece.find_last_not_of(trim_value);
    if (start_pos == std::string::npos)
    {
        str->clear();
        return;
    }
    *str = str->substr(start_pos, end_pos - start_pos + 1);
}

void StringTrim(std::string* str, const StringPiece& trim_value)
{
    DoStringTrim(str, trim_value);
}

void StringTrim(StringPiece* str, const StringPiece& trim_value)
{
    DoStringTrim(str, trim_value);
}

std::string StringTrim(const StringPiece& str, const StringPiece& trim_value) {
    std::string res = str.as_string();
    StringTrim(&res, trim_value);
    return res;
}

} // namespace toft
