// Copyright (c) 2012, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 01/12/12
// Description: HttpHeaders class

#include "toft/net/http/headers.h"

#include <utility>
#include "toft/base/string/algorithm.h"
#include "toft/base/string/concat.h"
#include "toft/net/http/message.h"

#include "thirdparty/glog/logging.h"

namespace toft {

void HttpHeaders::AppendToString(std::string* result) const {
    size_t header_number = m_headers.size();
    for (size_t i = 0; i < header_number; ++i) {
        StringAppend(result, m_headers[i].first, ": ", m_headers[i].second, "\r\n");
    }
}

void HttpHeaders::ToString(std::string* result) const {
    result->clear();
    AppendToString(result);
}

std::string HttpHeaders::ToString() const {
    std::string result;
    AppendToString(&result);
    return result;
}

// Get a header value. return false if it does not exist.
// the header name is not case sensitive.
bool HttpHeaders::Get(const StringPiece& header_name,
                      std::string** header_value) {
    std::vector<std::pair<std::string, std::string> >::iterator iter;
    for (iter = m_headers.begin(); iter != m_headers.end(); ++iter) {
        if (header_name.ignore_case_equal(iter->first)) {
            *header_value = &iter->second;
            return true;
        }
    }
    return false;
}

bool HttpHeaders::Get(const StringPiece& header_name,
                      const std::string** header_value) const {
    return const_cast<HttpHeaders*>(this)->Get(
        header_name,
        const_cast<std::string**>(header_value));
}

bool HttpHeaders::Get(
        const StringPiece& header_name,
        std::string* value) const {
    const std::string* pvalue;
    if (Get(header_name, &pvalue)) {
        *value = *pvalue;
        return true;
    }
    return false;
}

// Used when a http header appears multiple times.
// return false if it doesn't exist.
bool HttpHeaders::Get(const StringPiece& header_name,
                      std::vector<std::string>* header_values) const {
    header_values->clear();
    std::vector<std::pair<std::string, std::string> >::const_iterator iter;
    for (iter = m_headers.begin(); iter != m_headers.end(); ++iter) {
        if (header_name.ignore_case_equal(iter->first)) {
            header_values->push_back(iter->second);
        }
    }
    return header_values->size() > 0;
}

// Set a header field. if it exists, overwrite the header value.
HttpHeaders& HttpHeaders::Set(const StringPiece& header_name,
                              const StringPiece& header_value) {
    // NOTE: their may be multiple headers share the same name,
    // remove all headers firstly
    Remove(header_name);
    Add(header_name, header_value);
    return *this;
}

// Add a header field, just append, no overwrite.
HttpHeaders& HttpHeaders::Add(const StringPiece& header_name,
                              const StringPiece& header_value) {
    m_headers.push_back(make_pair(header_name.as_string(), header_value.as_string()));
    return *this;
}

HttpHeaders& HttpHeaders::Add(const HttpHeaders& rhs) {
    m_headers.insert(m_headers.end(), rhs.m_headers.begin(), rhs.m_headers.end());
    return *this;
}

bool HttpHeaders::Remove(const StringPiece& header_name) {
    bool result = false;
    std::vector<std::pair<std::string, std::string> >::iterator iter;
    for (iter = m_headers.begin(); iter != m_headers.end();) {
        if (header_name.ignore_case_equal(iter->first)) {
            iter = m_headers.erase(iter);
            result = true;
        } else {
            ++iter;
        }
    }
    return result;
}

bool HttpHeaders::Has(const StringPiece& header_name) const {
    std::vector<std::pair<std::string, std::string> >::const_iterator iter;
    for (iter = m_headers.begin(); iter != m_headers.end(); ++iter) {
        if (header_name.ignore_case_equal(iter->first)) {
            return true;
        }
    }
    return false;
}

size_t HttpHeaders::Count() const {
    return m_headers.size();
}

bool HttpHeaders::GetAt(int index, std::pair<std::string, std::string>* header) const {
    if (index < 0 || index >= static_cast<int>(m_headers.size()))
        return false;
    *header = m_headers[index];
    return true;
}

size_t HttpHeaders::Parse(const StringPiece& data, int* error)
{
    int error_placeholder;
    if (error == NULL)
        error = &error_placeholder;

    // Starts with empty line means empty headers.
    if (StringStartsWith(data, "\n") || StringStartsWith(data, "\r\n")) {
        m_headers.clear();
        return (data[0] == '\r') + 1; // sizeof \n or \r\n
    }

    size_t end_pos;
    size_t tail_size;
    if ((end_pos = data.find("\r\n\r\n")) != std::string::npos) {
        tail_size = 4;
    } else if ((end_pos = data.find("\n\n")) != std::string::npos) {
        tail_size = 2;
    } else {
        *error = HttpMessage::ERROR_MESSAGE_NOT_COMPLETE;
        return 0;
    }

    std::vector<StringPiece> lines;
    SplitLines(data.substr(0, end_pos + tail_size), &lines);

    if (lines.empty()) {
        *error = HttpMessage::ERROR_MESSAGE_NOT_COMPLETE;
        return 0;
    }

    m_headers.clear();

    // Skip the head line and the last line(empty but '\n')
    for (int i = 0; i < static_cast<int>(lines.size() - 1); ++i) {
        StringPiece line = lines[i];
        size_t pos = line.find(':');
        if (pos != StringPiece::npos) {
            StringPiece name = line.substr(0, pos);
            StringPiece value = line.substr(pos + 1);
            StringTrim(&name);
            StringTrim(&value);
            // Push an empty element and modify it to avoid copy.
            m_headers.push_back(std::pair<std::string, std::string>());
            std::pair<std::string, std::string> &header = m_headers.back();
            name.copy_to_string(&header.first);
            value.copy_to_string(&header.second);
        } else {
            if (!lines[i].empty()) {
                VLOG(3) << "Invalid http header" << lines[i] << ", ignore";
            } else {
                *error = HttpMessage::ERROR_FIELD_NOT_COMPLETE;
                m_headers.clear();
                return 0;
            }
        }
    }

    *error = HttpMessage::SUCCESS;
    return end_pos + tail_size;
}

void HttpHeaders::Clear() {
    m_headers.clear();
}

void HttpHeaders::Swap(HttpHeaders* rhs) {
    m_headers.swap(rhs->m_headers);
}

} // namespace toft
