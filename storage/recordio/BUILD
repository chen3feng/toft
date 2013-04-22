cc_library(
    name = 'recordio',
    srcs = [
        'record_io.cc'
    ],
    deps = [
        '//toft/storage/file:file',
        '//thirdparty/protobuf:protobuf'
    ]
)

cc_library(
    name = 'inv_recordio',
    srcs = [
        'inv_record_io.cc'
    ],
    deps = [
        '//toft/storage/file:file',
        '//thirdparty/protobuf:protobuf'
    ]
)


proto_library(
    name = 'document_proto',
    srcs = 'document.proto'
)

#cc_test(
#    name = 'recordio_test',
#    srcs = 'record_io_test.cc',
#    deps = [
#        ':recordio',
#        ':document_proto'
#    ],
#    testdata = [
#        'document.proto'
#    ]
#)

#cc_test(
#    name = 'inv_recordio_test',
#    srcs = 'inv_record_io_test.cc',
#    deps = [
#        ':inv_recordio',
#        ':document_proto'
#    ],
#    testdata = [
#        'document.proto'
#    ]
#)
