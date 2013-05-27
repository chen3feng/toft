cc_library(
    name = 'file',
    srcs = [
        'file.cpp',
        'local_file.cpp',
    ],
    deps = [
        '//toft/base/string:string',
        '//toft/base:class_registry',
        '//toft/storage/path:path',
        '//toft/text:wildcard',
    ],
    link_all_symbols=True
)

cc_test(
    name = 'file_test',
    srcs = 'file_test.cpp',
    deps = ':file',
    testdata = 'testdata'
)

cc_library(
    name = 'mock_file',
    srcs = [
        'mock_file.cpp',
    ],
    deps = [
        ':file',
        '//thirdparty/gmock:gmock',
    ],
    link_all_symbols=True
)

cc_test(
    name = 'mock_file_test',
    srcs = 'mock_file_test.cpp',
    deps = ':mock_file'
)
