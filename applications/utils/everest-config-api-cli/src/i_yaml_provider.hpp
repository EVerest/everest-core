// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#pragma once

#include <everest_api_types/config_service/codec.hpp>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace everest::config_cli {

using namespace everest::lib::API::V1_0::types::config_service;

class IYamlProvider {
public:
    virtual ~IYamlProvider() = default;

    // Reads a YAML file and extracts the active_modules key and value as string
    virtual std::string extract_active_modules_string(const std::filesystem::path& path) = 0;

    // Formats a configuration tree into YAML string
    virtual std::string format_configuration(const GetConfigurationResult& config) = 0;

    // Parses a YAML which contains parameter updates and returns an update request object
    virtual ConfigurationParameterUpdateRequest parse_parameter_updates(int slot_id,
                                                                        const std::filesystem::path& path) = 0;
};

} // namespace everest::config_cli
