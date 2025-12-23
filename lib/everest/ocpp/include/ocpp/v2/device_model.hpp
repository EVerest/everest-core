// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#ifndef DEVICE_MODEL_HPP
#define DEVICE_MODEL_HPP

#include <type_traits>

#include <everest/logging.hpp>

#include <ocpp/v2/ctrlr_component_variables.hpp>
#include <ocpp/v2/device_model_abstract.hpp>
#include <ocpp/v2/device_model_storage_interface.hpp>

namespace ocpp {
namespace v2 {

using on_variable_changed = std::function<void(
    const std::unordered_map<std::int64_t, VariableMonitoringMeta>& monitors, const Component& component,
    const Variable& variable, const VariableCharacteristics& characteristics, const VariableAttribute& attribute,
    const std::string& value_previous, const std::string& value_current)>;

using on_monitor_updated = std::function<void(const VariableMonitoringMeta& updated_monitor, const Component& component,
                                              const Variable& variable, const VariableCharacteristics& characteristics,
                                              const VariableAttribute& attribute, const std::string& current_value)>;

/// \brief This class manages access to the device model representation and to the device model interface and provides
/// functionality to support the use cases defined in the functional block Provisioning
class DeviceModel : public DeviceModelAbstract {

private:
    DeviceModelMap device_model_map;
    std::unique_ptr<DeviceModelStorageInterface> device_model;

    /// \brief Listener for the internal change of a variable
    on_variable_changed variable_listener;
    /// \brief Listener for the internal update of a monitor
    on_monitor_updated monitor_update_listener;

    /// \brief Private helper method that does some checks with the device model representation in memory to evaluate if
    /// a value for the given parameters can be requested. If it can be requested it will be retrieved from the device
    /// model interface and the given \p value will be set to the value that was retrieved
    /// \param component_id
    /// \param variable_id
    /// \param attribute_enum
    /// \param value string reference to value: will be set to requested value if value is present
    /// \param allow_write_only true to allow a writeOnly value to be read.
    /// \return GetVariableStatusEnum that indicates the result of the request
    GetVariableStatusEnum request_value_internal(const Component& component_id, const Variable& variable_id,
                                                 const AttributeEnum& attribute_enum, std::string& value,
                                                 bool allow_write_only) const;

    /// \brief Iterates over the given \p component_criteria and converts this to the variable names
    /// (Active,Available,Enabled,Problem). If any of the variables can not be found as part of a component this
    /// function returns false. If any of those variable's value is true, this function returns true (except for
    /// criteria problem). If all variable's value are false, this function returns false
    ///  \param component_id
    ///  \param /// component_criteria
    ///  \return
    bool component_criteria_match(const Component& component_id,
                                  const std::vector<ComponentCriterionEnum>& component_criteria);

    ///
    /// \brief Helper function to check if a variable has a value.
    /// \param component_variable   Component variable to check.
    /// \param attribute            Attribute to check.
    ///
    /// \throws DeviceModelError if variable has no value or value is an empty string.
    ///
    void check_variable_has_value(const ComponentVariable& component_variable,
                                  const AttributeEnum attribute = AttributeEnum::Actual);

    ///
    /// \brief Helper function to check if a required variable has a value.
    /// \param required_variable    Required component variable to check.
    /// \param supported_versions   The current supported ocpp versions.
    /// \throws DeviceModelError if variable has no value or value is an empty string.
    ///
    void check_required_variable(const RequiredComponentVariable& required_variable,
                                 const std::vector<OcppProtocolVersion>& supported_versions);

    ///
    /// \brief Loop over all required variables to check if they have a value.
    ///
    /// This will check for all required variables from `ctrlr_component_variables.cpp` `required_variables`.
    /// It will also check for specific required variables that belong to a specific controller. If a controller is not
    /// available, the 'required' variables of that component are not required at this point.
    ///
    /// \throws DeviceModelError if one of the variables does not have a value or value is an empty string.
    ///
    void check_required_variables();

public:
    /// \brief Constructor for the device model
    /// \param device_model_storage_interface pointer to a device model interface class
    explicit DeviceModel(std::unique_ptr<DeviceModelStorageInterface> device_model_storage_interface);

    GetVariableStatusEnum get_variable(const Component& component_id, const Variable& variable_id,
                                       const AttributeEnum& attribute_enum, std::string& value,
                                       bool allow_write_only = false) const override;

    SetVariableStatusEnum set_value(const Component& component_id, const Variable& variable_id,
                                    const AttributeEnum& attribute_enum, const std::string& value,
                                    const std::string& source, bool allow_read_only = false) override;

    SetVariableStatusEnum set_read_only_value(const Component& component_id, const Variable& variable_id,
                                              const AttributeEnum& attribute_enum, const std::string& value,
                                              const std::string& source) override;

    std::optional<MutabilityEnum> get_mutability(const Component& component_id, const Variable& variable_id,
                                                 const AttributeEnum& attribute_enum) override;

    std::optional<VariableMetaData> get_variable_meta_data(const Component& component_id,
                                                           const Variable& variable_id) override;

    std::vector<ReportData> get_base_report_data(const ReportBaseEnum& report_base) override;

    std::vector<ReportData> get_custom_report_data(
        const std::optional<std::vector<ComponentVariable>>& component_variables = std::nullopt,
        const std::optional<std::vector<ComponentCriterionEnum>>& component_criteria = std::nullopt) override;

    std::vector<SetMonitoringResult>
    set_monitors(const std::vector<SetMonitoringData>& requests,
                 const VariableMonitorType type = VariableMonitorType::CustomMonitor) override;

    bool update_monitor_reference(std::int32_t monitor_id, const std::string& reference_value) override;

    std::vector<VariableMonitoringPeriodic> get_periodic_monitors() override;

    std::vector<MonitoringData> get_monitors(const std::vector<MonitoringCriterionEnum>& criteria,
                                             const std::vector<ComponentVariable>& component_variables) override;

    std::vector<ClearMonitoringResult> clear_monitors(const std::vector<int>& request_ids,
                                                      bool allow_protected = false) override;

    std::int32_t clear_custom_monitors() override;

    void register_variable_listener(on_variable_changed&& listener) override {
        variable_listener = std::move(listener);
    }

    void register_monitor_listener(on_monitor_updated&& listener) override {
        monitor_update_listener = std::move(listener);
    }

    void check_integrity(const std::map<std::int32_t, std::int32_t>& evse_connector_structure) override;
};

} // namespace v2
} // namespace ocpp

#endif // DEVICE_MODEL_HPP
