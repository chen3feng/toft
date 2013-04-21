cc_library(
    name = 'sync_object',
    srcs = [
        'condition_variable.cpp',
        'mutex.cpp'
    ],
    deps = [
        '//toft/system:check_error',
        '#pthread'
    ]
)

cc_library(
    name = '_this_thread',
    srcs = 'this_thread.cpp',
    deps = '#pthread',
)

cc_library(
    name = '_thread',
    srcs = [
        'thread.cpp',
        'base_thread.cpp',
    ],
    deps = [
        '//toft/system:check_error',
        '#pthread',
    ]
)

cc_test(
    name = 'mutex_test',
    srcs = 'mutex_test.cpp',
    deps = ':sync_object',
)

cc_test(
    name = 'this_thread_test',
    srcs = 'this_thread_test.cpp',
    deps = [
        ':_this_thread',
        ':_thread',
    ]
)

cc_test(
    name = 'thread_test',
    srcs = 'thread_test.cpp',
    deps = [
        ':_this_thread',
        ':_thread',
    ]
)
