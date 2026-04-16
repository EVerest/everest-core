// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "OCPPExtensionExample.hpp"
#include "everest/logging.hpp"
#include <mutex>

namespace module {
using ConfigChangeResult = Everest::config::ConfigChangeResult;

void OCPPExtensionExample::init() {
    invoke_init(*p_data_transfer);
}

void OCPPExtensionExample::ready() {
    invoke_ready(*p_data_transfer);
    event_keys_to_monitor();

    // anytime this configuration key is changed by the CSMS and we have
    // registered a monitor, this callback is executed
    r_ocpp->subscribe_event_data([this](types::ocpp::EventData event_data) { event_key_updated(event_data); });

    std::vector<types::ocpp::SetVariableRequest> set_variable_requests;
    set_variable_requests.push_back({{{""}, {"ExampleConfigurationKey"}}, "ExampleValue"});

    EVLOG_info << "Setting custom configuration key...";
    const auto set_variable_results = r_ocpp->call_set_variables(set_variable_requests, "example");

    for (const auto& set_variable_result : set_variable_results) {
        if (set_variable_result.status == types::ocpp::SetVariableStatusEnumType::Accepted) {
            EVLOG_info << "Successfully set ExampleConfigurationKey";
        } else {
            EVLOG_info << "Could not set ExampleConfigurationKey: "
                       << types::ocpp::set_variable_status_enum_type_to_string(set_variable_result.status);
        }
    }

    // adding a configuration key that does not exist to show that this will be
    // part of the unknown keys of the result
    std::vector<types::ocpp::ComponentVariable> component_variables;
    component_variables.push_back({{""}, {"KeyThatIsNotConfigured"}});

    std::vector<types::ocpp::GetVariableRequest> get_variables_requests;
    for (const auto& component_variable : component_variables) {
        get_variables_requests.push_back({component_variable});
    }

    EVLOG_info << "Requesting configuration keys from OCPP...";
    const auto get_variables_results = r_ocpp->call_get_variables(get_variables_requests);

    for (const auto& get_variables_result : get_variables_results) {
        if (get_variables_result.status == types::ocpp::GetVariableStatusEnumType::Accepted) {
            EVLOG_info << "Key: " << get_variables_result.component_variable.variable.name << ": "
                       << get_variables_result.value.value();
        } else {
            EVLOG_info << "Unknown: " << get_variables_result.component_variable.variable.name;
        }
    }

    types::ocpp::DataTransferRequest data_transfer_request;
    data_transfer_request.vendor_id = "EVerest";
    data_transfer_request.data.emplace("hi");
    auto data_transfer_response = r_data_transfer->call_data_transfer(data_transfer_request);
    switch (data_transfer_response.status) {
    case types::ocpp::DataTransferStatus::Accepted:
        EVLOG_info << "Data transfer was accepted";
        break;
    case types::ocpp::DataTransferStatus::Rejected:
        EVLOG_info << "Data transfer was rejected";
        break;
    case types::ocpp::DataTransferStatus::UnknownVendorId:
        EVLOG_info << "Data transfer was rejected (UnknownVendorId)";
        break;
    case types::ocpp::DataTransferStatus::UnknownMessageId:
        EVLOG_info << "Data transfer was rejected (UnknownMessageId)";
        break;

    default:
        break;
    }
}

void OCPPExtensionExample::event_keys_to_monitor() {
    std::scoped_lock lock(mutex);

    std::istringstream ss(config.keys_to_monitor);
    std::vector<types::ocpp::ComponentVariable> component_variables;

    monitored_keys.clear();
    std::string key;
    while (std::getline(ss, key, ',')) {
        // Push each token into the vector
        component_variables.push_back({"", key}); // For OCPP1.6 we only need to specify the variable.name
        monitored_keys.insert(std::move(key));
    }

    if (!component_variables.empty()) {
        // We register monitors for custom configuration keys here
        r_ocpp->call_monitor_variables(component_variables);
    }

    EVLOG_info << "monitoring keys: '" << config.keys_to_monitor << '\'';
}

void OCPPExtensionExample::event_key_updated(const types::ocpp::EventData& event_data) {
    std::scoped_lock lock(mutex);
    if (config.enable) {
        const auto& name = event_data.component_variable.variable.name;
        const auto& value = event_data.actual_value;
        if (auto it = monitored_keys.find(name); it != monitored_keys.end()) {
            EVLOG_info << "Configuration key: " << name << " has been changed by CSMS to: " << value;
            if (name == "Heartbeat") {
                // publish externally over MQTT
                mqtt.publish("heartbeat-updated", "");
            }
        }
    }
}

ConfigChangeResult OCPPExtensionExample::on_enable_changed(const bool& value) {
    std::scoped_lock lock(mutex);
    rw_config.enable = value;
    return ConfigChangeResult::Accepted();
}

ConfigChangeResult OCPPExtensionExample::on_id_changed(const int& value) {
    std::scoped_lock lock(mutex);
    rw_config.id = value;
    return ConfigChangeResult::AcceptedRebootRequired();
}

ConfigChangeResult OCPPExtensionExample::on_keys_to_monitor_changed(const std::string& value) {
    {
        std::scoped_lock lock(mutex);
        rw_config.keys_to_monitor = value;
    }
    event_keys_to_monitor();
    return ConfigChangeResult::Accepted();
}

} // namespace module
