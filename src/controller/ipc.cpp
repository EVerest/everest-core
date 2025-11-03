// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include "ipc.hpp"

#include <cerrno>
#include <iterator>
#include <sys/socket.h>
#include <sys/time.h>
#include <system_error>
#include <unistd.h>

// FIXME (aw): this needs be done better!
namespace {
constexpr auto raw_message_buffer_size = 16 * std::size_t{1024};
std::array<char, raw_message_buffer_size> raw_message_buffer;
} // namespace

namespace Everest {
namespace controller_ipc {

void set_read_timeout(int fd, int timeout_in_ms) {

    const int seconds = timeout_in_ms / 1000;
    const int u_seconds = (timeout_in_ms - seconds * 1000) * 1000;

    struct timeval socket_timeout {
        seconds, u_seconds
    };

    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, static_cast<void*>(&socket_timeout), sizeof(socket_timeout));
}

void send_message(int fd, const nlohmann::json& msg) {
    auto raw = nlohmann::json::to_bson(msg);
    auto raw_it = raw.begin();
    size_t already_sent = 0;

    while (already_sent < raw.size()) {
        const ssize_t c = write(fd, &(*raw_it), raw.size() - already_sent);

        if (c == -1) {
            throw std::system_error(errno, std::system_category(), "Error while sending message");
        }

        already_sent += c;
        std::advance(raw_it, c);
    }
}

// FIXME (aw): recv should be buffered and memory is not that costly here!
Message receive_message(int fd) {
    auto retval = read(fd, raw_message_buffer.data(), raw_message_buffer.size());

    if (retval == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return {MESSAGE_RETURN_STATUS::TIMEOUT, nullptr};
        }

        return {MESSAGE_RETURN_STATUS::ERROR, {{"error", strerror(errno)}}};
    }

    const auto& msg_size = retval;

    return {MESSAGE_RETURN_STATUS::OK,
            nlohmann::json::from_bson(raw_message_buffer.begin(), raw_message_buffer.begin() + msg_size)};
}

} // namespace controller_ipc
} // namespace Everest
