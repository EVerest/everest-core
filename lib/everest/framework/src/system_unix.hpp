// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#include <cstdint>
#include <poll.h>
#include <sys/types.h>

namespace Everest::system {

class SubProcess {
public:
    static SubProcess create(const std::string& run_as_user, const std::vector<std::string>& capabilities = {});
    bool is_child() const {
        return this->pid == 0;
    }

    void send_error_and_exit(const std::string& message);

    pid_t check_child_executed();

private:
    const std::size_t MAX_PIPE_MESSAGE_SIZE = 1024;
    SubProcess(int fd, pid_t pid) : fd(fd), pid(pid){};
    int fd{};
    pid_t pid{0};
    bool check_child_executed_done{false};
};

bool keep_caps();

std::string set_caps(const std::vector<std::string>& capabilities);

std::string set_real_user(const std::string& user_name);

std::string set_user_and_capabilities(const std::string& run_as_user, const std::vector<std::string>& capabilities);

class SignalPolling {
public:
    SignalPolling();

    std::optional<uint32_t> poll_signal();

private:
    bool available = false;
    int signal_fd = -1;
    struct pollfd pollfds[1];
};

} // namespace Everest::system
