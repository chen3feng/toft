// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#ifndef TOFT_ENCODING_VARINT_H
#define TOFT_ENCODING_VARINT_H

#include <stdint.h>

#include <string>
#include <vector>

#include "toft/base/string/string_piece.h"

namespace toft {

struct Varint {
public:
    // Standard Put... routines append to a string
    static void Put32(std::string* dst, uint32_t value);
    static void Put64(std::string* dst, uint64_t value);
    static void PutLengthPrefixedStringPiece(std::string* dst, const StringPiece& value);

    // Standard Get... routines parse a value from the beginning of a StringPiece
    // and advance the StringPiece past the parsed value.
    static bool Get32(StringPiece* input, uint32_t* value);
    static bool Get64(StringPiece* input, uint64_t* value);
    static bool GetLengthPrefixedStringPiece(StringPiece* input, StringPiece* result);
    static const char* GetLengthPrefixedStringPiece(const char* p,
                                                    const char* limit,
                                                    StringPiece* result);

    // Pointer-based variants of Get...  These either store a value
    // in *v and return a pointer just past the parsed value, or return
    // NULL on error.  These routines only look at bytes in the range
    // [p..limit-1]
    static const char* Get32Ptr(const char* p, const char* limit, uint32_t* v);
    static const char* Get64Ptr(const char* p, const char* limit, uint64_t* v);

    // Returns the length of the varint32 or varint64 encoding of "v"
    static int Length(uint64_t v);

    // Lower-level versions of Put... that write directly into a character buffer
    // and return a pointer just past the last byte written.
    // REQUIRES: dst has enough space for the value being written
    static char* Encode32(char* dst, uint32_t value);
    static char* Encode64(char* dst, uint64_t value);

    // Internal routine for use by fallback path of Get32Ptr
    static const char* Get32PtrFallback(const char* p, const char* limit, uint32_t* value);
};
}  // namespace toft

#endif  // TOFT_ENCODING_VARINT_H
