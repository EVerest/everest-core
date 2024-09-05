// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <optional>
#include <variant>
#include <vector>

#include "common.hpp"

namespace iso15118::message_20 {

struct PowerDeliveryRequest {
    enum class Progress {
        Start,
        Stop,
        Standby,
        ScheduleRenegotiation,
    };

    struct PowerScheduleEntry {
        uint32_t duration;
        RationalNumber power;
        std::optional<RationalNumber> power_l2;
        std::optional<RationalNumber> power_l3;
    };

    struct Dynamic_EVPPTControlMode {
        // intentionally left blank
    };

    enum class PowerToleranceAcceptance : uint8_t {
        NotConfirmed,
        Confirmed,
    };

    struct Scheduled_EVPPTControlMode {
        NumericID selected_schedule;
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

    Header header;

    Processing processing;
    Progress charge_progress;

    std::optional<PowerProfile> power_profile;
    std::optional<ChannelSelection> channel_selection;
};

struct PowerDeliveryResponse {
    Header header;
    ResponseCode response_code;

    std::optional<EvseStatus> status;
};

} // namespace iso15118::message_20
