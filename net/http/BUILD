cc_library(
    name = 'http',
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

cc_test(
    name = 'http_headers_test',
    srcs = 'http_headers_test.cpp',
    deps = ':http'
)

cc_test(
    name = 'http_message_test',
    srcs = 'http_message_test.cpp',
    deps = ':http'
)

cc_test(
    name = 'http_request_test',
    srcs = 'http_request_test.cpp',
    deps = ':http'
)

cc_test(
    name = 'http_response_test',
    srcs = 'http_response_test.cpp',
    deps = ':http'
)

cc_test(
    name = 'http_time_test',
    srcs = 'http_time_test.cpp',
    deps = ':http'
)

cc_test(
    name = 'http_version_test',
    srcs = 'http_version_test.cpp',
    deps = ':http'
)

