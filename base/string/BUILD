cc_library(
    name = '_compare',
    srcs = 'compare.cpp'
)

cc_library(
    name = '_concat',
    srcs = 'concat.cpp',
    deps = ':_number',
)

cc_library(
    name = '_string_piece',
    srcs = 'string_piece.cpp',
    deps = ':_compare'
)

cc_library(
    name = '_algorithm',
    srcs = 'algorithm.cpp',
    deps = ':_string_piece'
)

cc_library(
    name = '_number',
    srcs = [
        'number.cpp',
    ]
)

cc_library(
    name = 'string',
    deps = [
        ':_algorithm',
        ':_compare',
        ':_concat',
        ':_string_piece',
        ':_number',
        './format:_format',
    ]
)

cc_test(
    name = 'concat_test',
    srcs = ['concat_test.cpp'],
    deps = [
        ':_concat',
        './format:_format',
        '//thirdparty/perftools:profiler'
    ],
    optimize = '-O3'
)

cc_test(
    name = 'number_test',
    srcs = ['number_test.cpp'],
    deps = [':_number'],
    extra_cppflags = '-Wno-float-equal'
)

cc_test(
    name = 'algorithm_test',
    srcs = ['algorithm_test.cpp'],
    deps = [':_algorithm']
)

cc_test(
    name = 'string_piece_test',
    srcs = ['string_piece_test.cpp'],
    deps = [':_string_piece']
)

