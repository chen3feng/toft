cc_library(
    name = 'ip_address',
    srcs = [
        'ip_address.cpp'
    ]
)

cc_library(
    name = 'socket',
    srcs = 'socket.cpp',
    deps = ':socket_address'
)

cc_library(
    name = 'domain_resolver',
    srcs = 'domain_resolver.cpp',
    deps = ':ip_address'
)

cc_library(
    name = 'socket_address',
    srcs = 'socket_address.cpp',
    deps = ':ip_address'
)

cc_library(
    name = 'net',
    deps = [
        ':domain_resolver',
        ':ip_address',
        ':socket',
        ':socket_address'
    ]
)

cc_test(
    name = 'domain_resolver_test',
    srcs = 'domain_resolver_test.cpp',
    deps = ':domain_resolver'
)

cc_test(
    name = 'ip_address_test',
    srcs = 'ip_address_test.cpp',
    deps = ':ip_address'
)

cc_test(
    name = 'socket_address_test',
    srcs = 'socket_address_test.cpp',
    deps = ':socket_address'
)

cc_test(
    name = 'socket_test',
    srcs = 'socket_test.cpp',
    deps = [
        ':socket',
        '//toft/system/threading:threading'
    ]
)

