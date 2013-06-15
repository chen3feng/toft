// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

// Defines several base class and registers for testing. We intentionally
// define them in a separate file as some compilers don't correctly support to
// define static variable in inline function, they create a separate copy each
// time it's included. We want to make sure it doesn't happen to us.

#ifndef TOFT_BASE_CLASS_REGISTRY_CLASS_REGISTRY_TEST_HELPER_H
#define TOFT_BASE_CLASS_REGISTRY_CLASS_REGISTRY_TEST_HELPER_H

#include <string>

#include "toft/base/class_registry/class_registry.h"

class Mapper {
public:
    Mapper() {}
    virtual ~Mapper() {}

    virtual std::string GetMapperName() const = 0;
};

TOFT_CLASS_REGISTRY_DEFINE(mapper_registry, Mapper);

#define REGISTER_MAPPER(mapper_name) \
    TOFT_CLASS_REGISTRY_REGISTER_CLASS( \
        mapper_registry, Mapper, #mapper_name, mapper_name)

#define CREATE_MAPPER(mapper_name_as_string) \
    TOFT_CLASS_REGISTRY_CREATE_OBJECT(mapper_registry, mapper_name_as_string)

TOFT_CLASS_REGISTRY_DEFINE(second_mapper_registry, Mapper);

#define REGISTER_SECONDARY_MAPPER(mapper_name) \
    TOFT_CLASS_REGISTRY_REGISTER_CLASS( \
        second_mapper_registry, Mapper, #mapper_name, mapper_name)

#define CREATE_SECONDARY_MAPPER(mapper_name_as_string) \
    TOFT_CLASS_REGISTRY_CREATE_OBJECT(second_mapper_registry, \
                                      mapper_name_as_string)

class Reducer {
public:
    Reducer() {}
    virtual ~Reducer() {}

    virtual std::string GetReducerName() const = 0;
};

TOFT_CLASS_REGISTRY_DEFINE(reducer_registry, Reducer);

#define REGISTER_REDUCER(reducer_name) \
    TOFT_CLASS_REGISTRY_REGISTER_CLASS( \
        reducer_registry, Reducer, #reducer_name, reducer_name)

#define CREATE_REDUCER(reducer_name_as_string) \
    TOFT_CLASS_REGISTRY_CREATE_OBJECT(reducer_registry, reducer_name_as_string)

class FileSystem {
public:
    FileSystem() {}
    virtual ~FileSystem() {}

    virtual std::string GetFileSystemName() const = 0;
};

TOFT_CLASS_REGISTRY_DEFINE_SINGLETON(file_system_registry, FileSystem);

#define REGISTER_FILE_SYSTEM(path_prefix_as_string, file_impl_name) \
    TOFT_CLASS_REGISTRY_REGISTER_CLASS_SINGLETON( \
        file_system_registry, FileSystem, path_prefix_as_string, file_impl_name)

#define GET_FILE_SYSTEM(path_prefix_as_string) \
    TOFT_CLASS_REGISTRY_GET_SINGLETON(file_system_registry, path_prefix_as_string)

#define FILE_SYSTEM_COUNT() \
    TOFT_CLASS_REGISTRY_CLASS_COUNT(file_system_registry)

#define FILE_SYSTEM_NAME(i) \
    TOFT_CLASS_REGISTRY_CLASS_NAME(file_system_registry, i)

#endif  // TOFT_BASE_CLASS_REGISTRY_CLASS_REGISTRY_TEST_HELPER_H
