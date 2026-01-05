// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <goose/frame.hpp>

#include "driver_utils.hpp"

namespace fusion_charger {
namespace goose {

// note: more or less a factory
struct StopChargeRequest {
    enum class Reason {
        // The charging is stopped normally.
        NORMAL = 0x1000,
        // The charging connector is disconnected. (During charging, the voltage at
        // detection point 1 is not 4 V.)
        CONNECTOR_DISCONNECTED = 0x1001,
        // The charging connector is not properly inserted.
        CONNECTOR_NOT_PROPERLY_INSERTED = 0x1002,
        // An insulation fault occurs.
        INSULATION_FAULT = 0x1003,
        EPO_FAULT = 0x1004,
        VEHICLE_CHARGER_NOT_MATCHING = 0x1005,
        OTHER_FAULT_ON_CHARGER = 0x1006,
        OTHER_FAULT_ON_VEHICLE = 0x1007,
        VEHICLE_BMS_NOT_CONNECTED = 0x1008,
        POWER_UNIT_CANNOT_BE_CHARGED = 0x1009,
    };

    std::uint16_t charging_connector_no;
    std::uint16_t charging_sn = 0xffff;
    Reason reason;

    ::goose::frame::GoosePDU to_pdu(::goose::frame::GooseTimestamp time = ::goose::frame::GooseTimestamp::now()) const {
        ::goose::frame::GoosePDU pdu;
        strcpy(pdu.go_cb_ref, "CC/0$GO$ShutdownRequest");
        pdu.time_allowed_to_live = 10000;
        strcpy(pdu.dat_set, "CC/0$GO$ShutdownRequest");
        strcpy(pdu.go_id, "CC/0$GO$ShutdownRequest");
        pdu.timestamp = time;
        pdu.conf_rev = 1;
        pdu.simulation = false;
        pdu.ndsCom = false;
        pdu.apdu_entries.resize(5);
        pdu.apdu_entries[0] = utils::make_u16(charging_connector_no);
        pdu.apdu_entries[1] = utils::make_u16(charging_sn);
        pdu.apdu_entries[2] = utils::make_u16(static_cast<std::uint16_t>(reason));
        pdu.apdu_entries[3] = utils::make_u16(0xffff);
        pdu.apdu_entries[4] = utils::make_u16(0xffff);
        return pdu;
    }

    ::goose::frame::GooseTimestamp from_pdu(const ::goose::frame::GoosePDU& input) {
        if (input.apdu_entries.size() < 5) {
            throw std::runtime_error("StopChargeRequest: input has too few entries");
        }

        if (strcmp(input.go_cb_ref, "CC/0$GO$ShutdownRequest") != 0) {
            throw std::runtime_error("StopChargeRequest: expected go_cb_ref "
                                     "CC/0$GO$ShutdownRequest, got " +
                                     std::string(input.go_cb_ref));
        }

        charging_connector_no = utils::expect_u16(input.apdu_entries[0]);
        charging_sn = utils::expect_u16(input.apdu_entries[1]);
        reason = static_cast<Reason>(utils::expect_u16(input.apdu_entries[2]));

        return input.timestamp;
    }
};

}; // namespace goose
}; // namespace fusion_charger
