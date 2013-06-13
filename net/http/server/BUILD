cc_library(
    name = 'server',
    srcs = [
        'connection.cpp',
        'handler.cpp',
        'server.cpp',
    ],
    deps = [
        '//toft/net/http:types',
        '//toft/system/event_dispatcher:event_dispatcher',
        '//toft/system/net:net',
        '//thirdparty/gflags:gflags',
        '//thirdparty/glog:glog',
    ],
)

cc_binary(
    name = 'example_server',
    srcs = 'example.cpp',
    deps = ':server'
)

cc_test(
    name = 'server_test',
    srcs = 'server_test.cpp',
    deps = ':server'
)


