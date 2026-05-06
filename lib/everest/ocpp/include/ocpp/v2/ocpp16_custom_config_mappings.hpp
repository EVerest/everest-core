// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <filesystem>
#include <map>
#include <stdexcept>
#include <string>

#include <nlohmann/json_fwd.hpp>
#include <ocpp/v2/ocpp_types.hpp>

namespace ocpp::v2 {

using Ocpp16CustomConfigMappings = std::map<std::string, std::pair<ocpp::v2::Component, ocpp::v2::Variable>>;

class Ocpp16CustomConfigMappingsError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

/// \brief Schema for YAML-based OCPP 1.6 custom mapping files.
///
/// The schema is expressed as JSON schema draft-07 and validated against the parsed YAML content.
nlohmann::json get_ocpp16_custom_mapping_schema();

/// \brief Load OCPP 1.6 custom mappings from YAML file, validate against schema and convert to mapping map.
/// \param mapping_file_path Path to YAML mapping file.
/// \returns Parsed mappings for use with patch_component_config_with_ocpp16.
/// \throws Ocpp16CustomConfigMappingsError on file parsing, schema validation or conversion errors.
Ocpp16CustomConfigMappings load_ocpp16_custom_config_mappings_from_yaml(const std::filesystem::path& mapping_file_path);

} // namespace ocpp::v2
