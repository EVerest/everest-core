// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest

#include "auth_token_providerImpl.hpp"

namespace module {
namespace main {

void auth_token_providerImpl::init() {
    // initialize serial driver
    std::cerr << "port: " << config.serial_port << " baud: " << config.baud_rate;
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
    auto configure_sam = serial.configureSAM();
    if (configure_sam.get()) {
        EVLOG_debug << "Configured SAM" << std::endl;
    }

    auto firmware_version = serial.getFirmwareVersion().get();
    if (firmware_version.valid) {
        std::shared_ptr<FirmwareVersion> fv = std::dynamic_pointer_cast<FirmwareVersion>(firmware_version.message);
        EVLOG_info << "PN532 firmware version: " << std::to_string(fv->ver) << "." << std::to_string(fv->rev)
                  << std::endl;
    }

    while (true) {
        auto in_list_passive_target_response = serial.inListPassiveTarget().get();
        if (in_list_passive_target_response.valid) {
            std::shared_ptr<InListPassiveTargetResponse> in_list_passive_target_response_message =
                std::dynamic_pointer_cast<InListPassiveTargetResponse>(in_list_passive_target_response.message);
            auto target_data = in_list_passive_target_response_message->target_data;
            for (auto entry : target_data) {
                EVLOG_info << "Got raw rfid/nfc tag: " << entry.getNFCID() << std::endl;
                Object rfid_token;
                rfid_token["token"] = entry.getNFCID();
                rfid_token["type"] = "rfid";
                rfid_token["timeout"] = config.timeout;
                EVLOG_info << "Publishing new rfid/nfc token: " << rfid_token;
                this->publish_token(rfid_token);
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(config.read_timeout));
    }
}

} // namespace main
} // namespace module
