// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <optional>
#include <variant>
#include <vector>

#include "common_types.hpp"

namespace iso15118::message_20 {

namespace datatypes {

enum class Progress {
    Start,
    Stop,
    Standby,
    ScheduleRenegotiation,
};

struct Dynamic_EVPPTControlMode {
    // intentionally left blank
};

enum class PowerToleranceAcceptance : uint8_t {
    NotConfirmed,
    Confirmed,
};

struct Scheduled_EVPPTControlMode {
    NumericId selected_schedule;
    std::optional<PowerToleranceAcceptance> power_tolerance_acceptance;
};

struct PowerProfile {
    uint64_t time_anchor;
    std::variant<Dynamic_EVPPTControlMode, Scheduled_EVPPTControlMode> control_mode;
    std::vector<PowerScheduleEntry> entries; // maximum 2048
};

enum class ChannelSelection : uint8_t {
    Charge,
    Discharge,
};

}; // namespace datatypes

struct PowerDeliveryRequest {
    Header header;

    datatypes::Processing processing;
    datatypes::Progress charge_progress;

    std::optional<datatypes::PowerProfile> power_profile;
    std::optional<datatypes::ChannelSelection> channel_selection;
};

struct PowerDeliveryResponse {
    Header header;
    datatypes::ResponseCode response_code;

    std::optional<datatypes::EvseStatus> status;
};

} // namespace iso15118::message_20
