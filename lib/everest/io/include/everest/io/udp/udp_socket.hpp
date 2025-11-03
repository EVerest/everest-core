// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once

#include <array>
#include <cstdint>
#include <everest/io/event/unique_fd.hpp>
#include <everest/io/udp/udp_payload.hpp>
#include <optional>
#include <string>

namespace everest::lib::io::udp {

/**
 * @struct udp_info
 * Collection of information needed to send/receive via UDP
 */
struct udp_info {
    /** Adress */
    uint32_t addr;
    /** Port */
    uint16_t port;
    /** Family */
    uint16_t family;
};

/**
 * udp_socket_base bundles basic <a href="https://man7.org/linux/man-pages/man7/udp.7.html">UDP</a>
 * related functionality. This includes lifetime management, reading, writing and fundamental
 * error checking.
 * Although this class can be used on its own, the main purpose is to be used as base class for
 * implementation the \p ClientPolicy of \ref event::fd_event_client
 */
class udp_socket_base {
public:
    /**
     * The class is default constructed
     */
    udp_socket_base() = default;
    virtual ~udp_socket_base() = default;

    /**
     * @brief Open the socket as a server
     * @details Sets the socket non blocking
     * @param[in] port The port to listen to
     * @return True on success, false otherwise
     */
    bool open_as_server(uint16_t port);

    /**
     * @brief Open the socket as a server
     * @details Sets the socket non blocking
     * @param[in] remote The host to connect to
     * @param[in] port The port to listen to
     * @return True on success, false otherwise
     */
    bool open_as_client(std::string const& remote, uint16_t port);

    /**
     * @brief Check if the objects owns a socket
     * @return True if a object  owns a socket, false otherwise
     */
    bool is_open();

    /**
     * @brief Close the owned socket
     */
    void close();

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

protected:
    /**
     * @brief Send data to the default destination.
     * @details To be used in sever mode. The operation will fail is the device is not open or is the
     * data could not be send for a different reason.
     * @param[in] payload Payload
     * @param[in] size Size of the payload.
     * @return True on success, false otherwise.
     */
    bool tx_impl(void const* payload, size_t size);

    /**
     * @brief Send data to the default destination.
     * @details Can be used in server and client mode. The operation will fail if the device is not open or if the
     * data could not be send for a different reason.
     * @param[in] payload Payload
     * @param[in] size Size of the payload.
     * @param[in] destination The destination for the message
     * @return True on success, false otherwise.
     */
    bool tx_impl(void const* payload, size_t size, udp_info const& destination);

    /**
     * @brief Receive data from the socket
     * @details Can be used in server and client mode.
     * @param[inout] buffer Destination for the data
     * @param[in] buffer_size Size of the buffer
     * @param[in] payload_size The size of the payload
     * @return std::nullopt when \p payload_size is zero, the dataset otherwise
     */
    std::optional<udp_info> rx_impl(void* buffer, size_t buffer_size, ssize_t& payload_size);

private:
    event::unique_fd m_owned_udp_fd;
};
/**
 * A basic <a href="https://man7.org/linux/man-pages/man7/udp.7.html">UDP</a> client.
 * Although this class can be used on its own, the main purpose is to be used as base class for
 * implementation the \p ClientPolicy of \ref event::fd_event_client
 */
class udp_client_socket : public udp_socket_base {
public:
    /**
     * @var PayloadT
     * @brief Type of the payload for TX and RX operations
     */
    using PayloadT = udp_payload;

    /**
     * @brief The class is default constructed
     */
    udp_client_socket() = default;
    ~udp_client_socket() = default;

    /**
     * @brief Open a UDP client socket.
     * @details Sets the socket non blocking. <br>
     * Implementation for \p ClientPolicy
     * @param[in] remote The host to connect to
     * @param[in] port The port on host
     * @return True on success, false otherwise.
     */
    bool open(std::string const& remote, uint16_t port);

    /**
     * @brief Write a \ref udp_payload to the socket
     * @details Implementation for \p ClientPolicy
     * @param[in] payload Payload
     * @return True on success, False otherwise
     */
    bool tx(udp_payload const& payload);

    /**
     * @brief Read a \ref udp_payload from the socket
     * @details Implementation for \p ClientPolicy
     * @param[out] payload Payload
     * @return True on success, False otherwise
     */
    bool rx(udp_payload& payload);

private:
    std::array<uint8_t, udp_payload::max_size> rx_buffer;
};

} // namespace everest::lib::io::udp
