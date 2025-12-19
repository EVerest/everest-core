// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once

#include <cstdint>
#include <everest/io/event/unique_fd.hpp>
#include <functional>
#include <string>
#include <vector>

namespace everest::lib::io::raw {

/**
 * raw_socket bundles basic <a href="https://man7.org/linux/man-pages/man7/raw.7.html">RAW SOCKET</a>
 * related functionality. This includes lifetime management, reading, writing and fundamental
 * error checking.
 * Although this class can be used on its own, the main purpose is to be used as base class for
 * implementation the \p ClientPolicy of \ref event::fd_event_client
 */
class raw_socket {
public:
    /**
     * @var PayloadT
     * @brief Type of the payload for TX and RX operations
     */
    using PayloadT = std::vector<uint8_t>;

    /**
     * @brief The class is default constructed
     */
    raw_socket() = default;
    ~raw_socket() = default;

    /**
     * @brief Open a RAW socket.
     * @details Sets the socket non blocking. <br>
     * Implementation for \p ClientPolicy
     * @param[in] if_name Name of the ethernet device
     * @return True on success, false otherwise.
     */
    bool open(std::string const& if_name);

    /**
     * @brief Write data to the socket. Partial writes may occur.
     * @details Implementation for \p ClientPolicy
     * @param[inout] payload Payload. Will be modified on partial writes to hold only the remaining data.
     * @return True on complete transmission of the payload, False otherwise (including partial writes)
     */
    bool tx(PayloadT& payload);

    /**
     * @brief Read as much data as available from the socket
     * @details Implementation for \p ClientPolicy
     * @param[out] buffer Buffer to write to
     * @return True on success, False otherwise
     */
    bool rx(PayloadT& buffer);

    /**
     * @brief Get the file descriptor of the socket
     * @details Implementation for \p ClientPolicy
     * @return The file descriptor of the socket
     */
    int get_fd() const;

    /**
     * @brief Get the file descriptor of the socket
     * @details Implementation for \p ClientPolicy
     * @return The file descriptor of the socket
     */
    /**
     * @brief Get pending errors on the socket.
     * @details Implementation for \p ClientPolicy
     * @return The current errno of the socket. Zero with no pending error.
     */
    int get_error() const;

    /**
     * @brief Check if the objects owns a socket
     * @return True if a object  owns a socket, false otherwise
     */
    bool is_open() const;

    /**
     * @brief Close the owned socket
     */
    void close();

private:
    event::unique_fd m_fd;
    static constexpr size_t default_buffer_size{65536};
};
} // namespace everest::lib::io::raw
