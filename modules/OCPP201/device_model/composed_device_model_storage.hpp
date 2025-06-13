// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <device_model/everest_device_model_storage.hpp>
#include <ocpp/v2/device_model_storage_interface.hpp>
#include <ocpp/v2/device_model_storage_sqlite.hpp>

namespace module::device_model {

using ComponentVariableSourceMap = std::map<ocpp::v2::Component, std::map<ocpp::v2::Variable, std::string>>;
class ComposedDeviceModelStorage : public ocpp::v2::DeviceModelStorageInterface {
private:
    std::map<std::string, std::unique_ptr<ocpp::v2::DeviceModelStorageInterface>>
        device_model_storages; // key is identifier for the device model storage
    ComponentVariableSourceMap component_variable_source_map;

public:
    ComposedDeviceModelStorage();

    /// \brief Register a device model storage.
    /// \param device_model_storage_id   The id of the device model storage. Component variable combinations can be
    /// used to map to this id to identify which device model is adressed for certain requests.
    /// \param  device_model_storage The device model storage to register.
    /// \return True if the device model storage name is not yet registered, false otherwise.
    bool register_device_model_storage(std::string device_model_storage_id,
                                       std::unique_ptr<ocpp::v2::DeviceModelStorageInterface> device_model_storage);
    virtual ~ComposedDeviceModelStorage() override = default;
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
    ///
    /// \brief Get variable source of given variable.
    /// \param component    Component the variable belongs to.
    /// \param variable     The variable to get the source from.
    /// \return The variable source. Defaults to 'OCPP'.
    ///
    std::string get_variable_source(const ocpp::v2::Component& component, const ocpp::v2::Variable& variable);
};
} // namespace module::device_model
