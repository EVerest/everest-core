// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include "everest_config_mapping.hpp"
#include "generated/types/ocpp.hpp"

#include <c4/yml/std/map.hpp>
#include <filesystem>
#include <ryml.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace module {

using types::ocpp::Component;
using types::ocpp::ComponentVariable;
using types::ocpp::Variable;

using OcppToEverestConfigMapping = std::unordered_map<ComponentVariable, EverestConfigMapping>;

/**
 * Collection of functions to read the mapping file.
 */
namespace mapping_reader {

/**
 * Reads the mapping file and returns the mapping.
 *
 * @param mapping_file_name The file name of the mapping file.
 * @param schema_file_name The file name of the schema file.
 * @return The mapping.
 * @throws std::runtime_error if the files can't be opened.
 * @throws std::exception if the schema validation fails.
 */
OcppToEverestConfigMapping read_mapping(const std::filesystem::path& mapping_file_name,
                                        const std::filesystem::path& schema_file_name);

}; // namespace mapping_reader

} // namespace module