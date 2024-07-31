// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "config_writer.hpp"
#include "util.hpp"
#include "utils/yaml_loader.hpp"
#include <everest/logging.hpp>
#include <fstream>
#include <sstream>

namespace module {

ConfigWriter::ConfigWriter(const std::string& user_config_file_in) :
    config_tree{load_existing_user_config(user_config_file_in)}, user_config_file{user_config_file_in} {
}

void ConfigWriter::write_to_config(const EverestModuleMapping& module_mapping, const std::string& config_value) {
    auto root = config_tree.rootref();
    if (!root.is_map()) {
        root |= ryml::MAP; // mark root as map
    }

    auto active_modules = root["active_modules"];
    if (!active_modules.is_map()) {
        active_modules |= ryml::MAP; // mark active_modules as map
    }

    const auto config_key_csubstr = ryml::to_csubstr(module_mapping.config_key.c_str());
    auto config_key_node = active_modules[config_key_csubstr];
    if (!config_key_node.is_map()) {
        config_key_node |= ryml::MAP; // mark config_key as map
    }

    const auto module_name_csubstr = ryml::to_csubstr(module_mapping.module_name.c_str());
    auto module_node = config_key_node[module_name_csubstr];
    if (!module_node.is_map()) {
        module_node |= ryml::MAP; // mark module as map
    }

    auto config_module_node = module_node["config_module"];
    config_module_node |= ryml::MAP;

    const auto module_config_key_csubstr = ryml::to_csubstr(module_mapping.module_config_key.c_str());
    auto module_config_key_node = config_module_node[module_config_key_csubstr];
    const auto config_value_csubstr = ryml::to_csubstr(config_value.c_str());
    module_config_key_node << config_value_csubstr;
}

// Function to save the updated configuration to a file
void ConfigWriter::save_config() {
    //    Util::save_yaml_file(user_config_file, config_tree);
}
ryml::Tree ConfigWriter::load_existing_user_config(const std::string& user_config_file) {
    try {
        // file exists
        return Util::load_yaml_file(user_config_file);
    }
    // file doesn't exist
    catch (const std::runtime_error& e) {
        return ryml::Tree();
    }
}

} // namespace module