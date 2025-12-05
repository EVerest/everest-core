// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef CONTROLLER_IPC_HPP
#define CONTROLLER_IPC_HPP

#include <nlohmann/json.hpp>

#define MAGIC_CONTROLLER_ARG0 "MT.EVEREST"

namespace Everest {
namespace controller_ipc {

enum class MESSAGE_RETURN_STATUS {
    OK,
    ERROR,
    TIMEOUT,
};

struct Message {
    Message(MESSAGE_RETURN_STATUS status, nlohmann::json json) : status(status), json(std::move(json)){};
    const MESSAGE_RETURN_STATUS status;
    const nlohmann::json json;
};

// FIXME (aw): add return value for failed set_timeout
void set_read_timeout(int fd, int timeout_in_ms);

// FIXME (aw): add return value for failed send
void send_message(int fd, const nlohmann::json& msg);
Message receive_message(int fd);

} // namespace controller_ipc
} // namespace Everest

#endif // CONTROLLER_IPC_HPP
