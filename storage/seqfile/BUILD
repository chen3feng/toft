cc_library(
    name = 'local_sequence_file_reader',
    srcs = [
         'local_sequence_file_reader.cpp'
    ],
    deps = [
        '//thirdparty/glog:glog',
        '//toft/storage/file:file',
        '//toft/storage/file:uri_utils',
    ]
)

cc_test(
    name = 'local_sequence_file_reader_test',
    srcs = [
        'local_sequence_file_reader_test.cpp',
    ],
    deps = [
        ':local_sequence_file_reader',
         '//toft/storage/file:uri_utils',
         '//toft/storage/file:file'
    ],
    testdata = [
        'testdata/seq_file',
        'testdata/big_seq_file'
    ]
)

cc_library(
    name = 'local_sequence_file_writer',
    srcs = [
        'local_sequence_file_writer.cpp'
    ],
    deps = [
        '//thirdparty/glog:glog',
        '//toft/storage/file:file',
        '//toft/storage/file:uri_utils',
        '//toft/system/time:time',
        ':local_sequence_file_reader',
    ]
)

cc_test(
    name = 'local_sequence_file_writer_test',
    srcs = [
        'local_sequence_file_writer_test.cpp',
    ],
    deps = [
        ':local_sequence_file_writer',
        ':local_sequence_file_reader',
        '//toft/base/string:string',
        '//toft/system/time:time'
    ]
)
