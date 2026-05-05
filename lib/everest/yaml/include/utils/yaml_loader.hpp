// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef UTILS_YAML_LOADER_HPP
#define UTILS_YAML_LOADER_HPP

#include <filesystem>

#include <nlohmann/json.hpp>

namespace Everest {

/// \brief Read the YAML file at \p path and returns its content as JSON
nlohmann::ordered_json load_yaml(const std::filesystem::path& path);

/// \brief Saves the JSON \p data to a YAML file at \p path
void save_yaml(const nlohmann::ordered_json& data, const std::filesystem::path& path);

} // namespace Everest

#endif // UTILS_YAML_LOADER_HPP
