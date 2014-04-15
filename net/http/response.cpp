// Copyright (c) 2012, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 03/05/12
// Description: HttpResponse class implementation

#include "toft/net/http/response.h"

#include "toft/base/string/algorithm.h"
#include "toft/base/string/concat.h"
#include "toft/base/string/number.h"

namespace toft {

static const struct {
    int status_code;
    const char* reason_phrase;
    const char* status_description;
} kResponseStatus[] = {
    { 100, "Continue", "Request received, please continue" },
    { 101, "Switching Protocols", "Switching to new protocol; obey Upgrade header" },
    { 200, "OK", "Request fulfilled, document follows" },
    { 201, "Created", "Document created, URL follows" },
    { 202, "Accepted", "Request accepted, processing continues off-line" },
    { 203, "Non-Authoritative Information", "Request fulfilled from cache" },
    { 204, "No Content", "Request fulfilled, nothing follows" },
    { 205, "Reset Content", "Clear input form for further input." },
    { 206, "Partial Content", "Partial content follows." },
    { 300, "Multiple Choices", "Object has several resources -- see URI list" },
    { 301, "Moved Permanently", "Object moved permanently -- see URI list" },
    { 302, "Found", "Object moved temporarily -- see URI list" },
    { 303, "See Other", "Object moved -- see Method and URL list" },
    { 304, "Not Modified", "Document has not changed since given time" },
    { 305, "Use Proxy", "You must use proxy specified in Location to access this resource." },
    { 307, "Temporary Redirect", "Object moved temporarily -- see URI list" },
    { 400, "Bad Request", "Bad request syntax or unsupported method" },
    { 401, "Unauthorized", "No permission -- see authorization schemes" },
    { 402, "Payment Required", "No payment -- see charging schemes" },
    { 403, "Forbidden", "Request forbidden -- authorization will not help" },
    { 404, "Not Found", "Nothing matches the given URI" },
    { 405, "Method Not Allowed", "Specified method is invalid for this resource." },
    { 406, "Not Acceptable", "URI not available in preferred format." },
    { 407, "Proxy Authentication Required", "You must authenticate with this proxy before proceeding." },  // NOLINT
    { 408, "Request Timeout", "Request timed out; try again later." },
    { 409, "Conflict", "Request conflict." },
    { 410, "Gone", "URI no longer exists and has been permanently removed." },
    { 411, "Length Required", "Client must specify Content-Length." },
    { 412, "Precondition Failed", "Precondition in headers is false." },
    { 413, "Request Entity Too Large", "Entity is too large." },
    { 414, "Request-URI Too Long", "URI is too long." },
    { 415, "Unsupported Media Type", "Entity body in unsupported format." },
    { 416, "Requested Range Not Satisfiable", "Cannot satisfy request range." },
    { 417, "Expectation Failed", "Expect condition could not be satisfied." },
    { 500, "Internal Server Error", "Server got itself in trouble" },
    { 501, "Not Implemented", "Server does not support this operation" },
    { 502, "Bad Gateway", "Invalid responses from another server/proxy." },
    { 503, "Service Unavailable", "The server cannot process the request due to a high load" },
    { 504, "Gateway Timeout", "The gateway server did not receive a timely response" },
    { 505, "HTTP Version Not Supported", "Cannot fulfill request." },
    { -1, NULL, NULL },
};

// static
const char* HttpResponse::InternalStatusCodeToReasonPhrase(
    StatusCode status_code,
    const char* no_match)
{
    for (int i = 0; ; ++i) {
        if (kResponseStatus[i].status_code == -1) {
            return no_match;
        }
        if (kResponseStatus[i].status_code == status_code) {
            return kResponseStatus[i].reason_phrase;
        }
    }
}

const char* HttpResponse::StatusCodeToDescription(StatusCode status_code)
{
    for (int i = 0; ; ++i) {
        if (kResponseStatus[i].status_code == -1) {
            return "Unknown";
        }
        if (kResponseStatus[i].status_code == status_code) {
            return kResponseStatus[i].status_description;
        }
    }
}

bool HttpResponse::ParseStartLine(const StringPiece& data, HttpMessage::ErrorCode* error) {
    ErrorCode error_placeholder;
    if (error == NULL)
        error = &error_placeholder;

    std::vector<std::string> fields;
    SplitString(data, " ", &fields);
    if (fields.size() < 2) {
        *error = ERROR_START_LINE_NOT_COMPLETE;
        return false;
    }

    if (!ParseVersion(fields[0])) {
        *error = ERROR_VERSION_UNSUPPORTED;
        return false;
    }

    int status;
    if (!StringToNumber(fields[1], &status, 10)) {
        *error = ERROR_RESPONSE_STATUS_NOT_FOUND;
        return false;
    }
    if (status < 100 || status > 999) {
        *error = ERROR_RESPONSE_STATUS_NOT_FOUND;
        return false;
    }

    m_status = static_cast<StatusCode>(status);
    return true;
}

void HttpResponse::AppendStartLineToString(std::string* result) const {
    StringAppend(result, "HTTP/", Version().Major(), ".", Version().Major(),
                 " ", m_status, " ", StatusCodeToReasonPhraseSafe(m_status));
}

void HttpResponse::FillWithHtmlPage(StatusCode code, const StringPiece& title,
                                    const StringPiece& body)
{
    SetStatus(code);
    SetHeader("Content-Type", "text/html");
    std::string* http_body = MutableBody();
    http_body->assign("<html>\n<head>\n</title>");
    if (!title.empty())
        title.append_to_string(http_body);
    else
        StringAppend(http_body, "HTTP ", code, " ", StatusCodeToReasonPhraseSafe(code));
    http_body->append("</title>\n</head>\n");
    http_body->append("<body>");
    if (!body.empty())
        body.append_to_string(http_body);
    else
        http_body->append(StatusCodeToDescription(code));
    http_body->append("</body>\n</html>\n");
}

void HttpResponse::Reset() {
    HttpMessage::Reset();
    m_status = Status_None;
}

} // namespace toft

