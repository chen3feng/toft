cc_library(
    name = 'encoding',
    srcs = [
        'ascii.cpp',
        'base64.cpp',
        'hex.cpp',
        'percent.cpp',
        'shell.cpp',
    ],
    deps = [
        '//toft/base/string:string',
        '//thirdparty/stringencoders:stringencoders'
    ]
)

cc_library(
    name = 'varint',
    srcs = 'varint.cpp',
    deps = [],
)

cc_test(
    name = 'varint_test',
    srcs = 'varint_test.cpp',
    deps = [':varint'],
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
    name = 'proto_json_format_test_proto',
    srcs = 'proto_json_format_test.proto',
)

cc_library(
    name = 'proto_json_format',
    srcs = 'proto_json_format.cpp',
    deps = [
        '//toft/base/string:string',
        '//thirdparty/glog:glog',
        '//thirdparty/jsoncpp:jsoncpp',
        '//thirdparty/protobuf:protobuf',
    ],
)

cc_test(
    name = 'proto_json_format_test',
    srcs = 'proto_json_format_test.cpp',
    deps = [
        ':proto_json_format',
        ':proto_json_format_test_proto',
        '//toft/storage/file:file',
    ],
    testdata = [
        'debug_string.txt',
        'json_fast_string.txt',
        'json_styled_string.txt',
    ],
)

cc_test(
    name = 'shell_test',
    srcs = 'shell_test.cpp',
    deps = ':encoding'
)

cc_test(
    name = 'hex_test',
    srcs = 'hex_test.cpp',
    deps = ':encoding',
)

cc_test(
    name = 'base64_test',
    srcs = 'base64_test.cpp',
    deps = ':encoding'
)


