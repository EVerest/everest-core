// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <goose/frame.hpp>

#include "driver_utils.hpp"

namespace fusion_charger {
namespace goose {

enum class RequirementType : std::uint16_t {
    ModulePlaceholderRequest = 0x01,
    InsulationDetectionVoltageOutput = 0x02,
    InsulationDetectionVoltageOutputStoppage = 0x03,
    PrechargeVoltageOutput = 0x04,
    Charging = 0x05,
};

enum class Mode : std::uint16_t {
    None = 0x00,
    ConstantVoltage = 0x01,
    ConstantCurrent = 0x02,
};

// todo: tests
// note: more or less a factory
struct PowerRequirementRequest {
    std::uint16_t charging_connector_no;
    std::uint16_t charging_sn = 0xffff;
    RequirementType requirement_type;
    Mode mode;
    float voltage;
    float current;

    // todo: better timestamp stuff
    ::goose::frame::GoosePDU
    to_pdu(::goose::frame::GooseTimestamp timestamp = ::goose::frame::GooseTimestamp::now()) const {
        ::goose::frame::GoosePDU pdu;
        strcpy(pdu.go_cb_ref, "CC/0$GO$PowerRequest");
        pdu.time_allowed_to_live = 10000;
        strcpy(pdu.dat_set, "CC/0$GO$PowerRequest");
        strcpy(pdu.go_id, "CC/0$GO$PowerRequest");
        pdu.simulation = false;
        pdu.conf_rev = 1;
        pdu.ndsCom = false;
        pdu.timestamp = timestamp;
        pdu.apdu_entries.resize(8);
        pdu.apdu_entries[0] = utils::make_u16(charging_connector_no);
        pdu.apdu_entries[1] = utils::make_u16(charging_sn);
        pdu.apdu_entries[2] = utils::make_u16(static_cast<std::uint16_t>(requirement_type));
        pdu.apdu_entries[3] = utils::make_u16(static_cast<std::uint16_t>(mode));
        pdu.apdu_entries[4] = utils::make_f32(voltage);
        pdu.apdu_entries[5] = utils::make_f32(current);
        pdu.apdu_entries[6] = utils::make_u16(0xffff);
        pdu.apdu_entries[7] = utils::make_u16(0xffff);
        return pdu;
    }

    // todo: test
    ::goose::frame::GooseTimestamp from_pdu(const ::goose::frame::GoosePDU& input) {
        if (input.apdu_entries.size() != 8) {
            throw std::runtime_error("Expected 8 APDU entries, got " + std::to_string(input.apdu_entries.size()));
        }

        if (strcmp(input.go_cb_ref, "CC/0$GO$PowerRequest") != 0) {
            throw std::runtime_error("Expected go_cb_ref CC/0$GO$PowerRequest, got " + std::string(input.go_cb_ref));
        }

        charging_connector_no = utils::expect_u16(input.apdu_entries[0]);
        charging_sn = utils::expect_u16(input.apdu_entries[1]);
        requirement_type = static_cast<RequirementType>(utils::expect_u16(input.apdu_entries[2]));
        mode = static_cast<Mode>(utils::expect_u16(input.apdu_entries[3]));
        voltage = utils::expect_f32(input.apdu_entries[4]);
        current = utils::expect_f32(input.apdu_entries[5]);

        return input.timestamp;
    }
};

// todo: test
struct PowerRequirementResponse {
    enum class Result : std::uint16_t {
        SUCCESS = 0,
        FAILURE = 1,
    };

    std::uint16_t charging_connector_no;
    std::uint16_t charging_sn = 0xffff;
    RequirementType requirement_type;
    Mode mode;
    Result result;
    float voltage;
    float current;

    // todo: better timestamp stuff
    ::goose::frame::GoosePDU
    to_pdu(::goose::frame::GooseTimestamp timestamp = ::goose::frame::GooseTimestamp::now()) const {
        ::goose::frame::GoosePDU pdu;
        strcpy(pdu.go_cb_ref, "CC/0$GO$PowerRequestReply");
        pdu.time_allowed_to_live = 10000;
        strcpy(pdu.dat_set, "CC/0$GO$PowerRequestReply");
        strcpy(pdu.go_id, "CC/0$GO$PowerRequestReply");
        pdu.simulation = false;
        pdu.conf_rev = 1;
        pdu.ndsCom = false;
        pdu.timestamp = timestamp;
        // DataSheet (Introduction to the communication ...)
        // says size of 8 entries, but 9 are given.
        // Setting size of 9
        pdu.apdu_entries.resize(9);
        pdu.apdu_entries[0] = utils::make_u16(charging_connector_no);
        pdu.apdu_entries[1] = utils::make_u16(charging_sn);
        pdu.apdu_entries[2] = utils::make_u16(static_cast<std::uint16_t>(requirement_type));
        pdu.apdu_entries[3] = utils::make_u16(static_cast<std::uint16_t>(result));
        pdu.apdu_entries[4] = utils::make_u16(static_cast<std::uint16_t>(mode));
        pdu.apdu_entries[5] = utils::make_f32(voltage);
        pdu.apdu_entries[6] = utils::make_f32(current);
        pdu.apdu_entries[7] = utils::make_u16(0xffff);
        pdu.apdu_entries[8] = utils::make_u16(0xffff);
        return pdu;
    }

    ::goose::frame::GooseTimestamp from_pdu(const ::goose::frame::GoosePDU& input) {
        if (input.apdu_entries.size() != 9) {
            throw std::runtime_error("Expected 9 APDU entries, got " + std::to_string(input.apdu_entries.size()));
        }

        if (strcmp(input.go_cb_ref, "CC/0$GO$PowerRequestReply") != 0) {
            throw std::runtime_error("Expected go_cb_ref CC/0$GO$PowerRequestReply, got " +
                                     std::string(input.go_cb_ref));
        }

        charging_connector_no = utils::expect_u16(input.apdu_entries[0]);
        charging_sn = utils::expect_u16(input.apdu_entries[1]);
        requirement_type = static_cast<RequirementType>(utils::expect_u16(input.apdu_entries[2]));
        result = static_cast<Result>(utils::expect_u16(input.apdu_entries[3]));
        mode = static_cast<Mode>(utils::expect_u16(input.apdu_entries[4]));
        voltage = utils::expect_f32(input.apdu_entries[5]);
        current = utils::expect_f32(input.apdu_entries[6]);

        return input.timestamp;
    }
};

}; // namespace goose
}; // namespace fusion_charger
