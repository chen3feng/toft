// Copyright (C) 2017, For authors.
// Author: An Qin (anqin.qin@gmail.com)
//
// Description:

#include "toft/storage/file/afs_file.h"

#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "thirdparty/glog/logging.h"
#include "thirdparty/gflags/gflags.h"
#include "toft/base/string/algorithm.h"
#include "toft/base/unique_ptr.h"
#include "toft/storage/file/uri_utils.h"
#include "toft/storage/path/path.h"
#include "toft/text/wildcard.h"

DEFINE_string(afs_log_level, "2", "");

namespace toft {

// change mode string to flags. Used in opening a afs file.
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

static inline dfs::FileType GetFileTypeFromInode(uint64_t id) {
    return static_cast<dfs::FileType>((id >> 49) & 0x0000000000000006);
}

class AFSFileIterator : public FileIterator {
public:
    explicit AFSFileIterator(AfsFS fs, const std::string& dir,
                             const std::string& pattern,
                             int include_types, int exclude_types)
        : m_dir(dir), m_pattern(pattern),
          m_include_types(include_types), m_exclude_types(exclude_types) {
        fs->Readdir(dir.c_str(), &m_infos);
        m_current_pos = 0;
    }
    ~AFSFileIterator() {}

    bool GetNext(FileEntry* entry) {
        for (;; ++m_current_pos) {
            if (static_cast<uint32_t>(m_current_pos) >= m_infos.size()
                || m_infos.size() == 0) {
                return false;
            }
            std::string name = m_infos[m_current_pos].name;
            if (strcmp(name.c_str(), ".") == 0 || strcmp(name.c_str(), "..") == 0) {
                continue;
            }

            int type = GetType(m_current_pos);
            if ((type & m_include_types) == 0)
                continue;
            if ((type & m_exclude_types) != 0)
                continue;

            if (!Wildcard::Match(m_pattern, name)) {
                continue;
            }

            entry->type = type;
            entry->name = name;
            m_current_pos++;
            return true;
        }
    }

private:
    int GetType(int pos) const {
        dfs::FileType ft = GetFileTypeFromInode(m_infos[pos].inode);
        if (ft == dfs::FT_REGULAR) {
            return FileType_Regular;
        } else if (ft == dfs::FT_DIRECTORY) {
            return FileType_Directory;
        }
        return FileType_None;
    }

private:
    std::string m_dir;
    std::string m_pattern;
    int m_include_types;
    int m_exclude_types;

    std::vector<dfs::DirEntry> m_infos;
    int m_current_pos;
};

} // namespace


////////////////////////////////////////////////////////////////
// AFS FS Cache Impl
AFSFileSystem::AFSFSCache::~AFSFSCache() {
    for (AFSFSMap::iterator i = fs_map_.begin(); i != fs_map_.end(); ++i) {
        int32_t status = i->second->DisConnect();
        if (status != 0) {
          // TODO: add error details
          LOG(ERROR) << "afsDisconnect(\"" << i->first.first
              << "\", " << i->first.second << ") failed: "
              << " Error(" << status << ") ";
        }
        delete i->second;
    }
}

AfsFS AFSFileSystem::AFSFSCache::GetLocal() {
    return NULL;
}

AfsFS AFSFileSystem::AFSFSCache::GetConnection(const std::string& uri) {
    return NULL;
}

AfsFS AFSFileSystem::AFSFSCache::GetConnection(const std::string& uri,
                                               const std::string& username,
                                               const std::string& password) {
    AFSFSMap::iterator i = fs_map_.find(std::make_pair(uri, username));
    if (i == fs_map_.end()) {
        AfsFS conn = new dfs::AfsFileSystem(uri.c_str(), username.c_str(),
                                            password.c_str(), NULL);
        if (conn == NULL) {
            return NULL;
        }

        conn->SetConfigStr("hadoop.log.level", FLAGS_afs_log_level.c_str());
        if (conn->Connect() != 0) {
            delete conn;
            return NULL;
        }
        fs_map_.insert(std::make_pair(std::make_pair(uri, username), conn));
        return conn;
    } else {
        return i->second;
    }
}


/////////////////////////////////////////////////////////////////////////////
// AFSFileSystem
//

const char* AFSFileSystem::AFS  = "afs";

AFSFileSystem::AFSFileSystem() {
}

AFSFileSystem::~AFSFileSystem() {
}

AfsFS AFSFileSystem::GetAFSFS(const std::string& file_path) {
    std::map<std::string, std::string> params;
    return GetAFSFS(file_path, &params);
}

AfsFS AFSFileSystem::GetAFSFS(const std::string& file_path,
        std::map<std::string, std::string>* params) {
    std::vector<std::string> sections;
    if (!UriUtils::Explode(file_path, '/', &sections)) {
        return NULL;
    }

    std::string schema = sections[0];

    // handle local files
    if (schema != "afs") {
       return m_fs_cache.GetLocal();
    }
    std::string cluster_name;

    if (!UriUtils::ParseParam(sections[1], &cluster_name, params)) {
        return NULL;
    }

    AfsFS fs;
    std::string uri = schema + "://" + sections[1];
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
        fs = m_fs_cache.GetConnection(uri, username, password);
    } else {
        fs = m_fs_cache.GetConnection(uri);
    }
    return fs;
}

File* AFSFileSystem::Open(const std::string& file_path, const char* mode) {
    std::map<std::string, std::string> params;
    AfsFS fs = GetAFSFS(file_path, &params);
    if (!fs) {
        return NULL;
    }
    int flags = 0;
    if (!ModeToFlags(mode, &flags)) {
        return NULL;
    }

    std::string shifted_path;
    if (!UriUtils::Shift(file_path, &shifted_path, 2, '/')) {
        return NULL;
    }

    AfsReader* reader = NULL;
    AfsWriter* writer = NULL;
    if (flags == O_WRONLY) {
        int32_t ret_code = fs->Create(shifted_path.c_str());
        if (ret_code != dfs::kExist && ret_code != dfs::kOk) {
            LOG(ERROR) << "Create file failed, errno: " << ret_code;
            return NULL;
        }
        writer = fs->OpenWriter(shifted_path.c_str());
        if (!writer) {
            LOG(ERROR) << "Open file for write failed, errno: "
                << dfs::GetErrno();
            return NULL;
        }
    }

    reader = fs->OpenReader(shifted_path.c_str());
    if (!reader) {
        LOG(ERROR) << "Open file for read failed, errno: "
            << dfs::GetErrno();
        return NULL;
    }
    return new AFSFile(reader, writer, fs, file_path, mode);
}

bool AFSFileSystem::Exists(const std::string& file_path) {
    AfsFS fs = GetAFSFS(file_path);
    std::string shifted_path;
    if (!UriUtils::Shift(file_path, &shifted_path, 2, '/')) {
        return false;
    }
    if (!fs) {
        return false;
    }
    return fs->Exist(shifted_path.c_str()) == 0;
}

bool AFSFileSystem::Delete(const std::string& file_path) {
    AfsFS fs = GetAFSFS(file_path);
    std::string shifted_path;
    if (!UriUtils::Shift(file_path, &shifted_path, 2, '/')) {
        return false;
    }
    if (!fs) {
        return false;
    }
    return fs->Delete(shifted_path.c_str()) == 0;
}

bool AFSFileSystem::Rename(const std::string& from, const std::string& to) {
    AfsFS from_fs = GetAFSFS(from);
    AfsFS to_fs = GetAFSFS(to);
    if (from_fs != to_fs) {
        return false;
    }
    if (!from_fs) {
        return false;
    }
    std::string shifted_from;
    if (!UriUtils::Shift(from, &shifted_from, 2, '/')) {
        return false;
    }
    std::string shifted_to;
    if (!UriUtils::Shift(to, &shifted_to, 2, '/')) {
        return false;
    }
    return from_fs->Rename(shifted_from.c_str(), shifted_to.c_str()) == 0;
}

bool AFSFileSystem::GetTimes(const std::string& file_path, FileTimes* times) {
    AfsFS fs = GetAFSFS(file_path);
    if (!fs) {
        return false;
    }
    std::string shifted_path;
    if (!UriUtils::Shift(file_path, &shifted_path, 2, '/')) {
        return false;
    }
    struct stat info;
    int status = fs->Stat(shifted_path.c_str(), &info);
    if (status != 0) {
        return false;
    }
    times->access_time = info.st_atime;
    times->modify_time = info.st_mtime;
    return true;
}

int64_t AFSFileSystem::GetSize(const std::string& file_path) {
    AfsFS fs = GetAFSFS(file_path);
    if (!fs) return -1;

    std::string shifted_path;
    if (!UriUtils::Shift(file_path, &shifted_path, 2, '/')) {
        return -1;
    }

    struct stat info;
    int status = fs->Stat(shifted_path.c_str(), &info);
    if (status != 0) {
        return -1;
    }
    int64_t file_size = info.st_size;
    return file_size;
}

FileIterator* AFSFileSystem::Iterate(const std::string& dir,
                              const std::string& pattern,
                              int include_types,
                              int exclude_types) {
    AfsFS fs = GetAFSFS(dir);
    if (!fs) return NULL;
    std::string shifted_path;
    if (!UriUtils::Shift(dir, &shifted_path, 2, '/')) {
        return NULL;
    }
    return new AFSFileIterator(fs, shifted_path, pattern,
                               include_types, exclude_types);
}

bool AFSFileSystem::Mkdir(const std::string& dir, int mode) {
    AfsFS fs = GetAFSFS(dir);
    if (!fs) {
        return false;
    }
    std::string shifted_path;
    if (!UriUtils::Shift(dir, &shifted_path, 2, '/')) {
        return false;
    }
    return fs->Mkdir(shifted_path.c_str()) == 0;
}

bool AFSFileSystem::Rmdir(const std::string& dir) {
    AfsFS fs = GetAFSFS(dir);
    if (!fs) {
        return false;
    }
    std::string shifted_path;
    if (!UriUtils::Shift(dir, &shifted_path, 2, '/')) {
        return false;
    }
    return fs->Delete(shifted_path.c_str(), true) == 0;
}

TOFT_REGISTER_FILE_SYSTEM("afs", AFSFileSystem);

///////////////////////////////////////////////////////////////////////////////////////////
//  AFSFile Implementation
AFSFile::AFSFile(AfsReader* reader, AfsWriter* writer, AfsFS fs,
                 const std::string& file_path, const char* mode)
        : File(file_path, mode),
        m_reader(reader),
        m_writer(writer),
        m_fs(fs),
        m_iseof(false),
        m_closed(false) {
}

AFSFile::~AFSFile() {
    if (!m_closed) {
        Close();
        m_closed = true;
    }
}

int64_t AFSFile::Read(void* buffer, int64_t size) {
    if (!m_reader) {
        LOG(ERROR) << "invalid operation for read on wirter handle";
        return -1;
    }
    int64_t read_size = m_reader->Read(buffer, size);
    if (read_size != size) {
        m_iseof = (Tell() == File::GetSize(file_path()));
    }
    return read_size;
}

int64_t AFSFile::Write(const void* buffer, int64_t size) {
    if (!m_writer) {
        LOG(ERROR) << "invalid operation for write on reader handle";
        return -1;
    }
    return m_writer->Append(buffer, size);
}

bool AFSFile::Flush() {
    return m_writer->Flush() == 0;
}

bool AFSFile::Close() {
    m_closed = true;
    int32_t ret = 0;
    if (m_reader) {
        ret |= m_fs->CloseReader(m_reader);
    }
    if (m_writer) {
        ret |= m_fs->CloseWriter(m_writer);
    }
    return ret == 0;
}

bool AFSFile::Seek(int64_t offset, int whence) {
    if (whence != SEEK_SET) {
        return false;
    }
    if (!m_reader) {
        LOG(ERROR) << "invalid operation for seek on wirter handle";
        return -1;
    }
    m_reader->Seek(offset);
    return true;
}

int64_t AFSFile::Tell() {
    if (m_writer) {
        return m_writer->Tell();
    }
    if (!m_reader) {
        LOG(ERROR) << "invalid operation for seek on wirter handle";
        return -1;
    }
    return m_reader->Tell();
}

bool AFSFile::ReadLine(std::string* line, size_t max_size) {
    return false;
}

bool AFSFile::ReadLineWithLineEnding(std::string* line, size_t max_size) {
    return false;
}

bool AFSFile::IsEof() {
    return m_iseof;
}

} // namespace toft
