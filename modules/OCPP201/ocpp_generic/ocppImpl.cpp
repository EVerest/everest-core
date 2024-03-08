// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ocppImpl.hpp"

namespace module {
namespace ocpp_generic {

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
    // your code for cmd set_variables goes here
    return {};
}

types::ocpp::ChangeAvailabilityResponse
ocppImpl::handle_change_availability(types::ocpp::ChangeAvailabilityRequest& request) {
    // your code for cmd change_availability goes here
    return {};
}

void ocppImpl::handle_monitor_variables(std::vector<types::ocpp::ComponentVariable>& component_variables) {
    // your code for cmd monitor_variables goes here
}

} // namespace ocpp_generic
} // namespace module
