// Copyright 2013, Baidu Inc.
// Author: An Qin (anqin.qin@gmail.com)
// A FileSystem implementation for libhdfs.


#ifndef  TOFT_STORAGE_FILE_HDFS_FILE_H_
#define  TOFT_STORAGE_FILE_HDFS_FILE_H_

#include <stdio.h>

#include <map>
#include <string>

#include "thirdparty/dfs-adapter/hdfs.h"
//#include "thirdparty/hybridfs/hybridfs.h"
#include "toft/storage/file/file.h"

#define HDFS_FS_DEFAULT_BUFFER_SIZE 4 * 1024 * 1024
#define HDFS_FS_DEFAULT_REPLICA 3
#define HDFS_FS_DEFAULT_BLOCKSIZE 256 * 1024 * 1024

namespace toft {

// a FileSystem like implementation of HDFS
// Note that HDFSFileSystem instances need to out-live HDFSFile created from it.
class HDFSFileSystem : public FileSystem {
public:
    HDFSFileSystem();
    virtual ~HDFSFileSystem();
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

    static HDFSFileSystem* GetRegisteredFileSystem() {
        return static_cast<HDFSFileSystem*>(TOFT_GET_FILE_SYSTEM(HDFS));
    }

    /**
     * A util method to get a hdfsFS obj from path.
     * @param file_path path to file, valid for both HDFS path like "/hdfs/szwg-ecomon:5313/user..."
     * or local path like "/home/work/..."
     * @return a hdfsFS obj if success, NULL if fail.
     */
    hdfsFS GetHDFSFS(const std::string& file_path);

public:
    static const char* HDFS /* = "hdfs" */;

protected:
    // A (process-wide) cache of hdfsFS objects.
    // These connections are shared across all threads and kept
    // open until the process terminates.
    // (Calls to hdfsDisconnect() by individual threads would terminate all
    // other connections handed out via hdfsConnect() to the same URI.)
    class HDFSFSCache {
    public:
        ~HDFSFSCache();

        hdfsFS GetLocal();
        // Get connection to specific fs by specifying the name node's
        // ipaddress or hostname and port.
        hdfsFS GetConnection(const std::string& schema,
                             const std::string& host_port);

        // Get connection to specific fs by specifying the name node's
        // ipaddress or hostname, port, username and password.
        hdfsFS GetConnection(const std::string& schema,
                             const std::string& host_port,
                             const std::string& username,
                             const std::string& password);

    private:
        typedef std::map<std::pair<std::string, std::string>, hdfsFS> HDFSFSMap;
        HDFSFSMap fs_map_;
    };

     /**
     * A util method to get a hdfsFS obj from path.
     * @param file_path path to file, valid for both HDFS path like "/hdfs/szwg-ecomon:5313/user..."
     * or local path like "/home/work/..."
     * @param params params parsed from second section of URI.
     * @return a hdfsFS obj if success, NULL if fail.
     */
    hdfsFS GetHDFSFS(const std::string& file_path,
            std::map<std::string, std::string>* params);

    // a internal hdfs connection cache
    HDFSFSCache m_fs_cache;
};

// Represent a file object on local mounted file system
class HDFSFile : public File {
public:
    virtual ~HDFSFile();

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
    friend class HDFSFileSystem;
    HDFSFile(hdfsFile file, hdfsFS fs, const std::string& file_path, const char* mode);

    hdfsFile m_file; // internal hdfsFile obj
    hdfsFS m_fs; // internal hdfsFS obj
    LineReader m_line_reader; //internal hdfsLineReader obj

    bool m_iseof;
    bool m_closed;
};

}

#endif  //TOFT_STORAGE_FILE_HDFS_FILE_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
