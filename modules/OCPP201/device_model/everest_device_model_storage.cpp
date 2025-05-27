// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <device_model/everest_device_model_storage.hpp>

namespace module::device_model {
ocpp::v2::DeviceModelMap EverestDeviceModelStorage::get_device_model() {
    return {};
}

std::optional<ocpp::v2::VariableAttribute>
EverestDeviceModelStorage::get_variable_attribute(const ocpp::v2::Component& /*component_id*/,
                                                  const ocpp::v2::Variable& /*variable_id*/,
                                                  const ocpp::v2::AttributeEnum& /*attribute_enum*/) {
    return std::nullopt;
}

std::vector<ocpp::v2::VariableAttribute>
EverestDeviceModelStorage::get_variable_attributes(const ocpp::v2::Component& /*component_id*/,
                                                   const ocpp::v2::Variable& /*variable_id*/,
                                                   const std::optional<ocpp::v2::AttributeEnum>& /*attribute_enum*/) {
    return {};
}

ocpp::v2::SetVariableStatusEnum EverestDeviceModelStorage::set_variable_attribute_value(
    const ocpp::v2::Component& component, const ocpp::v2::Variable& variable,
    const ocpp::v2::AttributeEnum& attribute_enum, const std::string& value, const std::string& source) {
    return ocpp::v2::SetVariableStatusEnum::Rejected;
}

std::optional<ocpp::v2::VariableMonitoringMeta>
EverestDeviceModelStorage::set_monitoring_data(const ocpp::v2::SetMonitoringData& /*data*/,
                                               const ocpp::v2::VariableMonitorType /*type*/) {
    return std::nullopt;
}

bool EverestDeviceModelStorage::update_monitoring_reference(const int32_t /*monitor_id*/,
                                                            const std::string& /*reference_value*/) {
    return false;
}

std::vector<ocpp::v2::VariableMonitoringMeta>
EverestDeviceModelStorage::get_monitoring_data(const std::vector<ocpp::v2::MonitoringCriterionEnum>& /*criteria*/,
                                               const ocpp::v2::Component& /*component_id*/,
                                               const ocpp::v2::Variable& /*variable_id*/) {
    return {};
}

ocpp::v2::ClearMonitoringStatusEnum EverestDeviceModelStorage::clear_variable_monitor(int /*monitor_id*/,
                                                                                      bool /*allow_protected*/) {
    return ocpp::v2::ClearMonitoringStatusEnum::NotFound;
}

int32_t EverestDeviceModelStorage::clear_custom_variable_monitors() {
    return 0;
}

void EverestDeviceModelStorage::check_integrity() {
}
} // namespace module::device_model
