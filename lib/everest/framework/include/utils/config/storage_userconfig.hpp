// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <nlohmann/json.hpp>

#include <utils/config/storage.hpp>

namespace everest::config {

/// \brief Implements StorageInterface with YAML user-config
class UserConfigStorage : public StorageInterface {

public:
    /// \brief Constructor
    /// \param user_config_path Path to user-config file
    UserConfigStorage(const fs::path& user_config_path);

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

    const nlohmann::json& get_user_config() const;

private:
    fs::path user_config_path;
    nlohmann::json user_config;
};

} // namespace everest::config
