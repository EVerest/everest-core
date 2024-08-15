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

    const auto module_id_node = node["module_id"];
    const auto config_key_node = node["config_key"];

    const auto module_id = parse_node(module_id_node);
    const auto config_key = parse_node(config_key_node);

    return {module_id, config_key};
}

OcppToEverestModuleMapping MappingReader::readMapping(const std::filesystem::path& file_path) {
    const auto hacked_file_path =
        std::filesystem::path{"modules"} / "OCPPConfiguration" / file_path; // TODO: this is very hacky
    const auto tree = Util::load_existing_user_config(hacked_file_path);

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