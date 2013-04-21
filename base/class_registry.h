// Copyright 2010, The TOFT Authors.
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef TOFT_BASE_CLASS_REGISTRY_H
#define TOFT_BASE_CLASS_REGISTRY_H
#pragma once

// Defines several helper macros for registering class and its singleton by a
// string name and creating/retrieving them later per the registered name.
// The motivation is to help implement the factory class. C++ doesn't support
// reflection so we define several macros to do this.
//
// All macros defined here are NOT used by final user directly, and they are
// used to create register macros for a specific base class. Here is an example:
/*
   mapper.h (the interface definition):
   #include "toft/base/class_registry.h"
   class Mapper {
   };

   TOFT_CLASS_REGISTRY_DEFINE(mapper_registry, Mapper);

   #define REGISTER_MAPPER(mapper_name) \
       TOFT_CLASS_REGISTRY_REGISTER_CLASS( \
           mapper_registry, Mapper, #mapper_name, mapper_name) \

   #define CREATE_MAPPER(mapper_name_as_string) \
       TOFT_CLASS_REGISTRY_CREATE_OBJECT(mapper_registry, mapper_name_as_string)

   hello_mapper.cc (an implementation of Mapper):
   #include "mapper.h"
   class HelloMapper : public Mapper {
   };
   REGISTER_MAPPER(HelloMapper);

   mapper_user.cc (the final user of all registered mappers):
   #include "mapper.h"
   Mapper* mapper = CREATE_MAPPER("HelloMapper");
*/

// Another usage is to register class by an arbitrary string instead of its
// class name, and register a singleton instance for each registered name.
// Here is an example:
/*
   file_system.h (the interface definition):
   class FileSystem {
   };

   TOFT_CLASS_REGISTRY_DEFINE_SINGLETON(file_system_registry, FileSystem);

   #define REGISTER_FILE_SYSTEM(path_prefix_as_string, file_system_name) \
       TOFT_CLASS_REGISTRY_REGISTER_CLASS_SINGLETON( \
           file_system_registry, FileSystem, path_prefix_as_string, file_system_name)

   #define GET_FILE_SYSTEM(path_prefix_as_string) \
       TOFT_CLASS_REGISTRY_GET_SINGLETON(file_system_registry, path_prefix_as_string)

   #define FILE_SYSTEM_COUNT() \
       TOFT_CLASS_REGISTRY_CLASS_COUNT(file_system_registry)

   #define FILE_SYSTEM_NAME(i) \
       TOFT_CLASS_REGISTRY_CLASS_NAME(file_system_registry, i)

   local_file.cc (an implementation of FileSystem):
   #include "file.h"
   class LocalFileSystem : public FileSystem {
   };
   REGISTER_FILE_SYSTEM("/local", LocalFileSystem);

   file_user.cc (the final user of all registered file implementations):
   #include "file_system.h"
   FileSystem* file_system = CET_FILE_SYSTEM("/local");
*/

#include "toft/base/class_registry/class_registry.h"

#endif // TOFT_BASE_CLASS_REGISTRY_H
