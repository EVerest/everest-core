// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "config_writer.hpp"
#include "util.hpp"
#include <fstream>
#include <sstream>

namespace module {

ConfigWriter::ConfigWriter(const std::string& user_config_file) : config_tree(Util::load_yaml_file(user_config_file)) {
}

void ConfigWriter::update_yaml_node(ryml::NodeRef node, const std::vector<std::string>& path_parts,
                                    const std::string& value) {
    ryml::NodeRef current = node;
    //    for (size_t i = 0; i < path_parts.size() - 1; ++i) {
    //        if (!current.has_child(path_parts[i].c_str())) {
    //            current |= ryml::MAP;
    //            current[path_parts[i].c_str()] = ryml::NodeRef{};
    //        }
    //        current = current[path_parts[i].c_str()];
    //    }
    //    current[path_parts.back().c_str()] = value;
}

void ConfigWriter::write_to_config(const std::string& module_name, const std::string& config_key,
                                   const std::string& config_value) {
    //    auto data_it = command_data.find(key);
    //    if (data_it != command_data.end()) {
    //        // Replace placeholders in the path
    //        std::string config_path = path;
    //        size_t pos = config_path.find("{id}");
    //        if (pos != std::string::npos) {
    //            config_path.replace(pos, 4, command_data.at("connector_id"));
    //        }
    //
    //        // Split the path into parts
    //        std::vector<std::string> path_parts;
    //        std::stringstream ss(config_path);
    //        std::string item;
    //        while (std::getline(ss, item, '.')) {
    //            path_parts.push_back(item);
    //        }
    //
    //        // Update the configuration
    //        update_yaml_node(config_tree.rootref(), path_parts, data_it->second);
    //    }
}

// Function to save the updated configuration to a file
void ConfigWriter::save_config() {
    Util::save_yaml_file(user_config_file, config_tree);
}

} // namespace module