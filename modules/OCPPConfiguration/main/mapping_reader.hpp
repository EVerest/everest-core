// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include "everest_config_mapping.hpp"
#include "generated/types/ocpp.hpp"
#include <c4/yml/std/map.hpp>
#include <filesystem>
#include <ryml.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace module {

using types::ocpp::Component;
using types::ocpp::ComponentVariable;
using types::ocpp::Variable;

using OcppToEverestConfigMapping = std::unordered_map<ComponentVariable, EverestConfigMapping>;

class MappingReader {
public:
    static OcppToEverestConfigMapping read_mapping(const std::filesystem::path& file_path);

private:
    static EverestConfigMapping parse_mapping_node(const ryml::NodeRef& node);
    static types::ocpp::Component parse_component_node(const c4::yml::NodeRef& node);
    static std::optional<types::ocpp::EVSE> parse_evse_node(const c4::yml::NodeRef node);
    static types::ocpp::Variable parse_variable_node(std::string variable_name, const c4::yml::NodeRef node);
    static ComponentVariable parse_component_variable_node(std::string variable_name, const c4::yml::NodeRef& node);
};

} // namespace module