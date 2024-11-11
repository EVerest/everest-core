// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <ocpp/v201/device_model_storage_interface.hpp>

namespace module::device_model {
class EverestDeviceModelStorage : public ocpp::v201::DeviceModelStorageInterface {
public:
    virtual ~EverestDeviceModelStorage() override = default;
    virtual ocpp::v201::DeviceModelMap get_device_model() override;
    virtual std::optional<ocpp::v201::VariableAttribute>
    get_variable_attribute(const ocpp::v201::Component& component_id, const ocpp::v201::Variable& variable_id,
                           const ocpp::v201::AttributeEnum& attribute_enum) override;
    virtual std::vector<ocpp::v201::VariableAttribute>
    get_variable_attributes(const ocpp::v201::Component& component_id, const ocpp::v201::Variable& variable_id,
                            const std::optional<ocpp::v201::AttributeEnum>& attribute_enum) override;
    virtual bool set_variable_attribute_value(const ocpp::v201::Component& component_id,
                                              const ocpp::v201::Variable& variable_id,
                                              const ocpp::v201::AttributeEnum& attribute_enum, const std::string& value,
                                              const std::string& source) override;
    virtual std::optional<ocpp::v201::VariableMonitoringMeta>
    set_monitoring_data(const ocpp::v201::SetMonitoringData& data, const ocpp::v201::VariableMonitorType type) override;
    virtual bool update_monitoring_reference(const int32_t monitor_id, const std::string& reference_value) override;
    virtual std::vector<ocpp::v201::VariableMonitoringMeta>
    get_monitoring_data(const std::vector<ocpp::v201::MonitoringCriterionEnum>& criteria,
                        const ocpp::v201::Component& component_id, const ocpp::v201::Variable& variable_id) override;
    virtual ocpp::v201::ClearMonitoringStatusEnum clear_variable_monitor(int monitor_id, bool allow_protected) override;
    virtual int32_t clear_custom_variable_monitors() override;
    virtual void check_integrity() override;
};
} // namespace module::device_model
