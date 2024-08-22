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

    const auto mapping_file_path = std::filesystem::path{config.mapping_file_path};
    event_handler = std::make_unique<EventHandler>(mapping_file_path);

    r_ocpp_module->call_monitor_variables(parse_config_monitor_variables());

    r_ocpp_module->subscribe_event_data(
        [&](const auto& event_data) { event_handler->handle_event(event_data, config.user_config_path); });
}

std::vector<types::ocpp::ComponentVariable> OCPPConfiguration::parse_config_monitor_variables() {
    const auto& monitor_variables_string = config.monitor_variables;

    if (monitor_variables_string.empty()) {
        EVLOG_info << "No monitor variables configured.";
        return std::vector<types::ocpp::ComponentVariable>();
    }

    EVLOG_info << "Monitoring variables: " << monitor_variables_string;

    // Split the monitor variables string by comma
    auto component_variables = std::vector<types::ocpp::ComponentVariable>();
    auto ss = std::stringstream{monitor_variables_string};
    auto variable_name = std::string{};

    while (std::getline(ss, variable_name, ',')) {
        auto component_variable = types::ocpp::ComponentVariable{};
        component_variable.variable.name = variable_name;
        component_variables.push_back(component_variable);
    }

    return component_variables;
}

} // namespace module
