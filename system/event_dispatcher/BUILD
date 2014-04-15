cc_library(
    name = 'event_dispatcher',
    srcs = [
        'event_dispatcher.cpp',
    ],
    extra_cppflags = '-Wno-strict-aliasing',
    deps = '//thirdparty/libev:ev',
)

cc_test(
    name = 'event_dispatcher_test',
    srcs = [
        'event_dispatcher_test.cpp',
    ],
    deps = ':event_dispatcher'
)

