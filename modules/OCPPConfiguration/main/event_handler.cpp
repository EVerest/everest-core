// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "event_handler.hpp"
#include "util.hpp"
#include <everest/logging.hpp>

namespace module {

EventHandler::EventHandler(const std::filesystem::path& config_mapping_path) :
    event_map(MappingReader::readMapping(config_mapping_path)) {
}

void EventHandler::handleEvent(const types::ocpp::EventData& event_data, const std::string& user_config_path_string) {

    // todo(tm): needs a way to handle ocpp 2.0.1 component variable combinations
    //           maybe add this to the mapping file
    const auto& event_name = event_data.component_variable.variable.name;
    const auto& component = event_data.component_variable.component;

    const auto& everest_module_mapping_opt = find_event_in_map_or_log_error(event_name);
    if (!everest_module_mapping_opt.has_value()) {
        return;
    }

    const auto& everest_module_mapping = everest_module_mapping_opt.value();

    write_event_to_config(event_data, user_config_path_string, everest_module_mapping, component);
}
void EventHandler::write_event_to_config(const types::ocpp::EventData& event_data, const std::string& user_config_path_string, const EverestModuleMapping& everest_module_mapping, const types::ocpp::Component& component) {
    const auto user_config_path = std::filesystem::path{user_config_path_string};
    auto tree = Util::load_existing_user_config(user_config_path);
    Util::write_value_to_tree(everest_module_mapping, event_data.actual_value, component, tree);
    Util::save_tree_to_yaml_file(tree, user_config_path);
}

const std::optional<EverestModuleMapping>
EventHandler::find_event_in_map_or_log_error(const std::string& event_name) const {
    try {
        const auto& everest_module_mapping = find_event_in_map(event_name);
        return everest_module_mapping;
    } catch (const std::runtime_error& e) {
        EVLOG_error << e.what();
        return std::nullopt;
    }
}

const EverestModuleMapping EventHandler::find_event_in_map(const std::string& event_name) const {
    const auto& event = event_map.find(event_name);

    // check if event is in event_map
    if (event == event_map.end()) {
        throw std::runtime_error("Event not found in event map: " + std::string(event_name));
    }

    return event->second;
}

} // namespace module
