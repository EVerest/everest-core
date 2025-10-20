// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once

#include <functional>
#include <string>

namespace everest::lib::io::utilities {

/**
 * This class bundles some handling logic for <a href="https://man7.org/linux/man-pages/man3/errno.3.html">errno</a>.
 * <br> Its main purpose is the factor out functionality from \ref event::generic_fd_event_client_impl
 */
class generic_error_state {
public:
    /**
     * @var cb_error
     * @brief Prototype for an on_error handler callback. It receives the current errno
     * and its string representation
     */
    using cb_error = std::function<void(int error, std::string const& msg)>;
    virtual ~generic_error_state() = default;

protected:
    /**
     * @brief Update the error state
     * @details Compares with internal error state and update it.
     * @param[in] error_code The error state to be set
     * @return False if error status is set, True otherwise
     */
    bool set_error_status(int error_code);

    /**
     * @brief Check if error handling is still needed.
     * @return True if there is an uncleared error, false otherwise
     */
    bool clear_error_pending() const;

    /**
     * @brief Check the error state
     * @return True if on error, false otherwise
     */
    bool on_error() const;

    /**
     * @brief Get the current error code
     * @return Current error code
     */
    int current_error() const;

    /**
     * @brief Call the error handler with current errno, if registered.
     * @param[in] handler The handler to be called
     */
    void call_error_handler(cb_error& handler) const;

    /**
     * @brief Call the error handler with errno=0 (success)
     * @param[in] handler The handler to be called
     */
    void clear_error_handler(cb_error& handler);

    /**
     * @brief Mark the current error as cleared
     */
    void set_error_cleared();

private:
    bool m_on_error{true};
    bool m_clear_error_pending{false};
    int m_current_error{0};
};

} // namespace everest::lib::io::utilities
