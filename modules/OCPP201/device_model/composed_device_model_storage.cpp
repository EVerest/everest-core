// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <device_model/composed_device_model_storage.hpp>

static constexpr auto VARIABLE_SOURCE_OCPP = "OCPP";

namespace module::device_model {
ComposedDeviceModelStorage::ComposedDeviceModelStorage() {
}

bool ComposedDeviceModelStorage::register_device_model_storage(
    std::string device_model_storage_id, std::unique_ptr<ocpp::v2::DeviceModelStorageInterface> device_model_storage) {
    if (this->device_model_storages.find(device_model_storage_id) != this->device_model_storages.end()) {
        return false;
    }
    this->device_model_storages[device_model_storage_id] = std::move(device_model_storage);
    return true;
}

ocpp::v2::DeviceModelMap ComposedDeviceModelStorage::get_device_model() {
    for (const auto& [name, device_model_storage] : this->device_model_storages) {
        this->device_model_map.merge(device_model_storage->get_device_model());
    }
    return this->device_model_map;
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

bool ComposedDeviceModelStorage::set_variable_attribute_value(const ocpp::v2::Component& component_id,
                                                              const ocpp::v2::Variable& variable_id,
                                                              const ocpp::v2::AttributeEnum& attribute_enum,
                                                              const std::string& value, const std::string& source) {
    // the "source" parameter is the VALUE_SOURCE
    const auto variable_source = get_variable_source(component_id, variable_id);
    if (this->device_model_storages.find(variable_source) == this->device_model_storages.end()) {
        return false;
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
    std::optional<std::string> variable_source = device_model_map[component][variable].source;
    if (variable_source.has_value() && variable_source.value() != VARIABLE_SOURCE_OCPP) {
        // For now, this just throws because we only have the libocpp source. When the config service is
        // implemented, this should not throw.
        throw ocpp::v2::DeviceModelError("Source is not 'OCPP', not sure what to do");
    }

    return VARIABLE_SOURCE_OCPP;
}

} // namespace module::device_model
