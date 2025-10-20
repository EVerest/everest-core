// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once
#include <everest/io/event/unique_fd.hpp>
#include <optional>
#include <string>

namespace everest::lib::io::serial {

/**
 * @struct pty
 * Bundle of data necessary to manage a <a href="https://man7.org/linux/man-pages/man7/pty.7.html">PTY</a>
 */
struct pty {
    /** @brief The master file descriptor */
    event::unique_fd master_fd;
    /** @brief The slave file descriptor */
    event::unique_fd slave_fd;
    /** @brief The path to the slave in the filesystem*/
    std::string slave_path;
};

/**
 * @brief Opens a PTY
 * @details The follows the steps defined in <a href="https://man7.org/linux/man-pages/man7/pty.7.html">PTY</a>,
 * which are:
 * -# Open the master via /dev/ptmx <a href="https://man7.org/linux/man-pages/man3/posix_openpt.3.html">posix_openpt</a>
 * -# Change ownership via <a href="https://man7.org/linux/man-pages/man3/grantpt.3.html">grantpt</a>
 * -# Unlock the slave via <a href="https://man7.org/linux/man-pages/man3/unlockpt.3.html">unlockpt</a>
 * -# Get the filename of the slave via <a href="https://man7.org/linux/man-pages/man3/ptsname.3.html">ptsname</a>
 * -# Open the slave via <a href="https://man7.org/linux/man-pages/man2/open.2.html">open</a>
 * @return Rerturns a everest::lib::io::serial::pty with valid data on success, \p std::nullopt otherwise.
 */
std::optional<pty> openpty();

/**
 * @brief Enable packet mode for the PTY
 * @details Refer to <a href="https://man7.org/linux/man-pages/man2/TIOCPKT.2const.html">TIOCPKT</a>
 * @param[in] fd This is supposed to be called on the master file descriptor
 * @return True on success, false otherwise.
 */
bool set_packet_mode(int fd);

/**
 * @brief Enabled EXTPROC for the PTY
 * @details Refer to <a href="https://lists.gnu.org/archive/html/bug-readline/2011-01/msg00004.html">EXTPROC</a>
 * This will trigger (E)POLLIN on the master, if there are changes on the slave status
 * @param[in] fd This is supposed to be called on the slave file descriptor
 * @return True on success, false otherwise
 */
bool set_extproc_flag(int fd);

/**
 * @brief Set RAW/Binary mode on a PTY
 * @details Refer to <a href="https://man7.org/linux/man-pages/man3/termios.3.html">termios</a>
 * This will disable any flow control any special handling for characters for the input modes and any
 * implementation specific output handling. For the output modes any signals and echoing as well as
 * the canonical mode are disabled. Settings for control mode are 8-bit characters, no parity, 1 stop bit.
 * For the non canonical mode used here, the settings are a minimum of 1 character and no timeout. This uses
 * \p cfmakeraw internally
 */
bool set_binary_mode(int fd);

/**
 * @brief Setup a <a href="https://man7.org/linux/man-pages/man7/pty.7.html">PTY</a> for (e)poll usage
 * @details This enabled packet mode via \ref everest::lib::io::serial::set_packet_mode and EXTPROC
 * via \ref everest::lib::io::serial::set_extproc_flag
 * In combination these settings allow integration into an event queue for data and settings updates on the slave
 * @param[in] item The PTY
 * @return True on success, False otherwise
 */
bool make_pty_mode_aware(pty const& item);

} // namespace everest::lib::io::serial
