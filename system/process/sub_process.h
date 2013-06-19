// Copyright (c) 2010, The Toft Authors.
// All rights reserved.
//
// Author: Chen Feng <chen3feng@gmail.com>

#ifndef TOFT_SYSTEM_PROCESS_SUB_PROCESS_H
#define TOFT_SYSTEM_PROCESS_SUB_PROCESS_H

#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <map>
#include <string>
#include <vector>

#include "toft/base/uncopyable.h"

namespace toft {

class SubProcess
{
    TOFT_DECLARE_UNCOPYABLE(SubProcess);

public:
    class CreateOptions {
        friend class SubProcess;
    public:
        CreateOptions();
        void ClearEnvironments();
        void SetEnvironments(const std::map<std::string, std::string>& env);
        void AddEnvironment(const char* name, const char* value);
        void RedirectStdInput(int fd);
        void RedirectStdOutput(int fd);
        void RedirectStdError(int fd);
        void EnableShell();
        void SetWorkDirectory(const std::string& cwd);
        void CloseFds();
    private:
        std::map<std::string, std::string> m_envs;
        bool m_replace_envs;
        int m_stdin_fd;
        int m_stdout_fd;
        int m_stderr_fd;
        bool m_shell;
        std::string m_cwd;
        bool m_close_fds;
    };

    SubProcess();
    ~SubProcess();

    bool Create(const std::string& cmdline,
                const CreateOptions& options = CreateOptions());
    bool Create(const std::vector<std::string>& cmdline,
                const CreateOptions& options = CreateOptions());
    bool Create(const char* const* cmdline,
                const CreateOptions& options = CreateOptions());

    // Get the process id of this SubProcess object
    pid_t Id() const;

    // Returns true if the m_pid is not equal to 0, otherwise returns false
    bool IsValid() const;

    // Whether the process is still alive.
    bool IsAlive() const;

    // Terminate the assocaited process
    // If it terminates successfully, returns true
    // otherwise returns false
    bool Terminate();

    // Wait indefinitely for the associated process to exit
    // If the process exits successfully, true, otherwise returns false
    bool WaitForExit();

    // Wait the specified number of milliseconds for the
    // associated process to exit. If the process exits successfully,
    // returns true, otherwise returns false
    bool TimedWaitForExit(int64_t timeout_milliseconds);

    // Must be called after waiting success.
    int ExitCode() const;

    // Send the signal to the associated process
    bool SendSignal(int signal);

#if 0 // Need pipe to implement.
    FILE* StdIn();
    FILE* StdOut();
    FILE* StdError();

    std::string StdOutText();
    std::string StdErrorText();
    bool Communicate(const std::string& in, std::string* out, std::string* error);
    bool Communicate(std::string* out, std::string* error);
#endif
private:
    static void DoExec(const char* const* args,
                       const CreateOptions& options,
                       int pipe_write_fd);
    bool UpdateExitStatus(int status);

private:
    pid_t m_pid;

    // 0: Not assocaited with any process
    // > 0: Assocaited with a running or a terminated but not waited process.
    // < 0: Assocaited with a terminated process.
    int m_exit_code;
};

} // namespace toft

#endif // TOFT_SYSTEM_PROCESS_SUB_PROCESS_H
