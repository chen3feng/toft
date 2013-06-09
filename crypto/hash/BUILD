cc_library(
    name = 'md5',
    srcs = 'md5.cpp',
    deps = [
        '//toft/base:int128',
        '//toft/base/string:string'
    ],
)

cc_test(
    name = 'md5_test',
    srcs = 'md5_test.cpp',
    deps = [':md5'],
)
