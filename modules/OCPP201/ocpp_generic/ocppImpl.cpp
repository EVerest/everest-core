// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ocppImpl.hpp"
#include <conversions.hpp>

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
    const auto _requests = conversions::to_ocpp_get_variable_data_vector(requests);
    const auto response = this->mod->charge_point->get_variables(_requests);
    return conversions::to_everest_get_variable_result_vector(response);
}

std::vector<types::ocpp::SetVariableResult>
ocppImpl::handle_set_variables(std::vector<types::ocpp::SetVariableRequest>& requests) {
    const auto _requests = conversions::to_ocpp_set_variable_data_vector(requests);
    const auto response_map = this->mod->charge_point->set_variables(_requests);
    std::vector<ocpp::v201::SetVariableResult> response;
    for (const auto& [set_variable_data, set_variable_result] : response_map) {
        response.push_back(set_variable_result);
    }
    return conversions::to_everest_set_variable_result_vector(response);
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
