// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "event_handler.hpp"
#include "util.hpp"
#include <everest/logging.hpp>

#include <unordered_set>

namespace module {

EventHandler::EventHandler(const std::filesystem::path& config_mapping_file_name) :
    config_mapping(mapping_reader::read_mapping(config_mapping_file_name)) {
}

void EventHandler::try_handle_event(const types::ocpp::EventData& event_data,
                                    const std::filesystem::path& user_config_file_name) noexcept {
    try {
        const auto& everest_module_mapping_opt =
            get_optional_mapping_by_component_variable(event_data.component_variable);

        if (!everest_module_mapping_opt.has_value()) {
            return;
        }

        const auto& everest_module_mapping = everest_module_mapping_opt.value();

        write_event_to_config(event_data, user_config_file_name, everest_module_mapping);

        EVLOG_info << "Handled ocpp configuration request: " << event_data.component_variable.variable.name << " "
                   << event_data.actual_value;

    } catch (const std::runtime_error& e) {
        EVLOG_info << "Couldn't handle ocpp configuration request: " << e.what();
    }
}

std::vector<types::ocpp::ComponentVariable> EventHandler::get_monitor_variables() const noexcept {
    auto monitor_variables = std::unordered_set<types::ocpp::ComponentVariable>{};

    for (const auto& [component_variable, _] : config_mapping) {
        monitor_variables.insert(component_variable);
    }

    return {monitor_variables.begin(), monitor_variables.end()};
}

void EventHandler::write_event_to_config(const types::ocpp::EventData& event_data,
                                         const std::filesystem::path& user_config_file_name,
                                         const EverestConfigMapping& everest_module_mapping) {
    const auto user_config_path = std::filesystem::path{USER_CONFIG_INSTALL_DIRECTORY} / user_config_file_name;
    auto tree = util::try_to_load_existing_user_config(user_config_path);
    util::write_value_to_tree(everest_module_mapping, event_data.actual_value, tree);
    util::save_tree_to_yaml_file(tree, user_config_path);
}

const std::optional<EverestConfigMapping> EventHandler::get_optional_mapping_by_component_variable(
    const types::ocpp::ComponentVariable& component_variable) const noexcept {
    try {
        const auto& everest_module_mapping = find_mapping_by_component_variable(component_variable);
        return everest_module_mapping;
    } catch (const std::runtime_error& e) {
        return std::nullopt;
    }
}

const EverestConfigMapping
EventHandler::find_mapping_by_component_variable(const types::ocpp::ComponentVariable& component_variable) const {
    const auto& mapping = config_mapping.find(component_variable);

    // check if event is in event_map
    if (mapping == config_mapping.end()) {
        throw std::runtime_error("Component Variable not found in config mapping:\n Component:\t" +
                                 std::string(component_variable.component.name) + "\n Variable:\t" +
                                 std::string(component_variable.variable.name));
    }

    return mapping->second;
}
} // namespace module
