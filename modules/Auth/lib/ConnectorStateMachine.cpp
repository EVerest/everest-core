// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ConnectorStateMachine.hpp>

namespace module {

ConnectorStateMachine::ConnectorStateMachine() {

    this->controller = std::make_shared<ConnectorStateMachineController>();

    this->sd_available.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch (ev.id) {
        case ConnectorStatusTransition::TRANSACTION_STARTED:
            return trans.set(this->sd_occupied);
        case ConnectorStatusTransition::FAULTED:
            return trans.set(this->sd_faulted);
        case ConnectorStatusTransition::DISABLE:
            return trans.set(this->sd_unavailable);
        default:
            return;
        }
    };

    this->sd_available.handler = [this](FSMContextType& ctx) {
        this->state = ConnectorState::AVAILABLE;
    };

    this->sd_occupied.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch (ev.id) {
        case ConnectorStatusTransition::SESSION_FINISHED:
            return trans.set(this->sd_available);
        case ConnectorStatusTransition::FAULTED:
            return trans.set(this->sd_faulted_occupied);
        default:
            return;
        }
    };

    this->sd_occupied.handler = [this](FSMContextType& ctx) {
        this->state = ConnectorState::OCCUPIED;
    };

    this->sd_faulted.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch(ev.id) {
            case ConnectorStatusTransition::ERROR_CLEARED:
                return trans.set(this->sd_available);
            case ConnectorStatusTransition::DISABLE:
                return trans.set(this->sd_unavailable_faulted);
            
        }
    };

    this->sd_faulted.handler = [this](FSMContextType& ctx) {
        this->state = ConnectorState::FAULTED;
    };

    this->sd_unavailable.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch(ev.id) {
            case ConnectorStatusTransition::ENABLE:
                return trans.set(this->sd_available);
            case ConnectorStatusTransition::FAULTED:
                return trans.set(this->sd_unavailable_faulted);
            
        }
    };

    this->sd_unavailable.handler = [this](FSMContextType& ctx) {
        this->state = ConnectorState::UNAVAILABLE;
    };

    this->sd_unavailable_faulted.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch(ev.id) {
            case ConnectorStatusTransition::ENABLE:
                return trans.set(this->sd_faulted);
            case ConnectorStatusTransition::ERROR_CLEARED:
                return trans.set(this->sd_unavailable);
            
        }
    };

    this->sd_unavailable_faulted.handler = [this](FSMContextType& ctx) {
        this->state = ConnectorState::UNAVAILABLE_FAULTED;
    };

    this->sd_faulted_occupied.transitions = [this](const EventBaseType& ev, TransitionType& trans) {
        switch(ev.id) {
            case ConnectorStatusTransition::SESSION_FINISHED:
                return trans.set(this->sd_faulted);
            case ConnectorStatusTransition::ERROR_CLEARED:
                return trans.set(this->sd_occupied);
            
        }
    };

    this->sd_faulted_occupied.handler = [this](FSMContextType& ctx) {
        this->state = ConnectorState::FAULTED_OCCUPIED;
    };
}

ConnectorState ConnectorStateMachine::get_state() const {
    return this->state;
}

} // namespace module
