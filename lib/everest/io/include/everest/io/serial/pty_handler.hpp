// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once
#include <cstdint>
#include <everest/io/serial/serial.hpp>
#include <vector>

namespace everest::lib::io::serial {

/**
 * @struct pty_status
 * Simplified status of a <a href="https://man7.org/linux/man-pages/man7/pty.7.html">PTY</a>.
 * Values map to the values defined <a href="https://linux.die.net/man/3/tcgetattr">here</a>
 */
struct pty_status {
    /**
     * @brief Software flow control for the output
     */
    bool ixon{false};
    /**
     * @brief Software flow control for the input
     */
    bool ixoff{false};
    /**
     * @brief Two stopbits instead of one
     */
    bool cstopb{false};
    /**
     * @brief The Baud reate
     */
    unsigned int cbaud{0};
};

/**
 * pty_handler bundles basic <a href="https://man7.org/linux/man-pages/man7/pty.7.html">PTY</a>
 * related functionality.  This includes lifetime management, reading, writing and fundamental
 * error checking. <br>
 * Although this class can be used on its own, the main purpose is to implement the
 * \p ClientPolicy of \ref event::fd_event_client
 */
class pty_handler {
public:
    /**
     * @var PayloadT
     * @brief Type of the payload for tX and RX operations
     */
    using PayloadT = std::vector<uint8_t>;

    /**
     * @brief The class is default constructed
     */
    pty_handler() = default;
    ~pty_handler() = default;

    /**
     * @brief Write a dataset to the PTY
     * @details Implementation for \p ClientPolicy
     * @param[in] data Payload
     * @return True on success, False on failure and partial writes.
     */
    bool tx(PayloadT& data);
    /**
     * @brief Read a dataset from the PTY
     * @details Implementation for \p ClientPolicy
     * @param[in] data Payload
     * @return True on success, False otherwise.
     */
    bool rx(PayloadT& data);

    /**
     * @brief Open the PTY
     * @details Activates <a href="https://lists.gnu.org/archive/html/bug-readline/2011-01/msg00004.html">EXTPROC</a>
     * an <a href="https://man7.org/linux/man-pages/man2/TIOCPKT.2const.html">TIOCPKT</a>
     * via \ref make_pty_mode_aware. <br>
     * Implementation for \p ClientPolicy
     * @return True on success, false otherwise.
     */
    bool open();

    /**
     * @brief Get the master file descriptor
     * @details Implementation for ClientPolicy
     * @return master file descriptor
     */
    int get_fd() const;

    /**
     * @brief Get the current error
     * @details Implementation for \p ClientPolicy
     * @return The last errno. Zero if there is no error.
     */
    int get_error() const;

    /**
     * @brief Get the current status of the PTY
     * @details Read the status information as set on the slave
     * @return Simplified status
     */
    pty_status get_status();

    /**
     * @brief The path of the slave in the file system
     * @return Path
     */
    std::string get_slave_path() const;

private:
    pty m_dev;
    int error_id{0};
    static constexpr size_t buffer_size_limit = 1400;
};

} // namespace everest::lib::io::serial
