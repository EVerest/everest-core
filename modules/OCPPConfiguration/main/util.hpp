// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include "generated/types/ocpp.hpp"
#include "mapping_reader.hpp"
#include "utils/yaml_loader.hpp"
#include <c4/yml/std/map.hpp>
#include <filesystem>
#include <ryml.hpp>
#include <sstream>

namespace module {
namespace Util {

ryml::Tree load_existing_user_config(const std::filesystem::path& user_config_file_path);

ryml::Tree load_yaml_file(const std::filesystem::path& file_path);
void save_tree_to_yaml_file(const ryml::Tree& tree, const std::filesystem::path& file_path);

void write_value_to_tree(const EverestModuleMapping& module_mapping, const std::string& config_value,
                         const types::ocpp::Component& component, ryml::Tree& config_tree);
void add_component_if_exists(const types::ocpp::Component& component, c4::yml::NodeRef& module_id_node);

}; // namespace Util
} // namespace module