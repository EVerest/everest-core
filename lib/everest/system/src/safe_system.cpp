// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <everest/system/safe_system.hpp>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <stdexcept>

namespace {
constexpr int STRERROR_BUF_SIZE = 128;

inline void kill_and_wait(int pid_fd) {
    syscall(SYS_pidfd_send_signal, pid_fd, SIGKILL, NULL, 0);
    siginfo_t status_info;
#ifndef P_PIDFD
    waitid(static_cast<idtype_t>(3), pid_fd, &status_info, WEXITED);
#else
    waitid(P_PIDFD, pid_fd, &status_info, WEXITED);
#endif
}

everest::lib::system::CommandResult wait_for_process(int pid, int timeout_s) {
    using CmdExecStatus = everest::lib::system::CommandExecutionStatus;
    using CmdResult = everest::lib::system::CommandResult;
    CmdResult result{CmdExecStatus::CMD_SETUP_FAILED, 0, false};

    // Try to get a pid file descriptor
    int pid_fd = syscall(SYS_pidfd_open, pid, 0);

    if (pid_fd == -1) {
        result.code = errno;

        kill(pid, SIGKILL);
        waitpid(pid, NULL, 0);
        return result;
    }

    // Try to open an epoll fd
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        result.code = errno;
        kill_and_wait(pid_fd);
        close(pid_fd);
        return result;
    }

    // Setup epoll
    struct epoll_event event_mask {};
    event_mask.events = EPOLLIN; // Process exit is signaled via EPOLLIN
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, pid_fd, &event_mask) == -1) {
        result.code = errno;
        kill_and_wait(pid_fd);
        close(pid_fd);
        close(epoll_fd);
        return result;
    }

    // Await either timeout or natural exit of process
    struct epoll_event event {};
    int timeout_ms = timeout_s * 1000;

    int event_count = epoll_wait(epoll_fd, &event, 1, timeout_ms);

    if (event_count == 0) {
        // Nothing happened -> timed out and process still runs
        syscall(SYS_pidfd_send_signal, pid_fd, SIGKILL, NULL, 0);
        result.timeout = true;
    }

    // Reap the child and get its final status
    siginfo_t status_info;
#ifndef P_PIDFD
    if (waitid(static_cast<idtype_t>(3), pid_fd, &status_info, WEXITED) != -1) {
#else
    if (waitid(P_PIDFD, pid_fd, &status_info, WEXITED) != -1) {
#endif
        if (status_info.si_code == CLD_EXITED) {
            result.status = CmdExecStatus::CMD_SUCCESS;
        } else if (status_info.si_code == CLD_KILLED) {
            result.status = CmdExecStatus::CMD_TERMINATED;
        }
        result.code = status_info.si_status;
    } else {
        result.status = CmdExecStatus::CMD_SETUP_FAILED;
        result.code = errno;
    }

    close(pid_fd);
    close(epoll_fd);

    return result;
}

everest::lib::system::CommandResult safe_system_command(int fd, const char* pathname, char* const argv[],
                                                        int timeout_s) {
    using CmdExecStatus = everest::lib::system::CommandExecutionStatus;

    pid_t pid = fork();

    if (pid == -1) {
        return {CmdExecStatus::CMD_SETUP_FAILED, errno, false};
    } else if (pid == 0) {
        // child process

        // keep stdout and stderr available
        (void)close(STDIN_FILENO);
        if (fd != -1) {
            (void)dup2(fd, STDOUT_FILENO);
        }

        if (execv(pathname, argv) == -1) {
            // if this is executed, execv did not replace the process
            perror("execv in safe_system_command failed");
            _exit(EXIT_FAILURE);
        }
    }

    // parent process
    return wait_for_process(pid, timeout_s);
}
} // namespace

namespace everest::lib::system {
std::string cmd_execution_status_to_string(const CommandExecutionStatus& status) {
    switch (status) {
    case CommandExecutionStatus::CMD_SUCCESS:
        return "CMD_SUCCESS";
    case CommandExecutionStatus::CMD_TERMINATED:
        return "CMD_TERMINATED";
    case CommandExecutionStatus::CMD_SETUP_FAILED:
        return "CMD_SETUP_FAILED";
    default:
        return "CMD_UNKNOWN_STATUS";
    }
}

std::string command_string_repr(std::string pathname, std::vector<std::string> args) {
    std::string ret{pathname};

    for (const auto& item : args) {
        ret.push_back(' ');
        ret.append(item);
    }

    return ret;
}

std::string command_string_repr(std::string_view pathname, std::initializer_list<std::string_view> args) {
    std::vector<std::string> arg_v(args.begin(), args.end());
    return command_string_repr(std::string(pathname), arg_v);
}

CommandResult safe_system(int fd, const std::string& pathname_str, std::vector<std::string>* args, int timeout_s) {
    std::vector<char*> argv;

    std::transform(args->begin(), args->end(), std::back_inserter(argv), [](std::string& s) { return s.data(); });
    argv.push_back(nullptr);

    return safe_system_command(fd, pathname_str.c_str(), argv.data(), timeout_s);
}

CommandResult safe_system(int fd, std::string_view pathname, std::initializer_list<std::string_view> args,
                          int timeout_s) {
    std::string pathname_str{pathname};

    std::vector<std::string> argv_str{args.begin(), args.end()};

    return safe_system(fd, pathname_str, &argv_str, timeout_s);
}

std::pair<std::string, std::vector<std::string>> split_command_line(const std::string& command) {
    auto whitespace = [](char c) { return std::isspace(c); };
    auto any = [](char c) { return std::isspace(c) || c == '"' || c == '\''; };

    if (std::any_of(command.cbegin(), command.cend(), [](char c) { return c == '\\'; })) {
        throw std::runtime_error("Command lines containg '\\' are considered invalid.");
    }

    std::vector<std::string> results;
    auto from = std::find_if_not(command.cbegin(), command.cend(), whitespace);
    if (from == command.cend()) {
        throw std::runtime_error("Empty command line string.");
    }
    results.emplace_back("");
    auto to = command.cend();
    while (true) {
        auto& current_token = results.back();
        auto current = std::find_if(from, to, any);
        std::copy(from, current, std::back_inserter(current_token));
        // If at the end, stop searching
        if (current == to) {
            break;
        }

        if (*current == '"' || *current == '\'') {
            from = std::next(current);
            auto closing = std::find_if(std::next(current), to, [&](char c) { return c == *current; });
            if (closing == to) {
                throw std::runtime_error(std::string("Unmatched ") + *current);
            }
            std::copy(std::next(current), closing, std::back_inserter(current_token));
            from = std::next(closing);
        } else {
            // it's a whitespace
            from = std::find_if_not(std::next(current), to, whitespace);
            if (from == to) {
                break;
            }
            results.emplace_back("");
        }
        if (from >= to) {
            break;
        }
    }
    std::string path{results[0]};
    size_t last_separator_pos = path.find_last_of("/\\");

    std::string_view executable_name =
        (last_separator_pos == std::string::npos) ? path : std::string_view(path).substr(last_separator_pos + 1);

    // Create the final required string
    results[0] = std::string(executable_name);

    return {path, results};
}

} // namespace everest::lib::system
