// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "yaml_provider.hpp"

#include <filesystem>
#include <string>

#include <everest_api_types/generic/codec.hpp>
#include <utils/yaml_loader.hpp>

namespace everest::config_cli {

std::string YamlProvider::extract_active_modules_string(const std::filesystem::path& path) {
    auto data = Everest::load_yaml(path);
    if (!data.contains("active_modules")) {
        throw std::runtime_error("YAML does not contain 'active_modules' key.");
    }

    nlohmann::ordered_json extracted;
    extracted["active_modules"] = data["active_modules"];

    return extracted.dump(4);
}

std::string YamlProvider::format_configuration(const GetConfigurationResult& config) {
    nlohmann::ordered_json root;

    auto serialized_config = everest::lib::API::V1_0::types::configuration::serialize(config);

    if (not nlohmann::json::accept(serialized_config) or
        nlohmann::json::parse(serialized_config).type() != nlohmann::json::value_t::object) {
        throw std::runtime_error("Failed to serialize configuration to JSON object.");
    }
    nlohmann::ordered_json get_cfg_result_json = nlohmann::json::parse(serialized_config);

    root["active_modules"] = get_cfg_result_json["module_configurations"];

    return root.dump(4);
}

ConfigurationParameterUpdateRequest YamlProvider::parse_parameter_updates(int slot_id,
                                                                          const std::filesystem::path& path) {
    ConfigurationParameterUpdateRequest req;
    req.slot_id = slot_id;

    auto data = Everest::load_yaml(path);
    if (!data.contains("active_modules")) {
        throw std::runtime_error("YAML does not contain 'active_modules' key.");
    }

    auto& modules = data["active_modules"];
    for (auto it = modules.begin(); it != modules.end(); ++it) {
        std::string module_id = it.key();
        auto& mod_obj = it.value();

        // cannot use the deserialization functions here, because there might be only a subset of the required fields
        // set in the YAML
        if (mod_obj.contains("config_module")) {
            for (auto& [param_name, param_val] : mod_obj["config_module"].items()) {
                ConfigurationParameterUpdate update;
                update.cfg_param_id.module_id = module_id;
                update.cfg_param_id.parameter_name = param_name;
                update.value = param_val.dump();
                req.parameter_updates.push_back(update);
            }
        }

        if (mod_obj.contains("config_implementation")) {
            for (auto& [impl_id, impl_obj] : mod_obj["config_implementation"].items()) {
                for (auto& [param_name, param_val] : impl_obj.items()) {
                    ConfigurationParameterUpdate update;
                    update.cfg_param_id.module_id = module_id;
                    update.cfg_param_id.implementation_id = impl_id;
                    update.cfg_param_id.parameter_name = param_name;

                    update.value = param_val.dump();

                    req.parameter_updates.push_back(update);
                }
            }
        }
    }

    return req;
}

} // namespace everest::config_cli
