// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "util.hpp"
#include "everest/logging.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>

namespace module {

namespace Util {

ryml::Tree load_existing_user_config(const std::filesystem::path& user_config_file_path) {
    try {
        // file exists
        return load_yaml_file(user_config_file_path);
    }
    // file doesn't exist
    catch (const std::runtime_error& e) {
        return ryml::Tree();
    }
}

ryml::Tree load_yaml_file(const std::filesystem::path& file_path) {
    auto file = std::ifstream{file_path};
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + file_path.string());
    }
    auto buffer = std::stringstream{};
    buffer << file.rdbuf();
    const auto data = buffer.str();
    return ryml::parse_in_arena(ryml::to_csubstr(data));
}

void save_tree_to_yaml_file(const ryml::Tree& tree, const std::filesystem::path& file_path_string) {
    const auto file_path = std::filesystem::path{file_path_string};
    const auto absolute_file_path = std::filesystem::absolute(file_path);
    std::ofstream file{file_path};
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + absolute_file_path.string());
    }
    file << tree;
}
ryml::Tree write_value_to_tree(const EverestModuleMapping& module_mapping, const std::string& config_value,
                               const ryml::Tree& config_tree) {
    auto root = config_tree.rootref();
    if (!root.is_map()) {
        root |= ryml::MAP; // mark root as map
    }

    auto active_modules = root["active_modules"];
    active_modules |= ryml::MAP; // mark active_modules as map

    const auto config_key_csubstr = ryml::to_csubstr(module_mapping.config_key.c_str());
    auto config_key_node = active_modules[config_key_csubstr];
    config_key_node |= ryml::MAP; // mark config_key as map

    const auto module_name_csubstr = ryml::to_csubstr(module_mapping.module_name.c_str());
    auto module_node = config_key_node[module_name_csubstr];
    module_node |= ryml::MAP; // mark module as map

    auto config_module_node = module_node["config_module"];
    config_module_node |= ryml::MAP;

    const auto module_config_key_csubstr = ryml::to_csubstr(module_mapping.module_config_key.c_str());
    auto module_config_key_node = config_module_node[module_config_key_csubstr];
    const auto config_value_csubstr = ryml::to_csubstr(config_value.c_str());
    module_config_key_node << config_value_csubstr;

    return config_tree;
}

} // namespace Util
} // namespace module