cc_library(
    name = '_http_message',
    srcs = [
        'http_headers.cpp',
        'http_message.cpp',
        'http_request.cpp',
        'http_response.cpp',
        'http_time.cpp',
    ],
    deps = [
        '//toft/base/string:string',
        '//toft/net/uri:url',
        '//thirdparty/glog:glog'
    ]
)

cc_library(
    name = '_http_client',
    srcs = 'http_client.cpp',
    deps = [
        ':_http_message',
        '//toft/net/uri:url',
        '//toft/system/net:net',
        '//thirdparty/glog:glog'
    ]
)

cc_library(
    name = 'http',
    deps = [
        ':_http_client',
        ':_http_message',
    ]
)

cc_test(
    name = 'http_client_test',
    srcs = 'http_client_test.cpp',
    deps = ':_http_client'
)

cc_test(
    name = 'http_headers_test',
    srcs = 'http_headers_test.cpp',
    deps = ':_http_message'
)

cc_test(
    name = 'http_message_test',
    srcs = 'http_message_test.cpp',
    deps = ':_http_message'
)

cc_test(
    name = 'http_request_test',
    srcs = 'http_request_test.cpp',
    deps = ':_http_message'
)

cc_test(
    name = 'http_response_test',
    srcs = 'http_response_test.cpp',
    deps = ':_http_message'
)

cc_test(
    name = 'http_time_test',
    srcs = 'http_time_test.cpp',
    deps = ':_http_message'
)

cc_test(
    name = 'http_version_test',
    srcs = 'http_version_test.cpp',
    deps = ':_http_message'
)

