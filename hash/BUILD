cc_library(
    name = 'hash',
    deps = [':jenkins_hash',
            ':murmur_hash',
            ':fingerprint32',
            ':super_fast_hash',
            ],
)

cc_library(
    name = 'jenkins_hash',
    srcs = 'jenkins_hash.cpp',
)

cc_library(
    name = 'fingerprint32',
    srcs = 'fingerprint32.cpp',
)

cc_library(
    name = 'super_fast_hash',
    srcs = 'super_fast_hash.cpp',
)


cc_library(
    name = 'murmur_hash',
    srcs = 'murmur_hash.cpp',
    deps = ['//thirdparty/stringencoders:stringencoders'],
)

cc_test(
    name = 'hash_unittest',
    srcs = 'hash_unittest.cpp',
    deps = [':hash'],
)

cc_benchmark(
    name = 'hash_benchmark',
    srcs = 'hash_benchmark.cpp',
    deps = [':hash'],
)
