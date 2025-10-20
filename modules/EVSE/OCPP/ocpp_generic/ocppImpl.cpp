// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ocppImpl.hpp"
#include "ocpp/v16/messages/ChangeAvailability.hpp"

#include <everest/conversions/ocpp/ocpp_conversions.hpp>

namespace module {
namespace ocpp_generic {

types::ocpp::KeyValue to_everest(const ocpp::v16::KeyValue& key_value) {
    types::ocpp::KeyValue _key_value;
    _key_value.key = key_value.key.get();
    _key_value.read_only = key_value.readonly;
    if (key_value.value.has_value()) {
        _key_value.value = key_value.value.value().get();
    }
    return _key_value;
}

types::ocpp::ConfigurationStatus to_everest(const ocpp::v16::ConfigurationStatus status) {
    switch (status) {
    case ocpp::v16::ConfigurationStatus::Accepted:
        return types::ocpp::ConfigurationStatus::Accepted;
    case ocpp::v16::ConfigurationStatus::Rejected:
        return types::ocpp::ConfigurationStatus::Rejected;
    case ocpp::v16::ConfigurationStatus::RebootRequired:
        return types::ocpp::ConfigurationStatus::RebootRequired;
    case ocpp::v16::ConfigurationStatus::NotSupported:
        return types::ocpp::ConfigurationStatus::NotSupported;
    default:
        EVLOG_warning << "Could not convert to ConfigurationStatus";
        return types::ocpp::ConfigurationStatus::Rejected;
    }
}

types::ocpp::GetConfigurationResponse to_everest(const ocpp::v16::GetConfigurationResponse& response) {
    types::ocpp::GetConfigurationResponse _response;
    std::vector<types::ocpp::KeyValue> configuration_keys;
    std::vector<std::string> unknown_keys;

    if (response.configurationKey.has_value()) {
        for (const auto& item : response.configurationKey.value()) {
            configuration_keys.push_back(to_everest(item));
        }
    }

    if (response.unknownKey.has_value()) {
        for (const auto& item : response.unknownKey.value()) {
            unknown_keys.push_back(item.get());
        }
    }

    _response.configuration_keys = configuration_keys;
    _response.unknown_keys = unknown_keys;
    return _response;
}

ocpp::v16::AvailabilityStatus to_ocpp(const types::ocpp::ChangeAvailabilityStatusEnumType& status) {
    switch (status) {
    case types::ocpp::ChangeAvailabilityStatusEnumType::Accepted:
        return ocpp::v16::AvailabilityStatus::Accepted;
    case types::ocpp::ChangeAvailabilityStatusEnumType::Rejected:
        return ocpp::v16::AvailabilityStatus::Rejected;
    case types::ocpp::ChangeAvailabilityStatusEnumType::Scheduled:
        return ocpp::v16::AvailabilityStatus::Scheduled;
    }
    throw std::out_of_range("unknown ChangeAvailabilityStatusEnumType");
}

types::ocpp::ChangeAvailabilityStatusEnumType to_everest(const ocpp::v16::AvailabilityStatus& status) {
    switch (status) {
    case ocpp::v16::AvailabilityStatus::Accepted:
        return types::ocpp::ChangeAvailabilityStatusEnumType::Accepted;
    case ocpp::v16::AvailabilityStatus::Rejected:
        return types::ocpp::ChangeAvailabilityStatusEnumType::Rejected;
    case ocpp::v16::AvailabilityStatus::Scheduled:
        return types::ocpp::ChangeAvailabilityStatusEnumType::Scheduled;
    }
    throw std::out_of_range("unknown AvailabilityStatus");
}

ocpp::v16::AvailabilityType to_ocpp(const types::ocpp::OperationalStatusEnumType& status) {
    switch (status) {
    case types::ocpp::OperationalStatusEnumType::Operative:
        return ocpp::v16::AvailabilityType::Operative;
    case types::ocpp::OperationalStatusEnumType::Inoperative:
        return ocpp::v16::AvailabilityType::Inoperative;
    }
    throw std::out_of_range("unknown OperationalStatusEnumType");
}

types::ocpp::ChangeAvailabilityResponse to_everest(const ocpp::v16::ChangeAvailabilityResponse& response) {
    types::ocpp::ChangeAvailabilityResponse everest_response{};
    everest_response.status = to_everest(response.status);
    return everest_response;
}

void ocppImpl::init() {
}

void ocppImpl::ready() {
}

bool ocppImpl::handle_stop() {
    if (this->mod->charge_point == nullptr) {
        EVLOG_warning << "ChargePoint not initialized, cannot handle stop command";
        return false;
    }

    std::lock_guard<std::mutex> lock(this->chargepoint_state_mutex);
    mod->charging_schedules_timer->stop();
    bool success = mod->charge_point->stop();
    if (success) {
        this->mod->ocpp_stopped = true;
    }
    return success;
}

bool ocppImpl::handle_restart() {
    if (this->mod->charge_point == nullptr) {
        EVLOG_warning << "ChargePoint not initialized, cannot handle restart command";
        return false;
    }

    std::lock_guard<std::mutex> lock(this->chargepoint_state_mutex);
    mod->charging_schedules_timer->interval(std::chrono::seconds(this->mod->config.PublishChargingScheduleIntervalS));
    bool success = mod->charge_point->restart();
    if (success) {
        this->mod->ocpp_stopped = false;
    }
    return success;
}

void ocppImpl::handle_security_event(types::ocpp::SecurityEvent& event) {
    if (this->mod->charge_point == nullptr) {
        EVLOG_warning << "ChargePoint not initialized, cannot handle security event command";
        return;
    }

    std::optional<ocpp::DateTime> timestamp;
    if (event.timestamp.has_value()) {
        timestamp = ocpp_conversions::to_ocpp_datetime_or_now(event.timestamp.value());
    }
    const auto event_type = ocpp::CiString<50>(event.type, ocpp::StringTooLarge::Truncate);
    std::optional<ocpp::CiString<255>> tech_info;
    if (event.info.has_value()) {
        tech_info = ocpp::CiString<255>(event.info.value(), ocpp::StringTooLarge::Truncate);
    }
    this->mod->charge_point->on_security_event(event_type, tech_info, event.critical, timestamp);
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

    std::vector<types::ocpp::GetVariableResult> results;

    // prepare ocpp_request to request configuration keys from ocpp::v16::ChargePoint
    ocpp::v16::GetConfigurationRequest ocpp_request;
    std::vector<ocpp::CiString<50>> configuration_keys;
    std::map<std::string, int> configuration_key_indices{};
    int request_index = 0;
    for (const auto& request : requests) {
        // only variable.name is relevant for OCPP1.6
        configuration_keys.emplace_back(request.component_variable.variable.name);
        configuration_key_indices[request.component_variable.variable.name] = request_index++;
    }
    ocpp_request.key = configuration_keys;

    // request configuration keys from ocpp::v16::ChargePoint
    const auto ocpp_response = this->mod->charge_point->get_configuration_key(ocpp_request);

    std::map<std::string, types::ocpp::GetVariableRequest> results_map{};
    if (ocpp_response.configurationKey.has_value()) {
        for (const auto& key_value : ocpp_response.configurationKey.value()) {
            // add result for each present configurationKey in the response
            types::ocpp::GetVariableResult result;
            result.component_variable = {
                {""},
                {key_value.key.get()}}; // we don't care about the component, only about the variable.name in OCPP1.6
            if (key_value.value.has_value()) {
                result.value = key_value.value.value().get();
                result.status = types::ocpp::GetVariableStatusEnumType::Accepted;
                result.attribute_type = types::ocpp::AttributeEnum::Actual;
            } else {
                result.status = types::ocpp::GetVariableStatusEnumType::UnknownVariable;
            }
            results.push_back(result);
        }
    }

    if (ocpp_response.unknownKey.has_value()) {
        for (const auto& key : ocpp_response.unknownKey.value()) {
            // add result for each unknownKey in the response
            types::ocpp::GetVariableResult result;
            result.component_variable = {
                {""}, {key.get()}}; // we don't care about the component, only about the variable.name in OCPP1.6
            result.status = types::ocpp::GetVariableStatusEnumType::UnknownVariable;
            results.push_back(result);
        }
    }

    std::sort(results.begin(), results.end(),
              [&configuration_key_indices](const types::ocpp::GetVariableResult& result_a,
                                           const types::ocpp::GetVariableResult& result_b) {
                  return configuration_key_indices[result_a.component_variable.variable.name] <
                         configuration_key_indices[result_b.component_variable.variable.name];
              });

    return results;
}

std::vector<types::ocpp::SetVariableResult>
ocppImpl::handle_set_variables(std::vector<types::ocpp::SetVariableRequest>& requests, std::string& /*source*/) {

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

    std::vector<types::ocpp::SetVariableResult> results;

    for (const auto& request : requests) {
        // add result for each present SetVariableRequest in the response
        types::ocpp::SetVariableResult result;
        result.component_variable = request.component_variable;

        // retrieve key and value from the request
        auto key = request.component_variable.variable.name;
        auto value = request.value;
        auto response = this->mod->charge_point->set_custom_configuration_key(key, value);

        switch (response) {
        case ocpp::v16::ConfigurationStatus::Accepted:
            result.status = types::ocpp::SetVariableStatusEnumType::Accepted;
            break;
        case ocpp::v16::ConfigurationStatus::RebootRequired:
            result.status = types::ocpp::SetVariableStatusEnumType::RebootRequired;
            break;
        case ocpp::v16::ConfigurationStatus::Rejected:
            result.status = types::ocpp::SetVariableStatusEnumType::Rejected;
            break;
        case ocpp::v16::ConfigurationStatus::NotSupported:
            // NotSupported in OCPP1.6 means that the configuration key is not known / not supported, so it's best to go
            // with UnknownVariable
            result.status = types::ocpp::SetVariableStatusEnumType::UnknownVariable;
            break;
        }
        results.push_back(result);
    }

    return results;
}

void ocppImpl::handle_monitor_variables(std::vector<types::ocpp::ComponentVariable>& component_variables) {

    if (this->mod->charge_point == nullptr) {
        EVLOG_warning << "Could not monitor variables. ChargePoint not initialized";
        return;
    }

    for (const auto& cv : component_variables) {
        this->mod->charge_point->register_configuration_key_changed_callback(
            cv.variable.name, // we dont care about the component, only about the variable.name in OCPP1.6
            [this, cv](const ocpp::v16::KeyValue key_value) {
                types::ocpp::EventData event_data;
                event_data.component_variable = cv;
                event_data.event_id = 0; // irrelevant for OCPP1.6
                event_data.timestamp = ocpp::DateTime();
                event_data.trigger = types::ocpp::EventTriggerEnum::Alerting; // default for OCPP1.6
                event_data.actual_value = key_value.value.value_or("");
                event_data.event_notification_type =
                    types::ocpp::EventNotificationType::CustomMonitor; // default for OCPP1.6
                this->publish_event_data(event_data);
            });
    }
}
types::ocpp::ChangeAvailabilityResponse
ocppImpl::handle_change_availability(types::ocpp::ChangeAvailabilityRequest& request) {

    if (this->mod->charge_point == nullptr) {
        EVLOG_warning << "ChargePoint not initialized, cannot handle change availability command";
        return types::ocpp::ChangeAvailabilityResponse{types::ocpp::ChangeAvailabilityStatusEnumType::Rejected};
    }

    ocpp::v16::ChangeAvailabilityRequest ocpp_request{};
    ocpp_request.type = to_ocpp(request.operational_status);
    if (request.evse.has_value()) {
        const auto& evse = request.evse.value();
        if (!evse.connector_id.has_value()) {
            return types::ocpp::ChangeAvailabilityResponse{
                types::ocpp::ChangeAvailabilityStatusEnumType::Rejected,
                types::ocpp::StatusInfoType{"InvalidInput",
                                            "No connector id specified; if the whole charging station is supposed to "
                                            "be addressed, parameter evse "
                                            "must have no value."}};
        }
        try {
            ocpp_request.connectorId = this->mod->get_ocpp_connector_id(evse.id, evse.connector_id.value());
        } catch (const std::out_of_range&) {
            return types::ocpp::ChangeAvailabilityResponse{
                types::ocpp::ChangeAvailabilityStatusEnumType::Rejected,
                types::ocpp::StatusInfoType{
                    "InvalidInput",
                    "Could not determine OCPP connector id from provided EVerest EVSE and Connector Ids."}};
        }
    }
    auto response = this->mod->charge_point->on_change_availability(ocpp_request);
    return to_everest(response);
}

} // namespace ocpp_generic
} // namespace module
