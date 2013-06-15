// Copyright (c) 2013, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-02-07

#ifndef TOFT_BASE_STRING_FORMAT_VPRINT_H
#define TOFT_BASE_STRING_FORMAT_VPRINT_H
#pragma once

#include <string>

#include "toft/base/string/format/print_arg.h"

namespace toft {

int VFormatPrint(FormatPrintTarget* target, const char* format,
                 const FormatPrintArg** args, int nargs);

int StringVPrintAppend(std::string* out, const char* format,
                       const FormatPrintArg** args, int argc);

int StringVPrintTo(std::string* out, const char* format,
                   const FormatPrintArg** args, int argc);

std::string StringVPrint(const char* format,
                         const FormatPrintArg** args, int argc);

} // namespace toft

#endif // TOFT_BASE_STRING_FORMAT_VPRINT_H
