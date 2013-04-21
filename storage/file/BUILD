cc_library(
    name = 'file',
    srcs = [
        'file.cpp',
        'local_file.cpp',
    ],
    deps = [
        '//toft/base:class_registry',
        '//toft/base/string:string',
    ],
    link_all_symbols=True
)

cc_test(
    name = 'file_test',
    srcs = 'file_test.cpp',
    deps = ':file',
    testdata = 'testfile.txt'
)
