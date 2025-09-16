// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once

#include <cstdint>
#include <everest/io/event/unique_fd.hpp>
#include <string>
#include <vector>

namespace everest::lib::io::tcp {

/**
 * tcp_socket bundles basic <a href="https://man7.org/linux/man-pages/man7/tcp.7.html">TCP</a>
 * related functionality. This includes lifetime management, reading, writing and fundamental
 * error checking.
 * Although this class can be used on its own, the main purpose is to be used as base class for
 * implementation the \p ClientPolicy of \ref event::fd_event_client
 */
class tcp_socket {
public:
    /**
     * @var PayloadT
     * @brief Type of the payload for TX and RX operations
     */
    using PayloadT = std::vector<uint8_t>;

    /**
     * @brief The class is default constructed
     */
    tcp_socket() = default;
    ~tcp_socket() = default;

    /**
     * @brief Open a TCP socket.
     * @details Sets the socket non blocking. <br>
     * Implementation for \p ClientPolicy
     * @param[in] remote The host to connect to
     * @param[in] port The port on host
     * @return True on success, false otherwise.
     */
    bool open(std::string const& remote, uint16_t port);

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

    /**
     * @brief Enable KEEPALIVE for the connection
     * @param[in] count  The maximum number of keepalive probes TCP should send
     *                   before dropping the connection.
     * @param[in] idle_s The time (in seconds) the connection needs to remain idle
     *                   before TCP starts sending keepalive probes.
     * @param[in] intval_s The time (in seconds) between individual keepalive probes.
     * @return 'true' on success, 'false otherwise'
     */
    bool set_keep_alive(uint32_t count, uint32_t idle_s, uint32_t intval_s);

    /**
     * @brief Set transmission timeout
     * @param[in] to_ms The timeout in milliseconds
     * @return 'true' on success 'false' otherwise
     */
    bool set_user_timeout(uint32_t to_ms);

private:
    event::unique_fd m_fd;
    static constexpr size_t default_buffer_size{1500};
};
} // namespace everest::lib::io::tcp
