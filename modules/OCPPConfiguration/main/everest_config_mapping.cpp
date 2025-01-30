// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "everest_config_mapping.hpp"

namespace types::ocpp {

bool operator==(const EVSE& lhs, const EVSE& rhs) {
    return lhs.id == rhs.id && lhs.connector_id == rhs.connector_id;
}

bool operator==(const Component& lhs, const Component& rhs) {
    return lhs.name == rhs.name && lhs.instance == rhs.instance && lhs.evse == rhs.evse;
}

bool operator==(const Variable& lhs, const Variable& rhs) {
    return lhs.name == rhs.name && lhs.instance == rhs.instance;
}

bool operator==(const ComponentVariable& lhs, const ComponentVariable& rhs) {
    return lhs.component == rhs.component && lhs.variable == rhs.variable;
}

} // namespace types::ocpp
