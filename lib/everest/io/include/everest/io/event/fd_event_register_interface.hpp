// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once

namespace everest::lib::io::event {

class fd_event_handler;

/**
 * Interface for classes implementing registration with existing fd_event_handler
 */
class fd_event_register_interface {
public:
    virtual ~fd_event_register_interface() = default;

    /**
     * @brief Register with existing event handler
     * @param[in] handler The event handler to register with
     * @return true on success, false otherwise
     */
    virtual bool register_events(fd_event_handler& handler) = 0;
    /**
     * @brief Unregister from existing event handler
     * @param[in] handler The event handler to unregister from
     * @return true on success, false otherwise
     */
    virtual bool unregister_events(fd_event_handler& handler) = 0;
};

} // namespace everest::lib::io::event
