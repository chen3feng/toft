// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/encoding/varint.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {

TEST(VarintUnittest, Varint32) {
    std::string s;
    for (uint32_t i = 0; i < (32 * 32); i++) {
        uint32_t v = (i / 32) << (i % 32);
        PutVarint32(&s, v);
    }

    const char* p = s.data();
    const char* limit = p + s.size();
    for (uint32_t i = 0; i < (32 * 32); i++) {
        uint32_t expected = (i / 32) << (i % 32);
        uint32_t actual;
        const char* start = p;
        p = GetVarint32Ptr(p, limit, &actual);
        ASSERT_TRUE(p != NULL);
        ASSERT_EQ(expected, actual);
        ASSERT_EQ(VarintLength(actual), p - start);
    }
    ASSERT_EQ(p, s.data() + s.size());
}

TEST(VarintUnittest, Varint64) {
    // Construct the list of values to check
    std::vector<uint64_t> values;
    // Some special values
    values.push_back(0);
    values.push_back(100);
    values.push_back(~static_cast<uint64_t>(0));
    values.push_back(~static_cast<uint64_t>(0) - 1);
    for (uint32_t k = 0; k < 64; k++) {
        // Test values near powers of two
        const uint64_t power = 1ull << k;
        values.push_back(power);
        values.push_back(power - 1);
        values.push_back(power + 1);
    };

    std::string s;
    for (size_t i = 0; i < values.size(); i++) {
        PutVarint64(&s, values[i]);
    }

    const char* p = s.data();
    const char* limit = p + s.size();
    for (size_t i = 0; i < values.size(); i++) {
        ASSERT_TRUE(p < limit);
        uint64_t actual;
        const char* start = p;
        p = GetVarint64Ptr(p, limit, &actual);
        ASSERT_TRUE(p != NULL);
        ASSERT_EQ(values[i], actual);
        ASSERT_EQ(VarintLength(actual), p - start);
    }
    ASSERT_EQ(p, limit);
}

TEST(VarintUnittest, Varint32Overflow) {
    uint32_t result;
    std::string input("\x81\x82\x83\x84\x85\x11");
    ASSERT_TRUE(GetVarint32Ptr(input.data(), input.data() + input.size(), &result) == NULL);
}

TEST(VarintUnittest, Varint32Truncation) {
    uint32_t large_value = (1u << 31) + 100;
    std::string s;
    PutVarint32(&s, large_value);
    uint32_t result;
    for (size_t len = 0; len < s.size() - 1; len++) {
        ASSERT_TRUE(GetVarint32Ptr(s.data(), s.data() + len, &result) == NULL);
    }
    ASSERT_TRUE(GetVarint32Ptr(s.data(), s.data() + s.size(), &result) != NULL);
    ASSERT_EQ(large_value, result);
}

TEST(VarintUnittest, Varint64Overflow) {
    uint64_t result;
    std::string input("\x81\x82\x83\x84\x85\x81\x82\x83\x84\x85\x11");
    ASSERT_TRUE(GetVarint64Ptr(input.data(), input.data() + input.size(), &result) == NULL);
}

TEST(VarintUnittest, Varint64Truncation) {
    uint64_t large_value = (1ull << 63) + 100ull;
    std::string s;
    PutVarint64(&s, large_value);
    uint64_t result;
    for (size_t len = 0; len < s.size() - 1; len++) {
        ASSERT_TRUE(GetVarint64Ptr(s.data(), s.data() + len, &result) == NULL);
    }
    ASSERT_TRUE(GetVarint64Ptr(s.data(), s.data() + s.size(), &result) != NULL);
    ASSERT_EQ(large_value, result);
}

TEST(VarintUnittest, Strings) {
    std::string s;
    PutLengthPrefixedStringPiece(&s, StringPiece(""));
    PutLengthPrefixedStringPiece(&s, StringPiece("foo"));
    PutLengthPrefixedStringPiece(&s, StringPiece("bar"));
    PutLengthPrefixedStringPiece(&s, StringPiece(std::string(200, 'x')));

    StringPiece input(s);
    StringPiece v;
    ASSERT_TRUE(GetLengthPrefixedStringPiece(&input, &v));
    ASSERT_EQ("", v.as_string());
    ASSERT_TRUE(GetLengthPrefixedStringPiece(&input, &v));
    ASSERT_EQ("foo", v.as_string());
    ASSERT_TRUE(GetLengthPrefixedStringPiece(&input, &v));
    ASSERT_EQ("bar", v.as_string());
    ASSERT_TRUE(GetLengthPrefixedStringPiece(&input, &v));
    ASSERT_EQ(std::string(200, 'x'), v.as_string());
    ASSERT_EQ("", input.as_string());
}

}  // namespace toft
