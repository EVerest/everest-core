// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <device_model/everest_device_model_storage.hpp>

namespace module::device_model {
ocpp::v201::DeviceModelMap EverestDeviceModelStorage::get_device_model() {
    return {};
}

std::optional<ocpp::v201::VariableAttribute>
EverestDeviceModelStorage::get_variable_attribute(const ocpp::v201::Component& /*component_id*/,
                                                  const ocpp::v201::Variable& /*variable_id*/,
                                                  const ocpp::v201::AttributeEnum& /*attribute_enum*/) {
    return std::nullopt;
}

std::vector<ocpp::v201::VariableAttribute>
EverestDeviceModelStorage::get_variable_attributes(const ocpp::v201::Component& /*component_id*/,
                                                   const ocpp::v201::Variable& /*variable_id*/,
                                                   const std::optional<ocpp::v201::AttributeEnum>& /*attribute_enum*/) {
    return {};
}

bool EverestDeviceModelStorage::set_variable_attribute_value(const ocpp::v201::Component& /*component_id*/,
                                                             const ocpp::v201::Variable& /*variable_id*/,
                                                             const ocpp::v201::AttributeEnum& /*attribute_enum*/,
                                                             const std::string& /*value*/,
                                                             const std::string& /*source*/) {
    return false;
}

std::optional<ocpp::v201::VariableMonitoringMeta>
EverestDeviceModelStorage::set_monitoring_data(const ocpp::v201::SetMonitoringData& /*data*/,
                                               const ocpp::v201::VariableMonitorType /*type*/) {
    return std::nullopt;
}

bool EverestDeviceModelStorage::update_monitoring_reference(const int32_t /*monitor_id*/,
                                                            const std::string& /*reference_value*/) {
    return false;
}

std::vector<ocpp::v201::VariableMonitoringMeta>
EverestDeviceModelStorage::get_monitoring_data(const std::vector<ocpp::v201::MonitoringCriterionEnum>& /*criteria*/,
                                               const ocpp::v201::Component& /*component_id*/,
                                               const ocpp::v201::Variable& /*variable_id*/) {
    return {};
}

ocpp::v201::ClearMonitoringStatusEnum EverestDeviceModelStorage::clear_variable_monitor(int /*monitor_id*/,
                                                                                        bool /*allow_protected*/) {
    return ocpp::v201::ClearMonitoringStatusEnum::NotFound;
}

int32_t EverestDeviceModelStorage::clear_custom_variable_monitors() {
    return 0;
}

void EverestDeviceModelStorage::check_integrity() {
}
} // namespace module::device_model
