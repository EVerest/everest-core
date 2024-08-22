// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include "generated/types/ocpp.hpp"
#include "mapping_reader.hpp"

namespace module {

/**
 * Handles incoming events and writes them to the user configuration.
 *
 * The EventHandler builds an in memory dictionary out of the mapping file.
 * It uses this dictionary to look up given events and write them to the user configuration with their values.
 */
class EventHandler {
public:
    /**
     * Create an EventHandler with the given mapping file.
     *
     * @param config_mapping_path Path to the mapping file.
     */
    explicit EventHandler(const std::filesystem::path& config_mapping_path);

    /**
     * Try to handle the given event. If successful, the event will be written to the user configuration file given.
     *
     * @param event_data The event to handle. Usually received from the OCPP module.
     * @param user_config_path_string The path to the user configuration file. This file will be read from and written
     * to.
     */
    void try_handle_event(const types::ocpp::EventData& event_data,
                          const std::string& user_config_path_string) noexcept;

    /**
     * Get the unique monitor variables that are used by this EventHandler.
     */
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
