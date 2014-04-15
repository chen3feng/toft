// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_NET_HTTP_VERSION_H
#define TOFT_NET_HTTP_VERSION_H
#pragma once

#include <stdint.h>

namespace toft {

// Wrapper for an HTTP (major,minor) version pair.
class HttpVersion {
public:
    // Default constructor (major=0, minor=0).
    HttpVersion() : m_value(0) { }

    // Build from unsigned major/minor pair.
    HttpVersion(int major, int minor) : m_value(major << 16 | minor) {}

    // Major version number.
    int Major() const { return m_value >> 16; }

    // Minor version number.
    int Minor() const { return m_value & 0xffff; }

    bool IsEmpty() const { return m_value == 0; }

    // Clear to empty.
    void Clear() { m_value = 0; }

    // Overloaded operators:
    bool operator==(const HttpVersion& v) const {
        return m_value == v.m_value;
    }
    bool operator!=(const HttpVersion& v) const {
        return m_value != v.m_value;
    }
    bool operator>(const HttpVersion& v) const {
        return m_value > v.m_value;
    }
    bool operator>=(const HttpVersion& v) const {
        return m_value >= v.m_value;
    }
    bool operator<(const HttpVersion& v) const {
        return m_value < v.m_value;
    }
    bool operator<=(const HttpVersion& v) const {
        return m_value <= v.m_value;
    }

private:
    uint32_t m_value; // Packed as <major>:<minor>
};

} // namespace toft

#endif // TOFT_NET_HTTP_VERSION_H
