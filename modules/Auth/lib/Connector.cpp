// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <Connector.hpp>

namespace module {

void Connector::submit_event(ConnectorEvent event) {
    state_machine.handle_event(event);
}

ConnectorState Connector::get_state() const {
    return this->state_machine.get_state();
}

bool Connector::is_unavailable() {
    return this->get_state() == ConnectorState::UNAVAILABLE || this->get_state() == ConnectorState::UNAVAILABLE_FAULTED;
}

namespace conversions {
std::string connector_state_to_string(const ConnectorState& state) {
    switch (state) {
    case ConnectorState::AVAILABLE:
        return "AVAILABLE";
    case ConnectorState::OCCUPIED:
        return "OCCUPIED";
    case ConnectorState::UNAVAILABLE:
        return "UNAVAILABLE";
    case ConnectorState::FAULTED:
        return "FAULTED";
    case ConnectorState::FAULTED_OCCUPIED:
        return "FAULTED_OCCUPIED";
    case ConnectorState::UNAVAILABLE_FAULTED:
        return "UNAVAILABLE_FAULTED";
    default:
        throw std::runtime_error("No known conversion for the given connector state");
    }
}

} // namespace conversions
} // namespace module
