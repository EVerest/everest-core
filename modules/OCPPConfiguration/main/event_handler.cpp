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
    EVLOG_critical << "Received event data:";
    EVLOG_critical << event_data.component_variable.variable.name;
    EVLOG_critical << event_data.actual_value;

    // lookup event in event_map
    const auto& event_name = event_data.component_variable.variable.name;

    const auto& config_pair = find_event_in_map(event_name);

    // write event to config_pair
    config_writer.write_to_config(std::get<0>(config_pair), std::get<1>(config_pair), event_data.actual_value);
    config_writer.save_config();
}

const EventHandler::EverestConfigPair EventHandler::find_event_in_map(const std::string& event_name) const {
    const auto& event = event_map.find(event_name);

    // check if event is in event_map
    if (event == event_map.end()) {
        throw std::runtime_error("Event not found in event map: " + std::string(event_name));
    }
}

} // namespace module
