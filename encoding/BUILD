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

