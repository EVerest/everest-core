// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once
#include <everest/io/event/fd_event_client.hpp>
#include <everest/io/serial/pty_handler.hpp>

namespace everest::lib::io::serial {

/**
 * @var event_pty_base
 * @brief Base type for a client for PTY implemented in terms of \ref event::fd_event_client
 * and \ref serial::pty_handler
 */
using event_pty_base = event::fd_event_client<pty_handler>::type;

/**
 * event_pty extends \ref event_pty_base for the special handling needed
 * for data and status, which are both received via RX
 */
class event_pty : public event_pty_base {
    /**
     * @var status
     * @brief Type for status information
     */
    using status = pty_status;

    /**
     * @var cb_status
     * @brief Prototype for status callback functions
     */
    using cb_status = std::function<void(status const&)>;

public:
    /**
     * @brief Register a callback for status RX
     * @details This handler will be called, when there are changes on settings of the slave, e.g. via
     * <a href="https://man7.org/linux/man-pages/man1/stty.1.html">stty</a>
     * @param[in] handler The callback to be used as status handler
     */
    void set_status_handler(cb_status const& handler);

    /**
     * @brief Register a callback for data RX
     * @details This handler will be called when new data is available on the slave
     * @param[in] handler The callback to be used as data handler
     */
    void set_data_handler(cb_rx const& handler);

    /**
     * @brief Get the path of the slave in the filesystem
     * @details The file at this path represents the slave and is to be used by the outside world
     * @return The path
     */
    std::string get_slave_path();

private:
    cb_status m_status;
    cb_rx m_data;
};

} // namespace everest::lib::io::serial
