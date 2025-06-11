// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <everest/database/sqlite/connection.hpp>
#include <utils/config/storage.hpp>

namespace everest::config {

/// \brief Implements StorageInterface with SQLite
class SqliteStorage : public StorageInterface {

public:
    /// \brief Constructor
    /// \param db_path Path to SQLite database file
    /// \param migration_files_path Path to SQL migration files
    /// \throws MigrationException if migration fails
    /// \throws std::runtime_error if database cannot be opened
    SqliteStorage(const fs::path& db_path, const std::filesystem::path& migration_files_path);

    GenericResponseStatus write_module_configs(const ModuleConfigurations& module_configs) override;
    GenericResponseStatus write_settings(const Everest::ManagerSettings& manager_settings) override;
    GenericResponseStatus wipe() override;
    GetModuleConfigsResponse get_module_configs() override;
    GetSettingsResponse get_settings() override;
    GetModuleConfigurationResponse get_module_config(const std::string& module_id) override;
    GetConfigurationParameterResponse
    get_configuration_parameter(const ConfigurationParameterIdentifier& identifier) override;
    GetSetResponseStatus update_configuration_parameter(const ConfigurationParameterIdentifier& identifier,
                                                        const std::string& value) override;
    GetSetResponseStatus write_configuration_parameter(const ConfigurationParameterIdentifier& identifier,
                                                       const ConfigurationParameterCharacteristics characteristics,
                                                       const std::string& value) override;

    /// \brief Checks if the database contains a valid configuration
    bool contains_valid_config();

    /// \brief Marks the current configuration as valid
    /// \param is_valid True if the configuration is valid, false otherwise
    /// \param config_dump JSON dump of the config file that was used to create the configuration
    /// \param yaml_file_path Path to the config file that was used to create the configuration
    void mark_valid(const bool is_valid, const std::string& config_dump,
                    const std::optional<fs::path>& config_file_path);

private:
    std::unique_ptr<everest::db::sqlite::ConnectionInterface> db;
    GenericResponseStatus write_module_data(const ModuleData& module_info);
    GenericResponseStatus write_module_fulfillment(const std::string& module_id, const Fulfillment& fulfillment);
    GenericResponseStatus write_module_tier_mapping(const std::string& module_id, const std::string& implementation_id,
                                                    const int32_t evse_id, const std::optional<int32_t> connector_id);
    GenericResponseStatus write_setting(const std::string& setting_name, const std::string& value);
    GetModuleFulfillmentsResponse get_module_fulfillments(const std::string& module_id);
    GetModuleDataResponse get_module_data(const std::string& module_id);
    GetModuleTierMappingsResponse get_module_tier_mappings(const std::string& module_id);
};

} // namespace everest::config
