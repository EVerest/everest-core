// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <c4/yml/std/map.hpp>
#include <ryml.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace module {

struct EverestModuleMapping {
    std::string config_key;
    std::string module_name;
    std::string module_config_key;
};

using OcppToEverestModuleMapping = std::unordered_map<std::string, EverestModuleMapping>;

class MappingReader {
public:
    static OcppToEverestModuleMapping readMapping(const std::string& file_path);

private:
    static EverestModuleMapping parseMappingNode(const ryml::NodeRef& node);
};

} // namespace module