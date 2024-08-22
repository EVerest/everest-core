// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include "generated/types/ocpp.hpp"
#include "mapping_reader.hpp"

namespace module {

class EventHandler {
public:
    EventHandler(const std::filesystem::path& config_mapping_path);
    void try_handle_event(const types::ocpp::EventData& event_data, const std::string& user_config_path_string) noexcept;
    std::vector<types::ocpp::ComponentVariable> get_monitor_variables() const noexcept;

private:
    const std::optional<EverestConfigMapping>
    get_optional_mapping_by_component_variable(const types::ocpp::ComponentVariable& component_variable) const noexcept;
    const EverestConfigMapping
    find_mapping_by_component_variable(const types::ocpp::ComponentVariable& component_variable) const;

    OcppToEverestConfigMapping config_mapping;
    void write_event_to_config(const types::ocpp::EventData& event_data, const std::string& user_config_path_string,
                               const EverestConfigMapping& everest_module_mapping);
};

} // namespace module
