// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/encoding/varint.h"

#include <assert.h>
#include <string.h>

namespace {
static const int kIncompleteMask = 128;
}

namespace toft {

char* Varint::UnsafeEncode32(char* dst, uint32_t v) {
    // Operate on characters as unsigneds
    unsigned char* ptr = reinterpret_cast<unsigned char*>(dst);
    if (v < (1 << 7)) {
        *(ptr++) = v;
    } else if (v < (1 << 14)) {
        *(ptr++) = v | kIncompleteMask;
        *(ptr++) = v >> 7;
    } else if (v < (1 << 21)) {
        *(ptr++) = v | kIncompleteMask;
        *(ptr++) = (v >> 7) | kIncompleteMask;
        *(ptr++) = v >> 14;
    } else if (v < (1 << 28)) {
        *(ptr++) = v | kIncompleteMask;
        *(ptr++) = (v >> 7) | kIncompleteMask;
        *(ptr++) = (v >> 14) | kIncompleteMask;
        *(ptr++) = v >> 21;
    } else {
        *(ptr++) = v | kIncompleteMask;
        *(ptr++) = (v >> 7) | kIncompleteMask;
        *(ptr++) = (v >> 14) | kIncompleteMask;
        *(ptr++) = (v >> 21) | kIncompleteMask;
        *(ptr++) = v >> 28;
    }
    return reinterpret_cast<char*>(ptr);
}

void Varint::Put32(std::string* dst, uint32_t v) {
    char buf[5];
    char* ptr = UnsafeEncode32(buf, v);
    dst->append(buf, ptr - buf);
}

char* Varint::UnsafeEncode64(char* dst, uint64_t v) {
    unsigned char* ptr = reinterpret_cast<unsigned char*>(dst);
    while (v >= static_cast<uint64_t>(kIncompleteMask)) {
        *(ptr++) = (v & (kIncompleteMask - 1)) | kIncompleteMask;
        v >>= 7;
    }
    *(ptr++) = static_cast<unsigned char>(v);
    return reinterpret_cast<char*>(ptr);
}

void Varint::Put64(std::string* dst, uint64_t v) {
    char buf[10];
    char* ptr = UnsafeEncode64(buf, v);
    dst->append(buf, ptr - buf);
}

char* Varint::Encode32(char* p, char* limit, uint32_t v) {
    if (EncodedLength(v) > limit - p)
        return NULL;
    return UnsafeEncode32(p, v);
}

char* Varint::Encode64(char* p, char* limit, uint64_t v) {
    if (EncodedLength(v) > limit - p)
        return NULL;
    return UnsafeEncode64(p, v);
}

void Varint::PutLengthPrefixedStringPiece(std::string* dst, const StringPiece& value) {
    Put32(dst, value.size());
    dst->append(value.data(), value.size());
}

char* Varint::PutLengthPrefixedStringPiece(char* p,
                                           char* limit,
                                           const StringPiece& value) {
    if (EncodedLength(value) > limit - p)
        return NULL;
    char* ptr = UnsafeEncode32(p, value.size());
    memcpy(ptr, value.data(), value.size());
    return ptr + value.size();
}

int Varint::EncodedLength(uint64_t v) {
    int len = 1;
    while (v >= static_cast<uint64_t>(kIncompleteMask)) {
        v >>= 7;
        len++;
    }
    return len;
}

int Varint::EncodedLength(const StringPiece& value) {
    return EncodedLength(value.size()) + static_cast<int>(value.size());
}

const char* Varint::Decode32Fallback(const char* p, const char* limit, uint32_t* value) {
    uint32_t result = 0;
    for (uint32_t shift = 0; shift <= 28 && p < limit; shift += 7) {
        uint32_t byte = *(reinterpret_cast<const unsigned char*>(p));
        p++;
        if (byte & kIncompleteMask) {
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

bool Varint::Get32(StringPiece* input, uint32_t* value) {
    const char* p = input->data();
    const char* limit = p + input->size();
    const char* q = Decode32(p, limit, value);
    if (q == NULL) {
        return false;
    } else {
        *input = StringPiece(q, limit - q);
        return true;
    }
}

const char* Varint::Decode64(const char* p, const char* limit, uint64_t* value) {
    uint64_t result = 0;
    for (uint32_t shift = 0; shift <= 63 && p < limit; shift += 7) {
        uint64_t byte = *(reinterpret_cast<const unsigned char*>(p));
        p++;
        if (byte & kIncompleteMask) {
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

bool Varint::Get64(StringPiece* input, uint64_t* value) {
    const char* p = input->data();
    const char* limit = p + input->size();
    const char* q = Decode64(p, limit, value);
    if (q == NULL) {
        return false;
    } else {
        *input = StringPiece(q, limit - q);
        return true;
    }
}

const char* Varint::GetLengthPrefixedStringPiece(const char* p,
                                                 const char* limit,
                                                 StringPiece* result) {
    uint32_t len;
    p = Decode32(p, limit, &len);
    if (p == NULL)
        return NULL;
    if (p + len > limit)
        return NULL;
    *result = StringPiece(p, len);
    return p + len;
}

bool Varint::GetLengthPrefixedStringPiece(StringPiece* input, StringPiece* result) {
    uint32_t len;
    if (Get32(input, &len) && input->size() >= len) {
        *result = StringPiece(input->data(), len);
        input->remove_prefix(len);
        return true;
    } else {
        return false;
    }
}
}  // namespace toft
