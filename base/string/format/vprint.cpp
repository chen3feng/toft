// Copyright (c) 2013, The TOFT Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 2013-02-07

#include "toft/base/string/format/vprint.h"

#include <string.h>
#include <string>

#include "toft/base/string/format/print_arg.h"
#include "toft/base/string/format/print_targets.h"
#include "toft/base/string/format/specification.h"

#include "thirdparty/glog/logging.h"

namespace toft {

#ifndef _GNU_SOURCE
static const char* strchrnul(const char* s, int ch)
{
    const char* p = s;
    while (*p && *p != ch) {
        ++p;
    }
    return p;
}
#endif

static int DoFillSpeciationFromArg(int* value,
                                   const FormatPrintArg** args, int nargs,
                                   int* arg_index)
{
    if (*arg_index >= nargs) {
        LOG(DFATAL) << "Arg out of bound";
        return -1;
    }
    *value = args[*arg_index ]->AsInt();
    if (*value < -'*') {
        LOG(ERROR) << "Can't convert arg " << *arg_index << " to int";
        return -1;
    }
    ++*arg_index;
    return 1;
}

static inline int FillSpeciationFromArg(int* value,
                                        const FormatPrintArg** args, int nargs,
                                        int* arg_index)
{
    if (*value == -'*') { // is '*'
        return DoFillSpeciationFromArg(value, args, nargs, arg_index);
    }
    return 0;
}

int VFormatPrint(FormatPrintTarget* target, const char* format,
                 const FormatPrintArg** args, int nargs)
{
    int total_printed = 0;
    int ai = 0;
    const char* f = format;
    while (*f != '\0') {
        const char* p = strchrnul(f, '%');
        target->WriteString(f, p - f);
        total_printed += p - f;
        f = p;
        if (*f) { // If not nul, must be '%'
            if (f[1] == '\0')
                return -1;
            if (f[1] == '%') {
                target->WriteChar('%');
                ++total_printed;
                f += 2;
            } else {
                ++f;
                PrintSpecification spec;
                int n = spec.Parse(f);
                if (n > 0) {
                    if (FillSpeciationFromArg(&spec.width, args, nargs, &ai) < 0)
                        return -1;
                    if (FillSpeciationFromArg(&spec.precision, args, nargs, &ai) < 0)
                        return -1;
                    if (ai >= nargs) {
                        LOG(DFATAL) << "Arg out of bound";
                        return -1;
                    }
                    int printed = args[ai]->Write(target, spec);
                    if (printed < 0)
                        return -1;
                    total_printed += printed;
                    f += n;
                    ++ai;
                } else {
                    return -1;
                }
            }
        }
    }

    if (ai != nargs) {
        LOG(WARNING) << "Extra param provided, expect " << ai << ", "
            << nargs << " provided";
    }
    return total_printed;
}

int StringVPrintAppend(std::string* target, const char* format,
                       const FormatPrintArg** args, int nargs)
{
    // Reserve spece befor write to reduce memory allocation.
    target->reserve(target->size() + 128);
    StringFormatPrintTarget t(target);
    return VFormatPrint(&t, format, args, nargs);
}

int StringVPrintTo(std::string* target, const char* format,
                   const FormatPrintArg** args, int nargs)
{
    target->clear();
    return StringVPrintAppend(target, format, args, nargs);
}

std::string StringVPrint(const char* format,
                         const FormatPrintArg** args, int nargs)
{
    std::string s;
    int n = StringVPrintAppend(&s, format, args, nargs);
    if (n < 0) {
        LOG(DFATAL) << "StringVPrint error, format: " << format;
    }
    return s;
}

} // namespace toft

