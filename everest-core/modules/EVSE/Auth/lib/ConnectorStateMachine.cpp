// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ConnectorStateMachine.hpp>

namespace module {

ConnectorStateMachine::ConnectorStateMachine(ConnectorState initial_state) : state(initial_state) {
}

bool ConnectorStateMachine::handle_event(ConnectorEvent event) {
    switch (state) {
    case ConnectorState::AVAILABLE:
        switch (event) {
        case ConnectorEvent::TRANSACTION_STARTED:
            state = ConnectorState::OCCUPIED;
            return true;
        case ConnectorEvent::DISABLE:
            state = ConnectorState::UNAVAILABLE;
            return true;
        case ConnectorEvent::FAULTED:
            state = ConnectorState::FAULTED;
            return true;
        default:
            return false;
        }

    case ConnectorState::UNAVAILABLE:
        switch (event) {
        case ConnectorEvent::ENABLE:
            state = ConnectorState::AVAILABLE;
            return true;
        case ConnectorEvent::FAULTED:
            state = ConnectorState::UNAVAILABLE_FAULTED;
            return true;
        default:
            return false;
        }

    case ConnectorState::FAULTED:
        switch (event) {
        case ConnectorEvent::ERROR_CLEARED:
            state = ConnectorState::AVAILABLE;
            return true;
        case ConnectorEvent::DISABLE:
            state = ConnectorState::UNAVAILABLE_FAULTED;
            return true;
        default:
            return false;
        }

    case ConnectorState::OCCUPIED:
        switch (event) {
        case ConnectorEvent::SESSION_FINISHED:
            state = ConnectorState::AVAILABLE;
            return true;
        case ConnectorEvent::FAULTED:
            state = ConnectorState::FAULTED_OCCUPIED;
            return true;
        default:
            return false;
        }

    case ConnectorState::FAULTED_OCCUPIED:
        switch (event) {
        case ConnectorEvent::ERROR_CLEARED:
            state = ConnectorState::OCCUPIED;
            return true;
        case ConnectorEvent::SESSION_FINISHED:
            state = ConnectorState::FAULTED;
            return true;
        default:
            return false;
        }

    case ConnectorState::UNAVAILABLE_FAULTED:
        switch (event) {
        case ConnectorEvent::ENABLE:
            state = ConnectorState::FAULTED;
            return true;
        case ConnectorEvent::ERROR_CLEARED:
            state = ConnectorState::UNAVAILABLE;
            return true;
        default:
            return false;
        }

    default:
        // could/should not happen!
        return false;
    }
}

ConnectorState ConnectorStateMachine::get_state() const {
    return this->state;
}

} // namespace module
