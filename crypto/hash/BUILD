cc_library(
    name = 'md5',
    srcs = 'md5.cpp',
    deps = [
        '//toft/base:int128',
        '//toft/base/string:string',
        '//toft/encoding:encoding',
    ],
)

cc_test(
    name = 'md5_test',
    srcs = 'md5_test.cpp',
    deps = [':md5'],
)

cc_library(
    name = 'sha1',
    srcs = 'sha1.cpp',
    deps = [
        '//toft/encoding:encoding',
    ],
)

cc_test(
    name = 'sha1_test',
    srcs = 'sha1_test.cpp',
    deps = [
        ':sha1',
    ],
)
