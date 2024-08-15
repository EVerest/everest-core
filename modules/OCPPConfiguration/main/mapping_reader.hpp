// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <c4/yml/std/map.hpp>
#include <filesystem>
#include <ryml.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace module {

struct EverestModuleMapping {
    std::string module_id;
    std::string config_key;
};

using OcppToEverestModuleMapping = std::unordered_map<std::string, EverestModuleMapping>;

class MappingReader {
public:
    static OcppToEverestModuleMapping readMapping(const std::filesystem::path& file_path_string);

private:
    static EverestModuleMapping parseMappingNode(const ryml::NodeRef& node);
};

} // namespace module