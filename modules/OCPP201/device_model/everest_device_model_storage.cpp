// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <device_model/definitions.hpp>
#include <device_model/everest_device_model_storage.hpp>
#include <ocpp/v2/ocpp_types.hpp>

using ocpp::v2::Component;
using ocpp::v2::DataEnum;
using ocpp::v2::EVSE;
using ocpp::v2::Variable;
using ocpp::v2::VariableAttribute;
using ocpp::v2::VariableCharacteristics;
using ocpp::v2::VariableMap;
using ocpp::v2::VariableMetaData;

namespace module::device_model {

namespace {

Component get_evse_component(const int32_t evse_id) {
    Component component;
    component.name = "EVSE";
    component.evse = EvseDefinitions::get_evse(evse_id);
    return component;
}

Component get_connector_component(const int32_t evse_id, const int32_t connector_id) {
    Component component;
    component.name = "Connector";
    component.evse = EvseDefinitions::get_evse(evse_id, connector_id);
    return component;
}

// Helper function to construct VariableData with common structure
VariableData make_variable(const ocpp::v2::VariableCharacteristics& characteristics, const std::string& value = "") {
    VariableData var_data;
    var_data.characteristics = characteristics;
    var_data.source = "Internal";

    VariableAttribute attr;
    attr.type = ocpp::v2::AttributeEnum::Actual;
    attr.value = value;
    attr.mutability = ocpp::v2::MutabilityEnum::ReadOnly;

    var_data.attributes[ocpp::v2::AttributeEnum::Actual] = attr;
    return var_data;
}

// Populates EVSE variables
Variables build_evse_variables() {
    return {{ocpp::v2::EvseComponentVariables::Available,
             make_variable(EvseDefinitions::Characteristics::Available, "true")},
            {ocpp::v2::EvseComponentVariables::AvailabilityState,
             make_variable(EvseDefinitions::Characteristics::AvailabilityState, "Available")},
            {ocpp::v2::EvseComponentVariables::Power, make_variable(EvseDefinitions::Characteristics::EVSEPower)},
            {ocpp::v2::EvseComponentVariables::SupplyPhases,
             make_variable(EvseDefinitions::Characteristics::SupplyPhases)}};
}

// Populates Connector variables
Variables build_connector_variables() {
    return {{ocpp::v2::ConnectorComponentVariables::Available,
             make_variable(ConnectorDefinitions::Characteristics::Available, "true")},
            {ocpp::v2::ConnectorComponentVariables::AvailabilityState,
             make_variable(ConnectorDefinitions::Characteristics::AvailabilityState, "Available")},
            {ocpp::v2::ConnectorComponentVariables::Type,
             make_variable(ConnectorDefinitions::Characteristics::ConnectorType)},
            {ocpp::v2::ConnectorComponentVariables::SupplyPhases,
             make_variable(ConnectorDefinitions::Characteristics::SupplyPhases, "3")}};
}

} // anonymous namespace

// Refactored Constructor
EverestDeviceModelStorage::EverestDeviceModelStorage(
    const std::vector<std::unique_ptr<evse_managerIntf>>& r_evse_manager) :
    r_evse_manager(r_evse_manager) {

    for (const auto& evse_manager : r_evse_manager) {
        const auto evse_info = evse_manager->call_get_evse();

        // TODO: variables:
        //  max power
        //  connector type

        // Build EVSE Component
        Component evse_component = get_evse_component(evse_info.id);
        this->components[evse_component] = build_evse_variables();

        evse_manager->subscribe_hw_capabilities([this, &evse_info](const types::evse_board_support::HardwareCapabilities hw_capabilities) {
            this->components[get_evse_component(evse_info.id)][ocpp::v2::EvseComponentVariables::SupplyPhases].attributes[ocpp::v2::AttributeEnum::Actual].value =
                std::to_string(hw_capabilities.max_phase_count_import);
            
        });

        this->components[get_evse_component(evse_info.id)][ocpp::v2::EvseComponentVariables::Power].characteristics.maxLimit = 10000.0f;

        // Build Connector Components
        for (const auto& connector : evse_info.connectors) {
            Component connector_component = get_connector_component(evse_info.id, connector.id);
            this->components[connector_component] = build_connector_variables();

            if (connector.type.has_value()) {
                this->components[connector_component][ocpp::v2::ConnectorComponentVariables::Type]
                .attributes[ocpp::v2::AttributeEnum::Actual]
                .value = types::evse_manager::connector_type_enum_to_string(connector.type.value());
            }
        }
    }
}

ocpp::v2::DeviceModelMap EverestDeviceModelStorage::get_device_model() {
    ocpp::v2::DeviceModelMap device_model;

    for (const auto& [component, variables] : components) {
        ocpp::v2::VariableMap variable_map;
        for (const auto& [variable, variable_data] : variables) {
            VariableMetaData meta_data = static_cast<VariableMetaData>(variable_data);
            variable_map[variable] = meta_data;
        }
        device_model[component] = variable_map;
    }

    return device_model;
}

std::optional<ocpp::v2::VariableAttribute>
EverestDeviceModelStorage::get_variable_attribute(const ocpp::v2::Component& component_id,
                                                  const ocpp::v2::Variable& variable_id,
                                                  const ocpp::v2::AttributeEnum& attribute_enum) {
    auto component_it = components.find(component_id);
    if (component_it == components.end()) {
        return std::nullopt;
    }
    auto variable_it = component_it->second.find(variable_id);
    if (variable_it == component_it->second.end()) {
        return std::nullopt;
    }
    auto variable_data = variable_it->second;
    auto attribute_it = variable_data.attributes.find(attribute_enum);
    if (attribute_it == variable_data.attributes.end()) {
        return std::nullopt;
    }
    return attribute_it->second;
}

std::vector<ocpp::v2::VariableAttribute>
EverestDeviceModelStorage::get_variable_attributes(const ocpp::v2::Component& component_id,
                                                   const ocpp::v2::Variable& variable_id,
                                                   const std::optional<ocpp::v2::AttributeEnum>& attribute_enum) {
    return {};
}

bool EverestDeviceModelStorage::set_variable_attribute_value(const ocpp::v2::Component& component_id,
                                                             const ocpp::v2::Variable& variable_id,
                                                             const ocpp::v2::AttributeEnum& attribute_enum,
                                                             const std::string& value, const std::string& source) {
    auto component_it = components.find(component_id);
    if (component_it == components.end()) {
        return ocpp::v2::SetVariableStatusEnum::UnknownComponent;
    }
    auto variable_it = component_it->second.find(variable_id);
    if (variable_it == component_it->second.end()) {
        return ocpp::v2::SetVariableStatusEnum::UnknownVariable;
    }
    auto& variable_data = variable_it->second;
    auto attribute_it = variable_data.attributes.find(attribute_enum);
    if (attribute_it == variable_data.attributes.end()) {
        return ocpp::v2::SetVariableStatusEnum::NotSupportedAttributeType;
    }
    auto& attribute = attribute_it->second;
    attribute.value = value;
    variable_data.source = source;
    return ocpp::v2::SetVariableStatusEnum::Accepted;
}

std::optional<ocpp::v2::VariableMonitoringMeta>
EverestDeviceModelStorage::set_monitoring_data(const ocpp::v2::SetMonitoringData& data,
                                               const ocpp::v2::VariableMonitorType type) {
    return std::nullopt;
}

bool EverestDeviceModelStorage::update_monitoring_reference(const int32_t monitor_id,
                                                            const std::string& reference_value) {
    return false;
}

std::vector<ocpp::v2::VariableMonitoringMeta>
EverestDeviceModelStorage::get_monitoring_data(const std::vector<ocpp::v2::MonitoringCriterionEnum>& criteria,
                                               const ocpp::v2::Component& component_id,
                                               const ocpp::v2::Variable& variable_id) {
    return {};
}

ocpp::v2::ClearMonitoringStatusEnum EverestDeviceModelStorage::clear_variable_monitor(int monitor_id,
                                                                                      bool allow_protected) {
    return ocpp::v2::ClearMonitoringStatusEnum::NotFound;
}

int32_t EverestDeviceModelStorage::clear_custom_variable_monitors() {
    return 0;
}

void EverestDeviceModelStorage::check_integrity() {
}
} // namespace module::device_model
