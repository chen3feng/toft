// Copyright (c) 2013, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-02-05

#ifndef TOFT_BASE_STRING_FORMAT_VSCAN_H
#define TOFT_BASE_STRING_FORMAT_VSCAN_H
#pragma once

#include <string>

#include "toft/base/string/format/scan_arg.h"

namespace toft {

// Scan string with arguments vector
// Return: number filelds of success scanned.
int StringVScan(const char* string, const char* format, const FormatScanArg** args, int nargs);

} // namespace toft

#endif // TOFT_BASE_STRING_FORMAT_VSCAN_H
