// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <device_model/composed_device_model_storage.hpp>

namespace module::device_model {
ComposedDeviceModelStorage::ComposedDeviceModelStorage(const std::string& libocpp_device_model_storage_address,
                                                       const bool libocpp_initialize_device_model,
                                                       const std::string& device_model_migration_path,
                                                       const std::string& device_model_config_path) :
    everest_device_model_storage(std::make_unique<EverestDeviceModelStorage>()),
    libocpp_device_model_storage(std::make_unique<ocpp::v201::DeviceModelStorageSqlite>(
        libocpp_device_model_storage_address, device_model_migration_path, device_model_config_path,
        libocpp_initialize_device_model)) {
}

ocpp::v201::DeviceModelMap ComposedDeviceModelStorage::get_device_model() {
    ocpp::v201::DeviceModelMap everest_dm = everest_device_model_storage->get_device_model();
    ocpp::v201::DeviceModelMap libocpp_dm = libocpp_device_model_storage->get_device_model();
    everest_dm.merge(libocpp_dm);
    return everest_dm;
}

std::optional<ocpp::v201::VariableAttribute>
ComposedDeviceModelStorage::get_variable_attribute(const ocpp::v201::Component& component_id,
                                                   const ocpp::v201::Variable& variable_id,
                                                   const ocpp::v201::AttributeEnum& attribute_enum) {
    return libocpp_device_model_storage->get_variable_attribute(component_id, variable_id, attribute_enum);
}

std::vector<ocpp::v201::VariableAttribute>
ComposedDeviceModelStorage::get_variable_attributes(const ocpp::v201::Component& component_id,
                                                    const ocpp::v201::Variable& variable_id,
                                                    const std::optional<ocpp::v201::AttributeEnum>& attribute_enum) {
    return libocpp_device_model_storage->get_variable_attributes(component_id, variable_id, attribute_enum);
}

bool ComposedDeviceModelStorage::set_variable_attribute_value(const ocpp::v201::Component& component_id,
                                                              const ocpp::v201::Variable& variable_id,
                                                              const ocpp::v201::AttributeEnum& attribute_enum,
                                                              const std::string& value, const std::string& source) {
    return libocpp_device_model_storage->set_variable_attribute_value(component_id, variable_id, attribute_enum, value,
                                                                      source);
}

std::optional<ocpp::v201::VariableMonitoringMeta>
ComposedDeviceModelStorage::set_monitoring_data(const ocpp::v201::SetMonitoringData& data,
                                                const ocpp::v201::VariableMonitorType type) {
    return libocpp_device_model_storage->set_monitoring_data(data, type);
}

bool ComposedDeviceModelStorage::update_monitoring_reference(const int32_t monitor_id,
                                                             const std::string& reference_value) {
    return libocpp_device_model_storage->update_monitoring_reference(monitor_id, reference_value);
}

std::vector<ocpp::v201::VariableMonitoringMeta>
ComposedDeviceModelStorage::get_monitoring_data(const std::vector<ocpp::v201::MonitoringCriterionEnum>& criteria,
                                                const ocpp::v201::Component& component_id,
                                                const ocpp::v201::Variable& variable_id) {
    return libocpp_device_model_storage->get_monitoring_data(criteria, component_id, variable_id);
}

ocpp::v201::ClearMonitoringStatusEnum ComposedDeviceModelStorage::clear_variable_monitor(int monitor_id,
                                                                                         bool allow_protected) {
    return libocpp_device_model_storage->clear_variable_monitor(monitor_id, allow_protected);
}

int32_t ComposedDeviceModelStorage::clear_custom_variable_monitors() {
    return libocpp_device_model_storage->clear_custom_variable_monitors();
}

void ComposedDeviceModelStorage::check_integrity() {
    everest_device_model_storage->check_integrity();
    libocpp_device_model_storage->check_integrity();
}
} // namespace module::device_model
