// Copyright 2011, The Toft Authors.
// Author: Hangjun Ye <yehangjun@gmail.com>
// Xiaokang Liu <liuxk02@gmail.com>

#ifndef TOFT_NET_HTTP_MESSAGE_H
#define TOFT_NET_HTTP_MESSAGE_H

#include <map>
#include <string>
#include <vector>
#include "toft/base/string/string_piece.h"
#include "toft/net/http/headers.h"
#include "toft/net/http/version.h"

namespace toft {

// Describes an http message, which is the base class for http request and
// response. It includes the start line, headers and body.
class HttpMessage {
public:
    enum ErrorCode {
        SUCCESS = 0,
        ERROR_NO_START_LINE,
        ERROR_START_LINE_NOT_COMPLETE,
        ERROR_VERSION_UNSUPPORTED,
        ERROR_RESPONSE_STATUS_NOT_FOUND,
        ERROR_FIELD_NOT_COMPLETE,
        ERROR_METHOD_NOT_FOUND,
        ERROR_MESSAGE_NOT_COMPLETE,
    };

    HttpMessage() : m_version(1, 1) {}
    virtual ~HttpMessage() {}
    virtual void Reset();

public:
    // Parse http headers (including the start line) from data.
    // return: error code which is defined as ErrorCode.
    virtual size_t ParseHeaders(const StringPiece& data, ErrorCode* error = NULL);

    std::string StartLine() const {
        std::string result;
        AppendStartLineToString(&result);
        return result;
    }

    HttpVersion Version() const { return m_version; }
    void SetVersion(const HttpVersion& version) {
        m_version = version;
    }

    void SetVersion(int major, int minor) {
        m_version = HttpVersion(major, minor);
    }
    void ClearVersion() {
        m_version.Clear();
    }

    const std::string& Body() const { return m_body; }
    std::string* MutableBody() { return &m_body; }

    void SetBody(const StringPiece& body) {
        m_body.assign(body.data(), body.size());
    }

    // string of GNU libstdc++ use reference count to reduce copy
    // keep these overloadings to help it
    void SetBody(const std::string& body) {
        m_body = body;
    }
    void SetBody(const char* body) {
        m_body.assign(body);
    }

    int GetContentLength();
    bool IsKeepAlive() const;

    // Get the header value.
    const HttpHeaders& Headers() const {
        return m_headers;
    }

    HttpHeaders& Headers() {
        return m_headers;
    }

    // Return false if it doesn't exist.
    bool GetHeader(const StringPiece& header_name, std::string** value);
    bool GetHeader(const StringPiece& header_name, const std::string** value) const;
    bool GetHeader(const StringPiece& header_name, std::string* value) const;
    std::string GetHeader(const StringPiece& header_name) const;

    // Used when a http header appears multiple times.
    // return false if it doesn't exist.
    bool GetHeaders(const StringPiece& header_name,
                    std::vector<std::string>* header_values) const;
    // Set a header field. if it exists, overwrite the header value.
    void SetHeader(const StringPiece& header_name,
                   const StringPiece& header_value);
    // Replace the header with those in parameters 'headers'
    void SetHeaders(const HttpHeaders& headers);
    // Add a header field, just append, no overwrite.
    void AddHeader(const StringPiece& header_name,
                   const StringPiece& header_value);
    // Insert the items from 'headers'
    void AddHeaders(const HttpHeaders& headers);
    // Remove an http header field.
    bool RemoveHeader(const StringPiece& header_name);

    // If has a header
    bool HasHeader(const StringPiece& header_name) const;

    // Convert start line and headers to string.
    void AppendHeadersToString(std::string* result) const;
    void HeadersToString(std::string* result) const;
    std::string HeadersToString() const;

    void AppendToString(std::string* result) const;
    void ToString(std::string* result) const;
    std::string ToString() const;

protected:
    bool ParseVersion(const std::string& version_str);

    // append without ending "\r\n"
    virtual void AppendStartLineToString(std::string* result) const = 0;
    virtual bool ParseStartLine(const StringPiece& data, HttpMessage::ErrorCode* error) = 0;
    void Swap(HttpMessage* other) {
        using std::swap;
        swap(m_version, other->m_version);
        m_headers.Swap(&other->m_headers);
        swap(m_body, other->m_body);
    }

private:
    HttpVersion m_version;
    HttpHeaders m_headers;
    std::string m_body;
};

} // namespace toft

#endif // TOFT_NET_HTTP_MESSAGE_H
