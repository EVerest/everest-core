// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <mutex>

#include <generated/interfaces/evse_manager/Interface.hpp>
#include <generated/types/evse_board_support.hpp>

#include <ocpp/v2/device_model_storage_interface.hpp>

namespace module::device_model {

/// \brief Extends VariableMetaData to include a map of VariableAttributes that contain the actual values of a variable
struct VariableData : ocpp::v2::VariableMetaData {
    std::map<ocpp::v2::AttributeEnum, ocpp::v2::VariableAttribute> attributes;
};

using Variables = std::map<ocpp::v2::Variable, VariableData>;
using ComponentsMap = std::map<ocpp::v2::Component, Variables>;

class EverestDeviceModelStorage : public ocpp::v2::DeviceModelStorageInterface {
public:
    EverestDeviceModelStorage(
        const std::vector<std::unique_ptr<evse_managerIntf>>& r_evse_manager,
        const std::map<int32_t, types::evse_board_support::HardwareCapabilities>& evse_hardware_capabilities_map);
    virtual ~EverestDeviceModelStorage() override = default;
    virtual ocpp::v2::DeviceModelMap get_device_model() override;
    virtual std::optional<ocpp::v2::VariableAttribute>
    get_variable_attribute(const ocpp::v2::Component& component_id, const ocpp::v2::Variable& variable_id,
                           const ocpp::v2::AttributeEnum& attribute_enum) override;
    virtual std::vector<ocpp::v2::VariableAttribute>
    get_variable_attributes(const ocpp::v2::Component& component_id, const ocpp::v2::Variable& variable_id,
                            const std::optional<ocpp::v2::AttributeEnum>& attribute_enum) override;
    virtual ocpp::v2::SetVariableStatusEnum set_variable_attribute_value(const ocpp::v2::Component& component_id,
                                                                         const ocpp::v2::Variable& variable_id,
                                                                         const ocpp::v2::AttributeEnum& attribute_enum,
                                                                         const std::string& value,
                                                                         const std::string& source) override;
    virtual std::optional<ocpp::v2::VariableMonitoringMeta>
    set_monitoring_data(const ocpp::v2::SetMonitoringData& data, const ocpp::v2::VariableMonitorType type) override;
    virtual bool update_monitoring_reference(const int32_t monitor_id, const std::string& reference_value) override;
    virtual std::vector<ocpp::v2::VariableMonitoringMeta>
    get_monitoring_data(const std::vector<ocpp::v2::MonitoringCriterionEnum>& criteria,
                        const ocpp::v2::Component& component_id, const ocpp::v2::Variable& variable_id) override;
    virtual ocpp::v2::ClearMonitoringStatusEnum clear_variable_monitor(int monitor_id, bool allow_protected) override;
    virtual int32_t clear_custom_variable_monitors() override;
    virtual void check_integrity() override;

private:
    const std::vector<std::unique_ptr<evse_managerIntf>>& r_evse_manager;
    ComponentsMap device_model;
    std::mutex device_model_mutex;

    void update_hw_capabilities(const ocpp::v2::Component& evse_component,
                                const types::evse_board_support::HardwareCapabilities& hw_capabilities);
};
} // namespace module::device_model
