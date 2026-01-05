// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <netinet/in.h>
#include <sys/socket.h>

#include <modbus-server/transport.hpp>

#include "poll.h"

using namespace modbus_server;

ModbusSocketTransport::ModbusSocketTransport(int socket) : socket(socket) {
}

std::vector<std::uint8_t> ModbusSocketTransport::read_bytes(size_t count) {
    std::vector<std::uint8_t> buf(count);
    size_t read = 0;
    while (read < count) {
        ssize_t err = recv(this->socket, buf.data() + read, count - read, 0);
        read += err;

        if (err == 0) {
            throw transport_exceptions::ConnectionClosedException("Socket closed");
        }

        // todo: build custom errors for this kind of error
        if (err < 0) {
            throw std::runtime_error("Failed to read bytes, got " + std::to_string(count) +
                                     " bytes, err: " + std::to_string(err) + "errno: " + std::to_string(errno));
        }
    }

    return buf;
}

std::optional<std::vector<std::uint8_t>> modbus_server::ModbusSocketTransport::try_read_bytes(size_t count) {
    struct pollfd pfd[1];
    pfd[0].fd = this->socket;
    pfd[0].events = POLLIN;

    auto result_code = poll(pfd, 1, 50);
    auto error = errno;
    if (result_code < 0) {
        throw std::runtime_error("Failed to poll modbus data, errno: " + std::to_string(error));
    }

    if (result_code == 0) {
        return std::nullopt;
    }

    return read_bytes(count);
}
void ModbusSocketTransport::write_bytes(const std::vector<std::uint8_t>& bytes) {
    int err = send(this->socket, bytes.data(), bytes.size(), 0);

    // todo: build custom errors for this kind of error
    if (err < 0) {
        throw std::runtime_error("Failed to write bytes, got " + std::to_string(bytes.size()) +
                                 " bytes, err: " + std::to_string(err) + "errno: " + std::to_string(errno));
    }
}
