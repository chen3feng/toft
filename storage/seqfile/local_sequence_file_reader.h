// Copyright 2013, Baidu Inc.
// Author: Liu Cheng <liucheng02@baidu.com>
//
#ifndef  TOFT_STORAGE_SEQFILE_LOCAL_SEQUENCE_FILE_READER_H_
#define  TOFT_STORAGE_SEQFILE_LOCAL_SEQUENCE_FILE_READER_H_

#include <stddef.h>
#include <string>

#include "toft/base/string/string_piece.h"
#include "toft/storage/seqfile/data_input_buffer.h"

namespace toft {

class LocalSequenceFileReader {
public:
    explicit LocalSequenceFileReader(File* file);

    // Findout if the file is seq file
    bool Init();

    virtual ~LocalSequenceFileReader();

    /**
     * Read a record into file.
     * @param key key string
     * @param value value string
     * @return whether succeeded or not
     */
    virtual bool ReadRecord(std::string* key, std::string* value);

    /**
     * Read a record into file, StringPiece manner. Returns 2 StringPiece pointing to
     * the data buffer hold by libhdfs. Enables user to peek into record without copying
     * to a string.
     * @param key key StringPiece
     * @param value value StringPiece
     * @return whether succeeded or not
     */
    virtual bool ReadRecord(StringPiece* key, StringPiece* value);

    /**
     * Skip to the next sync marker.
     * @return whether succeeded or not
     */

    virtual int64_t SkipToSync(int64_t offset);
    /**
     * Seek to a offset in the file.
     * @param offset to seek to
     * @return whether succeeded or not
     */
    virtual bool Seek(int64_t offset);

    /**
     * Tell the current position in the file in bytes.
     * @return offset in bytes.
     */
    virtual int64_t Tell();

    /**
     * Close the file. No return value.
     */
    virtual void Close();

    /**
     * seek to the last nth record, this is used for implementing 'tail -n'
     * @return succeed of not
     */
    bool SeekToTail(int n);

    int64_t GetFileSize() { return m_data_input->GetFileSize(); }

    bool is_finished() { return m_data_input->IsFinished(); }

    bool has_sync_marker() { return m_has_sync; }

    std::string sync_marker() { return std::string(m_sync, SYNC_HASH_SIZE); }

    std::string key_class_name() { return m_key_class_name; }

    std::string value_class_name() { return m_value_class_name; }

    int version() { return m_version; }

    bool is_compressed() { return m_decompress; }

    bool is_block_compressed() { return m_block_decompress; }

private:
    bool LoadSequenceFileHeader();

    bool CheckSequenceFileVersion();

    bool LoadKeyValueClassName();

    bool LoadCompressInfo();

    bool LoadMetadata();

    bool LoadSync();

    bool LoadRecordLen(int* record_len);

    bool FindLastNthRecordOffset(int try_offset, int n, int64_t* off);

private:
    // loaded data from file
    DataInputBuffer* m_data_input;

    // is head parsed yet? we'd better have a init method
    bool m_is_head_parsed;

    // compress information in sequence file header
    bool m_decompress;
    bool m_block_decompress;

    static const int SYNC_ESCAPE = -1;
    static const int SYNC_HASH_SIZE = 16;
    static const int SYNC_SIZE = 4 + SYNC_HASH_SIZE;

    // sync string in sequcen file
    char m_sync[SYNC_HASH_SIZE];
    bool m_has_sync;
    int m_version;

    std::string m_key_class_name;
    std::string m_value_class_name;
    std::string m_err;

    static const int HAS_SYNC_VERSION = 1;
    static const int HAS_COMPRESS_VERSION = 2;
    static const int BLOCK_COMPRESS_VERSION = 4;
    static const int CUSTOM_COMPRESS_VERSION = 5;
    static const int VERSION_WITH_METADATA = 6;
    static const char* VERSION /* = "SEQ" */;
};

}  // namespace toft

#endif  // TOFT_STORAGE_SEQFILE_LOCAL_SEQUENCE_FILE_READER_H_
