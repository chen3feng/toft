// Copyright 2013, Baidu Inc. All rights reserved.
// Author: Liu Cheng <liucheng02@baidu.com>
//
#include "toft/storage/seqfile/local_sequence_file_writer.h"

#include <netinet/in.h>
#include <string.h>

#include "glog/logging.h"
#include "toft/base/scoped_array.h"
#include "toft/storage/seqfile/local_sequence_file_reader.h"
#include "toft/system/time/timestamp.h"

static const char* kKeyClassName = "org.apache.hadoop.io.BytesWritable";
static const char* kValueClassName = "org.apache.hadoop.io.BytesWritable";

namespace toft {

LocalSequenceFileWriter::LocalSequenceFileWriter(File* file)
    : m_file(file),
      m_records_written_after_sync(0) {
}

LocalSequenceFileWriter::~LocalSequenceFileWriter() {
    if (NULL != m_file) {
        Close();
    }
}

bool LocalSequenceFileWriter::Init() {
    if (std::string::npos != m_file->mode().find('a') && TryingToLoadHeader()) {
        return true;
    }
    // generate sync first
    GenerateSync();

    // write header: sequence file version, compression info, etc..
    return WriteHeader();
}

bool LocalSequenceFileWriter::TryingToLoadHeader() {
    std::string file_path = m_file->file_path();
    toft::LocalFileSystem local;
    toft::File* file = local.Open(file_path, "r");
    CHECK_NOTNULL(file);
    toft::LocalSequenceFileReader reader(file);
    if (!reader.Init()) {
        return false;
    }
    if (!reader.has_sync_marker() || reader.is_compressed() || reader.is_block_compressed()) {
        return false;
    }
    std::string sync_marker = reader.sync_marker();
    m_key_class_name = reader.key_class_name();
    m_value_class_name = reader.value_class_name();
    m_version = reader.version();
    reader.Close();
    memcpy(m_sync, sync_marker.c_str(), sync_marker.size());
    return true;
}

bool LocalSequenceFileWriter::IsDefaultHeader() {
    return m_version == VERSION_WITH_METADATA
           && m_key_class_name == std::string(kKeyClassName)
           && m_value_class_name == std::string(kValueClassName);
}

bool LocalSequenceFileWriter::Close() {
    if (NULL == m_file) {
        return true;
    }
    bool ret = m_file->Close();

    delete m_file;
    m_file = NULL;
    return ret;
}

bool LocalSequenceFileWriter::WriteRecord(const std::string& key, const std::string& value) {
    StringPiece key_piece(key.c_str(), key.size());
    StringPiece value_piece(value.c_str(), value.size());
    return WriteRecord(key_piece, value_piece);
}

bool LocalSequenceFileWriter::WriteRecord(const StringPiece& key, const StringPiece& value) {
    int record_size = key.size() + value.size();
    int key_len = key.size();
    int int_len = sizeof(int);
    int size = 2 * int_len + record_size;
    int used_bytes = 0;
    scoped_array<char> data(new char[size]);

    if (m_records_written_after_sync >= kSyncInterval) {
        size += int_len + SYNC_HASH_SIZE;
        data.reset(new char[size]);
        used_bytes += WriteSyncToBuf(data.get() + used_bytes);
        m_records_written_after_sync = 0;
    }

    // format:
    // record_size|key_len|key|value

    used_bytes += WriteInt(data.get() + used_bytes, record_size);
    used_bytes += WriteInt(data.get() + used_bytes, key_len);

    memcpy(data.get() + used_bytes, key.data(), key_len);
    used_bytes += key_len;
    memcpy(data.get() + used_bytes, value.data(), value.size());
    used_bytes += value.size();

    CHECK(used_bytes == size);

    if (!Write(data.get(), used_bytes)) {
        return false;
    }

    ++m_records_written_after_sync;
    return true;
}

bool LocalSequenceFileWriter::Write(const char* addr, int len) {
    return len == m_file->Write(addr, len);
}


int LocalSequenceFileWriter::WriteSyncToBuf(char* buf) {
    int used_bytes = WriteInt(buf, kSyncEscape);
    used_bytes += WriteSync(buf + used_bytes);
    return used_bytes;
}

int LocalSequenceFileWriter::WriteInt(char* buf, int i) {
    int net_int = htonl(i);
    memcpy(buf, reinterpret_cast<char*>(&net_int), sizeof(net_int));
    return sizeof(net_int);
}

void LocalSequenceFileWriter::GenerateSync() {
    unsigned seed = GetTimestampInMs() + getpid();
    for (int i = 0; i < SYNC_HASH_SIZE; ++i) {
        m_sync[i] = static_cast<char>(rand_r(&seed) % 256);
    }
}

bool LocalSequenceFileWriter::WriteHeader() {
    scoped_array<char> header(new char[kMaxHeaderLen]);
    int used_bytes = 0;
    used_bytes += WriteSequenceFileVersion(header.get());
    used_bytes += WriteKeyValueClassName(header.get() + used_bytes);
    used_bytes += WriteCompressInfo(header.get() + used_bytes);
    used_bytes += WriteMetadata(header.get() + used_bytes);
    used_bytes += WriteSync(header.get() + used_bytes);
    CHECK_LE(used_bytes, kMaxHeaderLen);
    return Write(header.get(), used_bytes);
}

static const char* VERSION = "SEQ";
int LocalSequenceFileWriter::WriteSequenceFileVersion(char* addr) {
    addr[0] = VERSION[0];
    addr[1] = VERSION[1];
    addr[2] = VERSION[2];
    addr[3] = VERSION_WITH_METADATA;
    m_version = VERSION_WITH_METADATA;
    return 4;
}

int LocalSequenceFileWriter::WriteVLong(char* buf, int64_t i) {
    int used_bytes = 0;
    if (i >= -112 && i <= 127) {
        buf[used_bytes] = static_cast<char>(i);
        ++used_bytes;
        return used_bytes;
    }

    int len = -112;
    if (i < 0) {
        i ^= -1L; // take one's complement'
        len = -120;
    }

    int64_t tmp = i;
    while (tmp != 0) {
        tmp = tmp >> 8;
        len--;
    }

    buf[used_bytes] = static_cast<char>(len);
    ++used_bytes;

    len = (len < -120) ? -(len + 120) : -(len + 112);

    for (int idx = len; idx != 0; idx--) {
        int shiftbits = (idx - 1) * 8;
        int64_t mask = 0xFFL << shiftbits;
        char c = (static_cast<char>((i & mask) >> shiftbits));
        buf[used_bytes] = c;
        ++used_bytes;
    }
    return used_bytes;
}

int LocalSequenceFileWriter::WriteKeyValueClassName(char* addr) {
    int key_class_name_len = strlen(kKeyClassName);
    int value_class_name_len = strlen(kValueClassName);

    int used_bytes = WriteVLong(addr, key_class_name_len);
    memcpy(addr + used_bytes, kKeyClassName, key_class_name_len);
    used_bytes += key_class_name_len;

    used_bytes += WriteVLong(addr + used_bytes, value_class_name_len);
    memcpy(addr + used_bytes, kValueClassName, value_class_name_len);
    used_bytes += value_class_name_len;

    m_key_class_name = std::string(kKeyClassName);
    m_value_class_name = std::string(kValueClassName);
    return used_bytes;
}

int LocalSequenceFileWriter::WriteCompressInfo(char* addr) {
    // no need decompress
    addr[0] = '\0';

    // no need block decompress
    addr[1] = '\0';

    return 2;
}

int LocalSequenceFileWriter::WriteMetadata(char* addr) {
    // meta data num is 0
    *reinterpret_cast<int*>(addr) = 0;
    return 4;  // sizeof(int)
}

int LocalSequenceFileWriter::WriteSync(char* addr) {
    memcpy(addr, m_sync, SYNC_HASH_SIZE);
    return SYNC_HASH_SIZE;
}

}  // namespace toft
