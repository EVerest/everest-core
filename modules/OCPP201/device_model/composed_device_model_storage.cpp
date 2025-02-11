// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <device_model/composed_device_model_storage.hpp>

static constexpr auto VARIABLE_SOURCE_OCPP = "OCPP";

namespace module::device_model {
ComposedDeviceModelStorage::ComposedDeviceModelStorage(const std::string& libocpp_device_model_storage_address,
                                                       const bool libocpp_initialize_device_model,
                                                       const std::string& device_model_migration_path,
                                                       const std::string& device_model_config_path) :
    everest_device_model_storage(std::make_unique<EverestDeviceModelStorage>()),
    libocpp_device_model_storage(std::make_unique<ocpp::v201::DeviceModelStorageSqlite>(
        libocpp_device_model_storage_address, device_model_migration_path, device_model_config_path,
        libocpp_initialize_device_model)),
    device_model_map(get_device_model()) {
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
    if (get_variable_source(component_id, variable_id) == VARIABLE_SOURCE_OCPP) {
        return libocpp_device_model_storage->get_variable_attribute(component_id, variable_id, attribute_enum);
    }

    return everest_device_model_storage->get_variable_attribute(component_id, variable_id, attribute_enum);
}

std::vector<ocpp::v201::VariableAttribute>
ComposedDeviceModelStorage::get_variable_attributes(const ocpp::v201::Component& component_id,
                                                    const ocpp::v201::Variable& variable_id,
                                                    const std::optional<ocpp::v201::AttributeEnum>& attribute_enum) {
    if (get_variable_source(component_id, variable_id) == VARIABLE_SOURCE_OCPP) {
        return libocpp_device_model_storage->get_variable_attributes(component_id, variable_id, attribute_enum);
    }

    return everest_device_model_storage->get_variable_attributes(component_id, variable_id, attribute_enum);
}

bool ComposedDeviceModelStorage::set_variable_attribute_value(const ocpp::v201::Component& component_id,
                                                              const ocpp::v201::Variable& variable_id,
                                                              const ocpp::v201::AttributeEnum& attribute_enum,
                                                              const std::string& value, const std::string& source) {
    if (get_variable_source(component_id, variable_id) == VARIABLE_SOURCE_OCPP) {
        return libocpp_device_model_storage->set_variable_attribute_value(component_id, variable_id, attribute_enum,
                                                                          value, source);
    }
    return everest_device_model_storage->set_variable_attribute_value(component_id, variable_id, attribute_enum, value,
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
    if (get_variable_source(component_id, variable_id) == VARIABLE_SOURCE_OCPP) {
        return libocpp_device_model_storage->get_monitoring_data(criteria, component_id, variable_id);
    }

    return everest_device_model_storage->get_monitoring_data(criteria, component_id, variable_id);
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

std::string
module::device_model::ComposedDeviceModelStorage::get_variable_source(const ocpp::v201::Component& component,
                                                                      const ocpp::v201::Variable& variable) {
    std::optional<std::string> variable_source = device_model_map[component][variable].source;
    if (variable_source.has_value() && variable_source.value() != VARIABLE_SOURCE_OCPP) {
        // For now, this just throws because we only have the libocpp source. When the config service is
        // implemented, this should not throw.
        throw ocpp::v201::DeviceModelError("Source is not 'OCPP', not sure what to do");
    }

    return VARIABLE_SOURCE_OCPP;
}

} // namespace module::device_model
