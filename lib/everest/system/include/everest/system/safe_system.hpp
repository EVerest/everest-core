// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <cstdlib>
#include <initializer_list>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace everest::lib::system {

// An enum to describe the overall outcome
typedef enum {
    CMD_SUCCESS,     // Ran and exited normally
    CMD_TERMINATED,  // Terminated by signal
    CMD_SETUP_FAILED // Setup failed (fork, ...)
} CommandExecutionStatus;

// Execution result type
typedef struct {
    CommandExecutionStatus status;
    int code; // Child's exit code (if CMD_SUCCESS) OR terminating signal (CMD_TERMINATED) OR errno (CMD_SETUP_FAILED)
    bool timeout; // Command timed out
} CommandResult;

std::string cmd_execution_status_to_string(const CommandExecutionStatus& status);

std::string command_string_repr(std::string pathname, std::vector<std::string> args);
std::string command_string_repr(std::string_view pathname, std::initializer_list<std::string_view> args);

/// @brief convenience c++ function for safe_system_c
/// @param fd - file descriptor for stdout
/// @param pathname - the command to run
/// @param args - the arguments
/// @return exit code, 128 + signal number, errno ...
CommandResult safe_system(int fd, const std::string& pathname_str, std::vector<std::string>* args, int timeout_s = 30);

/// @brief convenience c++ function for safe_system_c
/// @param pathname - the command to run
/// @param args - the arguments
/// @return exit code, 128 + signal number, errno ...
inline CommandResult safe_system(const std::string& pathname_str, std::vector<std::string>* args, int timeout_s = 30) {
    return safe_system(-1, pathname_str, args, timeout_s);
}

/// @brief convenience c++ function for safe_system_c
/// @param fd - file descriptor for stdout
/// @param pathname - the command to run (accepts std::string_view, literals, std::string)
/// @param args - the arguments (accepts initializer_list of std::string_view, literals, std::string)
/// @return exit code, 128 + signal number, errno ...
CommandResult safe_system(int fd, std::string_view pathname, std::initializer_list<std::string_view> args,
                          int timeout_s = 30);

/// @brief convenience c++ function for safe_system_c
/// @param pathname - the command to run (accepts std::string_view, literals, std::string)
/// @param args - the arguments (accepts initializer_list of std::string_view, literals, std::string)
/// @return exit code, 128 + signal number, errno ...
inline CommandResult safe_system(std::string_view pathname, std::initializer_list<std::string_view> args,
                                 int timeout_s = 30) {
    return safe_system(-1, pathname, args, timeout_s);
}

/// @brief splits a given command line (cmd + args) into its components, respecting quoting
/// @param command - the commandline to be split
/// @return a vector
std::pair<std::string, std::vector<std::string>> split_command_line(const std::string& command);

/// @brief update the exit status from a child process
/// @param status - the exit status
/// @return updated status taking into account signals and other exit conditions
constexpr int update_exit_status(int status) {
    int updated_status = status;
    if (WIFEXITED(status)) {
        updated_status = WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
        updated_status = WTERMSIG(status) + 128;
    }
    return updated_status;
}

} // namespace everest::lib::system
