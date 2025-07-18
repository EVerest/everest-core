// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <device_model/composed_device_model_storage.hpp>

static constexpr auto VARIABLE_SOURCE_OCPP = "OCPP";

namespace module::device_model {
ComposedDeviceModelStorage::ComposedDeviceModelStorage() {
}

bool ComposedDeviceModelStorage::register_device_model_storage(
    std::string device_model_storage_id, std::shared_ptr<ocpp::v2::DeviceModelStorageInterface> device_model_storage) {
    if (this->device_model_storages.find(device_model_storage_id) != this->device_model_storages.end()) {
        return false;
    }

    const auto device_model_map = device_model_storage->get_device_model();
    // store the sources of each variable to be able to lookup requests to the device model storage
    for (const auto& [component, variable_map] : device_model_map) {
        for (const auto& [variable, variable_meta] : variable_map) {
            // check if component variable source is already exist in the map
            if (this->component_variable_source_map.find(component) != this->component_variable_source_map.end() &&
                this->component_variable_source_map.at(component).find(variable) !=
                    this->component_variable_source_map.at(component).end()) {
                EVLOG_warning << "Component variable source already exists for component: " << component.name
                              << ", variable: " << variable.name << ". Fix your device model configuration.";
            }

            // Note: Source should not be optional, should be changed in libocpp
            this->component_variable_source_map[component][variable] =
                variable_meta.source.value_or(VARIABLE_SOURCE_OCPP);
        }
    }

    this->device_model_storages[device_model_storage_id] = device_model_storage;
    return true;
}

ocpp::v2::DeviceModelMap ComposedDeviceModelStorage::get_device_model() {
    ocpp::v2::DeviceModelMap device_model_map;
    for (const auto& [name, device_model_storage] : this->device_model_storages) {
        const auto& partial_device_model = device_model_storage->get_device_model();
        for (const auto& [component, variable_map] : partial_device_model) {
            auto& existing_variable_map = device_model_map[component]; // Inserts if not present
            // Merge variable_map into existing_variable_map
            for (const auto& [variable, variable_meta_data] : variable_map) {
                if (existing_variable_map.find(variable) != existing_variable_map.end()) {
                    EVLOG_warning << "Variable " << variable.name << " already exists in component " << component.name
                                  << " but is also defined in device model storage: " << name;
                    EVLOG_AND_THROW(std::runtime_error(
                        "Variable already exists in component. Fix your device model configuration."));
                }
                existing_variable_map[variable] = variable_meta_data; // Overwrite or insert
            }
        }
    }
    return device_model_map;
}

std::optional<ocpp::v2::VariableAttribute>
ComposedDeviceModelStorage::get_variable_attribute(const ocpp::v2::Component& component_id,
                                                   const ocpp::v2::Variable& variable_id,
                                                   const ocpp::v2::AttributeEnum& attribute_enum) {
    const auto variable_source = get_variable_source(component_id, variable_id);
    if (this->device_model_storages.find(variable_source) == this->device_model_storages.end()) {
        return std::nullopt;
    }
    return this->device_model_storages.at(variable_source)
        ->get_variable_attribute(component_id, variable_id, attribute_enum);
}

std::vector<ocpp::v2::VariableAttribute>
ComposedDeviceModelStorage::get_variable_attributes(const ocpp::v2::Component& component_id,
                                                    const ocpp::v2::Variable& variable_id,
                                                    const std::optional<ocpp::v2::AttributeEnum>& attribute_enum) {
    const auto variable_source = get_variable_source(component_id, variable_id);
    if (this->device_model_storages.find(variable_source) == this->device_model_storages.end()) {
        return {};
    }
    return this->device_model_storages.at(variable_source)
        ->get_variable_attributes(component_id, variable_id, attribute_enum);
}

ocpp::v2::SetVariableStatusEnum ComposedDeviceModelStorage::set_variable_attribute_value(
    const ocpp::v2::Component& component_id, const ocpp::v2::Variable& variable_id,
    const ocpp::v2::AttributeEnum& attribute_enum, const std::string& value, const std::string& source) {
    // the "source" parameter is the VALUE_SOURCE
    const auto variable_source = get_variable_source(component_id, variable_id);
    if (this->device_model_storages.find(variable_source) == this->device_model_storages.end()) {
        return ocpp::v2::SetVariableStatusEnum::Rejected;
    }
    return this->device_model_storages.at(variable_source)
        ->set_variable_attribute_value(component_id, variable_id, attribute_enum, value, source);
}

std::optional<ocpp::v2::VariableMonitoringMeta>
ComposedDeviceModelStorage::set_monitoring_data(const ocpp::v2::SetMonitoringData& data,
                                                const ocpp::v2::VariableMonitorType type) {
    return this->device_model_storages.at(VARIABLE_SOURCE_OCPP)->set_monitoring_data(data, type);
}

bool ComposedDeviceModelStorage::update_monitoring_reference(const int32_t monitor_id,
                                                             const std::string& reference_value) {
    return this->device_model_storages.at(VARIABLE_SOURCE_OCPP)
        ->update_monitoring_reference(monitor_id, reference_value);
}

std::vector<ocpp::v2::VariableMonitoringMeta>
ComposedDeviceModelStorage::get_monitoring_data(const std::vector<ocpp::v2::MonitoringCriterionEnum>& criteria,
                                                const ocpp::v2::Component& component_id,
                                                const ocpp::v2::Variable& variable_id) {
    const auto variable_source = get_variable_source(component_id, variable_id);
    if (this->device_model_storages.find(variable_source) == this->device_model_storages.end()) {
        return {};
    }
    return this->device_model_storages.at(variable_source)->get_monitoring_data(criteria, component_id, variable_id);
}

ocpp::v2::ClearMonitoringStatusEnum ComposedDeviceModelStorage::clear_variable_monitor(int monitor_id,
                                                                                       bool allow_protected) {
    return this->device_model_storages.at(VARIABLE_SOURCE_OCPP)->clear_variable_monitor(monitor_id, allow_protected);
}

int32_t ComposedDeviceModelStorage::clear_custom_variable_monitors() {
    return this->device_model_storages.at(VARIABLE_SOURCE_OCPP)->clear_custom_variable_monitors();
}

void ComposedDeviceModelStorage::check_integrity() {
    for (const auto& [name, device_model_storage] : this->device_model_storages) {
        device_model_storage->check_integrity();
    }
}

std::string module::device_model::ComposedDeviceModelStorage::get_variable_source(const ocpp::v2::Component& component,
                                                                                  const ocpp::v2::Variable& variable) {
    if (this->component_variable_source_map.find(component) == this->component_variable_source_map.end()) {
        return VARIABLE_SOURCE_OCPP; // default source
    }
    const auto& variable_map = this->component_variable_source_map.at(component);
    if (variable_map.find(variable) == variable_map.end()) {
        return VARIABLE_SOURCE_OCPP; // default source
    }
    return variable_map.at(variable);
}

} // namespace module::device_model
