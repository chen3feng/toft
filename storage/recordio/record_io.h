// Copyright (C) 2013, The Toft Authors.
// Author: An Qin (anqin.qin@gmail.com)
//
// Description:

#ifndef TOFT_STORAGE_RECORDIO_RECORD_IO_H
#define TOFT_STORAGE_RECORDIO_RECORD_IO_H

#include <string>

#include "toft/base/scoped_array.h"
#include "toft/storage/file/file.h"
#include "thirdparty/protobuf/message.h"

namespace toft {

class RecordWriter {
public:
    RecordWriter();
    ~RecordWriter();

    bool Reset(File *file);
    bool WriteMessage(const ::google::protobuf::Message& message);
    bool WriteRecord(const char *data, uint32_t size);
    bool WriteRecord(const std::string& data);

private:
    bool Write(const char *data, uint32_t size);

private:
    File* m_file;
};

class RecordReader {
public:
    RecordReader();
    ~RecordReader();

    bool Reset(File *file);

    // for ok, return 1;
    // for no more data, return 0;
    // for error, return -1;
    int Next();

    bool ReadMessage(::google::protobuf::Message *message);
    bool ReadNextMessage(::google::protobuf::Message *message);
    bool ReadRecord(const char **data, uint32_t *size);
    bool ReadRecord(std::string *data);

private:
    bool Read(char *data, uint32_t size);

private:
    File* m_file;
    scoped_array<char> m_buffer;
    uint32_t m_file_size;
    uint32_t m_buffer_size;
    uint32_t m_data_size;
};

} // namespace toft

#endif // TOFT_STORAGE_RECORDIO_RECORD_IO_H
