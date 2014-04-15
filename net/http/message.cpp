// Copyright 2011, The Toft Authors.
// Author: Hangjun Ye <yehangjun@gmail.com>
// Xiaokang Liu <liuxk02@gmail.com>

#include "toft/net/http/message.h"

#include "toft/base/string/algorithm.h"
#include "toft/base/string/concat.h"
#include "toft/base/string/format.h"
#include "toft/base/string/number.h"

namespace toft {

bool HttpMessage::ParseVersion(const std::string& version_str)
{
    if (!StringStartsWith(version_str, "HTTP/"))
        return false;
    const std::string& ver = version_str.substr(5);
    int major, minor;
    if (StringScan(ver, "%d.%d", &major, &minor) != 2)
        return false;
    SetVersion(HttpVersion(major, minor));
    return true;
}

void HttpMessage::Reset() {
    m_version.Clear();
    m_headers.Clear();
    m_body.clear();
}

void HttpMessage::AppendHeadersToString(std::string* result) const {
    AppendStartLineToString(result);
    result->append("\r\n");
    m_headers.AppendToString(result);
    result->append("\r\n");
}

void HttpMessage::HeadersToString(std::string* result) const {
    result->clear();
    AppendHeadersToString(result);
}

std::string HttpMessage::HeadersToString() const {
    std::string result;
    AppendHeadersToString(&result);
    return result;
}

void HttpMessage::AppendToString(std::string* result) const
{
    AppendHeadersToString(result);
    result->append(m_body);
}

void HttpMessage::ToString(std::string* result) const
{
    result->clear();
    AppendToString(result);
}

std::string HttpMessage::ToString() const
{
    std::string result;
    AppendToString(&result);
    return result;
}

// Get a header value. return false if it does not exist.
// the header name is not case sensitive.
bool HttpMessage::GetHeader(const StringPiece& header_name,
                            std::string** header_value) {
    return m_headers.Get(header_name, header_value);
}

bool HttpMessage::GetHeader(const StringPiece& header_name,
                            const std::string** header_value) const {
    return m_headers.Get(header_name, header_value);
}

bool HttpMessage::GetHeader(
        const StringPiece& header_name,
        std::string* value) const {
    const std::string* pvalue;
    if (GetHeader(header_name, &pvalue)) {
        *value = *pvalue;
        return true;
    }
    return false;
}

std::string HttpMessage::GetHeader(const StringPiece& header_name) const {
    std::string header_value;
    GetHeader(header_name, &header_value);
    return header_value;
}

// Used when a http header appears multiple times.
// return false if it doesn't exist.
bool HttpMessage::GetHeaders(const StringPiece& header_name,
                             std::vector<std::string>* header_values) const {
    return m_headers.Get(header_name, header_values);
}

// Set a header field. if it exists, overwrite the header value.
void HttpMessage::SetHeader(const StringPiece& header_name,
                            const StringPiece& header_value) {
    m_headers.Set(header_name, header_value);
}

void HttpMessage::SetHeaders(const HttpHeaders& headers) {
    m_headers = headers;
}

// Add a header field, just append, no overwrite.
void HttpMessage::AddHeader(const StringPiece& header_name,
                            const StringPiece& header_value) {
    m_headers.Add(header_name, header_value);
}

void HttpMessage::AddHeaders(const HttpHeaders& headers) {
    m_headers.Add(headers);
}

bool HttpMessage::RemoveHeader(const StringPiece& header_name) {
    return m_headers.Remove(header_name);
}

bool HttpMessage::HasHeader(const StringPiece& header_name) const {
    return m_headers.Has(header_name);
}

size_t HttpMessage::ParseHeaders(const StringPiece& data, HttpMessage::ErrorCode* error) {
    HttpMessage::ErrorCode error_placeholder;
    if (error == NULL)
        error = &error_placeholder;

    StringPiece::size_type pos = data.find_first_of('\n');
    if (pos == StringPiece::npos) {
        pos = data.size();
    }
    std::string first_line =
        StringTrimRight(data.substr(0, pos), "\r");

    if (first_line.empty()) {
        *error = HttpMessage::ERROR_NO_START_LINE;
        return 0;
    }

    if (!ParseStartLine(first_line, error))
        return 0;

    int error_code = 0;
    size_t result = m_headers.Parse(data.substr(pos + 1), &error_code);
    *error = static_cast<HttpMessage::ErrorCode>(error_code);
    return pos + 1 + result;
}

int HttpMessage::GetContentLength() {
    std::string content_length;
    if (!GetHeader("Content-Length", &content_length)) {
        return -1;
    }
    int length = 0;
    bool ret = StringToNumber(content_length, &length);
    return (ret && length >= 0) ? length : -1;
};

bool HttpMessage::IsKeepAlive() const {
    const std::string* alive;
    if (!GetHeader("Connection", &alive)) {
        if (m_version < HttpVersion(1, 1)) {
            return false;
        }
        return true;
    }
    return strcasecmp(alive->c_str(), "keep-alive") == 0;
}

} // namespace toft
