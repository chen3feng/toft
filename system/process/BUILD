cc_library(
    name = 'process',
    srcs = [
        'process.cpp',
        'process_local_const.cpp',
        'this_process.cpp',
    ],
    deps = [
        '//toft/encoding:encoding',
        '//toft/storage/file:file',
        '//toft/system/threading:_this_thread',
        '//toft/system/time:time',
    ]
)

cc_test(
    name = 'process_local_const_test',
    srcs = 'process_local_const_test.cpp',
    deps = ':process'
)

cc_test(
    name = 'process_test',
    srcs = 'process_test.cpp',
    deps = [
        ':process'
    ]
)

cc_test(
    name = 'this_process_test',
    srcs = 'this_process_test.cpp',
    deps = [
        ':process'
    ]
)
