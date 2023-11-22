// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "ocppImpl.hpp"

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

void ocppImpl::init() {
}

void ocppImpl::ready() {
}

bool ocppImpl::handle_stop() {
    std::lock_guard<std::mutex>(this->m);
    mod->charging_schedules_timer->stop();
    bool success = mod->charge_point->stop();
    if (success) {
        this->mod->ocpp_stopped = true;
    }
    return success;
}

bool ocppImpl::handle_restart() {
    std::lock_guard<std::mutex>(this->m);
    mod->charging_schedules_timer->interval(std::chrono::seconds(this->mod->config.PublishChargingScheduleIntervalS));
    bool success = mod->charge_point->restart();
    if (success) {
        this->mod->ocpp_stopped = false;
    }
    return success;
}

void ocppImpl::handle_security_event(std::string& type, std::string& info) {
    this->mod->charge_point->on_security_event(type, info);
}

std::vector<types::ocpp::GetVariableResult>
ocppImpl::handle_get_variables(std::vector<types::ocpp::GetVariableRequest>& requests) {
    std::vector<types::ocpp::GetVariableResult> results;

    // prepare ocpp_request to request configuration keys from ocpp::v16::ChargePoint
    ocpp::v16::GetConfigurationRequest ocpp_request;
    std::vector<ocpp::CiString<50>> configuration_keys;
    for (const auto& request : requests) {
        // only variable.name is relevant for OCPP1.6
        configuration_keys.push_back(request.component_variable.variable.name);
    }
    ocpp_request.key = configuration_keys;

    // request configuration keys from ocpp::v16::ChargePoint
    const auto ocpp_response = this->mod->charge_point->get_configuration_key(ocpp_request);

    if (ocpp_response.configurationKey.has_value()) {
        for (const auto& key_value : ocpp_response.configurationKey.value()) {
            // add result for each present configurationKey in the response
            types::ocpp::GetVariableResult result;
            result.component_variable = {
                {""},
                {key_value.key.get()}}; // we dont care about the component, only about the variable.name in OCPP1.6
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
                {""}, {key.get()}}; // we dont care about the component, only about the variable.name in OCPP1.6
            result.status = types::ocpp::GetVariableStatusEnumType::UnknownVariable;
            results.push_back(result);
        }
    }
    return results;
}

std::vector<types::ocpp::SetVariableResult>
ocppImpl::handle_set_variables(std::vector<types::ocpp::SetVariableRequest>& requests) {

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
            // NotSupported in OCPP1.6 means that the configuration key is not known / not supported, so its best to go
            // with UnknownVariable
            result.status = types::ocpp::SetVariableStatusEnumType::UnknownVariable;
            break;
        }
        results.push_back(result);
    }

    return results;
}

void ocppImpl::handle_monitor_variables(std::vector<types::ocpp::ComponentVariable>& component_variables) {
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

} // namespace main
} // namespace module
