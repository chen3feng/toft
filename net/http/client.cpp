// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: DongPing HUANG <hdping99@gmail.com>
// Created: 11/05/11

#include "toft/net/http/client.h"

#include <algorithm>
#include <utility>

#include "toft/base/string/algorithm.h"
#include "toft/base/string/number.h"
#include "toft/base/unique_ptr.h"
#include "toft/net/http/message.h"
#include "toft/net/mime/mime.h"
#include "toft/net/uri/uri.h"
#include "toft/system/net/domain_resolver.h"

#include "thirdparty/glog/logging.h"

namespace toft {

namespace {

const char kHttpScheme[] = "http";
const char kDefaultPath[] = "/";
const char kDefaultHttpPort[] = "80";
size_t kDefaultMaxResponseLength = 1024 * 1024 * 2;

// Resolve domain address, output is a vector of SocketAddressInet4
// domain example:
//  www.qq.com
//  192.168.1.1
bool ResolveAddress(const std::string& host,
                    uint16_t port,
                    std::vector<SocketAddressInet4> *sa,
                    HttpClient::ErrorCode *error)
{
    std::vector<IpAddress> ipaddr;
    int error_code;
    if (!DomainResolver::ResolveIpAddress(
            host,
            &ipaddr,
            &error_code)) {
        *error = HttpClient::ERROR_FAIL_TO_RESOLVE_ADDRESS;
        return false;
    }

    std::vector<SocketAddressInet4> sock_addr;
    for (std::vector<IpAddress>::const_iterator it = ipaddr.begin();
        it != ipaddr.end();
        ++it) {
        sock_addr.push_back(SocketAddressInet4(*it, port));
    }
    sa->swap(sock_addr);

    return true;
}

// according to RFC2616, HTTP STATUS 1xx, 204, and 304 doesn't have a HTTP
// body.
bool ResponseStatusHasContent(int http_status)
{
    return http_status >= 200 && http_status != 204 && http_status != 304;
}

void AppendHeaderToRequest(const std::string& path,
                           const HttpHeaders& headers,
                           HttpRequest* request)
{
    size_t count = headers.Count();
    for (size_t k = 0; k < count; ++k) {
        std::pair<std::string, std::string> header;
        headers.GetAt(k, &header);
        request->AddHeader(header.first, header.second);
    }
}

class DownloadTask {
public:
    explicit DownloadTask(HttpClient *http_client)
        : m_connector(AF_INET, IPPROTO_TCP),
          m_error_code(HttpClient::SUCCESS),
          m_max_response_length(0)
    {
        m_connector.SetLinger(true, 1);
        m_http_client = http_client;
    }

    ~DownloadTask()
    {
    }

    bool ProcessRequest(const std::string& url,
                        const HttpClient::Options& options,
                        HttpRequest* request,
                        HttpResponse *response)
    {
        URI *uri = NULL;
        if (!m_uri.Parse(url)) {
            m_error_code = HttpClient::ERROR_INVALID_URI_ADDRESS;
            return false;
        }
        uri = &m_uri;

        // Apply HTTP HEADERS into request
        std::string path = uri->Path();
        AppendHeaderToRequest(path, options.Headers(), request);

        m_max_response_length = options.MaxResponseLength() ?
            options.MaxResponseLength() :
            kDefaultMaxResponseLength;

        std::string path_and_query = uri->PathAndQuery();
        std::string host = uri->Host();

        request->SetHeader("User-Agent", m_http_client->UserAgent());
        request->SetHeader("Host", host);

        if (!m_http_client->Proxy().empty()) {
            if (!m_proxy_uri.Parse(m_http_client->Proxy())) {
                m_error_code = HttpClient::ERROR_INVALID_PROXY_ADDRESS;
                return false;
            }
            uri = &m_proxy_uri;
            path_and_query = url;
        }

        request->SetUri(path_and_query.empty() ? kDefaultPath : path_and_query);

        std::string port_str;
        if (uri->HasPort()) {
            port_str = uri->Port();
        } else if (uri->Scheme().empty() || uri->Scheme() == kHttpScheme) {
            port_str = kDefaultHttpPort;
        } else {
            m_error_code = HttpClient::ERROR_PROTOCAL_NOT_SUPPORTED;
            return false;
        }

        uint32_t port;
        if (!StringToNumber(port_str, &port))
            return false;

        std::vector<SocketAddressInet4> sa;
        if (!ResolveAddress(host, port, &sa, &m_error_code)) {
            return false;
        }

        for (std::vector<SocketAddressInet4>::const_iterator it = sa.begin();
            it != sa.end();
            ++it) {
            if (ProcessRequest(*it, *request, response)) {
                return true;
            }
        }

        return false;
    }

    HttpClient::ErrorCode GetLastError() const
    {
        return m_error_code;
    }

private:
    bool ProcessRequest(const SocketAddressInet4& addr,
                        const HttpRequest& request,
                        HttpResponse* response)
    {
        if (!m_connector.Connect(addr)) {
            m_error_code = HttpClient::ERROR_FAIL_TO_CONNECT_SERVER;
            return false;
        }

        return SendRequest(request) && ReceiveResponse(response);
    }

    bool SendRequest(const HttpRequest& request)
    {
        std::string headers = request.HeadersToString();
        headers.append(request.Body());
        VLOG(5) << headers << std::endl;

        if (!m_connector.SendAll(headers.c_str(), headers.length())) {
            m_error_code = HttpClient::ERROR_FAIL_TO_SEND_REQUEST;
            return false;
        }
        return true;
    }

    bool ReceiveResponse(HttpResponse* response)
    {
        std::unique_ptr<char[]> buffer(new char[m_max_response_length]);
        char *buff = buffer.get();

        size_t total_received = 0;
        size_t received_length;
        size_t buffer_length = m_max_response_length - 1;
        char *p = NULL;
        // handle headers first.
        do {
            if (!m_connector.Receive(buff + total_received,
                                     buffer_length,
                                     &received_length)) {
                m_error_code = HttpClient::ERROR_FAIL_TO_GET_RESPONSE;
                return false;
            } else if (received_length == 0) {
                m_error_code = HttpClient::ERROR_FAIL_TO_GET_RESPONSE;
                VLOG(4) << "The peer reset the network connection.";
                return false;
            }
            total_received += received_length;
            buffer_length -= received_length;

            buff[total_received] = 0;

            p = strstr(buff, "\r\n\r\n");
        } while (p == NULL && buffer_length > 0);

        if (p == NULL) {
            m_error_code = HttpClient::ERROR_INVALID_RESPONSE_HEADER;
            return false;
        }
        p += 4;

        StringPiece piece(buff, p - buff);
        HttpMessage::ErrorCode message_error;
        if (!m_response.ParseHeaders(piece, &message_error)) {
            m_error_code = HttpClient::ERROR_INVALID_RESPONSE_HEADER;
            return false;
        }

        char* current = buff + total_received;
        char* end = current + buffer_length; // end of buffer for body
        if (!ResponseStatusHasContent(m_response.Status())) {
            // no content
        } else if (m_response.HasHeader("Transfer-Encoding")
               && m_response.GetHeader("Transfer-Encoding") != "identity") {
            // chunked content
            ReceiveBodyWithChunks(p, end, current);
        } else if (m_response.HasHeader("Content-Length")) {
            // Content-Length field is given
            ReceiveBodyWithContentLength(p, end, current);
        } else if (m_response.HasHeader("Content-Type") &&
                   m_response.GetHeader("Content-Type") == "multipart/byteranges") {
            // not supported yet
            m_error_code = HttpClient::ERROR_CONTENT_TYPE_NOT_SUPPORTED;
            return false;
        } else {
            // for the case the HTTP server close the connection
            ReceiveBodyWithConnectionReset(p, end, current);
        }

        std::swap(m_response, *response);

        if (response->Status() != HttpResponse::Status_OK)
            m_error_code = HttpClient::ERROR_HTTP_STATUS_CODE;

        return true;
    }

    // For response with a HEADER Content-Length
    void ReceiveBodyWithContentLength(char *begin, char *end, char* current)
    {
        std::string body;
        body.reserve(end - begin + 1);
        body.append(begin, current);

        size_t content_length = m_response.GetContentLength();
        // if received buffer is not enough
        if (content_length > body.length()) {
            size_t buf_len = end - current;
            size_t download = std::min(buf_len, content_length - body.length());

            size_t received = 0;
            if (!m_connector.ReceiveAll(current,
                                        download,
                                        &received)) {
                m_error_code = HttpClient::ERROR_FAIL_TO_GET_RESPONSE;
            }
            body.append(current, received);
        }
        m_response.MutableBody()->swap(body);
    }

    // For content data in chunks
    void ReceiveBodyWithChunks(char *begin, char *end, char* current)
    {
        std::string body;
        body.reserve(m_max_response_length);

        int buffer_length = end - current;
        while (begin < end) {
            size_t received = 0;

            *current = 0; // For the following strstr.
            char *p = strstr(begin, "\r\n");
            if (p != NULL) {
                int chunk_size = 0;
                if (sscanf(begin, "%x", &chunk_size) != 1) { // NOLINT(runtime/printf)
                    m_error_code = HttpClient::ERROR_FAIL_TO_READ_CHUNKSIZE;
                    return;
                }
                begin = p + 2;
                if (chunk_size == 0) {
                    // finish
                    m_response.MutableBody()->swap(body);
                    return;
                }

                chunk_size += 2; // "\r\n" is appended to the end of chunk
                int downloaded = current - begin;
                chunk_size = std::min(chunk_size, buffer_length);
                // if the downloaded content is not enough, download more.
                if (downloaded < chunk_size) {
                    size_t length = chunk_size - downloaded;
                    if (!m_connector.ReceiveAll(current, length, &received)) {
                        m_error_code = HttpClient::ERROR_FAIL_TO_GET_RESPONSE;
                        return;
                    }
                    current += received;
                    buffer_length -= received;
                }
                // remove this "\r\n"
                body.append(begin, chunk_size - 2);
                begin += chunk_size;
            } else {
                // there is not enough content to get a whole CHUNK header
                // download more data.
                if (!m_connector.Receive(current, buffer_length, &received)) {
                    m_error_code = HttpClient::ERROR_FAIL_TO_GET_RESPONSE;
                    return;
                }
                current += received;
                buffer_length -= received;
            }
        }
    }

    // Old HTTP servers will close connection after send response package
    void ReceiveBodyWithConnectionReset(char *begin, char *end, char* current)
    {
        size_t received;
        if (m_connector.ReceiveAll(current, end - current, &received) ||
           Socket::GetLastError() == ECONNRESET) {
            current += received;
            std::string body(begin, current - begin);
            m_response.MutableBody()->swap(body);
        }
    }

private:
    HttpClient *m_http_client;
    URI m_uri;
    URI m_proxy_uri;
    StreamSocket m_connector;
    HttpResponse m_response;
    HttpClient::ErrorCode m_error_code;
    size_t m_max_response_length;
};

} // namespace

HttpClient::Options& HttpClient::Options::AddHeader(const std::string& name,
                                                    const std::string& value)
{
    m_headers.Add(name, value);
    return *this;
}

const HttpHeaders& HttpClient::Options::Headers() const
{
    return m_headers;
}

HttpClient::Options& HttpClient::Options::SetMaxResponseLength(size_t length)
{
    m_max_response_length = length;
    return *this;
}

size_t HttpClient::Options::MaxResponseLength() const
{
    return m_max_response_length;
}

HttpClient::HttpClient()
{
    m_user_agent = "SosoDownloader/1.0(compatible; MSIE 7.0; Windows NT 5.1)";
}

HttpClient::~HttpClient()
{
}

const char* HttpClient::GetErrorMessage(ErrorCode error_code)
{
    switch (error_code) {
    case SUCCESS:
        return "Success";
    case ERROR_INVALID_URI_ADDRESS:
        return "Invalid URI address";
    case ERROR_INVALID_PROXY_ADDRESS:
        return "Invalid proxy address";
    case ERROR_INVALID_RESPONSE_HEADER:
        return "Invalid response header";
    case ERROR_FAIL_TO_RESOLVE_ADDRESS:
        return "Failed to resolve address";
    case ERROR_FAIL_TO_SEND_REQUEST:
        return "Failed to send request";
    case ERROR_FAIL_TO_GET_RESPONSE:
        return "Failed to get response";
    case ERROR_FAIL_TO_CONNECT_SERVER:
        return "Failed to connect to server";
    case ERROR_FAIL_TO_READ_CHUNKSIZE:
        return "Failed to read chunk size";
    case ERROR_PROTOCAL_NOT_SUPPORTED:
        return "Protocal is not supported";
    case ERROR_CONTENT_TYPE_NOT_SUPPORTED:
        return "Content type is not supported yet";
    case ERROR_HTTP_STATUS_CODE:
        return "Error http status code";
    case ERROR_TOO_MANY_REDIRECTS:
        return "Too many redirections";
    // DO NOT ADD default: here, or not handled error_code will be ignored.
    }

    return NULL;
}

HttpClient& HttpClient::SetProxy(const std::string& proxy)
{
    m_proxy = proxy;
    return *this;
}

const std::string& HttpClient::Proxy() const
{
    return m_proxy;
}

HttpClient& HttpClient::SetUserAgent(const std::string& user_agent)
{
    m_user_agent = user_agent;
    return *this;
}

const std::string& HttpClient::UserAgent() const
{
    return m_user_agent;
}

bool HttpClient::Request(HttpRequest::MethodType method,
                         const std::string& url,
                         const std::string& data,
                         const HttpClient::Options& options,
                         HttpResponse *response,
                         ErrorCode *error)
{
    ErrorCode error_placeholder;
    if (error == NULL) {
        error = &error_placeholder;
    }

    HttpRequest request;
    request.SetMethod(method);
    request.SetBody(data);
    request.SetHeader("Content-Length", IntegerToString(data.size()));

    DownloadTask task(this);
    bool ret = task.ProcessRequest(url, options, &request, response);

    *error = task.GetLastError();
    return ret && *error == SUCCESS;
}

bool HttpClient::Get(const std::string& url,
                     const Options& options,
                     HttpResponse *response,
                     ErrorCode *error)
{
    return Request(HttpRequest::METHOD_GET, url, "", options, response, error);
}

bool HttpClient::Get(const std::string& url,
                     HttpResponse* response,
                     ErrorCode *error)
{
    return Get(url, Options(), response, error);
}

bool HttpClient::Post(const std::string& url,
                      const std::string& data,
                      const Options& options,
                      HttpResponse *response,
                      ErrorCode *error)
{
    return Request(HttpRequest::METHOD_POST, url, data, options, response, error);
}

bool HttpClient::Post(const std::string& url,
                      const std::string& data,
                      HttpResponse* response,
                      ErrorCode *error)
{
    return Post(url, data, Options(), response, error);
}

bool HttpClient::Put(const std::string& url,
                     const std::string& data,
                     const Options& options,
                     HttpResponse* response,
                     ErrorCode* error)
{
    return Request(HttpRequest::METHOD_PUT, url, data, options, response, error);
}

bool HttpClient::Put(const std::string& url,
                     const std::string& data,
                     HttpResponse* response,
                     ErrorCode* error)
{
    return Put(url, data, Options(), response, error);
}

bool HttpClient::Delete(const std::string& url,
                        const Options& options,
                        HttpResponse* response,
                        ErrorCode* error)
{
    return Request(HttpRequest::METHOD_DELETE, url, "", options, response, error);
}

bool HttpClient::Delete(const std::string& url,
                        HttpResponse* response,
                        ErrorCode* error)
{
    return Delete(url, Options(), response, error);
}

} // namespace toft

