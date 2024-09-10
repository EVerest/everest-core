// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/detail/helper.hpp>

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <stdexcept>

namespace iso15118 {

std::string adding_err_msg(const std::string& msg) {
    return (msg + " (reason: " + strerror(errno) + ")");
}

void log_and_throw(const char* msg) {
    throw std::runtime_error(std::string(msg) + " (reason: " + strerror(errno) + ")");
}

} // namespace iso15118
