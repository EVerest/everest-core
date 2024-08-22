// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include "generated/types/ocpp.hpp"
#include <string>

// Required to use for std::unordered_map with types::ocpp::ComponentVariable as key
namespace types::ocpp {
bool operator==(const EVSE& lhs, const EVSE& rhs);
bool operator==(const Component& lhs, const Component& rhs);
bool operator==(const Variable& lhs, const Variable& rhs);
bool operator==(const ComponentVariable& lhs, const ComponentVariable& rhs);

} // namespace types::ocpp

template <> struct std::hash<types::ocpp::EVSE> {
    std::size_t operator()(const types::ocpp::EVSE& evse) const noexcept {
        return std::hash<std::size_t>{}(evse.id) ^ (std::hash<std::optional<std::size_t>>{}(evse.connector_id) << 1);
    }
};

template <> struct std::hash<types::ocpp::Component> {
    std::size_t operator()(const types::ocpp::Component& component) const noexcept {
        return std::hash<std::string>{}(component.name) ^
               (std::hash<std::optional<std::string>>{}(component.instance) << 1 ^
                (std::hash<std::optional<types::ocpp::EVSE>>{}(component.evse) << 2));
    }
};

template <> struct std::hash<types::ocpp::Variable> {
    std::size_t operator()(const types::ocpp::Variable& variable) const noexcept {
        return std::hash<std::string>{}(variable.name) ^
               (std::hash<std::optional<std::string>>{}(variable.instance) << 1);
    }
};

template <> struct std::hash<types::ocpp::ComponentVariable> {
    std::size_t operator()(const types::ocpp::ComponentVariable& component_variable) const noexcept {
        return std::hash<types::ocpp::Component>{}(component_variable.component) ^
               (std::hash<types::ocpp::Variable>{}(component_variable.variable) << 1);
    }
};

namespace module {

/**
 * Data structure to hold the user configuration mapping information.
 */
struct EverestConfigMapping {
    std::string module_id;    /// The module id as defined in the everest config schema.
    std::string config_param; /// The key for the `config_module` mapping in the config file.
};

} // namespace module
