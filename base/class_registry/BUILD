cc_library(
    name = '_class_registry',
    srcs = 'class_registry.cpp'
)

cc_test(
    name = 'class_registry_test',
    srcs = [
        'class_registry_test.cpp',
        'class_registry_test_helper.cpp'
    ],
    deps = ':_class_registry'
)


