// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "BUOcppConsumer.hpp"

// for the moment use std::cout
// #include <ftxui/component/component.hpp>
// #include <ftxui/component/screen_interactive.hpp>
// #include <ftxui/dom/table.hpp>

#include <iostream>

namespace module {

void BUOcppConsumer::init() {
    invoke_init(*p_ocpp);
    invoke_init(*p_ocpp_data_transfer);
}

void BUOcppConsumer::ready() {
    invoke_ready(*p_ocpp);
    invoke_ready(*p_ocpp_data_transfer);
    std::cout << "Ready...\n";
}

void BUOcppConsumer::event(const types::ocpp::GetVariableResult& ev) {
    std::cout << "GetVariable: " << ev.component_variable.variable.name << '=' << ev.value.value_or("<none>") << " ("
              << ev.status << ")\n";
}

void BUOcppConsumer::event(const std::string& value, const types::ocpp::SetVariableResult& ev) {
    std::cout << "SetVariable: " << ev.component_variable.variable.name << '=' << value << " (" << ev.status << ")\n";
}

void BUOcppConsumer::event_monitor_variable(const std::string& name) {
    std::cout << "MonitorVariable: " << name << '\n';
}

} // namespace module
