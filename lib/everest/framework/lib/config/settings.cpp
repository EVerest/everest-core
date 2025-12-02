// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <utils/config/settings.hpp>

namespace Everest {

RuntimeSettings create_runtime_settings(const fs::path& prefix, const fs::path& etc_dir, const fs::path& data_dir,
                                        const fs::path& modules_dir, const fs::path& logging_config_file,
                                        const std::string& telemetry_prefix, bool telemetry_enabled,
                                        bool validate_schema, bool forward_exceptions) {
    RuntimeSettings runtime_settings;
    runtime_settings.prefix = prefix;
    runtime_settings.etc_dir = etc_dir;
    runtime_settings.data_dir = data_dir;
    runtime_settings.modules_dir = modules_dir;
    runtime_settings.logging_config_file = logging_config_file;
    runtime_settings.telemetry_prefix = telemetry_prefix;
    runtime_settings.telemetry_enabled = telemetry_enabled;
    runtime_settings.validate_schema = validate_schema;
    runtime_settings.forward_exceptions = forward_exceptions;
    return runtime_settings;
}

void populate_runtime_settings(RuntimeSettings& runtime_settings, const fs::path& prefix, const fs::path& etc_dir,
                               const fs::path& data_dir, const fs::path& modules_dir,
                               const fs::path& logging_config_file, const std::string& telemetry_prefix,
                               bool telemetry_enabled, bool validate_schema, bool forward_exceptions) {
    runtime_settings.prefix = prefix;
    runtime_settings.etc_dir = etc_dir;
    runtime_settings.data_dir = data_dir;
    runtime_settings.modules_dir = modules_dir;
    runtime_settings.logging_config_file = logging_config_file;
    runtime_settings.telemetry_prefix = telemetry_prefix;
    runtime_settings.telemetry_enabled = telemetry_enabled;
    runtime_settings.validate_schema = validate_schema;
    runtime_settings.forward_exceptions = forward_exceptions;
}

} // namespace Everest

NLOHMANN_JSON_NAMESPACE_BEGIN
void adl_serializer<Everest::RuntimeSettings>::to_json(nlohmann::json& j, const Everest::RuntimeSettings& r) {
    j = {{"prefix", r.prefix},
         {"etc_dir", r.etc_dir},
         {"data_dir", r.data_dir},
         {"modules_dir", r.modules_dir},
         {"telemetry_prefix", r.telemetry_prefix},
         {"telemetry_enabled", r.telemetry_enabled},
         {"validate_schema", r.validate_schema},
         {"forward_exceptions", r.forward_exceptions}};
}

void adl_serializer<Everest::RuntimeSettings>::from_json(const nlohmann::json& j, Everest::RuntimeSettings& r) {
    r.prefix = j.at("prefix").get<std::string>();
    r.etc_dir = j.at("etc_dir").get<std::string>();
    r.data_dir = j.at("data_dir").get<std::string>();
    r.modules_dir = j.at("modules_dir").get<std::string>();
    r.telemetry_prefix = j.at("telemetry_prefix").get<std::string>();
    r.telemetry_enabled = j.at("telemetry_enabled").get<bool>();
    r.validate_schema = j.at("validate_schema").get<bool>();
    r.forward_exceptions = j.at("forward_exceptions").get<bool>();
}
NLOHMANN_JSON_NAMESPACE_END
