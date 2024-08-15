// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "util.hpp"
#include "everest/logging.hpp"
#include "generated/types/ocpp.hpp"
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

void write_value_to_tree(const EverestModuleMapping& module_mapping, const std::string& config_value,
                         const types::ocpp::Component& component, ryml::Tree& config_tree) {

    auto root = config_tree.rootref();
    if (!root.is_map()) {
        root |= ryml::MAP; // mark root as map
    }

    auto active_modules = root["active_modules"];
    active_modules |= ryml::MAP; // mark active_modules as map

    const auto module_id_csubstr = ryml::to_csubstr(module_mapping.module_id.c_str());
    auto module_id_node = active_modules[module_id_csubstr];
    module_id_node |= ryml::MAP; // mark module_id as map

    // component
    add_component_if_exists(component, module_id_node);

    auto config_module_node = module_id_node["config_module"];
    config_module_node |= ryml::MAP;

    const auto config_key_csubstr = ryml::to_csubstr(module_mapping.config_key.c_str());
    auto config_key_node = config_module_node[config_key_csubstr];
    const auto config_value_csubstr = ryml::to_csubstr(config_value.c_str());
    config_key_node << config_value_csubstr;
}

void add_component_if_exists(const types::ocpp::Component& component, c4::yml::NodeRef& module_id_node) {
    if (component.name.empty()) {
        if (module_id_node.has_child("component")) {
            module_id_node.remove_child("component");
        }
        return;
    }

    const auto component_name_csubstr = ryml::to_csubstr(component.name.c_str());
    const auto component_instance_csubstr =
        ryml::to_csubstr(component.instance.has_value() ? component.instance.value().c_str() : "");
    const auto component_evse_csubstr =
        ryml::to_csubstr(component.evse.has_value() ? std::to_string(component.evse.value().id).c_str() : "");

    auto component_node = module_id_node["component"];
    component_node |= ryml::MAP;
    auto component_name_node = component_node["name"];

    component_name_node << component_name_csubstr;
    auto component_instance_node = component_node["instance"];
    component_instance_node << component_instance_csubstr;
    auto component_evse_node = component_node["evse_id"];
    component_evse_node << component_evse_csubstr;
}

} // namespace Util
} // namespace module