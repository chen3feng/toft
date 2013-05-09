cc_library(
    name = 'mime',
    srcs = 'mime.cpp',
    deps = [
        '//toft/base/string:string',
        '//thirdparty/glog:glog'
    ]
)

cc_test(
    name = 'mime_test',
    srcs = 'mime_test.cpp',
    deps = ':mime'
)
