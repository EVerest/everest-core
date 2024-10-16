// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef _CONNECTOR_STATE_MACHINE_HPP_
#define _CONNECTOR_STATE_MACHINE_HPP_

#include <memory>
#include <string>

namespace module {

enum class ConnectorEvent {
    ENABLE,
    DISABLE,
    ERROR_CLEARED,
    FAULTED,
    TRANSACTION_STARTED,
    SESSION_FINISHED
};

/// @warning Do not change the order of ConnectorState, or if you do it, fix the code in ReservationHandler.
enum class ConnectorState {
    AVAILABLE,
    UNAVAILABLE,
    FAULTED,
    OCCUPIED,
    UNAVAILABLE_FAULTED,
    FAULTED_OCCUPIED
};

class ConnectorStateMachine {
public:
    explicit ConnectorStateMachine(ConnectorState initial_state);

    bool handle_event(ConnectorEvent event);

    ConnectorState get_state() const;

private:
    ConnectorState state;
};

} // namespace module

#endif //_CONNECTOR_STATE_MACHINE_HPP_
