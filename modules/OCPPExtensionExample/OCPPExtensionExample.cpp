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
    Array keys;

    std::string key;
    while (std::getline(ss, key, ',')) {
        // Push each token into the vector
        keys.push_back(key);
    }

    // We register monitors for custom configuration keys here
    this->r_ocpp->call_monitor_configuration_keys(keys);

    // anytime this configuration key is changed by the CSMS and we have
    // registered a monitor, this callback is executed
    this->r_ocpp->subscribe_configuration_key([](types::ocpp::KeyValue key_value) {
        // Add your custom handler here
        EVLOG_info << "Configuration key: " << key_value.key
                   << " has been changed by CSMS to: " << key_value.value.value();
    });

    EVLOG_info << "Setting custom configuration key...";
    const auto status = this->r_ocpp->call_set_custom_configuration_key("ExampleConfigurationKey", "ExampleValue");

    if (status == types::ocpp::ConfigurationStatus::Accepted) {
        EVLOG_info << "Successfully set ExampleConfigurationKey";
    } else {
        EVLOG_info << "Could not set ExampleConfigurationKey: " << types::ocpp::configuration_status_to_string(status);
    }

    // adding a configuration key that does not exist to show that this will be
    // part of the unknown keys of the result
    keys.push_back("KeyThatIsNotConfigured");
    EVLOG_info << "Requesting configuration keys from OCPP...";
    const auto result = this->r_ocpp->call_get_configuration_key(keys);

    for (const auto& key_value : result.configuration_keys) {
        if (key_value.value.has_value()) {
            EVLOG_info << "Key: " << key_value.key << ": " << key_value.value.value();
        }
    }

    for (const auto& unknown_key : result.unknown_keys) {
        EVLOG_info << "Unknown: " << unknown_key;
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
