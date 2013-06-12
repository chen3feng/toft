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
)

cc_test(
    name = 'sha1_test',
    srcs = 'sha1_test.cpp',
    deps = [
        ':sha1',
        '//toft/encoding:encoding',
    ],
)

cc_library(
    name = 'crc32',
    srcs = 'crc32.cpp',
)

cc_test(
    name = 'crc32_test',
    srcs = 'crc32_test.cpp',
    deps = [
        ':crc32',
        '//toft/encoding:encoding',
    ],
)
