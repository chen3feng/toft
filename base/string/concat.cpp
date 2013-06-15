// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 04/30/11
// Description: string concat

#include "toft/base/string/concat.h"

#include "toft/base/string/number.h"

// GLOBAL_NOLINT(runtime/int)

namespace toft {

template <typename T>
static void StringAppendInteger(std::string* str, T value)
{
    char buffer[kMaxIntegerStringSize];
    str->append(buffer, WriteIntegerToBuffer(value, buffer) - buffer);
}

void StringAppend(std::string* str, short value)
{
    StringAppendInteger(str, value);
}

void StringAppend(std::string* str, unsigned short value)
{
    StringAppendInteger(str, value);
}

void StringAppend(std::string* str, int value)
{
    StringAppendInteger(str, value);
}

void StringAppend(std::string* str, unsigned int value)
{
    StringAppendInteger(str, value);
}

void StringAppend(std::string* str, long value)
{
    StringAppendInteger(str, value);
}

void StringAppend(std::string* str, unsigned long value)
{
    StringAppendInteger(str, value);
}

void StringAppend(std::string* str, long long value)
{
    StringAppendInteger(str, value);
}

void StringAppend(std::string* str, unsigned long long value)
{
    StringAppendInteger(str, value);
}

void StringAppend(std::string* str, float value)
{
    char buffer[kMaxFloatStringSize];
    str->append(buffer, WriteFloatToBuffer(value, buffer));
}

void StringAppend(std::string* str, double value)
{
    char buffer[kMaxDoubleStringSize];
    str->append(buffer, WriteDoubleToBuffer(value, buffer));
}

void StringAppend(std::string* str, long double value)
{
    StringAppend(str, static_cast<double>(value));
}

} // namespace toft
