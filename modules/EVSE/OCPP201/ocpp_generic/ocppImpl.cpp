// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ocppImpl.hpp"
#include "everest/conversions/ocpp/evse_security_ocpp.hpp"
#include "ocpp/v2/ocpp_types.hpp"
#include <conversions.hpp>
#include <everest/conversions/ocpp/ocpp_conversions.hpp>

namespace {
inline module::ocpp_generic::ocppImpl::MonitorListEntry convert(const types::ocpp::ComponentVariable& cv) {
    using namespace module::conversions;
    return {to_ocpp_component(cv.component), to_ocpp_variable(cv.variable)};
}
} // namespace

namespace module {
namespace ocpp_generic {

void ocppImpl::init() {
}

void ocppImpl::ready() {
}

bool ocppImpl::handle_stop() {
    // Disconnects the websocket connection and stops the OCPP communication.
    // No OCPP messages will be stored and sent after a restart.

    bool result{false};
    if (mod->charge_point == nullptr) {
        EVLOG_warning << "ChargePoint not initialized, cannot handle stop command";
    } else {
        std::lock_guard lock(chargepoint_state_mutex);
        mod->charging_schedules_timer_stop();
        mod->charge_point->stop();
        result = true;
    }
    return result;
}

bool ocppImpl::handle_restart() {
    // Connects the websocket and enables OCPP communication after a previous
    // stop call.

    bool result{false};
    if (mod->charge_point == nullptr) {
        EVLOG_warning << "ChargePoint not initialized, cannot handle restart command";
    } else {
        std::lock_guard lock(chargepoint_state_mutex);
        mod->charging_schedules_timer_start();
        mod->charge_point->start(ocpp::v2::BootReasonEnum::ApplicationReset, true);
        result = true;
    }
    return result;
}

void ocppImpl::handle_security_event(types::ocpp::SecurityEvent& event) {
    if (this->mod->charge_point == nullptr) {
        EVLOG_warning << "ChargePoint not yet initialized. Cannot handle security event.";
        return;
    }

    std::optional<ocpp::DateTime> timestamp;
    if (event.timestamp.has_value()) {
        timestamp = ocpp_conversions::to_ocpp_datetime_or_now(event.timestamp.value());
    }
    this->mod->charge_point->on_security_event(event.type, event.info, event.critical, timestamp);
}

std::vector<types::ocpp::GetVariableResult>
ocppImpl::handle_get_variables(std::vector<types::ocpp::GetVariableRequest>& requests) {
    if (this->mod->charge_point == nullptr) {
        EVLOG_warning << "ChargePoint not yet initialized. Cannot handle get variables request.";
        std::vector<types::ocpp::GetVariableResult> results;
        for (const auto& req : requests) {
            types::ocpp::GetVariableResult result;
            result.status = types::ocpp::GetVariableStatusEnumType::Rejected;
            result.component_variable.component = req.component_variable.component;
            result.component_variable.variable = req.component_variable.variable;
            result.attribute_type = req.attribute_type;
            results.push_back(result);
        }
        return results;
    }

    const auto _requests = conversions::to_ocpp_get_variable_data_vector(requests);
    const auto response = this->mod->charge_point->get_variables(_requests);
    return conversions::to_everest_get_variable_result_vector(response);
}

std::vector<types::ocpp::SetVariableResult>
ocppImpl::handle_set_variables(std::vector<types::ocpp::SetVariableRequest>& requests, std::string& source) {
    if (this->mod->charge_point == nullptr) {
        EVLOG_warning << "ChargePoint not yet initialized. Cannot handle set variables request.";
        std::vector<types::ocpp::SetVariableResult> results;
        for (const auto& req : requests) {
            types::ocpp::SetVariableResult result;
            result.status = types::ocpp::SetVariableStatusEnumType ::Rejected;
            result.component_variable.component = req.component_variable.component;
            result.component_variable.variable = req.component_variable.variable;
            result.attribute_type = req.attribute_type;
            results.push_back(result);
        }
        return results;
    }

    const auto _requests = conversions::to_ocpp_set_variable_data_vector(requests);
    const auto response_map = this->mod->charge_point->set_variables(_requests, source);
    std::vector<ocpp::v2::SetVariableResult> response;
    for (const auto& [set_variable_data, set_variable_result] : response_map) {
        response.push_back(set_variable_result);
    }
    return conversions::to_everest_set_variable_result_vector(response);
}

types::ocpp::ChangeAvailabilityResponse
ocppImpl::handle_change_availability(types::ocpp::ChangeAvailabilityRequest& request) {
    using ChangeAvailabilityStatusEnum = ocpp::v2::ChangeAvailabilityStatusEnum;
    ocpp::v2::ChangeAvailabilityResponse result;
    result.status = ChangeAvailabilityStatusEnum::Rejected;

    if (mod->charge_point == nullptr) {
        EVLOG_warning << "ChargePoint not initialized, cannot handle change availability command";
    } else {
        const auto ocpp_request = conversions::to_ocpp_change_availability_request(request);
        result = mod->charge_point->on_change_availability(ocpp_request);
    }

    return conversions::to_everest_change_availability_response(result);
}

void ocppImpl::handle_monitor_variables(std::vector<types::ocpp::ComponentVariable>& component_variables) {
    using namespace ocpp::v2;

    if (mod->charge_point == nullptr) {
        EVLOG_warning << "ChargePoint not initialized, cannot handle monitor variables command";
    } else {
        std::lock_guard lock(monitor_list_mutex);

        if (monitor_list.empty()) {
            // register a handler
            mod->charge_point->register_variable_listener(
                [this](auto&, const Component& component, const Variable& variable, auto&, auto&, auto&,
                       const std::string& value) { variable_changed(component, variable, value); });
        }

        // add variables to monitor list
        for (const auto& cv : component_variables) {
            // failures to insert are likely to be the same variable being
            // requested again
            (void)monitor_list.insert(convert(cv));
        }
    }
}

void ocppImpl::variable_changed(const ocpp::v2::Component& component, const ocpp::v2::Variable& variable,
                                const std::string& value) {
    using namespace conversions;

    MonitorListEntry entry{component, variable};
    bool publish;
    {
        std::lock_guard lock(monitor_list_mutex);
        const auto it = monitor_list.find(entry);
        publish = it != monitor_list.end();
    }
    if (publish) {
        // monitor entry exists - publish
        types::ocpp::EventData event_data;
        event_data.component_variable.component = to_everest_component(component);
        event_data.component_variable.variable = to_everest_variable(variable);
        event_data.event_id = 0;
        event_data.timestamp = ocpp::DateTime();
        event_data.trigger = types::ocpp::EventTriggerEnum::Delta;
        event_data.actual_value = value;
        event_data.event_notification_type = types::ocpp::EventNotificationType::CustomMonitor;
        publish_event_data(event_data);
    }
}

} // namespace ocpp_generic
} // namespace module
