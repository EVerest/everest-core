// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "mapping_reader.hpp"
#include "everest/logging.hpp"
#include "util.hpp"
#include <fstream>
#include <iostream>

namespace module {
EverestModuleMapping MappingReader::parseMappingNode(const ryml::NodeRef& node) {

    const auto parse_node = [](const auto& node) {
        const auto val = node.val();
        return std::string{val.str, val.len};
    };

    const auto config_key_node = node["config_key"];
    const auto module_name_node = node["module_name"];
    const auto module_config_key_node = node["module_config_key"];

    const auto config_key = parse_node(config_key_node);
    const auto module_name = parse_node(module_name_node);
    const auto module_config_key = parse_node(module_config_key_node);

    return {config_key, module_name, module_config_key};
}

OcppToEverestModuleMapping MappingReader::readMapping(const std::string& file_path) {
    const auto tree = Util::load_yaml_file(file_path);

    const auto root = tree.rootref();

    auto mapping = OcppToEverestModuleMapping{};

    for (const auto& child : root.children()) {
        const auto key = child.key();
        auto message_type = std::string{key.str, key.len};
        auto module_mapping = parseMappingNode(child);

        mapping.insert({std::move(message_type), std::move(module_mapping)});
    }

    return mapping;
}

} // namespace module