// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest
#pragma once

#include <variant>
#include <vector>

#include "common.hpp"

namespace iso15118::message_20 {

struct AC_ChargeLoopRequest {

    struct Scheduled_AC_CLReqControlMode : Scheduled_CLReqControlMode {
        std::optional<RationalNumber> max_charge_power;
        std::optional<RationalNumber> max_charge_power_L2;
        std::optional<RationalNumber> max_charge_power_L3;
        std::optional<RationalNumber> min_charge_power;
        std::optional<RationalNumber> min_charge_power_L2;
        std::optional<RationalNumber> min_charge_power_L3;
        RationalNumber present_active_power;
        std::optional<RationalNumber> present_active_power_L2;
        std::optional<RationalNumber> present_active_power_L3;
        std::optional<RationalNumber> present_reactive_power;
        std::optional<RationalNumber> present_reactive_power_L2;
        std::optional<RationalNumber> present_reactive_power_L3;
    };

    struct BPT_Scheduled_AC_CLReqControlMode : Scheduled_AC_CLReqControlMode {
        std::optional<RationalNumber> max_discharge_power;
        std::optional<RationalNumber> max_discharge_power_L2;
        std::optional<RationalNumber> max_discharge_power_L3;
        std::optional<RationalNumber> min_discharge_power;
        std::optional<RationalNumber> min_discharge_power_L2;
        std::optional<RationalNumber> min_discharge_power_L3;
    };

    struct Dynamic_AC_CLReqControlMode : Dynamic_CLReqControlMode {
        RationalNumber max_charge_power;
        std::optional<RationalNumber> max_charge_power_L2;
        std::optional<RationalNumber> max_charge_power_L3;
        RationalNumber min_charge_power;
        std::optional<RationalNumber> min_charge_power_L2;
        std::optional<RationalNumber> min_charge_power_L3;
        RationalNumber present_active_power;
        std::optional<RationalNumber> present_active_power_L2;
        std::optional<RationalNumber> present_active_power_L3;
        RationalNumber present_reactive_power;
        std::optional<RationalNumber> present_reactive_power_L2;
        std::optional<RationalNumber> present_reactive_power_L3;
    };

    struct BPT_Dynamic_AC_CLReqControlMode : Dynamic_AC_CLReqControlMode {
        RationalNumber max_discharge_power;
        std::optional<RationalNumber> max_discharge_power_L2;
        std::optional<RationalNumber> max_discharge_power_L3;
        RationalNumber min_discharge_power;
        std::optional<RationalNumber> min_discharge_power_L2;
        std::optional<RationalNumber> min_discharge_power_L3;
        std::optional<RationalNumber> max_v2x_energy_request;
        std::optional<RationalNumber> min_v2x_energy_request;
    };

    Header header;

    // the following 2 are inherited from ChargeLoopReq
    std::optional<DisplayParameters> display_parameters;
    bool meter_info_requested;

    std::variant<Scheduled_AC_CLReqControlMode, BPT_Scheduled_AC_CLReqControlMode, Dynamic_AC_CLReqControlMode,
                 BPT_Dynamic_AC_CLReqControlMode>
        control_mode;
};

struct AC_ChargeLoopResponse {

    struct Scheduled_AC_CLResControlMode : Scheduled_CLResControlMode {
        std::optional<RationalNumber> target_active_power;
        std::optional<RationalNumber> target_active_power_L2;
        std::optional<RationalNumber> target_active_power_L3;
        std::optional<RationalNumber> target_reactive_power;
        std::optional<RationalNumber> target_reactive_power_L2;
        std::optional<RationalNumber> target_reactive_power_L3;
        std::optional<RationalNumber> present_active_power;
        std::optional<RationalNumber> present_active_power_L2;
        std::optional<RationalNumber> present_active_power_L3;
    };

    struct BPT_Scheduled_AC_CLResControlMode : Scheduled_AC_CLResControlMode {};

    struct Dynamic_AC_CLResControlMode : Dynamic_CLResControlMode {
        RationalNumber target_active_power;
        std::optional<RationalNumber> target_active_power_L2;
        std::optional<RationalNumber> target_active_power_L3;
        std::optional<RationalNumber> target_reactive_power;
        std::optional<RationalNumber> target_reactive_power_L2;
        std::optional<RationalNumber> target_reactive_power_L3;
        std::optional<RationalNumber> present_active_power;
        std::optional<RationalNumber> present_active_power_L2;
        std::optional<RationalNumber> present_active_power_L3;
    };

    struct BPT_Dynamic_AC_CLResControlMode : Dynamic_AC_CLResControlMode {};

    Header header;
    ResponseCode response_code;

    // the following 3 are inherited from ChargeLoopRes
    std::optional<EvseStatus> status;
    std::optional<MeterInfo> meter_info;
    std::optional<Receipt> receipt;

    std::optional<RationalNumber> target_frequency;

    std::variant<Scheduled_AC_CLResControlMode, BPT_Scheduled_AC_CLResControlMode, Dynamic_AC_CLResControlMode,
                 BPT_Dynamic_AC_CLResControlMode>
        control_mode = Scheduled_AC_CLResControlMode();
};

} // namespace iso15118::message_20
