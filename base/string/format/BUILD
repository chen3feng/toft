cc_library(
    name = '_format',
    srcs = [
        'print.cpp',
        'print_arg.cpp',
        'print_targets.cpp',
        'scan.cpp',
        'scan_arg.cpp',
        'specification.cpp',
        'vprint.cpp',
        'vscan.cpp',
    ],
    deps = [
        '//thirdparty/glog:glog',
        '//toft/base/string/:_number'
    ],
    extra_cppflags = '-Wno-format-nonliteral'
)

cc_test(
    name = 'scan_test',
    srcs = 'scan_test.cpp',
    deps = ':_format',
)

cc_test(
    name = 'print_test',
    srcs = 'print_test.cpp',
    optimize = '-O3',
    deps = [
        ':_format',
        '//thirdparty/perftools:profiler'
    ]
)


