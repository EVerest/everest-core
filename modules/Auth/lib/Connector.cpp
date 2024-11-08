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

bool Connector::is_unavailable() const {
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

bool EVSEContext::is_available() {
    bool occupied = false;
    bool available = false;
    for (const auto& connector : this->connectors) {
        if (connector.get_state() == ConnectorState::OCCUPIED ||
            connector.get_state() == ConnectorState::FAULTED_OCCUPIED) {
            occupied = true;
        }
        if (connector.get_state() != ConnectorState::UNAVAILABLE && connector.get_state() != ConnectorState::FAULTED) {

            available = true;
        }
    }

    if (occupied) {
        // When at least one connector is occupied, they are both not available.
        return false;
    }

    return available;
}

bool EVSEContext::is_unavailable() {
    for (const auto& connector : this->connectors) {
        if (!connector.is_unavailable()) {
            return false;
        }
    }

    return true;
}

// namespace conversions

// namespace conversions
} // namespace module
