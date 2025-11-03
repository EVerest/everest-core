// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once

#include <cstdint>
#include <everest/io/event/unique_fd.hpp>
#include <string>
#include <vector>

namespace everest::lib::io::tun_tap {

/**
 * tap_handler bundles basic <a href="https://docs.kernel.org/networking/tuntap.html">TAP device</a>
 * related functionality. This includes setup of the device, ip settings, lifetime management,
 * reading, writing and fundamental error checking.
 * Although this class can be used on its own, the main purpose is to be used as base class for
 * implementation the \p ClientPolicy of \ref event::fd_event_client
 */
class tap_handler {
public:
    /**
     * @var PayloadT
     * @brief The type of the payload
     */
    using PayloadT = std::vector<uint8_t>;
    /**
     * The class is default constructed
     */
    tap_handler() = default;

    /**
     * @brief Create and open a TAP device
     * @details This functions tries to create and bring up a new TAP device with the
     * given name and IP settings. A possible reason for failure is if the \p device name is not availble.
     * @param[in] device The requested name for the TAP device.
     * @param[in] ip IP address to be assigned for the TAP device
     * @param[in] netmask Netmask for the TAP device
     * @param[in] mtu The Maximum transmission unit, i.e. the maximum size of a message in bytes.
     * @return True on success, false otherwise.
     */
    bool open(std::string const& device, std::string const& ip, std::string const& netmask, int mtu);

    /**
     * @brief Write a dataset to the TAP
     * @details Implementation for \p ClientPolicy
     * @param[in] data Payload
     * @return True on success, False otherwise.
     */
    bool tx(PayloadT const& data);
    /**
     * @brief Read a dataset from the TAP
     * @details Implementation for \p ClientPolicy
     * @param[in] data Payload
     * @return True on success, False otherwise.
     */
    bool rx(PayloadT& data);
    /**
     * @brief Get the current error
     * @details Implementation for \p ClientPolicy
     * @return The last errno. Zero if there is no error.
     */
    int get_fd() const;
    /**
     * @brief Get the current error
     * @details Implementation for \p ClientPolicy
     * @return The last errno. Zero if there is no error.
     */
    int get_error() const;

private:
    event::unique_fd m_fd;
    int m_error{0};
    int m_mtu;
};

} // namespace everest::lib::io::tun_tap
