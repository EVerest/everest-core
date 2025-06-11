// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <map>
#include <string>

#include <nlohmann/json.hpp>

#include <utils/config/settings.hpp>
#include <utils/config/storage_types.hpp>

namespace everest::config {

/// \brief EVerest Storage Interface providing read and write access to configurations
class StorageInterface {
public:
    virtual ~StorageInterface() = default;

    /// \brief Writes given EVerest \p module_configs to persistent storage
    /// \param config EVerest config
    /// \return
    virtual GenericResponseStatus write_module_configs(const std::map<ModuleId, ModuleConfig>& module_configs) = 0;

    /// \brief Writes EVerest config \p settings to persistent storage
    /// \param settings EVerest settings configuration
    /// \return
    virtual GenericResponseStatus write_settings(const Everest::ManagerSettings& manager_settings) = 0;

    /// \brief Wipes all configuration entries from persistent storage
    virtual GenericResponseStatus wipe() = 0;

    /// \brief Gets EVerest config from persistent storage
    /// \return Response with status of operation and config. config is only set if status is OK. Config contains all
    /// module configurations and manager settings
    virtual GetModuleConfigsResponse get_module_configs() = 0;

    /// \brief Gets EVerest manager settings from persistent storage
    /// \return
    virtual GetSettingsResponse get_settings() = 0;

    /// \brief Gets EVerest config from persistent storage for a single module
    /// \param module_id
    /// \return Response with status of operation and module config. config is only set if status is OK
    virtual GetModuleConfigurationResponse get_module_config(const std::string& module_id) = 0;

    /// \brief Gets single configuration parameter from persistent storage
    /// \param identifier Identifies configuration parameter
    /// \return Response with status of operation and configuration parameter. configuration parameter is only set if
    /// status is OK.
    virtual GetConfigurationParameterResponse
    get_configuration_parameter(const ConfigurationParameterIdentifier& identifier) = 0;

    /// \brief Writes single configuration parameter to persistent storage
    /// \param identifier Identifies configuration parameter
    /// \param value
    /// \return Response with status of operation
    virtual GetSetResponseStatus update_configuration_parameter(const ConfigurationParameterIdentifier& identifier,
                                                                const std::string& value) = 0;

    /// \brief Writes single configuration parameter including characteristics to persistent storage
    /// \param identifier Identifies configuration parameter
    /// \param characteristics of the configuration parameter
    /// \param value
    /// \return Response with status of operation
    virtual GetSetResponseStatus
    write_configuration_parameter(const ConfigurationParameterIdentifier& identifier,
                                  const ConfigurationParameterCharacteristics characteristics,
                                  const std::string& value) = 0;
};

} // namespace everest::config
