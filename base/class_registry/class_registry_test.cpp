// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/base/class_registry/class_registry.h"

#include <set>

#include "toft/base/class_registry/class_registry_test_helper.h"
#include "toft/base/scoped_ptr.h"

#include "thirdparty/gtest/gtest.h"

namespace toft {
namespace base_namespace {

class SomeClass {
public:
    virtual ~SomeClass() {}
};

TOFT_CLASS_REGISTRY_DEFINE(some_class_registry, SomeClass);

#define REGISTER_SOME_CLASS(class_name) \
    TOFT_CLASS_REGISTRY_REGISTER_CLASS( \
        base_namespace::some_class_registry, \
        base_namespace::SomeClass, \
        #class_name, \
        class_name)

#define CREATE_SOME_CLASS(name_as_string) \
    TOFT_CLASS_REGISTRY_CREATE_OBJECT(base_namespace::some_class_registry, name_as_string)

} // namespace base_namespace

namespace other_namespace {

class OtherClass : public base_namespace::SomeClass { };

REGISTER_SOME_CLASS(OtherClass);

} // namespace other_namespace

TEST(ClassRegister, CrossNamespace) {
    scoped_ptr<base_namespace::SomeClass> p(CREATE_SOME_CLASS("OtherClass"));
    EXPECT_TRUE(p.get() != NULL);
}

TEST(ClassRegister, CreateMapper) {
    scoped_ptr<Mapper> mapper;
    mapper.reset(CREATE_MAPPER(""));
    EXPECT_TRUE(mapper.get() == NULL);

    mapper.reset(CREATE_MAPPER("HelloMapper "));
    EXPECT_TRUE(mapper.get() == NULL);

    mapper.reset(CREATE_MAPPER("HelloWorldMapper"));
    EXPECT_TRUE(mapper.get() == NULL);

    mapper.reset(CREATE_MAPPER("HelloReducer"));
    EXPECT_TRUE(mapper.get() == NULL);

    mapper.reset(CREATE_MAPPER("WorldReducer"));
    EXPECT_TRUE(mapper.get() == NULL);

    mapper.reset(CREATE_MAPPER("SecondaryMapper"));
    EXPECT_TRUE(mapper.get() == NULL);

    mapper.reset(CREATE_MAPPER("HelloMapper"));
    ASSERT_TRUE(mapper.get() != NULL);
    EXPECT_EQ("HelloMapper", mapper->GetMapperName());

    mapper.reset(CREATE_MAPPER("WorldMapper"));
    ASSERT_TRUE(mapper.get() != NULL);
    EXPECT_EQ("WorldMapper", mapper->GetMapperName());
}

TEST(ClassRegister, CreateSecondaryMapper) {
    scoped_ptr<Mapper> mapper;
    mapper.reset(CREATE_SECONDARY_MAPPER(""));
    EXPECT_TRUE(mapper.get() == NULL);

    mapper.reset(CREATE_SECONDARY_MAPPER("SecondaryMapper "));
    EXPECT_TRUE(mapper.get() == NULL);

    mapper.reset(CREATE_SECONDARY_MAPPER("HelloWorldMapper"));
    EXPECT_TRUE(mapper.get() == NULL);

    mapper.reset(CREATE_SECONDARY_MAPPER("HelloReducer"));
    EXPECT_TRUE(mapper.get() == NULL);

    mapper.reset(CREATE_SECONDARY_MAPPER("WorldReducer"));
    EXPECT_TRUE(mapper.get() == NULL);

    mapper.reset(CREATE_SECONDARY_MAPPER("HelloMapper"));
    EXPECT_TRUE(mapper.get() == NULL);

    mapper.reset(CREATE_SECONDARY_MAPPER("WorldMapper"));
    EXPECT_TRUE(mapper.get() == NULL);

    mapper.reset(CREATE_SECONDARY_MAPPER("SecondaryMapper"));
    ASSERT_TRUE(mapper.get() != NULL);
    EXPECT_EQ("SecondaryMapper", mapper->GetMapperName());
}

TEST(ClassRegister, CreateReducer) {
    scoped_ptr<Reducer> reducer;
    reducer.reset(CREATE_REDUCER(""));
    EXPECT_TRUE(reducer.get() == NULL);

    reducer.reset(CREATE_REDUCER("HelloReducer "));
    EXPECT_TRUE(reducer.get() == NULL);

    reducer.reset(CREATE_REDUCER("HelloWorldReducer"));
    EXPECT_TRUE(reducer.get() == NULL);

    reducer.reset(CREATE_REDUCER("HelloMapper"));
    EXPECT_TRUE(reducer.get() == NULL);

    reducer.reset(CREATE_REDUCER("WorldMapper"));
    EXPECT_TRUE(reducer.get() == NULL);

    reducer.reset(CREATE_REDUCER("HelloReducer"));
    ASSERT_TRUE(reducer.get() != NULL);
    EXPECT_EQ("HelloReducer", reducer->GetReducerName());

    reducer.reset(CREATE_REDUCER("WorldReducer"));
    ASSERT_TRUE(reducer.get() != NULL);
    EXPECT_EQ("WorldReducer", reducer->GetReducerName());
}

TEST(ClassRegister, FileSystemNames) {
    EXPECT_EQ(3u, FILE_SYSTEM_COUNT());

    std::set<std::string> file_system_names;
    for (size_t i = 0; i < FILE_SYSTEM_COUNT(); ++i) {
        file_system_names.insert(FILE_SYSTEM_NAME(i));
    }
    EXPECT_EQ(3u, file_system_names.size());
    EXPECT_TRUE(file_system_names.find("/mem") != file_system_names.end());
    EXPECT_TRUE(file_system_names.find("/nfs") != file_system_names.end());
    EXPECT_TRUE(file_system_names.find("/local") != file_system_names.end());
}

TEST(ClassRegister, FileSystemSingleton) {
    ASSERT_TRUE(GET_FILE_SYSTEM("/mem") != NULL);
    EXPECT_EQ("MemFileSystem",
              GET_FILE_SYSTEM("/mem")->GetFileSystemName());
    // Test if it's a "real" singleton.
    EXPECT_EQ(GET_FILE_SYSTEM("/mem"),
              GET_FILE_SYSTEM("/mem"));

    ASSERT_TRUE(GET_FILE_SYSTEM("/nfs") != NULL);
    EXPECT_EQ("NetworkFileSystem",
              GET_FILE_SYSTEM("/nfs")->GetFileSystemName());
    EXPECT_EQ(GET_FILE_SYSTEM("/nfs"),
              GET_FILE_SYSTEM("/nfs"));

    ASSERT_TRUE(GET_FILE_SYSTEM("/local") != NULL);
    EXPECT_EQ("LocalFileSystem",
              GET_FILE_SYSTEM("/local")->GetFileSystemName());
    EXPECT_EQ(GET_FILE_SYSTEM("/local"),
              GET_FILE_SYSTEM("/local"));

    EXPECT_TRUE(GET_FILE_SYSTEM("/") == NULL);
    EXPECT_TRUE(GET_FILE_SYSTEM("") == NULL);
    EXPECT_TRUE(GET_FILE_SYSTEM("/mem/") == NULL);
    EXPECT_TRUE(GET_FILE_SYSTEM("/mem2") == NULL);
}

} // namespace toft
