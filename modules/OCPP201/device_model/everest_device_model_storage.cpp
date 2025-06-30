// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <device_model/definitions.hpp>
#include <device_model/everest_device_model_storage.hpp>
#include <ocpp/v2/init_device_model_db.hpp>
#include <ocpp/v2/ocpp_types.hpp>

using ocpp::v2::Component;
using ocpp::v2::DataEnum;
using ocpp::v2::EVSE;
using ocpp::v2::Variable;
using ocpp::v2::VariableAttribute;
using ocpp::v2::VariableCharacteristics;
using ocpp::v2::VariableMap;
using ocpp::v2::VariableMetaData;

static constexpr auto VARIABLE_SOURCE_EVEREST = "EVEREST";

using ocpp::v2::ComponentKey;
using ocpp::v2::DbVariableAttribute;
using ocpp::v2::DeviceModelVariable;

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

ComponentKey get_evse_component_key(const int32_t evse_id) {
    ComponentKey component;
    component.name = "EVSE";
    component.evse_id = evse_id;
    return component;
}

ComponentKey get_connector_component_key(const int32_t evse_id, const int32_t connector_id) {
    ComponentKey component;
    component.name = "Connector";
    component.evse_id = evse_id;
    component.connector_id = connector_id;
    return component;
}

// Helper function to construct DeviceModelVariable with common structure
DeviceModelVariable make_variable(const std::string& name, const ocpp::v2::VariableCharacteristics& characteristics,
                                  const std::string& value = "") {
    DeviceModelVariable var_data;
    var_data.name = name;
    var_data.characteristics = characteristics;
    var_data.source = VARIABLE_SOURCE_EVEREST;

    DbVariableAttribute db_attr;
    VariableAttribute attr;
    attr.type = ocpp::v2::AttributeEnum::Actual;
    attr.value = value;
    attr.mutability = ocpp::v2::MutabilityEnum::ReadOnly;
    db_attr.variable_attribute = attr;

    var_data.attributes.push_back(db_attr);
    return var_data;
}

// Populates EVSE variables
std::vector<DeviceModelVariable> build_evse_variables(const float max_power) {
    std::vector<DeviceModelVariable> variables;

    auto evse_power_characteristics = EvseDefinitions::Characteristics::EVSEPower;
    evse_power_characteristics.maxLimit = max_power;

    return {make_variable(ocpp::v2::EvseComponentVariables::Available.name, EvseDefinitions::Characteristics::Available,
                          "true"),
            make_variable(ocpp::v2::EvseComponentVariables::AvailabilityState.name,
                          EvseDefinitions::Characteristics::AvailabilityState, "Available"),
            make_variable(ocpp::v2::EvseComponentVariables::Power.name, evse_power_characteristics),
            make_variable(ocpp::v2::EvseComponentVariables::SupplyPhases.name,
                          EvseDefinitions::Characteristics::SupplyPhases),
            make_variable(ocpp::v2::EvseComponentVariables::AllowReset.name,
                          EvseDefinitions::Characteristics::AllowReset, "false"),
            make_variable(ocpp::v2::EvseComponentVariables::ISO15118EvseId.name,
                          EvseDefinitions::Characteristics::ISO15118EvseId, "DEFAULT_EVSE_ID")};
}

// Populates Connector variables
std::vector<DeviceModelVariable> build_connector_variables() {

    return {make_variable(ocpp::v2::ConnectorComponentVariables::Available.name,
                          ConnectorDefinitions::Characteristics::Available, "true"),
            make_variable(ocpp::v2::ConnectorComponentVariables::AvailabilityState.name,
                          ConnectorDefinitions::Characteristics::AvailabilityState, "Available"),
            make_variable(ocpp::v2::ConnectorComponentVariables::Type.name,
                          ConnectorDefinitions::Characteristics::ConnectorType),
            make_variable(ocpp::v2::ConnectorComponentVariables::SupplyPhases.name,
                          ConnectorDefinitions::Characteristics::SupplyPhases)};
}

} // anonymous namespace

EverestDeviceModelStorage::EverestDeviceModelStorage(
    const std::vector<std::unique_ptr<evse_managerIntf>>& r_evse_manager,
    const std::map<int32_t, types::evse_board_support::HardwareCapabilities>& evse_hardware_capabilities_map,
    const std::filesystem::path& db_path, const std::filesystem::path& migration_files_path) :
    r_evse_manager(r_evse_manager) {

    std::map<ComponentKey, std::vector<DeviceModelVariable>> component_configs;

    for (const auto& evse_manager : r_evse_manager) {
        const auto evse_info = evse_manager->call_get_evse();
        const auto& hw_capabilities = evse_hardware_capabilities_map.at(evse_info.id);

        ComponentKey evse_component_key = get_evse_component_key(evse_info.id);
        const auto max_power = hw_capabilities.max_current_A_import * 230.0f * hw_capabilities.max_phase_count_import;
        component_configs[evse_component_key] = build_evse_variables(max_power);

        for (const auto& connector : evse_info.connectors) {
            ComponentKey connector_component_key = get_connector_component_key(evse_info.id, connector.id);
            component_configs[connector_component_key] = build_connector_variables();
        }
    }

    ocpp::v2::InitDeviceModelDb init_device_model_db(db_path, migration_files_path);
    init_device_model_db.initialize_database(component_configs, false);
    init_device_model_db.close_connection();
    this->device_model_storage = std::make_unique<ocpp::v2::DeviceModelStorageSqlite>(db_path);

    this->init_hw_capabilities(evse_hardware_capabilities_map);
}

void EverestDeviceModelStorage::init_hw_capabilities(
    const std::map<int32_t, types::evse_board_support::HardwareCapabilities>& evse_hardware_capabilities_map) {
    for (const auto& evse_manager : r_evse_manager) {
        const auto evse_info = evse_manager->call_get_evse();
        Component evse_component = get_evse_component(evse_info.id);

        if (evse_hardware_capabilities_map.find(evse_info.id) != evse_hardware_capabilities_map.end()) {
            this->update_hw_capabilities(evse_component, evse_hardware_capabilities_map.at(evse_info.id));
        } else {
            EVLOG_error << "No hardware capabilities found for EVSE with ID " << evse_info.id;
        }

        evse_manager->subscribe_hw_capabilities(
            [this, evse_component](const types::evse_board_support::HardwareCapabilities hw_capabilities) {
                this->update_hw_capabilities(evse_component, hw_capabilities);
            });

        for (const auto& connector : evse_info.connectors) {
            if (connector.type.has_value()) {
                const auto component = get_connector_component(evse_info.id, connector.id);
                std::lock_guard<std::mutex> lock(device_model_mutex);
                this->device_model_storage->set_variable_attribute_value(
                    component, ocpp::v2::ConnectorComponentVariables::Type, ocpp::v2::AttributeEnum::Actual,
                    types::evse_manager::connector_type_enum_to_string(connector.type.value()),
                    VARIABLE_SOURCE_EVEREST);
            }
        }
    }
}

void EverestDeviceModelStorage::update_hw_capabilities(
    const Component& evse_component, const types::evse_board_support::HardwareCapabilities& hw_capabilities) {
    std::lock_guard<std::mutex> lock(device_model_mutex);
    this->device_model_storage->set_variable_attribute_value(
        evse_component, ocpp::v2::EvseComponentVariables::SupplyPhases, ocpp::v2::AttributeEnum::Actual,
        std::to_string(hw_capabilities.max_phase_count_import), VARIABLE_SOURCE_EVEREST);
    // TODO: update EVSE.Power maxLimit value once device model storage interface supports it
}

void EverestDeviceModelStorage::update_power(const int32_t evse_id, const float total_power_active_import) {
    std::lock_guard<std::mutex> lock(device_model_mutex);
    Component evse_component = get_evse_component(evse_id);
    this->device_model_storage->set_variable_attribute_value(
        evse_component, ocpp::v2::EvseComponentVariables::Power, ocpp::v2::AttributeEnum::Actual,
        std::to_string(total_power_active_import), VARIABLE_SOURCE_EVEREST);
}

ocpp::v2::DeviceModelMap EverestDeviceModelStorage::get_device_model() {
    std::lock_guard<std::mutex> lock(device_model_mutex);
    return this->device_model_storage->get_device_model();
}

std::optional<ocpp::v2::VariableAttribute>
EverestDeviceModelStorage::get_variable_attribute(const ocpp::v2::Component& component_id,
                                                  const ocpp::v2::Variable& variable_id,
                                                  const ocpp::v2::AttributeEnum& attribute_enum) {
    std::lock_guard<std::mutex> lock(device_model_mutex);
    return this->device_model_storage->get_variable_attribute(component_id, variable_id, attribute_enum);
}

std::vector<ocpp::v2::VariableAttribute>
EverestDeviceModelStorage::get_variable_attributes(const ocpp::v2::Component& component_id,
                                                   const ocpp::v2::Variable& variable_id,
                                                   const std::optional<ocpp::v2::AttributeEnum>& attribute_enum) {
    std::lock_guard<std::mutex> lock(device_model_mutex);
    return this->device_model_storage->get_variable_attributes(component_id, variable_id, attribute_enum);
}

ocpp::v2::SetVariableStatusEnum EverestDeviceModelStorage::set_variable_attribute_value(
    const ocpp::v2::Component& component_id, const ocpp::v2::Variable& variable_id,
    const ocpp::v2::AttributeEnum& attribute_enum, const std::string& value, const std::string& source) {
    std::lock_guard<std::mutex> lock(device_model_mutex);

    int evse_id = 0;
    if (component_id.evse.has_value()) {
        evse_id = component_id.evse.value().id;
    }

    // FIXME: device_model_storage->set_variable_attribute_value does only return accepted or rejected, no other checks
    // are performed. Since libocpp contains the full device model in memory and does these checks independently, it's
    // currently only a minor issue.
    return this->device_model_storage->set_variable_attribute_value(component_id, variable_id, attribute_enum, value,
                                                                    source);
}

std::optional<ocpp::v2::VariableMonitoringMeta>
EverestDeviceModelStorage::set_monitoring_data(const ocpp::v2::SetMonitoringData& data,
                                               const ocpp::v2::VariableMonitorType type) {
    std::lock_guard<std::mutex> lock(device_model_mutex);
    return this->device_model_storage->set_monitoring_data(data, type);
}

bool EverestDeviceModelStorage::update_monitoring_reference(const int32_t monitor_id,
                                                            const std::string& reference_value) {
    std::lock_guard<std::mutex> lock(device_model_mutex);
    return this->device_model_storage->update_monitoring_reference(monitor_id, reference_value);
}

std::vector<ocpp::v2::VariableMonitoringMeta>
EverestDeviceModelStorage::get_monitoring_data(const std::vector<ocpp::v2::MonitoringCriterionEnum>& criteria,
                                               const ocpp::v2::Component& component_id,
                                               const ocpp::v2::Variable& variable_id) {
    std::lock_guard<std::mutex> lock(device_model_mutex);
    return this->device_model_storage->get_monitoring_data(criteria, component_id, variable_id);
}

ocpp::v2::ClearMonitoringStatusEnum EverestDeviceModelStorage::clear_variable_monitor(int monitor_id,
                                                                                      bool allow_protected) {
    std::lock_guard<std::mutex> lock(device_model_mutex);
    return this->device_model_storage->clear_variable_monitor(monitor_id, allow_protected);
}

int32_t EverestDeviceModelStorage::clear_custom_variable_monitors() {
    std::lock_guard<std::mutex> lock(device_model_mutex);
    return this->device_model_storage->clear_custom_variable_monitors();
}

void EverestDeviceModelStorage::check_integrity() {
}
} // namespace module::device_model
