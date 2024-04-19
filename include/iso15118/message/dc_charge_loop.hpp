// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <variant>
#include <vector>

#include "common.hpp"

namespace iso15118::message_20 {

struct DC_ChargeLoopRequest {
    struct Scheduled_DC_CLReqControlMode : Scheduled_CLReqControlMode {
        RationalNumber target_current;
        RationalNumber target_voltage;
        std::optional<RationalNumber> max_charge_power;
        std::optional<RationalNumber> min_charge_power;
        std::optional<RationalNumber> max_charge_current;
        std::optional<RationalNumber> max_voltage;
        std::optional<RationalNumber> min_voltage;
    };

    struct BPT_Scheduled_DC_CLReqControlMode : Scheduled_DC_CLReqControlMode {
        std::optional<RationalNumber> max_discharge_power;
        std::optional<RationalNumber> min_discharge_power;
        std::optional<RationalNumber> max_discharge_current;
    };

    struct Dynamic_DC_CLReqControlMode : Dynamic_CLReqControlMode {
        RationalNumber max_charge_power;
        RationalNumber min_charge_power;
        RationalNumber max_charge_current;
        RationalNumber max_voltage;
        RationalNumber min_voltage;
    };

    struct BPT_Dynamic_DC_CLReqControlMode : Dynamic_DC_CLReqControlMode {
        RationalNumber max_discharge_power;
        RationalNumber min_discharge_power;
        RationalNumber max_discharge_current;
        std::optional<RationalNumber> max_v2x_energy_request;
        std::optional<RationalNumber> min_v2x_energy_request;
    };

    Header header;

    // the following 2 are inherited from ChargeLoopReq
    std::optional<DisplayParameters> display_parameters;
    bool meter_info_requested;

    RationalNumber present_voltage;
    std::variant<Scheduled_DC_CLReqControlMode, BPT_Scheduled_DC_CLReqControlMode, Dynamic_DC_CLReqControlMode,
                 BPT_Dynamic_DC_CLReqControlMode>
        control_mode;
};

struct DC_ChargeLoopResponse {

    struct Scheduled_DC_CLResControlMode : Scheduled_CLResControlMode {
        std::optional<RationalNumber> max_charge_power;
        std::optional<RationalNumber> min_charge_power;
        std::optional<RationalNumber> max_charge_current;
        std::optional<RationalNumber> max_voltage;
    };

    struct BPT_Scheduled_DC_CLResControlMode : Scheduled_DC_CLResControlMode {
        std::optional<RationalNumber> max_discharge_power;
        std::optional<RationalNumber> min_discharge_power;
        std::optional<RationalNumber> max_discharge_current;
        std::optional<RationalNumber> min_voltage;
    };

    struct Dynamic_DC_CLResControlMode : Dynamic_CLResControlMode {
        RationalNumber max_charge_power;
        RationalNumber min_charge_power;
        RationalNumber max_charge_current;
        RationalNumber max_voltage;
    };

    struct BPT_Dynamic_DC_CLResControlMode : Dynamic_DC_CLResControlMode {
        RationalNumber max_discharge_power;
        RationalNumber min_discharge_power;
        RationalNumber max_discharge_current;
        RationalNumber min_voltage;
    };

    Header header;
    ResponseCode response_code;

    // the following 3 are inherited from ChargeLoopRes
    std::optional<EvseStatus> status;
    std::optional<MeterInfo> meter_info;
    std::optional<Receipt> receipt;

    RationalNumber present_current;
    RationalNumber present_voltage;
    bool power_limit_achieved;
    bool current_limit_achieved;
    bool voltage_limit_achieved;

    std::variant<Scheduled_DC_CLResControlMode, BPT_Scheduled_DC_CLResControlMode, Dynamic_DC_CLResControlMode,
                 BPT_Dynamic_DC_CLResControlMode>
        control_mode = Scheduled_DC_CLResControlMode();
};

} // namespace iso15118::message_20
