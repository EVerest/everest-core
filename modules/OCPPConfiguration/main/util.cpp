// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "util.hpp"
#include "everest/logging.hpp"
#include "generated/types/ocpp.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>

namespace module {

namespace util {

ryml::Tree try_to_load_existing_user_config(const std::filesystem::path& user_config_file_path) noexcept {
    try {
        // file exists
        return load_yaml_file(user_config_file_path);
    }
    // file doesn't exist
    catch (const std::runtime_error& e) {
        return {};
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

void write_value_to_tree(const EverestConfigMapping& module_mapping, const std::string& config_value,
                         ryml::Tree& config_tree) {

    auto root = config_tree.rootref();
    if (!root.is_map()) {
        root |= ryml::MAP; // mark root as map
    }

    auto active_modules = root["active_modules"];
    active_modules |= ryml::MAP; // mark active_modules as map

    const auto module_id_csubstr = ryml::to_csubstr(module_mapping.module_id.c_str());
    auto module_id_node = active_modules[module_id_csubstr];
    module_id_node |= ryml::MAP; // mark module_id as map

    auto config_module_node = module_id_node["config_module"];
    config_module_node |= ryml::MAP;

    const auto config_param_csubstr = ryml::to_csubstr(module_mapping.config_param.c_str());
    auto config_param_node = config_module_node[config_param_csubstr];
    const auto config_value_csubstr = ryml::to_csubstr(config_value.c_str());
    config_param_node << config_value_csubstr;
}

} // namespace util
} // namespace module