// Copyright 2013, For authors.
// Author: An Qin (anqin.qin@gmail.com)
// A FileSystem implementation for libhdfs.

#include "toft/storage/file/hdfs_file.h"

#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "thirdparty/glog/logging.h"
#include "toft/base/string/algorithm.h"
#include "toft/base/unique_ptr.h"
#include "toft/storage/file/uri_utils.h"
#include "toft/storage/path/path.h"
#include "toft/text/wildcard.h"

namespace toft {

using namespace std;

// change mode string to flags. Used in opening a hdfs file.
static bool ModeToFlags(const char* mode, int* flags) {
    // only read and write are supported
    if (!mode) {
        return false;
    }
    if (strcmp(mode, "r") == 0) {
        *flags = O_RDONLY;
        return true;
    }
    if (strcmp(mode, "w") == 0) {
        *flags = O_WRONLY;
        return true;
    }
    return false;
}


namespace {

class HDFSFileIterator : public FileIterator {
public:
    explicit HDFSFileIterator(hdfsFS fs, const std::string& dir,
                               const std::string& pattern,
                               int include_types, int exclude_types)
            : m_dir(dir), m_pattern(pattern),
            m_include_types(include_types), m_exclude_types(exclude_types) {
        m_infos = hdfsListDirectory(fs, dir.c_str(), &m_size);
        m_current_pos = 0;
    }
    ~HDFSFileIterator() {}

    bool GetNext(FileEntry* entry) {
        for (;; ++m_current_pos) {
            if (m_current_pos >= m_size || m_size == 0) {
                return false;
            }
            std::string full_file_name = m_infos[m_current_pos].mName;

            // NOTE: hdfsListDirectory will set full file name in mName, to keep the semantic
            // consistency with local file iterator, we will get the base file name
            std::string name = toft::Path::GetBaseName(full_file_name);
            if (strcmp(name.c_str(), ".") == 0 || strcmp(name.c_str(), "..") == 0) {
                continue;
            }

            int type = GetType(m_current_pos);
            if ((type & m_include_types) == 0)
                continue;
            if ((type & m_exclude_types) != 0)
                continue;

            if (!Wildcard::Match(m_pattern, name))
                continue;

            entry->type = type;
            entry->name = name;
            m_current_pos++;
            return true;
        }
    }

private:
    int GetType(int pos) const {
        tObjectKind hdfs_type = m_infos[pos].mKind;
        if (hdfs_type == kObjectKindFile) {
            return FileType_Regular;
        } else if (hdfs_type == kObjectKindDirectory) {
            return FileType_Directory;
        }
        return FileType_None;
    }

private:
    std::string m_dir;
    std::string m_pattern;
    int m_include_types;
    int m_exclude_types;

    hdfsFileInfo* m_infos;
    int m_current_pos;
    int m_size;
};

} // namespace


////////////////////////////////////////////////////////////////
// HDFS FS Cache Impl
HDFSFileSystem::HDFSFSCache::~HDFSFSCache() {
  for (HDFSFSMap::iterator i = fs_map_.begin(); i != fs_map_.end(); ++i) {
    int status = hdfsDisconnect(i->second);
    if (status != 0) {
      // TODO: add error details
      LOG(ERROR) << "hdfsDisconnect(\"" << i->first.first << "\", " << i->first.second
                 << ") failed: " << " Error(" << errno << "): " << strerror(errno);
    }
  }
}

hdfsFS HDFSFileSystem::HDFSFSCache::GetLocal() {
    return hdfsConnect(NULL, 0);
}

hdfsFS HDFSFileSystem::HDFSFSCache::GetConnection(const std::string& schema,
                                                  const std::string& host_port) {
    vector<string> split;
    if (!UriUtils::Explode(host_port, ':', &split) || split.size() < 2) {
        return NULL;
    }
    int port = atoi(split[1].c_str());
    string host = split[0];
    string fs_uri = schema + "://" + host_port;
    string schema_host = schema + "://" + host;

    HDFSFSMap::iterator i = fs_map_.find(std::make_pair(fs_uri, ""));
    if (i == fs_map_.end()) {
        hdfsFS conn = hdfsConnect(schema_host.c_str(), port);
        if (conn == NULL) {
            return NULL;
        }
        fs_map_.insert(std::make_pair(std::make_pair(fs_uri, ""), conn));
        return conn;
    } else {
        return i->second;
    }
}

hdfsFS HDFSFileSystem::HDFSFSCache::GetConnection(const std::string& schema,
                                                  const std::string& host_port,
                                                  const std::string& username,
                                                  const std::string& password) {
    vector<string> split;
    if (!UriUtils::Explode(host_port, ':', &split)) {
        return NULL;
    }
    int port = atoi(split[1].c_str());
    string host = split[0];
    string fs_uri = schema + "://" + host_port;
    string schema_host = schema + "://" + host;

    HDFSFSMap::iterator i = fs_map_.find(std::make_pair(fs_uri, username));
    if (i == fs_map_.end()) {
        hdfsFS conn = hdfsConnectAsUser(schema_host.c_str(), port,
                username.c_str(), password.c_str());
        if (conn == NULL) {
            return NULL;
        }
        fs_map_.insert(std::make_pair(std::make_pair(fs_uri, username), conn));
        return conn;
    } else {
        return i->second;
    }
}


/////////////////////////////////////////////////////////////////////////////
// HDFSFileSystem
//

const char* HDFSFileSystem::HDFS = "hdfs";

HDFSFileSystem::HDFSFileSystem() {
}

HDFSFileSystem::~HDFSFileSystem() {
}

hdfsFS HDFSFileSystem::GetHDFSFS(const std::string& file_path) {
    std::map<std::string, std::string> params;
    return GetHDFSFS(file_path, &params);
}


hdfsFS HDFSFileSystem::GetHDFSFS(const std::string& file_path,
        std::map<std::string, std::string>* params) {
    std::vector<std::string> sections;
    if (!UriUtils::Explode(file_path, '/', &sections)) {
        return NULL;
    }

    std::string schema = sections[0];

    // handle local files
    if (schema != "hdfs") {
       return m_fs_cache.GetLocal();
    }
    std::string cluster_name;

    if (!UriUtils::ParseParam(sections[1], &cluster_name, params)) {
        return NULL;
    }
    hdfsFS fs;
    // if there is username/password in params
    if (params->count("username") == 1) {
        std::map<std::string, std::string>::iterator it;
        it = params->find("username");
        std::string username = it->second;
        it = params->find("password");
        std::string password = it->second;
        if (it == params->end()) {
            return NULL;
        }
        fs = m_fs_cache.GetConnection(schema, cluster_name, username, password);
    } else {
        fs = m_fs_cache.GetConnection(schema, cluster_name);
    }
    return fs;
}

File* HDFSFileSystem::Open(const std::string& file_path, const char* mode) {
    std::map<std::string, std::string> params;
    hdfsFS fs = GetHDFSFS(file_path, &params);
    if (!fs) return NULL;
    int flags = 0;
    if (!ModeToFlags(mode, &flags)) {
        return NULL;
    }

    int bufferSize = HDFS_FS_DEFAULT_BUFFER_SIZE;
    int replication = HDFS_FS_DEFAULT_REPLICA;
    int blocksize = HDFS_FS_DEFAULT_BLOCKSIZE;
    if (params.count("bufferSize") == 1) {
        bufferSize = atoi(params["bufferSize"].c_str());
    }
    if (params.count("replication") == 1) {
        replication = atoi(params["replication"].c_str());
    }
    if (params.count("blocksize") == 1) {
        blocksize = atoi(params["blocksize"].c_str());
    }
    std::string shifted_path;
    if (!UriUtils::Shift(file_path, &shifted_path, 2, '/')) {
        return NULL;
    }
    hdfsFile file = hdfsOpenFile(fs, shifted_path.c_str(), flags,
                                 bufferSize, replication, blocksize);
    if (!file) {
        return NULL;
    }
    return new HDFSFile(file, fs, file_path, mode);
}

bool HDFSFileSystem::Exists(const std::string& file_path) {
    hdfsFS fs = GetHDFSFS(file_path);
    std::string shifted_path;
    if (!UriUtils::Shift(file_path, &shifted_path, 2, '/')) {
        return false;
    }
    if (!fs) return false;
    return hdfsExists(fs, shifted_path.c_str()) == 0;
}

bool HDFSFileSystem::Delete(const std::string& file_path) {
    hdfsFS fs = GetHDFSFS(file_path);
    std::string shifted_path;
    if (!UriUtils::Shift(file_path, &shifted_path, 2, '/')) {
        return false;
    }
    if (!fs) return false;
    return hdfsDelete(fs, shifted_path.c_str()) == 0;
}

bool HDFSFileSystem::Rename(const std::string& from, const std::string& to) {
    hdfsFS from_fs = GetHDFSFS(from);
    hdfsFS to_fs = GetHDFSFS(from);
    if (from_fs != to_fs) {
        return false;
    }
    if (!from_fs) return false;
    std::string shifted_from;
    if (!UriUtils::Shift(from, &shifted_from, 2, '/')) {
        return false;
    }
    std::string shifted_to;
    if (!UriUtils::Shift(to, &shifted_to, 2, '/')) {
        return false;
    }
    return (hdfsRename(from_fs, shifted_from.c_str(), shifted_to.c_str()) == 0);
}

bool HDFSFileSystem::GetTimes(const std::string& file_path, FileTimes* times) {
    hdfsFS fs = GetHDFSFS(file_path);
    if (!fs) return false;
    std::string shifted_path;
    if (!UriUtils::Shift(file_path, &shifted_path, 2, '/')) {
        return false;
    }
    hdfsFileInfo* info = hdfsGetPathInfo(fs, shifted_path.c_str());
    if (!info) {
        return false;
    }
    times->access_time = info->mLastAccess;
    times->modify_time = info->mLastMod;
    hdfsFreeFileInfo(info, 1);
    return true;
}

int64_t HDFSFileSystem::GetSize(const std::string& file_path) {
    hdfsFS fs = GetHDFSFS(file_path);
    if (!fs) return -1;

    std::string shifted_path;
    if (!UriUtils::Shift(file_path, &shifted_path, 2, '/')) {
        return -1;
    }

    int64_t file_size = -1;
    hdfsFileInfo* info = hdfsGetPathInfo(fs, shifted_path.c_str());
    if (info) {
        file_size = info->mSize;
        hdfsFreeFileInfo(info, 1);
    }
    return file_size;
}

FileIterator* HDFSFileSystem::Iterate(const std::string& dir,
                              const std::string& pattern,
                              int include_types,
                              int exclude_types) {
    hdfsFS fs = GetHDFSFS(dir);
    if (!fs) return NULL;
    std::string shifted_path;
    if (!UriUtils::Shift(dir, &shifted_path, 2, '/')) {
        return NULL;
    }
    return new HDFSFileIterator(fs, shifted_path, pattern, include_types, exclude_types);
}

bool HDFSFileSystem::Mkdir(const std::string& dir, int mode) {
    hdfsFS fs = GetHDFSFS(dir);
    if (!fs) return false;
    std::string shifted_path;
    if (!UriUtils::Shift(dir, &shifted_path, 2, '/')) {
        return false;
    }
    return hdfsCreateDirectory(fs, shifted_path.c_str());
}

bool HDFSFileSystem::Rmdir(const std::string& dir) {
    hdfsFS fs = GetHDFSFS(dir);
    if (!fs) return false;
    std::string shifted_path;
    if (!UriUtils::Shift(dir, &shifted_path, 2, '/')) {
        return false;
    }
    return hdfsDelete(fs, shifted_path.c_str());
}

TOFT_REGISTER_FILE_SYSTEM("hdfs", HDFSFileSystem);

///////////////////////////////////////////////////////////////////////////////////////////
//  HDFSFile Implementation
HDFSFile::HDFSFile(hdfsFile file, hdfsFS fs, const std::string& file_path, const char* mode)
        : File(file_path, mode),
        m_file(file),
        m_fs(fs),
        m_line_reader(NULL),
        m_iseof(false),
        m_closed(false) {
}

HDFSFile::~HDFSFile() {
    if (!m_closed) {
        Close();
        m_closed = true;
    }
}

int64_t HDFSFile::Read(void* buffer, int64_t size) {
    int64_t read_size = hdfsRead(m_fs, m_file, buffer, size);
    if (read_size != size) {
        m_iseof = (Tell() == File::GetSize(file_path()));
    }
    return read_size;
}
int64_t HDFSFile::Write(const void* buffer, int64_t size) {
    return hdfsWrite(m_fs, m_file, buffer, size);
}
bool HDFSFile::Flush() {
    return hdfsFlush(m_fs, m_file) == 0;
}

bool HDFSFile::Close() {
    m_closed = true;
    if (m_line_reader) {
        closeLineReader(m_line_reader);
    }
    return hdfsCloseFile(m_fs, m_file) == 0;
}

bool HDFSFile::Seek(int64_t offset, int whence) {
    if (whence != SEEK_SET) {
        return false;
    }
    return hdfsSeek(m_fs, m_file, offset) == 0;
}
int64_t HDFSFile::Tell() {
    return hdfsTell(m_fs, m_file);
}
bool HDFSFile::ReadLine(std::string* line, size_t max_size) {
    if (!m_line_reader) {
        m_line_reader = createLineReader(m_file);
    }
    if (!m_line_reader) {
        return false;
    }

    void* raw_line;
    int n_bytes = readLineByLineReader(m_fs, m_line_reader, &raw_line);
    if (-1 == n_bytes) m_iseof = true;

    if (!(n_bytes > 0) || !(static_cast<size_t>(n_bytes) < max_size)){
        return false;
    }
    line->assign(reinterpret_cast<char*>(raw_line), n_bytes);
    return true;
}

bool HDFSFile::ReadLineWithLineEnding(std::string* line, size_t max_size) {
    return false;
}

bool HDFSFile::IsEof() {
    return m_iseof;
}


} // namespace toft
