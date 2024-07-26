// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include "mapping_reader.hpp"
#include <c4/yml/std/map.hpp>
#include <ryml.hpp>
#include <string>
#include <unordered_map>

namespace module {
class ConfigWriter {
public:
    ConfigWriter(const std::string& user_config_file);

    void write_to_config(const std::string& module_name, const std::string& config_key,
                         const std::string& config_value);
    void save_config();

private:
    void update_yaml_node(ryml::NodeRef node, const std::vector<std::string>& path_parts, const std::string& value);

    ryml::Tree config_tree;
    const std::string user_config_file;
};

} // namespace module
