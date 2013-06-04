cc_library(
    name = 'hash',
    deps = [
        ':city',
        ':jenkins',
        ':murmur',
        ':fingerprint',
        ':super_fast',
    ],
)

cc_library(
    name = 'city',
    srcs = 'city.cpp',
    deps = '//thirdparty/cityhash:cityhash',
)

cc_library(
    name = 'jenkins',
    srcs = 'jenkins.cpp',
)

cc_library(
    name = 'fingerprint',
    srcs = [
        'fingerprint32.cpp',
        'fingerprint64.cpp',
    ],
    deps = [
        ':murmur',
        '//thirdparty/stringencoders:stringencoders'
    ],
)

cc_library(
    name = 'super_fast',
    srcs = 'super_fast.cpp',
)


cc_library(
    name = 'murmur',
    srcs = [
        'murmur/MurmurHash2_64.cpp',
        'murmur/MurmurHash2A.cpp',
        'murmur/MurmurHash2.cpp',
        'murmur/MurmurHash3.cpp',
        'murmur/MurmurHashAligned2.cpp',
        'murmur/MurmurHashNeutral2.cpp',
    ],
)

cc_test(
    name = 'hash_test',
    srcs = 'hash_test.cpp',
    deps = [':hash'],
)

cc_benchmark(
    name = 'hash_benchmark',
    srcs = 'hash_benchmark.cpp',
    deps = [':hash'],
)
