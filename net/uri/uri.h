// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 05/11/11
// Description: URI class, based on RFC 3986

#ifndef TOFT_NET_URI_URI_H
#define TOFT_NET_URI_URI_H

#include <cassert>
#include <string>
#include <utility>
#include "toft/base/string/string_piece.h"

namespace toft {

class UriAuthority
{
    friend class URI;
public:
    UriAuthority():
        m_has_user_info(false),
        m_has_port(false)
    {
    }

public: // Attributes
    bool HasUserInfo() const { return m_has_user_info; }
    const std::string& UserInfo() const {
        assert(m_has_user_info);
        return m_user_info;
    }
    void SetUserInfo(const std::string& value) {
        m_user_info = value;
        m_has_user_info = true;
    }
    void SetUserInfo(const char* value) {
        m_user_info = value;
        m_has_user_info = true;
    }
    void SetUserInfo(const char* value, size_t length)
    {
        m_user_info.assign(value, length);
        m_has_user_info = true;
    }
    void ClearUserInfo() {
        m_user_info.clear();
        m_has_user_info = false;
    }

    const std::string& Host() const { return m_host; }
    void SetHost(const std::string& value) { m_host = value; }
    void SetHost(const char* value) { m_host = value; }
    void SetHost(const char* value, size_t length)
    {
        m_host.assign(value, length);
    }

    bool HasPort() const { return m_has_port; }
    const std::string& Port() const {
        assert(m_has_port);
        return m_port;
    }
    void SetPort(const std::string& value) {
        m_port = value;
        m_has_port = true;
    }
    void SetPort(const char* value) {
        m_port = value;
        m_has_port = true;
    }
    void SetPort(const char* value, size_t length)
    {
        m_port.assign(value, length);
        m_has_port = true;
    }
    void ClearPort() {
        m_port.clear();
        m_has_port = false;
    }

    void Clear()
    {
        ClearUserInfo();
        m_host.clear();
        ClearPort();
    }

    void Swap(UriAuthority* other);
private:
    bool m_has_user_info;
    bool m_has_port;
    std::string m_user_info;
    std::string m_host;
    std::string m_port;
};

// RFC 3986 URI class
class URI
{
public:
    URI() :
        m_has_authority(false),
        m_has_query(false),
        m_has_fragment(false)
    {
    }

public: // Attributes
    // Scheme
    const std::string& Scheme() const { return m_scheme; }
    void SetScheme(const std::string& value) { m_scheme = value; }
    void SetScheme(const char* value) { m_scheme = value; }
    void SetScheme(const char* value, size_t length) { m_scheme.assign(value, length); }

    // Authority
    bool HasAuthority() const { return m_has_authority; }
    UriAuthority& Authority() {
        assert(m_has_authority);
        return m_authority;
    }
    const UriAuthority& Authority() const {
        assert(m_has_authority);
        return m_authority;
    }
    void SetAuthority(const UriAuthority& value) {
        m_has_authority = true;
        m_authority = value;
    }
    void ClearAuthority() {
        m_authority.Clear();
        m_has_authority = false;
    }

    // UserInfo
    bool HasUserInfo() const { return m_has_authority && m_authority.HasUserInfo(); }
    const std::string& UserInfo() const {
        assert(m_has_authority);
        return Authority().UserInfo();
    }
    void SetUserInfo(const std::string& value)
    {
        m_has_authority = true;
        m_authority.SetUserInfo(value);
    }
    void SetUserInfo(const char* value)
    {
        m_has_authority = true;
        m_authority.SetUserInfo(value);
    }
    void SetUserInfo(const char* value, size_t length)
    {
        m_has_authority = true;
        m_authority.SetUserInfo(value, length);
    }
    void ClearUserInfo() {
        assert(m_has_authority);
        m_authority.ClearUserInfo();
    }

    // Host
    bool HasHost() const { return m_has_authority; }
    const std::string& Host() const {
        assert(m_has_authority);
        return m_authority.Host();
    }
    void SetHost(const std::string& value)
    {
        m_has_authority = true;
        m_authority.SetHost(value);
    }
    void SetHost(const char* value, size_t length)
    {
        m_has_authority = true;
        m_authority.SetHost(value, length);
    }

    // Port
    bool HasPort() const { return m_has_authority && m_authority.HasPort(); }
    const std::string& Port() const {
        assert(m_has_authority);
        return m_authority.Port();
    }
    void SetPort(const std::string& value)
    {
        m_has_authority = true;
        m_authority.SetPort(value);
    }
    void SetPort(const char* value, size_t length)
    {
        m_has_authority = true;
        m_authority.SetPort(value, length);
    }
    void ClearPort() {
        assert(m_has_authority);
        m_authority.ClearPort();
    }

    // Path
    const std::string& Path() const { return m_path; }
    void SetPath(const std::string& value) { m_path = value; }
    void SetPath(const char* value) { m_path = value; }
    void SetPath(const char* value, size_t length)
    {
        m_path.assign(value, length);
    }

    // Query
    bool HasQuery() const { return m_has_query; }
    const std::string& Query() const { return m_query; }
    void SetQuery(const std::string& value)
    {
        m_query = value;
        m_has_query = true;
    }
    void SetQuery(const char* value)
    {
        m_query = value;
        m_has_query = true;
    }
    void SetQuery(const char* value, size_t length)
    {
        m_query.assign(value, length);
        m_has_query = true;
    }
    void ClearQuery() {
        m_query.clear();
        m_has_query = false;
    }

    std::string PathAndQuery() const { return HasQuery() ? Path() + "?" + Query() : Path(); }

    // Fragment
    bool HasFragment() const { return m_has_fragment; }
    const std::string& Fragment() const { return m_fragment; }
    void SetFragment(const std::string& value) {
        m_fragment = value;
        m_has_fragment = true;
    }
    void SetFragment(const char* value) {
        m_fragment = value;
        m_has_fragment = true;
    }
    void SetFragment(const char* value, size_t length)
    {
        m_fragment.assign(value, length);
        m_has_fragment = true;
    }
    void ClearFragment() {
        m_fragment.clear();
        m_has_fragment = false;
    }

public: // operations
    std::string& ToString(std::string* result) const;
    std::string ToString() const;

    bool WriteToBuffer(char* buffer, size_t buffer_size, size_t* result_size) const;
    bool WriteToBuffer(char* buffer, size_t buffer_size) const
    {
        size_t result_size; // ignore length
        return WriteToBuffer(buffer, buffer_size, &result_size);
    }

    // clear to empty
    void Clear();

    // swap with other URI object
    void Swap(URI* other);

    bool Normalize();
    bool ToAbsolute(const URI& base);

    // parse a length specified buffer
    // return parsed length
    size_t ParseBuffer(const char* uri, size_t uri_length);

    bool Parse(const char* uri);
    bool Parse(const std::string& uri)
    {
        return ParseBuffer(uri.data(), uri.length()) == uri.length();
    }

    bool Merge(const URI& base, bool strict = false);

    // Same as encodeURI in javascript
    static void Encode(const StringPiece& src, std::string* dest);
    static std::string Encode(const StringPiece& src);
    static void Encode(std::string* uri);

    // Same as encodeURIComponent in javascript
    static void EncodeComponent(const StringPiece& src, std::string* dest);
    static std::string EncodeComponent(const StringPiece& src);
    static void EncodeComponent(std::string* uri);

    // Decode % encoding string
    static bool Decode(const StringPiece& src, std::string* result);

private:
    static void StringLower(std::string* str)
    {
        for (size_t i = 0; i < str->length(); ++i)
            (*str)[i] = tolower((*str)[i]);
    }
private:
    bool m_has_authority;
    bool m_has_query;
    bool m_has_fragment;
    std::string m_scheme;
    UriAuthority m_authority;
    std::string m_path;
    std::string m_query;
    std::string m_fragment;
};

} // namespace toft

// fit to STL
namespace std
{

template <>
inline void swap(toft::UriAuthority& x, toft::UriAuthority& y)
{
    x.Swap(&y);
}

template <>
inline void swap(toft::URI& x, toft::URI& y)
{
    x.Swap(&y);
}

} // namespace std

#endif // TOFT_NET_URI_URI_H

