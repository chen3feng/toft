cc_library(
    name = 'encoding',
    srcs = [
        'ascii.cpp',
        'percent.cpp',
    ],
    deps = [
        '//toft/base/string:string',
    ]
)

cc_test(
    name = 'ascii_test',
    srcs = 'ascii_test.cpp',
    deps = ':encoding'
)

cc_test(
    name = 'percent_test',
    srcs = 'percent_test.cpp',
    deps = ':encoding'
)

proto_library(
    name = 'unittest_proto',
    srcs = 'unittest.proto',
)

cc_library(
    name = 'json_format',
    srcs = 'json_format.cpp',
    deps = ['//thirdparty/glog:glog',
            '//thirdparty/jsoncpp:jsoncpp',
            '//thirdparty/protobuf:protobuf',
            ],
)

cc_test(
    name = 'json_format_unittest',
    srcs = 'json_format_unittest.cpp',
    deps = [':json_format',
            ':unittest_proto',
            '//toft/storage/file:file',
            ],
    testdata = ['debug_string.txt',
                'json_fast_string.txt',
                'json_styled_string.txt',
                ]
)
