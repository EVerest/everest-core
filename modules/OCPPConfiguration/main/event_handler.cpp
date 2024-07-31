// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "event_handler.hpp"
#include <everest/logging.hpp>

namespace module {

EventHandler::EventHandler(const std::string& config_mapping_path, const std::string& user_config_path) :
    event_map(MappingReader::readMapping(config_mapping_path)), config_writer(user_config_path) {
    EVLOG_info << "EventHandler initialized";
}

void EventHandler::handleEvent(const types::ocpp::EventData& event_data) {
    // lookup event in event_map
    const auto& event_name = event_data.component_variable.variable.name;

    const auto& everest_module_mapping_opt = find_event_in_map_or_log_error(event_name);
    if (!everest_module_mapping_opt.has_value()) {
        return;
    }

    const auto& everest_module_mapping = everest_module_mapping_opt.value();

    // write event to config_pair
    config_writer.write_to_config(everest_module_mapping.config_key, everest_module_mapping.module_name,
                                  <#initializer #>, event_data.actual_value);
    config_writer.save_config();
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
