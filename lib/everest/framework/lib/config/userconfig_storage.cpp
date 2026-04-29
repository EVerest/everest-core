// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <exception>

#include <everest/logging.hpp>

#include <utils/config/storage.hpp>
#include <utils/config/storage_userconfig.hpp>
#include <utils/conversions.hpp>
#include <utils/yaml_loader.hpp>

namespace everest::config {

UserConfigStorage::UserConfigStorage(const fs::path& user_config_path) : user_config_path(user_config_path) {
    try {
        if (fs::exists(user_config_path)) {
            this->user_config = Everest::load_yaml(user_config_path);
        }
    } catch (const std::exception& e) {
        EVLOG_error << "Could not load user-config at " << user_config_path.string() << ": " << e.what();
    }
}

GenericResponseStatus UserConfigStorage::write_module_configs(const ModuleConfigurations& /*module_configs*/) {
    return GenericResponseStatus::Failed;
}
void UserConfigStorage::mark_valid(bool /*is_valid*/, const std::string& /*config_dump*/,
                                   const std::optional<std::filesystem::path>& /*config_file_path*/,
                                   const std::optional<std::string>& /*description*/) {
    // UserConfigStorage does not support marking configs as valid
}
GetModuleConfigsResponse UserConfigStorage::get_module_configs() {
    GetModuleConfigsResponse response;
    response.status = GenericResponseStatus::Failed;
    return response;
}
GetModuleConfigurationResponse UserConfigStorage::get_module_config(const std::string& /*module_id*/) {
    GetModuleConfigurationResponse response;
    response.status = GenericResponseStatus::Failed;
    return response;
}
GetConfigurationParameterResponse
UserConfigStorage::get_configuration_parameter(const ConfigurationParameterIdentifier& /*identifier*/) {
    GetConfigurationParameterResponse response;
    response.status = GetSetResponseStatus::Failed;
    return response;
}

GetSetResponseStatus
UserConfigStorage::update_configuration_parameter(const ConfigurationParameterIdentifier& /*identifier*/,
                                                  const std::string& /*value*/) {
    return GetSetResponseStatus::Failed;
}

GetSetResponseStatus
UserConfigStorage::write_configuration_parameter(const ConfigurationParameterIdentifier& identifier,
                                                 const ConfigurationParameterCharacteristics characteristics,
                                                 const std::string& value) {
    // TODO: expect module_id etc. to be valid already
    if (not this->user_config.contains("active_modules")) {
        this->user_config["active_modules"] = nlohmann::json::object();
    }
    auto& active_modules = this->user_config.at("active_modules");
    if (not active_modules.contains(identifier.module_id)) {
        active_modules[identifier.module_id] = json::object();
    }
    auto& module_cfg = active_modules[identifier.module_id];
    const auto impl_id = identifier.module_implementation_id.value_or("!module");
    if (impl_id == "!module") {
        if (not module_cfg.contains("config_module")) {
            module_cfg["config_module"] = json::object();
        }
        auto& config_module = module_cfg["config_module"];
        config_module[identifier.configuration_parameter_name] =
            everest::config::parse_config_value(characteristics.datatype, value);
    } else {
        if (not module_cfg.contains("config_implementation")) {
            module_cfg["config_implementation"] = json::object();
        }
        auto& config_implementation = module_cfg["config_implementation"];
        if (not config_implementation.contains(impl_id)) {
            config_implementation[impl_id] = json::object();
        }
        auto& config_implementation_impl_id = config_implementation[impl_id];
        config_implementation_impl_id[identifier.configuration_parameter_name] =
            everest::config::parse_config_value(characteristics.datatype, value);
    }

    Everest::save_yaml(this->user_config, this->user_config_path);
    return GetSetResponseStatus::OK;
}

const nlohmann::json& UserConfigStorage::get_user_config() const {
    return this->user_config;
}
} // namespace everest::config
