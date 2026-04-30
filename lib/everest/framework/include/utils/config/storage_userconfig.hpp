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
    GenericResponseStatus replace_module_configs(const ModuleConfigurations& module_configs) override;
    GetModuleConfigsResponse get_module_configs() override;
    GetModuleConfigurationResponse get_module_config(const std::string& module_id) override;
    GetConfigurationParameterResponse
    get_configuration_parameter(const ConfigurationParameterIdentifier& identifier) override;
    GetSetResponseStatus update_configuration_parameter(const ConfigurationParameterIdentifier& identifier,
                                                        const std::string& value) override;
    GetSetResponseStatus write_configuration_parameter(const ConfigurationParameterIdentifier& identifier,
                                                       const ConfigurationParameterCharacteristics characteristics,
                                                       const std::string& value) override;
    void mark_valid(bool is_valid, const std::string& config_dump,
                    const std::optional<std::filesystem::path>& config_file_path,
                    const std::optional<std::string>& description) override;

    const nlohmann::json& get_user_config() const;

private:
    fs::path user_config_path;
    nlohmann::json user_config;
};

} // namespace everest::config
