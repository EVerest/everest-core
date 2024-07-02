// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "OCPPExtensionExample.hpp"

namespace module {

void OCPPExtensionExample::init() {
    invoke_init(*p_data_transfer);
}

void OCPPExtensionExample::ready() {
    invoke_ready(*p_data_transfer);

    std::istringstream ss(this->config.keys_to_monitor);
    std::vector<types::ocpp::ComponentVariable> component_variables;

    std::string key;
    while (std::getline(ss, key, ',')) {
        // Push each token into the vector
        component_variables.push_back({{""}, {key}}); // For OCPP1.6 we only need to specify the variable.name
    }

    // We register monitors for custom configuration keys here
    this->r_ocpp->call_monitor_variables(component_variables);

    // anytime this configuration key is changed by the CSMS and we have
    // registered a monitor, this callback is executed
    this->r_ocpp->subscribe_event_data([](types::ocpp::EventData event_data) {
        // Add your custom handler here
        EVLOG_info << "Configuration key: " << event_data.component_variable.variable.name
                   << " has been changed by CSMS to: " << event_data.actual_value;
    });

    std::vector<types::ocpp::SetVariableRequest> set_variable_requests;
    set_variable_requests.push_back({{{""}, {"ExampleConfigurationKey"}}, "ExampleValue"});

    EVLOG_info << "Setting custom configuration key...";
    const auto set_variable_results = this->r_ocpp->call_set_variables(set_variable_requests);

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
    component_variables.push_back({{""}, {"KeyThatIsNotConfigured"}});

    std::vector<types::ocpp::GetVariableRequest> get_variables_requests;
    for (const auto& component_variable : component_variables) {
        get_variables_requests.push_back({component_variable});
    }

    EVLOG_info << "Requesting configuration keys from OCPP...";
    const auto get_variables_results = this->r_ocpp->call_get_variables(get_variables_requests);

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
    auto data_transfer_response = this->r_data_transfer->call_data_transfer(data_transfer_request);
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

} // namespace module
