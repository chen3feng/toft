// Copyright 2013, Baidu Inc.
// Author: Liu Cheng <liucheng02@baidu.com>
//
#include "toft/storage/seqfile/local_sequence_file_reader.h"

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <limits.h>
#include <list>

#include "toft/storage/file/local_file.h"

namespace toft {
namespace {

class SequenceFileUtil {
public:
    /**
     * Parse Text Format KeyValue ClassName from buffer
     * @param buffer, size: the buffer;
     * @return is_succeed; used_len of paring; the kv class name and error
     */
    static bool ParseTextKeyValueClassName(
        DataInputBuffer* data_input,
        std::string* key_class_name, std::string* value_class_name, std::string* err) {

        int key_class_name_len = 0;
        int value_class_name_len = 0;

        if (!data_input->ReadVInt(&key_class_name_len)) {
            *err = "Read KeyClassNameLen Failed";
            return false;
        }

        if (!data_input->ReadString(key_class_name, key_class_name_len)) {
            *err = "Read KeyClassName Failed";
            return false;
        }

        if (!data_input->ReadVInt(&value_class_name_len)) {
            *err = "Read ValueClassNameLen Failed";
            return false;
        }

        if (!data_input->ReadString(value_class_name, value_class_name_len)) {
            *err = "Read ValueClassName Failed";
            return false;
        }
        return true;
    }

    /**
     * Parse UTF8 Format KeyValue ClassName from buffer
     * @param buffer, size: the buffer;
     * @return is_succeed; used_len of paring; the kv class name and error
     */
    static bool ParseUTF8KeyValueClassName(
        DataInputBuffer* data_input,
        std::string* key_class_name, std::string* value_class_name, std::string* err) {
        int16_t key_class_name_len = 0;
        int16_t value_class_name_len = 0;

        if (!data_input->ReadShort(&key_class_name_len)) {
            *err = "Can't load KeyClassName Len";
            return false;
        }

        if (!data_input->ReadString(key_class_name, key_class_name_len)) {
            *err = "Can't load KeyClassName";
            return false;
        }

        LOG(ERROR) << "key class name:" << *key_class_name;

        if (!data_input->ReadShort(&value_class_name_len)) {
            *err = "Can't load ValueClassName Len";
            return false;
        }

        if (!data_input->ReadString(value_class_name, value_class_name_len)) {
            *err = "Can't load ValueClassName";
            return false;
        }
        return true;
    }

    /**
     * Parse MetaData from header
     * @param buffer, size: the buffer;
     * @return is_succeed; used_len of paring;
     */
    static bool ParseMetadata(DataInputBuffer* data_input, std::string* err) {
        int num = 0;
        if (!data_input->ReadInt(&num)) {
            *err = "ParseMetadata num Failed";
            return false;
        }

        std::string key_class_name, value_class_name;
        for (int i = 0; i < num; ++i) {
            if (!ParseTextKeyValueClassName(
                    data_input, &key_class_name, &value_class_name, err)) {
                return false;
            }
        }
        return true;
    }
};

}  // namespace

const char* LocalSequenceFileReader::VERSION = "SEQ";

LocalSequenceFileReader::LocalSequenceFileReader(File* file)
    : m_data_input(NULL),
    m_is_head_parsed(false),
    m_decompress(false),
    m_block_decompress(false),
    m_has_sync(false),
    m_version(-1) {
    m_data_input = new DataInputBuffer(file, &m_err);
}

bool LocalSequenceFileReader::Init() {
    if (!m_is_head_parsed) {
        if (!LoadSequenceFileHeader()) {
            return false;
        }
        m_is_head_parsed = true;
    }
    return true;
}

LocalSequenceFileReader::~LocalSequenceFileReader() {
    Close();
    delete m_data_input;
    m_data_input = NULL;
}

void LocalSequenceFileReader::Close() {
    m_data_input->Reset();
    m_is_head_parsed = false;
}

bool LocalSequenceFileReader::ReadRecord(std::string* key, std::string* value) {
    StringPiece key_piece, value_piece;
    if (!ReadRecord(&key_piece, &value_piece)) {
        return false;
    }

    key->assign(key_piece.data(), key_piece.size());
    value->assign(value_piece.data(), value_piece.size());
    return true;
}

bool LocalSequenceFileReader::ReadRecord(StringPiece* key, StringPiece* value) {
    if (!m_is_head_parsed) {
        if (!LoadSequenceFileHeader()) {
            return false;
        }
        m_is_head_parsed = true;
    }

    int record_len = 0;
    if (!LoadRecordLen(&record_len)) {
        return false;
    }

    int key_len = 0;
    if (!m_data_input->ReadInt(&key_len)) {
        return false;
    }

    if (!m_data_input->ReadKeyValue(key_len, record_len - key_len, key, value)) {
        return false;
    }
    return true;
}

// Return value is the offset of next valid sync
// if no sync found or any error happends, return -1 indicate error
int64_t LocalSequenceFileReader::SkipToSync(int64_t offset) {
    if (!m_is_head_parsed) {
        if (!LoadSequenceFileHeader()) {
            return -1;
        }
        m_is_head_parsed = true;
    }

    if (!m_data_input->Seek(offset)) {
        return -1;
    }

    int64_t returned_offset = offset;
    while (m_data_input->LoadAtLeast(SYNC_HASH_SIZE)) {
        int64_t old_offset = m_data_input->Offset();
        if (m_data_input->FindSync(m_sync, SYNC_HASH_SIZE)) {
            return returned_offset + (m_data_input->Offset() - old_offset);
        }
        returned_offset += (m_data_input->Offset() - old_offset);
    }
    // can't find sync, return -1
    return -1;
}

bool LocalSequenceFileReader::Seek(int64_t offset) {
    return (SkipToSync(offset) != -1);
}

int64_t LocalSequenceFileReader::Tell() {
    return m_data_input->Tell();
}

bool LocalSequenceFileReader::LoadRecordLen(int* record_len) {
    int length = 0;
    if (!m_data_input->ReadInt(&length)) {
        return false;
    }

    // do we need to check sync
    if (m_version > HAS_SYNC_VERSION && m_has_sync && length == SYNC_ESCAPE) {
        char sync_check[SYNC_HASH_SIZE] = {0};
        if (!m_data_input->ReadBuffer(sync_check, SYNC_HASH_SIZE)) {
            return false;
        }

        if (memcmp(m_sync, sync_check, SYNC_HASH_SIZE) != 0) {
            m_err = "Check Sync Failed";
            return false;
        }

        // Read record length again
        if (!m_data_input->ReadInt(&length)) {
            return false;
        }
    }
    *record_len = length;
    return true;
}

/** Head Format
 * version - A byte array: 3 bytes of magic header 'SEQ', followed by 1 byte
 * of actual version no. (e.g. SEQ4 or SEQ6)
 *
 * keyClassName - String
 * valueClassName - String
 *
 * compression - A boolean which specifies if compression is turned on for
 * keys/values in this file.
 *
 * blockCompression - A boolean which specifies if block compression is
 * turned on for keys/values in this file.
 *
 * compressor class - The classname of the CompressionCodec which is used to
 * compress/decompress keys and/or values in this SequenceFile (if
 * compression is enabled).
 *
 * metadata - SequenceFile.Metadata for this file (key/value pairs)
 * sync - A sync marker to denote end of the header.
 **/
bool LocalSequenceFileReader::LoadSequenceFileHeader() {
    if (!CheckSequenceFileVersion()) {
        return false;
    }

    if (!LoadKeyValueClassName()) {
        return false;
    }

    if (!LoadCompressInfo()) {
        return false;
    }

    if (!LoadMetadata()) {
        return false;
    }

    if (!LoadSync()) {
        return false;
    }
    return true;
}

bool LocalSequenceFileReader::CheckSequenceFileVersion() {
    const char* version_string = NULL;
    if (!m_data_input->ReadVersion(&version_string)) {
        return false;
    }

    if (version_string[0] != VERSION[0] ||
        version_string[1] != VERSION[1] ||
        version_string[2] != VERSION[2]) {
        m_err = "SequenceFileHeader Not SEQ";
        return false;
    }

    m_version = static_cast<int>(version_string[3]);
    if (m_version > VERSION_WITH_METADATA) {
        m_err = "SequenceFileHeader Version > 6";
        return false;
    }
    return true;
}

// Parsing Key ClassName and Value ClassName
bool LocalSequenceFileReader::LoadKeyValueClassName() {
    if (m_version <= BLOCK_COMPRESS_VERSION) {
        return SequenceFileUtil::ParseUTF8KeyValueClassName(
                m_data_input, &m_key_class_name, &m_value_class_name, &m_err);
    } else {
        return SequenceFileUtil::ParseTextKeyValueClassName(
                m_data_input, &m_key_class_name, &m_value_class_name, &m_err);
    }
}

bool LocalSequenceFileReader::LoadCompressInfo() {
    char c = '\0';
    if (m_version > HAS_COMPRESS_VERSION) {
        if (!m_data_input->ReadByte(&c)) {
            return false;
        }
        m_decompress = (c != 0);
    } else {
        m_decompress = false;
    }

    if (m_version > BLOCK_COMPRESS_VERSION) {
        if (!m_data_input->ReadByte(&c)) {
            return false;
        }
        m_block_decompress = (c != 0);
    } else {
        m_block_decompress = false;
    }

    if (m_decompress) {
        m_err = "Decompress is not supported in this parser";
        return false;
    }

    if (m_block_decompress) {
        m_err = "BlockDecompress is not supported in this parser";
        return false;
    }
    return true;
}

bool LocalSequenceFileReader::LoadMetadata() {
    if (m_version < VERSION_WITH_METADATA) {
        return true;
    }
    return SequenceFileUtil::ParseMetadata(m_data_input, &m_err);
}

bool LocalSequenceFileReader::LoadSync() {
    if (m_version <= HAS_SYNC_VERSION) {
        return true;
    }

    if (!m_data_input->ReadBuffer(m_sync, SYNC_HASH_SIZE)) {
        return false;
    }
    m_has_sync = true;
    return true;
}

namespace {

class OffsetKeeper {
public:
    explicit OffsetKeeper(int n) : m_count(0), kKeepCount(n) {}

    void PushBack(int64_t offset) {
        m_offsets.push_back(offset);
        if (m_count == kKeepCount) {
            m_offsets.pop_front();
        } else {
            ++m_count;
        }
    }

    int64_t First() {
        if (m_count > 0) {
            return *(m_offsets.begin());
        }
        return -1;
    }

    int64_t Count() {
        return m_count;
    }

private:
    std::list<int64_t> m_offsets;
    int m_count;
    const int kKeepCount;
};

}  // namespace

bool LocalSequenceFileReader::FindLastNthRecordOffset(int try_offset, int n, int64_t* off) {
    if (!this->Seek(try_offset)) {
        return false;
    }

    OffsetKeeper keeper(n + 1);
    keeper.PushBack(this->Tell());

    toft::StringPiece key, value;
    while (this->ReadRecord(&key, &value)) {
        keeper.PushBack(this->Tell());
    }

    *off = keeper.First();
    CHECK_NE(-1, *off);

    // if we are trying to find nth record from file start, we should stop
    // if we found after try offset, there exists n records, we should stop
    return (try_offset == 0 || keeper.Count() == (n + 1));
}

bool LocalSequenceFileReader::SeekToTail(int n) {
    if (n < 0) {
        n = 0;
    }

    int64_t file_size = GetFileSize();
    if (file_size == -1) {
        return false;
    }

    const int kStep = 128 * 1024;
    int64_t retry_offset = file_size;
    int64_t offset = 0;
    do {
        retry_offset -= kStep;
        if (retry_offset < 0) {
            retry_offset = 0;
        }
    } while (!FindLastNthRecordOffset(retry_offset, n, &offset));

    return m_data_input->Seek(offset);
}

}  // namespace toft
