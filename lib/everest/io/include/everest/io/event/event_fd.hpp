// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once

#include "unique_fd.hpp"
#include <cstdint>
#include <optional>

namespace everest::lib::io::event {

/**
 * event_fd creates an <a href="https://man7.org/linux/man-pages/man2/eventfd.2.html">event</a>.
 * The lifetime of the event is bound to the lifetime of this object. Events can be queued
 */
class event_fd {
public:
    /**
     * @brief Constructor
     */
    event_fd();

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
     * @brief Read from the event queue
     * @details Read an event from the event queue. Poll on this object will
     * return immediately until all events have been handled.
     * @return The value read from the event. This is the value previously written with \ref write
     * If the event cannot be read, the optional is a 'nullopt'
     */
    std::optional<std::uint64_t> read();

    /**
     * @brief Write to the event queue
     * @details Add an element to the event queue
     * @param[in] data Payload of the event
     * @return True on success, false otherwise
     */
    bool write(std::uint64_t data);

    /**
     * @brief Add a single event with default payload '1' to the event queue
     * @details Calles \ref write internally
     * @return True on success, false otherwise
     */
    bool notify();

private:
    unique_fd m_fd;
};

} // namespace everest::lib::io::event
