// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#pragma once

#include <string>
#include <vector>

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
    const size_t MAX_PIPE_MESSAGE_SIZE = 1024;
    SubProcess(int fd, pid_t pid) : fd(fd), pid(pid){};
    int fd{};
    pid_t pid{0};
    bool check_child_executed_done{false};
};

bool keep_caps();

std::string set_caps(const std::vector<std::string>& capabilities);

std::string set_real_user(const std::string& user_name);

std::string set_user_and_capabilities(const std::string& run_as_user, const std::vector<std::string>& capabilities);

} // namespace Everest::system
