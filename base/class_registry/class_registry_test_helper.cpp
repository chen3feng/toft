// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/base/class_registry/class_registry_test_helper.h"

class HelloMapper : public Mapper {
    virtual std::string GetMapperName() const {
        return "HelloMapper";
    }
};
REGISTER_MAPPER(HelloMapper);

class WorldMapper : public Mapper {
    virtual std::string GetMapperName() const {
        return "WorldMapper";
    }
};
REGISTER_MAPPER(WorldMapper);

class SecondaryMapper : public Mapper {
    virtual std::string GetMapperName() const {
        return "SecondaryMapper";
    }
};
REGISTER_SECONDARY_MAPPER(SecondaryMapper);

class HelloReducer : public Reducer {
    virtual std::string GetReducerName() const {
        return "HelloReducer";
    }
};
REGISTER_REDUCER(HelloReducer);

class WorldReducer : public Reducer {
    virtual std::string GetReducerName() const {
        return "WorldReducer";
    }
};
REGISTER_REDUCER(WorldReducer);

class LocalFileSystem : public FileSystem {
    virtual std::string GetFileSystemName() const {
        return "LocalFileSystem";
    }
};
REGISTER_FILE_SYSTEM("/local", LocalFileSystem);

class MemFileSystem : public FileSystem {
    virtual std::string GetFileSystemName() const {
        return "MemFileSystem";
    }
};
REGISTER_FILE_SYSTEM("/mem", MemFileSystem);

class NetworkFileSystem : public FileSystem {
    virtual std::string GetFileSystemName() const {
        return "NetworkFileSystem";
    }
};
REGISTER_FILE_SYSTEM("/nfs", NetworkFileSystem);
