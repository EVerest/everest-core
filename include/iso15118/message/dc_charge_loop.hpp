// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <variant>
#include <vector>

#include "common_types.hpp"

namespace iso15118::message_20 {

namespace datatypes {

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

} // namespace datatypes

struct DC_ChargeLoopRequest {

    Header header;

    // the following 2 are inherited from ChargeLoopReq
    std::optional<datatypes::DisplayParameters> display_parameters;
    bool meter_info_requested;

    datatypes::RationalNumber present_voltage;
    std::variant<datatypes::Scheduled_DC_CLReqControlMode, datatypes::BPT_Scheduled_DC_CLReqControlMode,
                 datatypes::Dynamic_DC_CLReqControlMode, datatypes::BPT_Dynamic_DC_CLReqControlMode>
        control_mode;
};

struct DC_ChargeLoopResponse {
    Header header;
    datatypes::ResponseCode response_code;

    // the following 3 are inherited from ChargeLoopRes
    std::optional<datatypes::EvseStatus> status;
    std::optional<datatypes::MeterInfo> meter_info;
    std::optional<datatypes::Receipt> receipt;

    datatypes::RationalNumber present_current{0, 0};
    datatypes::RationalNumber present_voltage{0, 0};
    bool power_limit_achieved{false};
    bool current_limit_achieved{false};
    bool voltage_limit_achieved{false};

    std::variant<datatypes::Scheduled_DC_CLResControlMode, datatypes::BPT_Scheduled_DC_CLResControlMode,
                 datatypes::Dynamic_DC_CLResControlMode, datatypes::BPT_Dynamic_DC_CLResControlMode>
        control_mode;

    DC_ChargeLoopResponse() : control_mode(std::in_place_type<datatypes::Scheduled_DC_CLResControlMode>){};
};

} // namespace iso15118::message_20
