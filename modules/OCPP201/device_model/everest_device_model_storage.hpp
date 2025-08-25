// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <mutex>

#include <generated/interfaces/evse_manager/Interface.hpp>
#include <generated/types/evse_board_support.hpp>
#include <generated/types/powermeter.hpp>

#include <device_model/mapping/variable_mapping.hpp>
#include <ocpp/v2/device_model_storage_interface.hpp>
#include <ocpp/v2/device_model_storage_sqlite.hpp>
#include <ocpp/v2/init_device_model_db.hpp>
#include <utils/config_service.hpp>

namespace module::device_model {
class EverestDeviceModelStorage : public ocpp::v2::DeviceModelStorageInterface {
public:
    EverestDeviceModelStorage(
        const std::vector<std::unique_ptr<evse_managerIntf>>& r_evse_manager,
        const std::map<int32_t, types::evse_board_support::HardwareCapabilities>& evse_hardware_capabilities_map,
        const std::filesystem::path& db_path, const std::filesystem::path& migration_files_path,
        std::unique_ptr<VariableMapping> variable_mapping,
        std::shared_ptr<Everest::config::ConfigServiceClient> config_service_client);
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

    /// \brief Updates the actual value of the EVSE Power variable to the given \p total_power_active_import value
    void update_power(const int32_t evse_id, const float total_power_active_import);

private:
    const std::vector<std::unique_ptr<evse_managerIntf>>& r_evse_manager;
    std::mutex device_model_mutex;
    std::unique_ptr<ocpp::v2::DeviceModelStorageSqlite> device_model_storage;
    std::set<ocpp::v2::ComponentVariable> stored_in_everest_config_service;
    std::shared_ptr<Everest::config::ConfigServiceClient> config_service_client;
    std::map<Everest::config::ModuleIdType, everest::config::ModuleConfigurationParameters> module_configs;
    std::map<std::string, ModuleTierMappings> mappings;
    std::unique_ptr<VariableMapping> variable_mapping;

    void init_hw_capabilities(
        const std::map<int32_t, types::evse_board_support::HardwareCapabilities>& evse_hardware_capabilities_map);
    void update_hw_capabilities(const ocpp::v2::Component& evse_component,
                                const types::evse_board_support::HardwareCapabilities& hw_capabilities);
    std::vector<ocpp::v2::DeviceModelVariable>
    build_everest_config_variables(const everest::config::ModuleConfigurationParameters& module_config,
                                   const std::string& module_id, const ocpp::v2::Component& component);
    ocpp::v2::DeviceModelMap apply_mappings(const ocpp::v2::DeviceModelMap& device_model_map);
};
} // namespace module::device_model
