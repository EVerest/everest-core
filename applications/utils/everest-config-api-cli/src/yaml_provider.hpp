// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#pragma once

#include "i_yaml_provider.hpp"

namespace everest::config_cli {

class YamlProvider : public IYamlProvider {
public:
    std::string extract_active_modules_string(const std::filesystem::path& path) override;
    std::string format_configuration(const GetConfigurationResult& config) override;
    ConfigurationParameterUpdateRequest parse_parameter_updates(int slot_id, const std::filesystem::path& path) override;
};

} // namespace everest::config_cli
