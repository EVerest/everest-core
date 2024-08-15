// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include "generated/types/ocpp.hpp"
#include "mapping_reader.hpp"

namespace module {

class EventHandler {
public:
    EventHandler(const std::filesystem::path& config_mapping_path);
    void handleEvent(const types::ocpp::EventData& event_data, const std::string& user_config_path_string);

private:
    const std::optional<EverestModuleMapping> find_event_in_map_or_log_error(const std::string& event_name) const;
    const EverestModuleMapping find_event_in_map(const std::string& event_name) const;

    OcppToEverestModuleMapping event_map;
    void write_event_to_config(const types::ocpp::EventData& event_data, const std::string& user_config_path_string,
                               const EverestModuleMapping& everest_module_mapping,
                               const types::ocpp::Component& component);
};

} // namespace module
