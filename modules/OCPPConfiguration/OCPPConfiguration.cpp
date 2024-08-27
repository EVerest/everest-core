// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "OCPPConfiguration.hpp"
#include "main/event_handler.hpp"
#include <everest/logging.hpp>

namespace module {

void OCPPConfiguration::init() {
    invoke_init(*p_main);
}

void OCPPConfiguration::ready() {
    invoke_ready(*p_main);

    const auto mapping_file_path = std::filesystem::path{config.mapping_file_name};

    try {
        event_handler = std::make_unique<EventHandler>(mapping_file_path);
    } catch (const std::runtime_error& e) {
        EVLOG_warning << "Failed to create event handler: " << e.what() << ".\nNo events will be handled!";
        return;
    }

    r_ocpp_module->call_monitor_variables(event_handler->get_monitor_variables());

    r_ocpp_module->subscribe_event_data(
        [&](const auto& event_data) { event_handler->try_handle_event(event_data, config.user_config_file_name); });
}

} // namespace module
