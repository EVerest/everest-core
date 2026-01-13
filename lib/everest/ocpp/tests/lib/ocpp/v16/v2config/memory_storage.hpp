// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

// v2 storage provider that uses memory rather than a database

#pragma once

#include <ocpp/v2/device_model_storage_interface.hpp>
#include <string_view>

namespace ocpp::v16::stubs {

class MemoryStorage : public ocpp::v2::DeviceModelStorageInterface {
public:
    using DeviceModelMap = ocpp::v2::DeviceModelMap;
    using VariableAttribute = ocpp::v2::VariableAttribute;
    using Component = ocpp::v2::Component;
    using Variable = ocpp::v2::Variable;
    using AttributeEnum = ocpp::v2::AttributeEnum;
    using SetVariableStatusEnum = ocpp::v2::SetVariableStatusEnum;
    using VariableMonitoringMeta = ocpp::v2::VariableMonitoringMeta;
    using SetMonitoringData = ocpp::v2::SetMonitoringData;
    using VariableMonitorType = ocpp::v2::VariableMonitorType;
    using MonitoringCriterionEnum = ocpp::v2::MonitoringCriterionEnum;
    using ClearMonitoringStatusEnum = ocpp::v2::ClearMonitoringStatusEnum;

    MemoryStorage();

    void set(const std::string_view& component, const std::string_view& variable, const std::string_view& value);
    std::string get(const std::string_view& component, const std::string_view& variable);
    void clear(const std::string_view& component, const std::string_view& variable);

    DeviceModelMap get_device_model() override;

    std::optional<VariableAttribute> get_variable_attribute(const Component& component_id, const Variable& variable_id,
                                                            const AttributeEnum& attribute_enum) override;

    std::vector<VariableAttribute>
    get_variable_attributes(const Component& component_id, const Variable& variable_id,
                            const std::optional<AttributeEnum>& attribute_enum = std::nullopt) override;

    SetVariableStatusEnum set_variable_attribute_value(const Component& component_id, const Variable& variable_id,
                                                       const AttributeEnum& attribute_enum, const std::string& value,
                                                       const std::string& source) override;

    std::optional<VariableMonitoringMeta> set_monitoring_data(const SetMonitoringData& data,
                                                              const VariableMonitorType type) override;

    bool update_monitoring_reference(const int32_t monitor_id, const std::string& reference_value) override;

    std::vector<VariableMonitoringMeta> get_monitoring_data(const std::vector<MonitoringCriterionEnum>& criteria,
                                                            const Component& component_id,
                                                            const Variable& variable_id) override;

    ClearMonitoringStatusEnum clear_variable_monitor(int monitor_id, bool allow_protected) override;

    std::int32_t clear_custom_variable_monitors() override;

    void check_integrity() override;
};

class MemoryStorageProxy : public ocpp::v2::DeviceModelStorageInterface {
private:
    MemoryStorage& storage;

public:
    using DeviceModelMap = ocpp::v2::DeviceModelMap;
    using VariableAttribute = ocpp::v2::VariableAttribute;
    using Component = ocpp::v2::Component;
    using Variable = ocpp::v2::Variable;
    using AttributeEnum = ocpp::v2::AttributeEnum;
    using SetVariableStatusEnum = ocpp::v2::SetVariableStatusEnum;
    using VariableMonitoringMeta = ocpp::v2::VariableMonitoringMeta;
    using SetMonitoringData = ocpp::v2::SetMonitoringData;
    using VariableMonitorType = ocpp::v2::VariableMonitorType;
    using MonitoringCriterionEnum = ocpp::v2::MonitoringCriterionEnum;
    using ClearMonitoringStatusEnum = ocpp::v2::ClearMonitoringStatusEnum;

    MemoryStorageProxy(MemoryStorage& obj) : storage(obj) {
    }

    DeviceModelMap get_device_model() override {
        return storage.get_device_model();
    }

    std::optional<VariableAttribute> get_variable_attribute(const Component& component_id, const Variable& variable_id,
                                                            const AttributeEnum& attribute_enum) override {
        return storage.get_variable_attribute(component_id, variable_id, attribute_enum);
    }

    std::vector<VariableAttribute>
    get_variable_attributes(const Component& component_id, const Variable& variable_id,
                            const std::optional<AttributeEnum>& attribute_enum = std::nullopt) override {
        return storage.get_variable_attributes(component_id, variable_id, attribute_enum);
    }

    SetVariableStatusEnum set_variable_attribute_value(const Component& component_id, const Variable& variable_id,
                                                       const AttributeEnum& attribute_enum, const std::string& value,
                                                       const std::string& source) override {
        return storage.set_variable_attribute_value(component_id, variable_id, attribute_enum, value, source);
    }

    std::optional<VariableMonitoringMeta> set_monitoring_data(const SetMonitoringData& data,
                                                              const VariableMonitorType type) override {
        return storage.set_monitoring_data(data, type);
    }

    bool update_monitoring_reference(const int32_t monitor_id, const std::string& reference_value) override {
        return storage.update_monitoring_reference(monitor_id, reference_value);
    }

    std::vector<VariableMonitoringMeta> get_monitoring_data(const std::vector<MonitoringCriterionEnum>& criteria,
                                                            const Component& component_id,
                                                            const Variable& variable_id) override {
        return storage.get_monitoring_data(criteria, component_id, variable_id);
    }

    ClearMonitoringStatusEnum clear_variable_monitor(int monitor_id, bool allow_protected) override {
        return storage.clear_variable_monitor(monitor_id, allow_protected);
    }

    std::int32_t clear_custom_variable_monitors() override {
        return storage.clear_custom_variable_monitors();
    }

    void check_integrity() override {
        storage.check_integrity();
    }
};

} // namespace ocpp::v16::stubs
