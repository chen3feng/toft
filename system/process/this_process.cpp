// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/system/process/this_process.h"

#include "toft/base/string/algorithm.h"
#include "toft/base/string/number.h"
#include "toft/storage/file/file.h"
#include "toft/storage/path/path.h"
#include "toft/system/process/process_local_const.h"

#include "thirdparty/glog/logging.h"

namespace toft {

static std::string ReadField(const char* proc_file, unsigned int field_pos)
{
    std::string data;
    if (File::ReadAll(proc_file, &data)) {
        std::vector<std::string> fields;
        SplitString(data, " ",  &fields);
        CHECK_LT(field_pos, fields.size());
        return fields[field_pos];
    } else {
        PLOG(ERROR) << "Read " << proc_file << " error";
        return "";
    }
}

static time_t GetStartTime()
{
    int hz = sysconf(_SC_CLK_TCK);
    double uptime;
    if (!StringToNumber(ReadField("/proc/uptime", 0), &uptime))
        return -1;
    double start_time;
    if (!StringToNumber(ReadField("/proc/self/stat", 21), &start_time))
        return -1;

    return static_cast<time_t>(time(NULL) - uptime + start_time / hz);
}

pid_t ThisProcess::GetId()
{
    return getpid();
}

time_t ThisProcess::StartTime()
{
    // Start time is a const for a given process so can be cached to avoid
    // obtain repately, but should be updated after fork.
    static toft::ProcessLocalConst<time_t> s_start_time(GetStartTime);
    return s_start_time.Value();
}

time_t ThisProcess::ElapsedTime()
{
    time_t start_time = ThisProcess::StartTime();
    if (start_time < 0)
        return -1;
    return time(NULL) - start_time;
}

std::string ThisProcess::BinaryPath()
{
    char path[PATH_MAX] = {0};
    ssize_t length = readlink("/proc/self/exe", path, sizeof(path));
    if (length > 0)
        return std::string(path, length);
    return std::string();
}

std::string ThisProcess::BinaryName()
{
    std::string binary_name;

    char path[PATH_MAX] = {0};
    FILE* fp = fopen("/proc/self/cmdline", "r");
    if (fp != NULL) {
        if (fgets(path, sizeof(path) - 1, fp))
            binary_name = Path::GetBaseName(path);
        fclose(fp);
    }
    // If fopen failed or the process is defunct,
    // read binary name from exe softlink.
    if (binary_name.empty()) {
        binary_name = Path::GetBaseName(BinaryPath());
    }
    return binary_name;
}

std::string ThisProcess::BinaryDirectory()
{
    std::string path = BinaryPath();
    return Path::GetDirectory(path);
}

} // namespace toft
