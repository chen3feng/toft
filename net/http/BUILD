cc_library(
    name = 'types',
    srcs = [
        'headers.cpp',
        'message.cpp',
        'request.cpp',
        'response.cpp',
        'time.cpp',
    ],
    deps = [
        '//toft/base/string:string',
        '//toft/net/uri:url',
        '//thirdparty/glog:glog'
    ]
)

cc_library(
    name = 'client',
    srcs = 'client.cpp',
    deps = [
        ':types',
        '//toft/net/uri:url',
        '//toft/system/net:net',
        '//thirdparty/glog:glog'
    ]
)

cc_test(
    name = 'client_test',
    srcs = 'client_test.cpp',
    deps = ':client'
)

cc_test(
    name = 'headers_test',
    srcs = 'headers_test.cpp',
    deps = ':types'
)

cc_test(
    name = 'message_test',
    srcs = 'message_test.cpp',
    deps = ':types'
)

cc_test(
    name = 'request_test',
    srcs = 'request_test.cpp',
    deps = ':types'
)

cc_test(
    name = 'response_test',
    srcs = 'response_test.cpp',
    deps = ':types'
)

cc_test(
    name = 'time_test',
    srcs = 'time_test.cpp',
    deps = ':types'
)

cc_test(
    name = 'version_test',
    srcs = 'version_test.cpp',
    deps = ':types'
)

