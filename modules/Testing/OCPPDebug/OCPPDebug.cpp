// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "OCPPDebug.hpp"
#include <thread>

namespace module {

void OCPPDebug::init() {
    invoke_init(*p_ocpp_debug);

    if (!r_ocpp_debug.empty()) {
        r_ocpp_debug.at(0)->subscribe_ocpp_message([](types::ocpp::Message ocpp_message) {
            switch (ocpp_message.direction) {
            case types::ocpp::MessageDirection::CSMSToChargingStation:
                EVLOG_info << "CSMS->ChargingStation";
                break;
            case types::ocpp::MessageDirection::ChargingStationToCSMS:
                EVLOG_info << "ChargingStation->CSMS: " << ocpp_message.message;
                break;
            }
        });
    }
}

void OCPPDebug::ready() {
    invoke_ready(*p_ocpp_debug);

    std::this_thread::sleep_for(std::chrono::seconds(5));
    types::ocpp::Message msg;
    msg.direction = types::ocpp::MessageDirection::ChargingStationToCSMS;
    msg.message = "[2,\"6bfd3daf-f1db-4f93-af3d-7b8372125a53\",\"StatusNotification\",{\"connectorId\":0,\"errorCode\":"
                  "\"NoError\",\"status\":\"Available\",\"timestamp\":\"2026-02-23T17:33:52.821Z\"}]";
    p_ocpp_debug->publish_ocpp_message(msg);
}

} // namespace module
