// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once

#include "unique_fd.hpp"
#include <cstdint>
#include <optional>

namespace everest::lib::io::event {

/**
 * event_fd_base creates an <a href="https://man7.org/linux/man-pages/man2/eventfd.2.html">event</a>.
 * The lifetime of the event is bound to the lifetime of this object.
 */
class event_fd_base {
public:
    /**
     * @brief Constructor
     * @param[in] initval The initial value for the internal counter of the eventfd
     * @param[in] flags for the creation of the internal eventfd.
     */
    event_fd_base(unsigned int initval, int flags);

    /**
     * @brief Declared virtual to ensure proper cleanup via base class pointer,
     * but defaulted to use the compiler-generated implementation.
     */
    virtual ~event_fd_base() = default;

    event_fd_base(const event_fd_base&) = delete;
    event_fd_base& operator=(const event_fd_base&) = delete;

    event_fd_base(event_fd_base&&) = default;
    event_fd_base& operator=(event_fd_base&&) = default;

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
     * @brief Check if an event is held by this object
     * @details Compares internally to \ref unique_fd::NO_DESCRIPTOR_SENTINEL
     * @return True if an event filedescriptor is held, false otherwise
     */
    bool valid() const;

    /**
     * @brief Read from the eventfd
     * @details This returns the value of the eventfd internal counter.
     * Return immediately is the counter is non-zero. Depending on wether the eventfd
     * is used as a semaphore or not, calling this function either decrements the internal counter
     * (semaphore) or resets it to zero.
     * @return The value of the event counter read from the eventfd.
     * If the event cannot be read, the optional is a 'nullopt'
     */
    std::optional<std::uint64_t> read();

    /**
     * @brief Write to the eventfd
     * @details Adds 'data' to the eventfds internal counter.
     * This call blocks if adding 'data' to the internal counter would exceed the maximum value.
     * A call to read() is necessary to unblock.
     * @param[in] data Payload of the event
     * @return True on success, false otherwise
     */
    bool write(std::uint64_t data);

    /**
     * @brief Add a single event with default payload '1' to the event queue
     * @details Calles \ref write(1) internally.
     * @return True on success, false otherwise
     */
    bool notify();

private:
    unique_fd m_fd;
};

/**
 * event_fd creates a blocking eventfd with initial value '0'
 * The lifetime of the event is bound to the lifetime of this object.
 */
class event_fd : public event_fd_base {
public:
    event_fd();
};

/**
 * event_fd creates a blocking eventfd as semaphore with initial value '0'
 * The lifetime of the event is bound to the lifetime of this object.
 */
class semaphore_fd : public event_fd_base {
public:
    semaphore_fd();
};

} // namespace everest::lib::io::event
