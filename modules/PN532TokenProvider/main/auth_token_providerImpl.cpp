// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest

#include "auth_token_providerImpl.hpp"

namespace module {
namespace main {

void auth_token_providerImpl::init() {
    // initialize serial driver
    if (config.debug) {
        serial.enableDebug();
        EVLOG_info << "Serial port: " << config.serial_port << " baud rate: " << config.baud_rate;
    }
    if (!serial.openDevice(config.serial_port.c_str(), config.baud_rate)) {
        EVLOG_AND_THROW(EVEXCEPTION(Everest::EverestConfigError, "Could not open serial port ", config.serial_port,
                                    " with baud rate ", config.baud_rate));
        return;
    }
}

void auth_token_providerImpl::ready() {
    serial.run();

    serial.reset();
    // configure Secure Access Module (SAM)
    auto configure_sam_future = serial.configureSAM();
    while (configure_sam_future.wait_for(std::chrono::milliseconds(100)) != std::future_status::ready) {
        EVLOG_verbose << "Retrying to configure SAM";
        configure_sam_future = serial.configureSAM();
    }
    auto configure_sam = configure_sam_future.get();
    if (configure_sam) {
        EVLOG_debug << "Configured SAM" << std::endl;
    }

    auto firmware_version_future = serial.getFirmwareVersion();
    while (firmware_version_future.wait_for(std::chrono::milliseconds(100)) != std::future_status::ready) {
        EVLOG_verbose << "Retrying to get firmware version";
        firmware_version_future = serial.getFirmwareVersion();
    }
    auto firmware_version = firmware_version_future.get();
    if (firmware_version.valid) {
        std::shared_ptr<FirmwareVersion> fv = std::dynamic_pointer_cast<FirmwareVersion>(firmware_version.message);
        if (config.debug) {
            EVLOG_info << "PN532 firmware version: " << std::to_string(fv->ver) << "." << std::to_string(fv->rev);
        }
    }

    while (true) {
        auto in_list_passive_target_response_future = serial.inListPassiveTarget();
        while (in_list_passive_target_response_future.wait_for(std::chrono::seconds(5)) != std::future_status::ready) {
            EVLOG_verbose << "Retrying to get target";
            in_list_passive_target_response_future = serial.inListPassiveTarget();
        }
        auto in_list_passive_target_response = in_list_passive_target_response_future.get();
        if (in_list_passive_target_response.valid) {
            std::shared_ptr<InListPassiveTargetResponse> in_list_passive_target_response_message =
                std::dynamic_pointer_cast<InListPassiveTargetResponse>(in_list_passive_target_response.message);
            auto target_data = in_list_passive_target_response_message->target_data;
            for (auto entry : target_data) {
                types::authorization::ProvidedIdToken provided_token;
                provided_token.id_token = {entry.getNFCID(), types::authorization::IdTokenType::ISO14443};
                provided_token.authorization_type = types::authorization::AuthorizationType::RFID;
                if (config.debug) {
                    EVLOG_info << "Publishing new rfid/nfc token: " << provided_token;
                }
                this->publish_provided_token(provided_token);
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(config.read_timeout));
    }
}

} // namespace main
} // namespace module
