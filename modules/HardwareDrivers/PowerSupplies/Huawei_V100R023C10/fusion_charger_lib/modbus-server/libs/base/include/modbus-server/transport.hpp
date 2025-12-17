// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef MODBUS_SERVER__TRANSPORT_HPP
#define MODBUS_SERVER__TRANSPORT_HPP

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <vector>

#include "frames.hpp"

namespace modbus_server {

namespace transport_exceptions {

class ConnectionClosedException : public std::runtime_error {
public:
    ConnectionClosedException() : std::runtime_error("Connection closed") {
    }
    ConnectionClosedException(const std::string& details) : std::runtime_error("Connection closed: " + details) {
    }
};

} // namespace transport_exceptions

/**
 * @brief The transport layer for modbus communication. Must implement reading
 * and writing.
 */
class ModbusTransport {
public:
    virtual ~ModbusTransport() = default;

    /**
     * @brief Read a number of bytes from the transport layer. The number of bytes
     * to read is specified by the \c count parameter
     *
     * @param count The number of bytes to read
     * @return std::vector<std::uint8_t> The read bytes
     * @throws ConnectionClosedException if the connection is closed
     */
    virtual std::vector<std::uint8_t> read_bytes(size_t count) = 0;

    virtual std::optional<std::vector<std::uint8_t>> try_read_bytes(size_t count) = 0;

    /**
     * @brief Write a number of bytes to the transport layer. The buffer to write
     * is specified by the \c bytes parameter
     *
     * @param bytes The bytes to write
     * @throws ConnectionClosedException if the connection is closed
     */
    virtual void write_bytes(const std::vector<std::uint8_t>& bytes) = 0;
};

/**
 * @brief The transport layer implementation for a socket connection using recv
 * and send
 *
 */
class ModbusSocketTransport : public ModbusTransport {
protected:
    int socket;

public:
    /**
     * @brief Create a new ModbusSocketTransport using a socket file descriptor
     *
     * @param socket The socket file descriptor, as returned by \c socket()
     */
    ModbusSocketTransport(int socket);
    virtual ~ModbusSocketTransport() = default;

    std::vector<std::uint8_t> read_bytes(size_t count) override;
    std::optional<std::vector<std::uint8_t>> try_read_bytes(size_t count) override;
    void write_bytes(const std::vector<std::uint8_t>& bytes) override;
};

} // namespace modbus_server

#endif
