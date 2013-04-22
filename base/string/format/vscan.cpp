// Copyright (c) 2013, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-02-05

#include "toft/base/string/format/vscan.h"

#include <stdlib.h>
#include <string.h>

#include "toft/base/string/format/scan_arg.h"

#include "thirdparty/glog/logging.h"

namespace toft {

int StringVScan(const char* string, const char* format,
                const FormatScanArg** args, int nargs)
{
    int success_count = 0;
    int ai = 0;
    const char* f = format;
    const char* s = string;
    while (*f != '\0') {
        if (*f == '%') {
            if (f[1] == '\0')
                return -1;
            if (f[1] == '%') {
                if (*s == '%') {
                    f += 2;
                    ++s;
                } else {
                    // Not match '%'
                    break;
                }
            } else {
                ++f;
                ScanSpecification spec;
                int fw = spec.Parse(f);
                if (fw == 0)
                    break;
                bool success;
                int sw = 0;
                if (spec.skip) {
                    sw = FormatScanArg::ParseSkipped(s, spec);
                    success = sw > 0;
                } else {
                    if (spec.specifier == 'n') {
                        success = args[ai]->WriteInt(s - string, spec);
                        if (success)
                            ++success_count;
                        else
                            LOG(ERROR) << "Invalid argument " << ai << " for %n";
                    } else {
                        sw = args[ai]->Parse(s, spec);
                        success = sw > 0;
                        if (success) {
                            ++success_count;
                            ++ai;
                        }
                    }
                }
                if (success) {
                    f += fw;
                    s += sw;
                } else {
                    break;
                }
            }
        } else {
            if (*s == *f) {
                ++f;
                ++s;
            } else {
                // Not match
                break;
            }
        }
    }

    if (ai != nargs) {
        LOG(WARNING) << "Extra param provided, expect " << ai << ", "
            << nargs << " provided";
    }
    return success_count;
}

} // namespace toft
