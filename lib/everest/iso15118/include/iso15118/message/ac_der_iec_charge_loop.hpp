// SPDX-License-Identifier: Apache-2.0
// Copyright 2026 Pionix GmbH and Contributors to EVerest
#pragma once

#include "ac_charge_loop.hpp"
#include "ac_der_types.hpp"

namespace iso15118::message_20 {

namespace datatypes {

struct DsoQSetpoint {
    RationalNumber dso_q_setpoint_value;
    std::optional<RationalNumber> dso_q_setpoint_value_L2;
    std::optional<RationalNumber> dso_q_setpoint_value_L3;
    bool pt1_response_reactive_power;
    RationalNumber step_response_time_constant_reactive_power;
};

struct DsoCosPhiSetpoint {
    RationalNumber dso_cos_phi_setpoint_value;
    std::optional<RationalNumber> dso_cos_phi_setpoint_value_L2;
    std::optional<RationalNumber> dso_cos_phi_setpoint_value_L3;
    PowerFactorExcitation excitation;
    bool pt1_response_reactive_power;
    RationalNumber step_response_time_constant_reactive_power;
};

struct DER_Scheduled_AC_CLReqControlMode : Scheduled_AC_CLReqControlMode {
    RationalNumber max_discharge_power;
    std::optional<RationalNumber> max_discharge_power_L2;
    std::optional<RationalNumber> max_discharge_power_L3;
    RationalNumber min_discharge_power;
    std::optional<RationalNumber> min_discharge_power_L2;
    std::optional<RationalNumber> min_discharge_power_L3;
    std::optional<RationalNumber> max_charge_reactive_power;
    std::optional<RationalNumber> max_charge_reactive_power_L2;
    std::optional<RationalNumber> max_charge_reactive_power_L3;
    std::optional<RationalNumber> max_discharge_reactive_power;
    std::optional<RationalNumber> max_discharge_reactive_power_L2;
    std::optional<RationalNumber> max_discharge_reactive_power_L3;
    uint8_t grid_event_condition;
};

struct DER_Dynamic_AC_CLReqControlMode : Dynamic_AC_CLReqControlMode {
    RationalNumber max_discharge_power;
    std::optional<RationalNumber> max_discharge_power_L2;
    std::optional<RationalNumber> max_discharge_power_L3;
    RationalNumber min_discharge_power;
    std::optional<RationalNumber> min_discharge_power_L2;
    std::optional<RationalNumber> min_discharge_power_L3;
    std::optional<RationalNumber> max_charge_reactive_power;
    std::optional<RationalNumber> max_charge_reactive_power_L2;
    std::optional<RationalNumber> max_charge_reactive_power_L3;
    std::optional<RationalNumber> max_discharge_reactive_power;
    std::optional<RationalNumber> max_discharge_reactive_power_L2;
    std::optional<RationalNumber> max_discharge_reactive_power_L3;
    uint8_t grid_event_condition;
    std::optional<RationalNumber> max_v2x_energy_request;
    std::optional<RationalNumber> min_v2x_energy_request;
    std::optional<RationalNumber> session_total_discharge_energy_available;
};

struct DER_Scheduled_AC_CLResControlMode : Scheduled_AC_CLResControlMode {
    RationalNumber max_charge_power;
    std::optional<RationalNumber> max_charge_power_L2;
    std::optional<RationalNumber> max_charge_power_L3;
    RationalNumber max_discharge_power;
    std::optional<RationalNumber> max_discharge_power_L2;
    std::optional<RationalNumber> max_discharge_power_L3;
    std::optional<RationalNumber> dso_discharge_power;
    std::optional<RationalNumber> dso_discharge_power_L2;
    std::optional<RationalNumber> dso_discharge_power_L3;
    std::optional<DsoQSetpoint> dso_q_setpoint;
    std::optional<DsoCosPhiSetpoint> dso_cos_phi_setpoint;
};
struct DER_Dynamic_AC_CLResControlMode : Dynamic_AC_CLResControlMode {
    RationalNumber max_charge_power;
    std::optional<RationalNumber> max_charge_power_L2;
    std::optional<RationalNumber> max_charge_power_L3;
    RationalNumber max_discharge_power;
    std::optional<RationalNumber> max_discharge_power_L2;
    std::optional<RationalNumber> max_discharge_power_L3;
    std::optional<RationalNumber> dso_discharge_power;
    std::optional<RationalNumber> dso_discharge_power_L2;
    std::optional<RationalNumber> dso_discharge_power_L3;
    std::optional<DsoQSetpoint> dso_q_setpoint;
    std::optional<DsoCosPhiSetpoint> dso_cos_phi_setpoint;
};

} // namespace datatypes

struct DER_AC_ChargeLoopRequest {

    Header header;

    // the following 2 are inherited from ChargeLoopReq
    std::optional<datatypes::DisplayParameters> display_parameters;
    bool meter_info_requested;

    std::variant<datatypes::Scheduled_AC_CLReqControlMode, datatypes::BPT_Scheduled_AC_CLReqControlMode,
                 datatypes::Dynamic_AC_CLReqControlMode, datatypes::BPT_Dynamic_AC_CLReqControlMode,
                 datatypes::DER_Scheduled_AC_CLReqControlMode, datatypes::DER_Dynamic_AC_CLReqControlMode>
        control_mode;
};

struct DER_AC_ChargeLoopResponse {

    Header header;
    datatypes::ResponseCode response_code;

    // the following 3 are inherited from ChargeLoopRes
    std::optional<datatypes::EvseStatus> status;
    std::optional<datatypes::MeterInfo> meter_info;
    std::optional<datatypes::Receipt> receipt;

    std::optional<datatypes::RationalNumber> target_frequency;

    std::variant<datatypes::Scheduled_AC_CLResControlMode, datatypes::BPT_Scheduled_AC_CLResControlMode,
                 datatypes::Dynamic_AC_CLResControlMode, datatypes::BPT_Dynamic_AC_CLResControlMode,
                 datatypes::DER_Scheduled_AC_CLResControlMode, datatypes::DER_Dynamic_AC_CLResControlMode>
        control_mode = datatypes::Scheduled_AC_CLResControlMode();
};

} // namespace iso15118::message_20
