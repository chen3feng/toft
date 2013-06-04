// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/encoding/varint.h"

#include <assert.h>
#include <string.h>

namespace toft {

char* EncodeVarint32(char* dst, uint32_t v) {
    // Operate on characters as unsigneds
    unsigned char* ptr = reinterpret_cast<unsigned char*>(dst);
    static const int B = 128;
    if (v < (1 << 7)) {
        *(ptr++) = v;
    } else if (v < (1 << 14)) {
        *(ptr++) = v | B;
        *(ptr++) = v >> 7;
    } else if (v < (1 << 21)) {
        *(ptr++) = v | B;
        *(ptr++) = (v >> 7) | B;
        *(ptr++) = v >> 14;
    } else if (v < (1 << 28)) {
        *(ptr++) = v | B;
        *(ptr++) = (v >> 7) | B;
        *(ptr++) = (v >> 14) | B;
        *(ptr++) = v >> 21;
    } else {
        *(ptr++) = v | B;
        *(ptr++) = (v >> 7) | B;
        *(ptr++) = (v >> 14) | B;
        *(ptr++) = (v >> 21) | B;
        *(ptr++) = v >> 28;
    }
    return reinterpret_cast<char*>(ptr);
}

void PutVarint32(std::string* dst, uint32_t v) {
    char buf[5];
    char* ptr = EncodeVarint32(buf, v);
    dst->append(buf, ptr - buf);
}

char* EncodeVarint64(char* dst, uint64_t v) {
    static const int B = 128;
    unsigned char* ptr = reinterpret_cast<unsigned char*>(dst);
    while (v >= static_cast<uint64_t>(B)) {
        *(ptr++) = (v & (B - 1)) | B;
        v >>= 7;
    }
    *(ptr++) = static_cast<unsigned char>(v);
    return reinterpret_cast<char*>(ptr);
}

void PutVarint64(std::string* dst, uint64_t v) {
    char buf[10];
    char* ptr = EncodeVarint64(buf, v);
    dst->append(buf, ptr - buf);
}

void PutLengthPrefixedStringPiece(std::string* dst, const StringPiece& value) {
    PutVarint32(dst, value.size());
    dst->append(value.data(), value.size());
}

int VarintLength(uint64_t v) {
    int len = 1;
    while (v >= 128) {
        v >>= 7;
        len++;
    }
    return len;
}

const char* GetVarint32PtrFallback(const char* p, const char* limit, uint32_t* value) {
    uint32_t result = 0;
    for (uint32_t shift = 0; shift <= 28 && p < limit; shift += 7) {
        uint32_t byte = *(reinterpret_cast<const unsigned char*>(p));
        p++;
        if (byte & 128) {
            // More bytes are present
            result |= ((byte & 127) << shift);
        } else {
            result |= (byte << shift);
            *value = result;
            return reinterpret_cast<const char*>(p);
        }
    }
    return NULL;
}

bool GetVarint32(StringPiece* input, uint32_t* value) {
    const char* p = input->data();
    const char* limit = p + input->size();
    const char* q = GetVarint32Ptr(p, limit, value);
    if (q == NULL) {
        return false;
    } else {
        *input = StringPiece(q, limit - q);
        return true;
    }
}

const char* GetVarint64Ptr(const char* p, const char* limit, uint64_t* value) {
    uint64_t result = 0;
    for (uint32_t shift = 0; shift <= 63 && p < limit; shift += 7) {
        uint64_t byte = *(reinterpret_cast<const unsigned char*>(p));
        p++;
        if (byte & 128) {
            // More bytes are present
            result |= ((byte & 127) << shift);
        } else {
            result |= (byte << shift);
            *value = result;
            return reinterpret_cast<const char*>(p);
        }
    }
    return NULL;
}

bool GetVarint64(StringPiece* input, uint64_t* value) {
    const char* p = input->data();
    const char* limit = p + input->size();
    const char* q = GetVarint64Ptr(p, limit, value);
    if (q == NULL) {
        return false;
    } else {
        *input = StringPiece(q, limit - q);
        return true;
    }
}

const char* GetLengthPrefixedStringPiece(const char* p, const char* limit, StringPiece* result) {
    uint32_t len;
    p = GetVarint32Ptr(p, limit, &len);
    if (p == NULL)
        return NULL;
    if (p + len > limit)
        return NULL;
    *result = StringPiece(p, len);
    return p + len;
}

bool GetLengthPrefixedStringPiece(StringPiece* input, StringPiece* result) {
    uint32_t len;
    if (GetVarint32(input, &len) && input->size() >= len) {
        *result = StringPiece(input->data(), len);
        input->remove_prefix(len);
        return true;
    } else {
        return false;
    }
}

}  // namespace toft
