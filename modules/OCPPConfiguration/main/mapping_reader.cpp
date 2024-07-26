// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "mapping_reader.hpp"
#include "util.hpp"
#include <iostream>

namespace module {
MappingReader::EverestConfigPair MappingReader::parseMappingNode(const ryml::NodeRef& node) {
    auto config_pair = EverestConfigPair{};
    for (const auto& child : node.children()) {
        auto key = std::string{child.key().str};
        auto value = std::string{child.val().str};
        //        config_pair[key] = value;
    }
    return config_pair;
}

MappingReader::ConfigMap MappingReader::readMapping(const std::string& file_path) {
    auto tree = Util::load_yaml_file(file_path);
    auto mapping = ConfigMap{};

    for (const auto& child : tree.rootref().children()) {
        auto messageType = std::string{child.key().str};
        mapping[messageType] = parseMappingNode(child);
    }

    return mapping;
}

} // namespace module