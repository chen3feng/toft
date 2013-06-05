// Copyright (c) 2011, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#include "toft/system/process/sub_process.h"

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>

#include "toft/base/string/algorithm.h"
#include "toft/encoding/shell.h"
#include "toft/system/eintr_ignored.h"
#include "toft/system/time/clock.h"

#include "thirdparty/glog/logging.h"

namespace toft {

SubProcess::CreateOptions::CreateOptions() :
    m_replace_envs(false),
    m_stdin_fd(-1),
    m_stdout_fd(-1),
    m_stderr_fd(-1),
    m_shell(false),
    m_close_fds(false)
{
}

void SubProcess::CreateOptions::ClearEnvironments()
{
    m_envs.clear();
    m_replace_envs = true;
}

void SubProcess::CreateOptions::SetEnvironments(const std::map<std::string, std::string>& envs)
{
    m_envs = envs;
    m_replace_envs = true;
}

void SubProcess::CreateOptions::AddEnvironment(const char* name, const char* value)
{
    m_envs[name] = value;
}

void SubProcess::CreateOptions::RedirectStdInput(int fd)
{
    m_stdin_fd = fd;
}

void SubProcess::CreateOptions::RedirectStdOutput(int fd)
{
    m_stdout_fd = fd;
}

void SubProcess::CreateOptions::RedirectStdError(int fd)
{
    m_stderr_fd = fd;
}

void SubProcess::CreateOptions::EnableShell()
{
    m_shell = true;
}

void SubProcess::CreateOptions::SetWorkDirectory(const std::string& cwd)
{
    m_cwd = cwd;
}

void SubProcess::CreateOptions::CloseFds()
{
    m_close_fds = true;
}

SubProcess::SubProcess() : m_pid(0), m_exit_code(0)
{
}

SubProcess::~SubProcess()
{
    if (IsValid()) {
        WaitForExit();
    }
}

pid_t SubProcess::Id() const
{
    return m_pid;
}

bool SubProcess::IsValid() const
{
    return m_pid > 0;
}

static void StringVectorToCStringVector(
    const std::vector<std::string>& vs,
    std::vector<const char*>* vcs)
{
    for (size_t i = 0; i < vs.size(); ++i) {
        vcs->push_back(const_cast<char*>(vs[i].c_str()));
    }
    vcs->push_back(NULL);
}

static void SetArgWithShell(const std::string& cmdline,
                            std::vector<const char*>* cargs)
{
    cargs->push_back("/bin/sh");
    cargs->push_back("-c");
    cargs->push_back(cmdline.c_str());
    cargs->push_back(NULL);
}

bool SubProcess::Create(const std::string& cmdline, const CreateOptions& options)
{
    std::vector<const char*> cargs;
    std::vector<std::string> args;
    if (options.m_shell) {
        SetArgWithShell(cmdline, &cargs);
    } else {
        if (!SplitCommandLine(cmdline, &args)) {
            LOG(ERROR) << "Can't Parse command: " << cmdline;
            return false;
        }
        StringVectorToCStringVector(args, &cargs);
    }
    return Create(&cargs[0], options);
}

bool SubProcess::Create(const std::vector<std::string>& args,
                        const CreateOptions& options)
{
    std::vector<const char*> cargs;
    std::string command;
    if (options.m_shell) {
        command = JoinCommandLine(args);
        SetArgWithShell(command, &cargs);
    } else {
        StringVectorToCStringVector(args, &cargs);
    }
    return Create(&cargs[0], options);
}

static void GetAllEnvironments(std::map<std::string, std::string>* envs)
{
    envs->clear();
    int i = 0;
    while (environ[i]) {
        char* env = environ[i];
        char* p = strchr(env, '=');
        if (p) {
            (*envs)[std::string(env, p)] = p + 1;
        }
        ++i;
    }
}

static void AppendEnvironments(std::map<std::string, std::string>* envs,
                        const std::map<std::string, std::string>& append,
                        bool overwrite)
{
    if (overwrite) {
        for (std::map<std::string, std::string>::const_iterator i = append.begin();
             i != append.end(); ++i) {
            (*envs)[i->first] = i->second;
        }
    } else {
        envs->insert(append.begin(), append.end());
    }
}

static void EnvironmentsToStringVector(
    const std::map<std::string, std::string>& envs,
    std::vector<std::string>* vs)
{
    for (std::map<std::string, std::string>::const_iterator i = envs.begin();
         i != envs.end(); ++i) {
        vs->push_back(i->first + "=" + i->second);
    }
}

static bool SetCloexec(int fd)
{
    int flags = fcntl(fd, F_GETFD, 0);
    if (flags < 0)
        return false;
    flags |= FD_CLOEXEC;
    return fcntl(fd, F_SETFD, flags) >= 0;
}

void SubProcess::DoExec(const char* const* args,
                        const CreateOptions& options,
                        int pipe_write_fd)
{
    // Child process
    std::map<std::string, std::string> envs;
    std::vector<std::string> venvs;
    std::vector<const char*> cenvs;
    char** env = ::environ;
    if (options.m_replace_envs || !options.m_envs.empty()) {
        if (!options.m_replace_envs)
            GetAllEnvironments(&envs);
        AppendEnvironments(&envs, options.m_envs, true);
        EnvironmentsToStringVector(envs, &venvs);
        StringVectorToCStringVector(venvs, &cenvs);
        env = const_cast<char**>(&cenvs[0]);
    }

    if (options.m_stdin_fd != -1) {
        dup2(options.m_stdin_fd, STDIN_FILENO);
    }
    if (options.m_stdout_fd != -1) {
        dup2(options.m_stdout_fd, STDOUT_FILENO);
    }
    if (options.m_stderr_fd != -1) {
        dup2(options.m_stderr_fd, STDERR_FILENO);
    }

    if (!options.m_cwd.empty()) {
        if (chdir(options.m_cwd.c_str()) != 0) {
            PLOG(ERROR) << "Can't chdir to " << options.m_cwd.c_str();
        }
    }

    if (options.m_close_fds) {
        int table_size = getdtablesize();
        for (int i = 3; i < table_size; ++i) {
            if (i != pipe_write_fd)
                close(i);
        }
    }

    execvpe(args[0], const_cast<char**>(args), env);
    int error = errno;
    PLOG(ERROR) << "Failed to execvpe " << args[0];

    // Notify parent process.
    if (write(pipe_write_fd, &error, sizeof(error)) != sizeof(error)) {
        PLOG(ERROR) << "Write to pipe error";
    }
    close(pipe_write_fd);
    _Exit(127);
}

bool SubProcess::Create(const char* const* args, const CreateOptions& options)
{
    CHECK_LE(m_pid, 0) << "Already running or not waited";

    int fds[2]; // To pass the error of execvpe.
    if (pipe(fds) < 0) {
        PLOG(ERROR) << "Can't create pipe";
        return false;
    }
    int pipe_read_fd = fds[0];
    int pipe_write_fd = fds[1];
    SetCloexec(pipe_read_fd);
    SetCloexec(pipe_write_fd);

    pid_t pid = fork();
    CHECK_GE(pid, 0) << "Fork child process " << args[0] << " error";
    if (pid == 0) {
        close(pipe_read_fd);
        DoExec(args, options, pipe_write_fd);
    } else {
        close(pipe_write_fd);
        LOG(INFO) << "Start process " << args[0] << ", pid " << pid;
        int error = 0;
        int nread = read(pipe_read_fd, &error, sizeof(error));
        close(pipe_read_fd);
        if (nread < 0) {
            PLOG(ERROR) << "Error reading from pipe_read_fd";
            return false;
        }
        if (nread == sizeof(error)) {
            LOG(ERROR) << "Start process " << args[0] << " error: " << error
                       << ", " << strerror(error);
            TOFT_EINTR_IGNORED(waitpid(pid, NULL, 0));
            return false;
        }
        m_pid = pid;
        m_exit_code = 0;
    }
    return true;
}

bool SubProcess::Terminate()
{
    CHECK_GT(m_pid, 1) << "Invalid pid";

    // First we use SIGTERM to kill the process.
    if (kill(m_pid, SIGTERM) < 0) {
        PLOG(ERROR) << "Unable to terminate process: " << m_pid;
        return false;
    }

    if (TimedWaitForExit(60 * 1000))
        return true;

    // If can't terminate it graceful, kill it rudely.
    if (kill(m_pid, SIGKILL) < 0) {
        PLOG(ERROR) << "Unable to terminate process: " << m_pid;
        return false;
    }

    return WaitForExit();
}

bool SubProcess::SendSignal(int signal)
{
    CHECK_GT(m_pid, 1) << "Invalid pid";
    return kill(m_pid, signal) == 0;
}

bool SubProcess::IsAlive() const
{
    if (m_pid <= 0)
        return false;
    return kill(m_pid, 0) == 0;
}

bool SubProcess::WaitForExit()
{
    if (m_pid < 0)
        return true;

    int status;
    if (TOFT_EINTR_IGNORED(waitpid(m_pid, &status, 0)) == -1) {
        PLOG(ERROR) << "Wait for exit failure: " << m_pid;
        return false;
    }
    return UpdateExitStatus(status);
}

bool SubProcess::TimedWaitForExit(int64_t timeout_milliseconds)
{
    if (m_pid < 0)
        return true;

    int64_t end_time = RealtimeClock.MilliSeconds() + timeout_milliseconds;

    for (;;) {
        int status = -1;
        pid_t pid = TOFT_EINTR_IGNORED(waitpid(m_pid, &status, WNOHANG));
        if (pid < 0) {
            PLOG(ERROR) << "Error waiting pid: " << m_pid;
            return false;
        }
        if (pid > 0)
            return UpdateExitStatus(status);

        int64_t now = RealtimeClock.MilliSeconds();

        int64_t sleep_time = end_time - now;
        if (sleep_time <= 0)
            return false;
        if (sleep_time > 10)
            sleep_time = 10;
        usleep(sleep_time * 1000);
    }
}

bool SubProcess::UpdateExitStatus(int status)
{
    if (WIFSIGNALED(status)) {
        m_pid = -m_pid;
        m_exit_code = -WTERMSIG(status);
        return true;
    }

    if (WIFEXITED(status)) {
        m_pid = -m_pid;
        m_exit_code = WEXITSTATUS(status);
        return true;
    }

    return false;
}

int SubProcess::ExitCode() const
{
    CHECK_LT(m_pid, 0);
    return m_exit_code;
}

} // namespace toft

