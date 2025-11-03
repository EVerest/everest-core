// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once

namespace everest::lib::io::event {

/**
 * @brief Possible outcomes of syncing
 */
enum class sync_status {
    /** Sync was successful */
    ok,
    /** Sync timed out */
    timeout,
    /** Sync was not successful*/
    error
};

/**
 * Interface for classes implementing default syncing (E/POLLIN only)for fd_event_handler
 */
class fd_event_sync_interface {
public:
    virtual ~fd_event_sync_interface() = default;

    /**
     * @brief Access to the internal event handler
     * @details Call \ref sync on read (E/POLLIN).
     * Override if an additional layer of event handler is necessary.
     * @return The file descriptor of the internal event handler
     */
    virtual int get_poll_fd() = 0;

    /**
     * @brief Sync internal event handler
     * @details Blocks until an event occurs.
     * @return Result of sync operation
     */
    virtual everest::lib::io::event::sync_status sync() = 0;
};

} // namespace everest::lib::io::event
