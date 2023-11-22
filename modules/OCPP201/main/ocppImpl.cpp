// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ocppImpl.hpp"

namespace module {
namespace main {

void ocppImpl::init() {
}

void ocppImpl::ready() {
}

bool ocppImpl::handle_stop() {
    // your code for cmd stop goes here
    return true;
}

bool ocppImpl::handle_restart() {
    // your code for cmd restart goes here
    return true;
}

void ocppImpl::handle_security_event(std::string& type, std::string& info) {
    // your code for cmd security_event goes here
}

std::vector<types::ocpp::GetVariableResult>
ocppImpl::handle_get_variables(std::vector<types::ocpp::GetVariableRequest>& requests) {
    // your code for cmd get_variables goes here
    return {};
}

std::vector<types::ocpp::SetVariableResult>
ocppImpl::handle_set_variables(std::vector<types::ocpp::SetVariableRequest>& requests) {
    return {};
}

void ocppImpl::handle_monitor_variables(std::vector<types::ocpp::ComponentVariable>& component_variables) {
    // your code for cmd monitor_variables goes here
}

} // namespace main
} // namespace module
