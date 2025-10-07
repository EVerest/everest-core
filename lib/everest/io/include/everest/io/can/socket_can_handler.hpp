// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once

#include <everest/io/can/can_payload.hpp>
#include <everest/io/event/unique_fd.hpp>
#include <string>

namespace everest::lib::io {

namespace can {

/**
 * socket_can_handler bundles basic <a href="https://docs.kernel.org/networking/can.html">socket_can</a>
 * related functionality. This includes lifetime management, reading, writing and fundamental
 * error checking. It also addresses the common issue of
 * <a href=" https://rtime.felk.cvut.cz/can/socketcan-qdisc-final.pdf">failing writes</a>,
 * after write ([E]POLLOUT) notifications. <br>
 * Although this class can be used on its own, the main purpose is to implement the
 * \p ClientPolicy of \ref event::fd_event_client
 */
class socket_can_handler {
public:
    /**
     * @var PayloadT
     * @brief Type of the payload for TX and RX operations
     */
    using PayloadT = can_dataset;

    /**
     * @brief The class is default constructed
     */
    socket_can_handler() = default;
    ~socket_can_handler() = default;

    /**
     * @brief Raw implementation for writing data to the socket
     * @details Prior to sending the function checks the device status and the payload
     * @param[in] can_id ID of the target device on the CAN bus.
     * @param[in] len8_dlc Optional (9..15) if \p payload size is 8 but DLC is higher (ISO 11898-1)
     * @param[in] payload Payload of up to 8 bytes. Implicitly defines DLC
     * @return The errno of <a href="https://man7.org/linux/man-pages/man2/write.2.html">write</a>.
     * Zero indicates success.
     */
    int tx(uint32_t can_id, uint8_t len8_dlc, can_payload const& payload);

    /**
     * @brief Raw implementation for reading data from the socket.
     * @param[in] can_id ID of the target device on the CAN bus
     * @param[in] len8_dlc Optional (9..15) if \p payload size is 8 but DLC is higher (ISO 11898-1)
     * @param[in] payload Payload of up to 8 bytes. Implicitly defines DLC
     * @return The errno of <a href="https://man7.org/linux/man-pages/man2/read.2.html">read</a>.
     * Zero indicates success
     */
    int rx(uint32_t& can_id, uint8_t& len8_dlc, can_payload& payload);

    /**
     * @brief Write a \ref can_dataset to the socket
     * @details Implementation for \p ClientPolicy
     * @param[in] data Payload
     * @return True on success, False otherwise
     */
    bool tx(can_dataset const& data);
    /**
     * @brief Read a \ref can_dataset from the socket
     * @details Implementation for \p ClientPolicy
     * @param[out] data Payload
     * @return True on success, False otherwise
     */
    bool rx(can_dataset& data);

    /**
     * @brief Open the socket_can device.
     * @details Sets the socket non blocking and reduces send buffer. <br>
     * Implementation for \p ClientPolicy
     * @param[in] can_dev The device to bind the socket to.
     * @return True on success, false otherwise.
     */
    bool open(std::string const& can_dev);

    /**
     * @brief Check if the objects owns a device
     * @return True if a device is owned, false otherwise
     */
    bool is_open() const;

    /**
     * @brief Close the owned device
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

    /**
     * @brief Check if the payload is valid
     * @details Internally checks is the size of the message is smaller than the maximum for CAN
     * @param[in] payload The payload
     * @return True if valid, false otherwise
     */
    static bool data_valid(can_payload const& payload);

private:
    int open_device();

    event::unique_fd m_owned_can_fd;
    std::string m_can_dev;
};
} // namespace can
} // namespace everest::lib::io
