// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json_fwd.hpp>

#include <utils/config/types.hpp>

namespace everest::config {

enum class GenericResponseStatus {
    OK,
    Failed
};

enum class GetSetResponseStatus {
    OK,
    Failed,
    NotFound
};

struct ModuleData {
    std::string module_id;
    std::string module_name;
    bool standalone = false;
    std::optional<std::string> capabilities;
};

struct GetConfigurationParameterResponse {
    GetSetResponseStatus status = GetSetResponseStatus::Failed;
    std::optional<ConfigurationParameter> configuration_parameter;
};

struct GetModuleConfigsResponse {
    GenericResponseStatus status = GenericResponseStatus::Failed;
    ModuleConfigurations module_configs;
};

struct GetSettingsResponse {
    GenericResponseStatus status = GenericResponseStatus::Failed;
    std::optional<Settings> settings;
};

struct GetModuleFulfillmentsResponse {
    GenericResponseStatus status = GenericResponseStatus::Failed;
    std::vector<Fulfillment> module_fulfillments;
};

struct GetModuleTierMappingsResponse {
    GenericResponseStatus status = GenericResponseStatus::Failed;
    ModuleTierMappings module_tier_mappings;
};

struct GetModuleConfigurationResponse {
    GenericResponseStatus status = GenericResponseStatus::Failed;
    std::optional<ModuleConfig> config;
};

struct GetModuleDataResponse {
    GenericResponseStatus status = GenericResponseStatus::Failed;
    std::optional<ModuleData> module_data;
};

struct ConfigurationParameterIdentifier {
    std::string module_id;
    std::string configuration_parameter_name;
    std::optional<std::string> module_implementation_id;
};

} // namespace everest::config
