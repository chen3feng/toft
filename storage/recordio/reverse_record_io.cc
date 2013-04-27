// Copyright (C) 2013, The Toft Authors.
// Author: An Qin (anqin.qin@gmail.com)
//
// Description:

#include "toft/storage/recordio/inv_record_io.h"

#include "thirdparty/glog/logging.h"

namespace toft {

InvRecordWriter::InvRecordWriter() {}

InvRecordWriter::~InvRecordWriter() {}

bool InvRecordWriter::Reset(File *file) {
    DCHECK(file != NULL);
    m_file = file;
    return true;
}

bool InvRecordWriter::WriteMessage(const ::google::protobuf::Message& message) {
    std::string output;
    if (!message.IsInitialized()) {
        LOG(WARNING) << "Missing required fields."
                     << message.InitializationErrorString();
        return false;
    }
    if (!message.AppendToString(&output)) {
        return false;
    }
    if (!WriteRecord(output.data(), output.size())) {
        return false;
    }
    return true;
}

bool InvRecordWriter::WriteRecord(const char *data, uint32_t size) {
    if (!Write(data, size)) {
        return false;
    }
    if (!Write(reinterpret_cast<char*>(&size), sizeof(size))) {
        return false;
    }
    return true;
}

bool InvRecordWriter::WriteRecord(const std::string& data) {
    return WriteRecord(data.data(), data.size());
}

bool InvRecordWriter::Write(const char *data, uint32_t size) {
    uint32_t write_size = 0;
    while (write_size < size) {
        int32_t ret = m_file->Write(data + write_size, size - write_size);
        if (ret == -1) {
            LOG(ERROR) << "InvRecordWriter error.";
            return false;
        }
        write_size += ret;
    }
    return true;
}


InvRecordReader::InvRecordReader()
    : m_buffer_size(1 * 1024 * 1024) {
    m_buffer.reset(new char[m_buffer_size]);
}

InvRecordReader::~InvRecordReader() {
}

bool InvRecordReader::Reset(File *file) {
    DCHECK(file != NULL);
    m_file = file;
    if (-1 == m_file->Seek(0, SEEK_END)) {
        LOG(ERROR) << "InvRecordReader Reset error.";
        return false;
    }
    return true;
}

int InvRecordReader::Next() {
    // read size
    int64_t ret = m_file->Tell();
    if (ret == -1) {
        LOG(ERROR) << "Tell error.";
        return -1;
    }

    if (ret == 0) {
        return 0;
    } else if (ret >= static_cast<int64_t>(sizeof(m_data_size))) { // NO_LINT
        if (!Read(reinterpret_cast<char*>(&m_data_size), sizeof(m_data_size))) {
            LOG(ERROR) << "Read size error.";
            return -1;
        }
    }

    // read data
    ret = m_file->Tell();
    if (ret == -1) {
        LOG(ERROR) << "Tell error.";
        return -1;
    }

    if (ret == 0 && ret != m_data_size) {
        LOG(ERROR) << "read error.";
        return -1;
    } else if (ret >= m_data_size) { // NO_LINT
        if (m_data_size > m_buffer_size) {
            while (m_data_size > m_buffer_size) {
                m_buffer_size *= 2;
            }
            m_buffer.reset(new char[m_buffer_size]);
        }

        if (!Read(m_buffer.get(), m_data_size)) {
            LOG(ERROR) << "Read data error.";
            return -1;
        }
    }

    return 1;
}

bool InvRecordReader::ReadMessage(::google::protobuf::Message *message) {
    std::string str(m_buffer.get(), m_data_size);
    if (!message->ParseFromArray(m_buffer.get(), m_data_size)) {
        LOG(WARNING) << "Missing required fields.";
        return false;
    }
    return true;
}

bool InvRecordReader::ReadNextMessage(::google::protobuf::Message *message) {
    while (Next() == 1) {
        std::string str(m_buffer.get(), m_data_size);
        if (message->ParseFromArray(m_buffer.get(), m_data_size)) {
            return true;
        }
    }
    return false;
}

bool InvRecordReader::ReadRecord(const char **data, uint32_t *size) {
    *data = m_buffer.get();
    *size = m_data_size;
    return true;
}

bool InvRecordReader::ReadRecord(std::string *data) {
    data->assign(m_buffer.get());
    return true;
}

bool InvRecordReader::Read(char *data, uint32_t size) {
    // After reading, file pointer need to seek back to the start position of
    // data. Prepare for next reading.
    // Seek back
    int32_t offset = 0 - size;
    if (-1 == m_file->Seek(offset, SEEK_CUR)) {
        LOG(ERROR) << "Seek error before read.";
        return false;
    }

    // Read
    uint32_t read_size = 0;
    while (read_size < size) {
        int64_t ret = m_file->Read(data + read_size, size - read_size);
        if (ret == -1) {
            LOG(ERROR) << "Read error.";
            return false;
        }
        read_size += ret;
    }

    // Seek back
    if (-1 == m_file->Seek(offset, SEEK_CUR)) {
        LOG(ERROR) << "Seek error after read.";
        return false;
    }

    return true;
}

} // namespace toft
