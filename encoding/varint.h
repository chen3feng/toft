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

// Standard Put... routines append to a string
void PutVarint32(std::string* dst, uint32_t value);
void PutVarint64(std::string* dst, uint64_t value);
void PutLengthPrefixedStringPiece(std::string* dst, const StringPiece& value);

// Standard Get... routines parse a value from the beginning of a StringPiece
// and advance the StringPiece past the parsed value.
bool GetVarint32(StringPiece* input, uint32_t* value);
bool GetVarint64(StringPiece* input, uint64_t* value);
bool GetLengthPrefixedStringPiece(StringPiece* input, StringPiece* result);

// Pointer-based variants of GetVarint...  These either store a value
// in *v and return a pointer just past the parsed value, or return
// NULL on error.  These routines only look at bytes in the range
// [p..limit-1]
const char* GetVarint32Ptr(const char* p, const char* limit, uint32_t* v);
const char* GetVarint64Ptr(const char* p, const char* limit, uint64_t* v);

// Returns the length of the varint32 or varint64 encoding of "v"
int VarintLength(uint64_t v);

// Lower-level versions of Put... that write directly into a character buffer
// and return a pointer just past the last byte written.
// REQUIRES: dst has enough space for the value being written
char* EncodeVarint32(char* dst, uint32_t value);
char* EncodeVarint64(char* dst, uint64_t value);

// Lower-level versions of Get... that read directly from a character buffer
// without any bounds checking.

// Internal routine for use by fallback path of GetVarint32Ptr
const char* GetVarint32PtrFallback(const char* p, const char* limit, uint32_t* value);
inline const char* GetVarint32Ptr(const char* p, const char* limit, uint32_t* value) {
    if (p < limit) {
        uint32_t result = *(reinterpret_cast<const unsigned char*>(p));
        if ((result & 128) == 0) {
            *value = result;
            return p + 1;
        }
    }
    return GetVarint32PtrFallback(p, limit, value);
}

}  // namespace toft

#endif  // TOFT_ENCODING_VARINT_H
