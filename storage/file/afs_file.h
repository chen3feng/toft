// Copyright (C) 2017, Baidu Inc.
// Author: An Qin (anqin.qin@gmail.com)
//
// Description: A FileSystem implementation for AFS API

#ifndef  TOFT_STORAGE_FILE_AFS_FILE_H
#define  TOFT_STORAGE_FILE_AFS_FILE_H

#include <stdio.h>

#include <map>
#include <string>

#include "thirdparty/dfs-adapter/afs.h"
#include "toft/storage/file/file.h"

typedef dfs::AfsFileSystem* AfsFS;
typedef dfs::Reader AfsReader;
typedef dfs::Writer AfsWriter;

namespace toft {

// a FileSystem like implementation of AFS
// Note that AFSFileSystem instances need to out-live AFSFile created from it.
class AFSFileSystem : public FileSystem {
public:
    AFSFileSystem();
    virtual ~AFSFileSystem();
    virtual File* Open(const std::string& file_path, const char* mode);
    virtual bool Exists(const std::string& file_path);
    virtual bool Delete(const std::string& file_path);
    virtual bool Rename(const std::string& from, const std::string& to);
    virtual bool GetTimes(const std::string& file_path, FileTimes* times);
    virtual int64_t GetSize(const std::string& file_path);
    virtual FileIterator* Iterate(const std::string& dir,
                                  const std::string& pattern,
                                  int include_types,
                                  int exclude_types);
    virtual bool Mkdir(const std::string& dir, int mode);
    virtual bool Rmdir(const std::string& dir);

    static AFSFileSystem* GetRegisteredFileSystem() {
        return static_cast<AFSFileSystem*>(TOFT_GET_FILE_SYSTEM(AFS));
    }

    /**
     * A util method to get a AfsFS obj from path.
     * @param file_path path to file, valid for both AFS path like "/afs/szwg-ecomon:5313/user..."
     * or local path like "/home/work/..."
     * @return a AfsFS obj if success, NULL if fail.
     */
    AfsFS GetAFSFS(const std::string& file_path);

public:
    static const char* AFS  /* = "afs" */;

protected:
    // A (process-wide) cache of AfsFS objects.
    // These connections are shared across all threads and kept
    // open until the process terminates.
    // (Calls to afsDisconnect() by individual threads would terminate all
    // other connections handed out via afsConnect() to the same URI.)
    class AFSFSCache {
    public:
        ~AFSFSCache();

        AfsFS GetLocal();
        // Get connection to specific fs by specifying the name node's
        // ipaddress or hostname and port.
        AfsFS GetConnection(const std::string& uri);

        // Get connection to specific fs by specifying the name node's
        // ipaddress or hostname, port, username and password.
        AfsFS GetConnection(const std::string& uri,
                            const std::string& username,
                            const std::string& password);

    private:
        typedef std::map<std::pair<std::string, std::string>, AfsFS> AFSFSMap;
        AFSFSMap fs_map_;
    };

     /**
     * A util method to get a AfsFS obj from path.
     * @param file_path path to file, valid for both AFS path like "/afs/szwg-ecomon:5313/user..."
     * or local path like "/home/work/..."
     * @param params params parsed from second section of URI.
     * @return a AfsFS obj if success, NULL if fail.
     */
    AfsFS GetAFSFS(const std::string& file_path,
            std::map<std::string, std::string>* params);

    // a internal afs connection cache
    AFSFSCache m_fs_cache;
};

// Represent a file object on local mounted file system
class AFSFile : public File {
public:
    virtual ~AFSFile();

    // Implement File interface.
    //
    virtual int64_t Read(void* buffer, int64_t size);
    virtual int64_t Write(const void* buffer, int64_t size);
    virtual bool Flush();
    virtual bool Close();
    virtual bool Seek(int64_t offset, int whence);
    virtual int64_t Tell();
    virtual bool ReadLine(std::string* line, size_t max_size);
    virtual bool ReadLineWithLineEnding(std::string* line, size_t max_size);
    virtual bool IsEof();
private:
    friend class AFSFileSystem;
    AFSFile(AfsReader* reader, AfsWriter* writer, AfsFS fs,
            const std::string& file_path,
            const char* mode);

    AfsReader* m_reader;
    AfsWriter* m_writer;
    AfsFS m_fs;

    bool m_iseof;
    bool m_closed;
};

}  // namespace toft

#endif  //TOFT_STORAGE_FILE_AFS_FILE_H
