// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ocppImpl.hpp"

namespace module {
namespace ocpp {

void ocppImpl::init() {
    // TODO(james-ctc): populate variable_store with some initial values
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

void ocppImpl::handle_security_event(types::ocpp::SecurityEvent& event) {
    // your code for cmd security_event goes here
}

std::vector<types::ocpp::GetVariableResult>
ocppImpl::handle_get_variables(std::vector<types::ocpp::GetVariableRequest>& requests) {
    // your code for cmd get_variables goes here
    std::vector<types::ocpp::GetVariableResult> resultList;
    for (const auto& i : requests) {
        types::ocpp::GetVariableResult result{types::ocpp::GetVariableStatusEnumType::Rejected, i.component_variable};
        const std::string& name = i.component_variable.variable.name;
        if (const auto it = variable_store.find(name); it != variable_store.end()) {
            result.value = it->second;
            result.status = types::ocpp::GetVariableStatusEnumType::Accepted;
        }
        mod->event(result);
        resultList.push_back(result);
    }
    return resultList;
}

std::vector<types::ocpp::SetVariableResult>
ocppImpl::handle_set_variables(std::vector<types::ocpp::SetVariableRequest>& requests, std::string& source) {
    // your code for cmd set_variables goes here
    std::vector<types::ocpp::SetVariableResult> resultList;
    for (const auto& i : requests) {
        types::ocpp::SetVariableResult result{types::ocpp::SetVariableStatusEnumType::Rejected, i.component_variable};
        const std::string& name = i.component_variable.variable.name;
        if (!name.empty()) {
            variable_store[name] = i.value;
            result.status = types::ocpp::SetVariableStatusEnumType::Accepted;
            if (const auto it = std::find(variable_monitor.begin(), variable_monitor.end(), name);
                it != variable_monitor.end()) {
                types::ocpp::EventData data{i.component_variable,
                                            -1,
                                            Everest::Date::to_rfc3339(date::utc_clock::now()),
                                            types::ocpp::EventTriggerEnum::Delta,
                                            i.value,
                                            types::ocpp::EventNotificationType::PreconfiguredMonitor};
                mod->p_ocpp->publish_event_data(data);
            }
        }
        mod->event(i.value, result);
        resultList.push_back(result);
    }
    return resultList;
}

types::ocpp::ChangeAvailabilityResponse
ocppImpl::handle_change_availability(types::ocpp::ChangeAvailabilityRequest& request) {
    // your code for cmd change_availability goes here
    return {};
}

void ocppImpl::handle_monitor_variables(std::vector<types::ocpp::ComponentVariable>& component_variables) {
    // your code for cmd monitor_variables goes here
    for (const auto& i : component_variables) {
        const std::string& name = i.variable.name;
        if (!name.empty()) {
            variable_monitor.push_back(name);
            mod->event_monitor_variable(name);
        }
    }
}

} // namespace ocpp
} // namespace module
