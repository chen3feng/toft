cc_library(
    name = 'hash',
    srcs = 'hash.cpp',
    deps = ['//thirdparty/stringencoders:stringencoders'],
)

cc_library(
    name = 'md5',
    srcs = 'md5.cpp',
)

cc_test(
    name = 'hash_unittest',
    srcs = 'hash_unittest.cpp',
    deps = [':hash'],
)
