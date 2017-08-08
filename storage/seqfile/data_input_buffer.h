// Copyright 2013, Baidu Inc.
// Author: Liu Cheng <liucheng02@baidu.com>
//
#ifndef  TOFT_STORAGE_SEQFILE_DATA_INPUT_BUFFER_H
#define  TOFT_STORAGE_SEQFILE_DATA_INPUT_BUFFER_H

#include <netinet/in.h>

#include <algorithm>
#include <cstring>
#include <string>

#include "glog/logging.h"
#include "toft/storage/file/file.h"

namespace toft {

class DataInputBuffer {
public:
    DataInputBuffer(File* file, std::string* err)
        : m_file(file),
          m_err(err) {
        m_buffer_size = DEFAULT_BUFFER_SIZE;
        m_buffer = new char[m_buffer_size];

        m_offset = 0;
        m_size = 0;
        m_is_finished = false;
    }

    ~DataInputBuffer() {
        delete m_file;
        delete[] m_buffer;
        m_buffer = NULL;
        m_buffer_size = 0;
    }

    void Reset() {
        m_file->Close();

        m_offset = 0;
        m_size = 0;
        m_is_finished = false;
    }

    void ResetBuffer() {
        m_offset = 0;
        m_size = 0;
    }

    const char* Data() const { return m_buffer + m_offset; }

    int64_t Offset() const { return m_offset; }

    int64_t BufferSize() const { return m_buffer_size; }

    bool IsFinished() const { return m_is_finished; }

    int64_t Tell() const {
        int64_t file_offset = m_file->Tell();
        return file_offset - (m_size - m_offset);
    }

    bool LoadAtLeast(int64_t n) {
        // If buffer size is not large enough, enlarge by a factor of 2
        if (m_buffer_size < n) {
            int new_buffer_size = std::max(n, m_buffer_size * 2);
            char* buffer = new char[new_buffer_size];
            memcpy(buffer, m_buffer + m_offset, m_size - m_offset);

            delete[] m_buffer;
            m_buffer = buffer;

            m_buffer_size = new_buffer_size;
            m_size = m_size - m_offset;
            m_offset = 0;
        }

        // Loop until we have at least n bytes in buffer
        while (m_size - m_offset < n) {
            if (!LoadMoreContent()) {
                return false;
            }
        }
        return true;
    }

    bool LoadMoreContent() {
        // Ajust the data to the start addr of buffer
        if (m_offset != 0) {
            memmove(m_buffer, m_buffer + m_offset, m_size - m_offset);
            m_size = m_size - m_offset;
            m_offset = 0;
        }

        int empty_buffer_size = m_buffer_size - m_size;
        int64_t ret = m_file->Read(m_buffer + m_size, empty_buffer_size);
        if (ret <= 0) {
            *m_err = "Read SequenceFile Failed";
            m_is_finished = true;
            return false;
        }

        m_size = m_size + ret;
        return true;
    }

    bool ReadVersion(const char** version) {
        if (!LoadAtLeast(4)) {
            return false;
        }
        *version = m_buffer;
        m_offset += 4;
        return true;
    }

    bool ReadByte(char* c) {
        if (!LoadAtLeast(sizeof(char))) {  // NOLINT
            return false;
        }
        *c = m_buffer[m_offset];
        ++m_offset;
        return true;
    }

    bool ReadShort(short* s) {  // NOLINT
        if (!LoadAtLeast(sizeof(short))) {  // NOLINT
            return false;
        }

        *s = ntohl(*reinterpret_cast<short*>(m_buffer + m_offset));  // NOLINT
        m_offset += sizeof(short);  // NOLINT
        return true;
    }

    bool ReadInt(int* i) {
        if (!LoadAtLeast(sizeof(int))) {  // NOLINT
            return false;
        }

        *i = ntohl(*reinterpret_cast<int*>(m_buffer + m_offset));
        m_offset += sizeof(int);  // NOLINT
        return true;
    }

    bool ReadBuffer(char* buf, int size) {
        if (!LoadAtLeast(size)) {
            return false;
        }
        memcpy(buf, m_buffer + m_offset, size);
        m_offset += size;
        return true;
    }

    bool ReadString(std::string* s, int size) {
        if (!LoadAtLeast(size)) {
            return false;
        }
        s->assign(m_buffer + m_offset, size);
        m_offset += size;
        return true;
    }

    // Text in Hive means VInt + Chars (Length prefixed UTF-8 characters)
    bool ReadText(std::string* s) {
        int length = 0;
        if (!ReadVInt(&length)) {
            return false;
        }
        if (length < 0) {
            return false;
        }
        return ReadString(s, length);
    }

    bool ReadKeyValue(int key_len, int value_len, StringPiece* key, StringPiece* value) {
        if (!LoadAtLeast(key_len + value_len)) {
            return false;
        }

        key->set(m_buffer + m_offset, key_len);
        value->set(m_buffer + m_offset + key_len, value_len);
        m_offset += (key_len + value_len);
        return true;
    }

    bool Seek(int64_t offset) {
        if (!m_file->Seek(offset, SEEK_SET)) {
            *m_err = "Seek to offset failed";
            return false;
        }

        ResetBuffer();
        return true;
    }

    // FindSync, if sync found, skip sync_size
    bool FindSync(const char* sync, int sync_size) {
        while (m_offset + sync_size <= m_size) {
            if (memcmp(m_buffer + m_offset, sync, sync_size) == 0) {
                m_offset += sync_size;
                return true;
            }
            m_offset++;
        }
        return false;
    }

    int DecodeVIntSize(char value) {
        if (value >= -112) {
            return 1;
        } else if (value < -120) {
            return -119 - value;
        }
        return -111 - value;
    }

    bool IsNegativeVInt(char value) {
        return value < -120 || (value >= -112 && value < 0);
    }

    bool ReadVInt(int* value) {
        if (!LoadAtLeast(1)) {
            *m_err = "ReadVint at least size == 1";
            return false;
        }

        char first_byte = m_buffer[m_offset];
        m_offset++;

        int len = DecodeVIntSize(first_byte);
        if (len == 1) {
            *value = static_cast<int>(first_byte);
            return true;
        }

        if (!LoadAtLeast(len - 1)) {
            *m_err = "ReadVInt at least len";
            return false;
        }

        int64_t i = 0;
        for (int idx = 0; idx < len - 1; ++idx, ++m_offset) {
            char b = m_buffer[m_offset];
            i = i << 8;
            i = i | (b & 0xFF);
        }
        *value = static_cast<unsigned>((IsNegativeVInt(first_byte) ? (i ^ -1L) : i));
        return true;
    }

    int64_t GetFileSize() {
        // save the previous offset
        int64_t offset = m_file->Tell();

        // seek end to get the file size
        if (!m_file->Seek(0, SEEK_END)) {
            return -1;
        }
        int64_t size = m_file->Tell();

        // reset file offset to original
        if (!m_file->Seek(offset, SEEK_SET)) {
            return -1;
        }
        return size;
    }

    bool IsEof() { return m_file->IsEof(); }

private:
    File* m_file;

    // the buffer
    char* m_buffer;
    int64_t m_buffer_size;

    // buffered data size
    int64_t m_size;
    int64_t m_offset;

    bool m_is_finished;

    // error message
    std::string* m_err;

    static const int DEFAULT_BUFFER_SIZE = 65536;
};

}  // namespace toft

#endif  // TOFT_STORAGE_SEQFILE_DATA_INPUT_BUFFER_H
