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
namespace util {

/**
 * Try to load the existing user configuration file.
 *
 * @param user_config_file_path The path to the user configuration file.
 * @return The loaded tree or an empty tree if the file doesn't exist.
 */
ryml::Tree try_to_load_existing_user_config(const std::filesystem::path& user_config_file_path) noexcept;

/**
 * Load a YAML file.
 *
 * @param file_path The path to the YAML file.
 * @return The loaded tree.
 * @throws std::runtime_error if the file can't be opened.
 */
ryml::Tree load_yaml_file(const std::filesystem::path& file_path);

/**
 * Save a tree to a YAML file.
 *
 * @param tree The tree to save.
 * @param file_path The path to the file.
 * @throws std::runtime_error if the file can't be opened.
 */
void save_tree_to_yaml_file(const ryml::Tree& tree, const std::filesystem::path& file_path);

/**
 * Write a value to the tree. The tree is modified in place.
 *
 * @param module_mapping The module mapping.
 * @param config_value The value to write.
 * @param config_tree The tree to write to.
 */
void write_value_to_tree(const EverestConfigMapping& module_mapping, const std::string& config_value,
                         ryml::Tree& config_tree);

}; // namespace util
} // namespace module