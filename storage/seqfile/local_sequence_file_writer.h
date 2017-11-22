// Copyright 2013, Baidu Inc.
// Author: Liu Cheng <liucheng02@baidu.com>
//
// It is a simple writer, create a sequence file from empty, do not support append
// use org.apache.hadoop.io.BytesWritable as key and value class
// sync is inserted into file every 100 records
// curious about the sequence file format, refer to:
// http://grepcode.com/file/repository.cloudera.com/content/repositories/releases/
// com.cloudera.hadoop/hadoop-core/0.20.2-737/org/apache/hadoop/io/SequenceFile.java
//
#ifndef  TOFT_STORAGE_SEQFILE_LOCAL_SEQUENCE_FILE_WRITER_H_
#define  TOFT_STORAGE_SEQFILE_LOCAL_SEQUENCE_FILE_WRITER_H_

#include <stddef.h>
#include <string>

#include "toft/base/string/string_piece.h"
#include "toft/storage/file/local_file.h"

namespace toft {

class LocalSequenceFileWriter {
public:
    explicit LocalSequenceFileWriter(File* file);

    virtual ~LocalSequenceFileWriter();

    // Init will write sequence file header, return false if write failed
    virtual bool Init();

    // Write record, return false if write failed
    virtual bool WriteRecord(const std::string& key, const std::string& value);

    virtual bool WriteRecord(const StringPiece& key, const StringPiece& value);

    virtual bool Close();

    bool IsDefaultHeader();

private:
    void GenerateSync();

    bool WriteHeader();

    bool Write(const char* addr, int len);

    int WriteSequenceFileVersion(char* addr);

    int WriteVLong(char* buf, int64_t i);

    int WriteKeyValueClassName(char* addr);

    int WriteCompressInfo(char* addr);

    int WriteMetadata(char* addr);

    int WriteSync(char* addr);

    // used when write record
    int WriteSyncToBuf(char* addr);

    int WriteInt(char* buf, int i);

    bool TryingToLoadHeader();

private:
    static const int SYNC_HASH_SIZE = 16;
    char m_sync[SYNC_HASH_SIZE];
    toft::File* m_file;  // the really local file
    int m_version;
    std::string m_key_class_name;
    std::string m_value_class_name;

    int64_t m_records_written_after_sync;
    static const int VERSION_WITH_METADATA = 6;  // the maximum version of sequence file
    static const int kMaxHeaderLen = 10240;  // simple sequence file header size < 10240 bytes
    static const int kSyncInterval = 100;  // sync every 100 bytes
    static const int kSyncEscape = -1;  // the sync escape
};

}  // namespace toft

#endif  // TOFT_STORAGE_SEQFILE_LOCAL_SEQUENCE_FILE_WRITER_H_
