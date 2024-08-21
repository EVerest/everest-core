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
    const std::optional<EverestConfigMapping>
    find_mapping_by_component_variable_or_log_error(const types::ocpp::ComponentVariable& component_variable) const;
    const EverestConfigMapping
    find_mapping_by_component_variable(const types::ocpp::ComponentVariable& component_variable) const;

    OcppToEverestConfigMapping config_mapping;
    void write_event_to_config(const types::ocpp::EventData& event_data, const std::string& user_config_path_string,
                               const EverestConfigMapping& everest_module_mapping);
};

} // namespace module
