// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once

#include "unique_fd.hpp"
#include <chrono>

namespace everest::lib::io::event {

/**
 * timer_fd creates and configures a <a href="https://man7.org/linux/man-pages/man2/timerfd_create.2.html">timer</a>.
 * The lifetime of the timer is bound to the lifetime of this object.
 */
class timer_fd {
public:
    /**
     * @brief Constructor
     * @details After construction the timeout is undefined. It must be set manually.
     */
    timer_fd();

    /**
     * @brief Explicit conversion to file descriptor
     * @return The internal file descriptor
     */
    explicit operator int() const;

    /**
     * @brief Access to internal file descriptor
     * @return The internal file descriptor
     */
    int get_raw_fd() const;

    /**
     * @brief Check if a timer is held by this object
     * @details Compares internally to \ref unique_fd::NO_DESCRIPTOR_SENTINEL
     * @return True if a timer is held, false otherwise
     */
    bool valid() const;

    /**
     * @brief Read from timer
     * @details This acknowledges, that the timer event has been handled. Poll on this object will
     * return immediately until this function has been called after a timeout event.
     * @return The value read from the timer
     */
    int read();

    /**
     * @name Configuring the notification timeout
     * This set of functions allows to set the time after which the timer file fire
     * @{
     */
    /**
     * @brief Milliseconds
     * @param[in] to timeout in milliseconds
     * @return True on success, false otherwise
     */
    bool set_timeout_ms(long long to);
    /**
     * @brief Microseconds
     * @param[in] to timeout in microseconds
     * @return True on success, false otherwise
     */
    bool set_timeout_us(long long to);
    /**
     * @brief Nanoseconds
     * @param[in] to timeout in nanoseconds
     * @return True on success, false otherwise
     */
    bool set_timeout_ns(long long to);

    /**
     * @brief <a href="https://en.cppreference.com/w/cpp/chrono/duration"> std::chrono::duration </a>
     * @param[in] timeout timeout in abitrary  units
     * @return True on success, false otherwise
     */
    template <class Rep, class Period> bool set_timeout(std::chrono::duration<Rep, Period> timeout) {
        auto interval = std::chrono::duration_cast<std::chrono::nanoseconds>(timeout);
        return set_timeout_ns(interval.count());
    }
    /**
     * @}
     */

private:
    unique_fd m_fd;
};

} // namespace everest::lib::io::event
