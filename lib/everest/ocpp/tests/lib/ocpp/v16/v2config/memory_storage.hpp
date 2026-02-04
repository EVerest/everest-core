// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

// v2 storage provider that uses memory rather than a database

#pragma once

#include <ocpp/v2/device_model_interface.hpp>
#include <ocpp/v2/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>

#include <set>
#include <string>
#include <string_view>

namespace ocpp::v16::stubs {

class MemoryStorage : public ocpp::v2::DeviceModelInterface {
private:
    std::set<std::string> read_only;

public:
    using VariableAttribute = ocpp::v2::VariableAttribute;
    using Component = ocpp::v2::Component;
    using ComponentVariable = ocpp::v2::ComponentVariable;
    using ComponentCriterionEnum = ocpp::v2::ComponentCriterionEnum;
    using Variable = ocpp::v2::Variable;
    using AttributeEnum = ocpp::v2::AttributeEnum;
    using GetVariableStatusEnum = ocpp::v2::GetVariableStatusEnum;
    using SetVariableStatusEnum = ocpp::v2::SetVariableStatusEnum;
    using VariableMonitoringMeta = ocpp::v2::VariableMonitoringMeta;
    using SetMonitoringData = ocpp::v2::SetMonitoringData;
    using SetMonitoringResult = ocpp::v2::SetMonitoringResult;
    using VariableMonitorType = ocpp::v2::VariableMonitorType;
    using VariableMonitoringPeriodic = ocpp::v2::VariableMonitoringPeriodic;
    using MonitoringCriterionEnum = ocpp::v2::MonitoringCriterionEnum;
    using MonitoringData = ocpp::v2::MonitoringData;
    using ClearMonitoringStatusEnum = ocpp::v2::ClearMonitoringStatusEnum;
    using ClearMonitoringResult = ocpp::v2::ClearMonitoringResult;
    using MutabilityEnum = ocpp::v2::MutabilityEnum;
    using VariableCharacteristics = ocpp::v2::VariableCharacteristics;
    using VariableMetaData = ocpp::v2::VariableMetaData;
    using ReportData = ocpp::v2::ReportData;
    using ReportBaseEnum = ocpp::v2::ReportBaseEnum;

    MemoryStorage();

    void apply_full_config();

    void set_readonly(const std::string& key);
    void set(const std::string_view& component, const std::string_view& variable, const std::string_view& value);
    std::string get(const std::string_view& component, const std::string_view& variable);
    void clear(const std::string_view& component, const std::string_view& variable);

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

    void register_variable_listener(
        std::function<void(const std::unordered_map<std::int64_t, VariableMonitoringMeta>& monitors,
                           const Component& component, const Variable& variable,
                           const VariableCharacteristics& characteristics, const VariableAttribute& attribute,
                           const std::string& value_previous, const std::string& value_current)>&& listener) override;

    void register_monitor_listener(
        std::function<void(const VariableMonitoringMeta& updated_monitor, const Component& component,
                           const Variable& variable, const VariableCharacteristics& characteristics,
                           const VariableAttribute& attribute, const std::string& current_value)>&& listener) override;

    void check_integrity(const std::map<std::int32_t, std::int32_t>& evse_connector_structure) override;
};

class MemoryStorageProxy : public ocpp::v2::DeviceModelInterface {
private:
    MemoryStorage& storage;

public:
    using VariableAttribute = ocpp::v2::VariableAttribute;
    using Component = ocpp::v2::Component;
    using ComponentVariable = ocpp::v2::ComponentVariable;
    using ComponentCriterionEnum = ocpp::v2::ComponentCriterionEnum;
    using Variable = ocpp::v2::Variable;
    using AttributeEnum = ocpp::v2::AttributeEnum;
    using GetVariableStatusEnum = ocpp::v2::GetVariableStatusEnum;
    using SetVariableStatusEnum = ocpp::v2::SetVariableStatusEnum;
    using VariableMonitoringMeta = ocpp::v2::VariableMonitoringMeta;
    using SetMonitoringData = ocpp::v2::SetMonitoringData;
    using SetMonitoringResult = ocpp::v2::SetMonitoringResult;
    using VariableMonitorType = ocpp::v2::VariableMonitorType;
    using VariableMonitoringPeriodic = ocpp::v2::VariableMonitoringPeriodic;
    using MonitoringCriterionEnum = ocpp::v2::MonitoringCriterionEnum;
    using MonitoringData = ocpp::v2::MonitoringData;
    using ClearMonitoringStatusEnum = ocpp::v2::ClearMonitoringStatusEnum;
    using ClearMonitoringResult = ocpp::v2::ClearMonitoringResult;
    using MutabilityEnum = ocpp::v2::MutabilityEnum;
    using VariableCharacteristics = ocpp::v2::VariableCharacteristics;
    using VariableMetaData = ocpp::v2::VariableMetaData;
    using ReportData = ocpp::v2::ReportData;
    using ReportBaseEnum = ocpp::v2::ReportBaseEnum;

    MemoryStorageProxy(MemoryStorage& obj) : storage(obj) {
    }

    GetVariableStatusEnum get_variable(const Component& component_id, const Variable& variable_id,
                                       const AttributeEnum& attribute_enum, std::string& value,
                                       bool allow_write_only = false) const {
        return storage.get_variable(component_id, variable_id, attribute_enum, value);
    }

    SetVariableStatusEnum set_value(const Component& component_id, const Variable& variable_id,
                                    const AttributeEnum& attribute_enum, const std::string& value,
                                    const std::string& source, bool allow_read_only = false) {
        return storage.set_value(component_id, variable_id, attribute_enum, value, source);
    }

    SetVariableStatusEnum set_read_only_value(const Component& component_id, const Variable& variable_id,
                                              const AttributeEnum& attribute_enum, const std::string& value,
                                              const std::string& source) {
        return storage.set_read_only_value(component_id, variable_id, attribute_enum, value, source);
    }

    std::optional<MutabilityEnum> get_mutability(const Component& component_id, const Variable& variable_id,
                                                 const AttributeEnum& attribute_enum) {
        return storage.get_mutability(component_id, variable_id, attribute_enum);
    }

    std::optional<VariableMetaData> get_variable_meta_data(const Component& component_id, const Variable& variable_id) {
        return storage.get_variable_meta_data(component_id, variable_id);
    }

    std::vector<ReportData> get_base_report_data(const ReportBaseEnum& report_base) {
        return storage.get_base_report_data(report_base);
    }

    std::vector<ReportData> get_custom_report_data(
        const std::optional<std::vector<ComponentVariable>>& component_variables = std::nullopt,
        const std::optional<std::vector<ComponentCriterionEnum>>& component_criteria = std::nullopt) {
        return storage.get_custom_report_data(component_variables, component_criteria);
    }

    std::vector<SetMonitoringResult> set_monitors(const std::vector<SetMonitoringData>& requests,
                                                  const VariableMonitorType type = VariableMonitorType::CustomMonitor) {
        return storage.set_monitors(requests);
    }

    bool update_monitor_reference(std::int32_t monitor_id, const std::string& reference_value) {
        return storage.update_monitor_reference(monitor_id, reference_value);
    }

    std::vector<VariableMonitoringPeriodic> get_periodic_monitors() {
        return storage.get_periodic_monitors();
    }

    std::vector<MonitoringData> get_monitors(const std::vector<MonitoringCriterionEnum>& criteria,
                                             const std::vector<ComponentVariable>& component_variables) {
        return storage.get_monitors(criteria, component_variables);
    }

    std::vector<ClearMonitoringResult> clear_monitors(const std::vector<int>& request_ids,
                                                      bool allow_protected = false) {
        return storage.clear_monitors(request_ids);
    }

    std::int32_t clear_custom_monitors() {
        return storage.clear_custom_monitors();
    }

    void register_variable_listener(
        std::function<void(const std::unordered_map<std::int64_t, VariableMonitoringMeta>& monitors,
                           const Component& component, const Variable& variable,
                           const VariableCharacteristics& characteristics, const VariableAttribute& attribute,
                           const std::string& value_previous, const std::string& value_current)>&& listener) {
        return storage.register_variable_listener(std::move(listener));
    }

    void register_monitor_listener(
        std::function<void(const VariableMonitoringMeta& updated_monitor, const Component& component,
                           const Variable& variable, const VariableCharacteristics& characteristics,
                           const VariableAttribute& attribute, const std::string& current_value)>&& listener) {
        return storage.register_monitor_listener(std::move(listener));
    }

    void check_integrity(const std::map<std::int32_t, std::int32_t>& evse_connector_structure) {
        return storage.check_integrity(evse_connector_structure);
    }
};

} // namespace ocpp::v16::stubs
