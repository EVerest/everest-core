// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "mapping_reader.hpp"
#include "everest/logging.hpp"
#include "util.hpp"
#include <fstream>
#include <iostream>

namespace module {
OcppToEverestConfigMapping MappingReader::read_mapping(const std::filesystem::path& file_path) {
    const auto hacked_file_path =
        std::filesystem::path{"modules"} / "OCPPConfiguration" / file_path; // TODO: this is very hacky
    const auto tree = Util::load_existing_user_config(hacked_file_path);

    const auto root = tree.rootref();

    auto mapping = OcppToEverestConfigMapping{};

    for (const auto& variables_sequence_node : root.children()) {
        const auto key = variables_sequence_node.key();
        const auto variable_name = std::string{key.str, key.len};

        for (const auto& mappings_sequence_node : variables_sequence_node.children()) {

            for (auto const& mapping_node : mappings_sequence_node.children()) {
                auto component_variable = parse_component_variable_node(variable_name, mapping_node);

                auto module_mapping = parse_mapping_node(mapping_node["maps_to"]);

                mapping.insert({std::move(component_variable), std::move(module_mapping)});
            }
        }
    }

    return mapping;
}

ComponentVariable MappingReader::parse_component_variable_node(std::string variable_name, const c4::yml::NodeRef& node) {
    auto component = node.has_child("component") ? parse_component_node(node["component"]) : Component{};
    auto variable = node.has_child("variable") ? parse_variable_node(variable_name, node["variable"])
                                               : Variable{.name = variable_name};
    return {std::move(component), std::move(variable)};
}

Component MappingReader::parse_component_node(const c4::yml::NodeRef& node) {
    auto evse = node.has_child("evse") ? parse_evse_node(node["evse"]) : std::nullopt;

    auto instance_optinal_val = node.has_child("instance") ? std::optional{node["instance"].val()} : std::nullopt;
    auto instance = instance_optinal_val.has_value()
                        ? std::optional{std::string{instance_optinal_val.value().str, instance_optinal_val.value().len}}
                        : std::nullopt;

    auto component_name = node.has_child("name") ? std::string{node["name"].val().str, node["name"].val().len} : "";
    return {std::move(component_name), std::move(instance), std::move(evse)};
}

Variable MappingReader::parse_variable_node(std::string variable_name, const c4::yml::NodeRef node) {
    auto instance_optional_val = node.has_child("instance") ? std::optional{node["instance"].val()} : std::nullopt;
    auto instance =
        instance_optional_val.has_value()
            ? std::optional{std::string{instance_optional_val.value().str, instance_optional_val.value().len}}
            : std::nullopt;
    return {std::move(variable_name), std::move(instance)};
}

std::optional<types::ocpp::EVSE> MappingReader::parse_evse_node(const c4::yml::NodeRef node) {
    if (!node.has_child("id")) {
        throw std::runtime_error("EVSE node must have an id");
    }
    auto id_val = node["id"].val();
    auto id = std::stoi(std::string{id_val.str, id_val.len});

    auto connector_id_optional_val = node.has_child("id") ? std::optional{node["id"].val()} : std::nullopt;
    auto connector_id = connector_id_optional_val.has_value()
                            ? std::optional{std::stoi(std::string{connector_id_optional_val.value().str,
                                                                  connector_id_optional_val.value().len})}
                            : std::nullopt;

    return types::ocpp::EVSE{std::move(id), std::move(connector_id)};
}

EverestConfigMapping MappingReader::parse_mapping_node(const ryml::NodeRef& node) {

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

} // namespace module