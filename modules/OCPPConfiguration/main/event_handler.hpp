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
     * @param config_mapping_file_name Name of the mapping file.
     * @param mapping_schema_file_name Name of the schema file.
     */
    EventHandler(const std::filesystem::path& config_mapping_file_name,
                 const std::filesystem::path& mapping_schema_file_name);

    /**
     * Try to handle the given event. If successful, the event will be written to the user configuration file given.
     *
     * @param event_data The event to handle. Usually received from the OCPP module.
     * @param user_config_file_name File name of the user configuration file. This name should match the loaded configs
     * name.
     */
    void try_handle_event(const types::ocpp::EventData& event_data,
                          const std::filesystem::path& user_config_file_name) noexcept;

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
    void write_event_to_config(const types::ocpp::EventData& event_data,
                               const std::filesystem::path& user_config_file_name,
                               const EverestConfigMapping& everest_module_mapping);
};

} // namespace module