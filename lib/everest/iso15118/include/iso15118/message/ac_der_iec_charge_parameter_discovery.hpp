// SPDX-License-Identifier: Apache-2.0
// Copyright 2026 Pionix GmbH and Contributors to EVerest
#pragma once

#include "ac_charge_parameter_discovery.hpp"
#include "ac_der_types.hpp"

namespace iso15118::message_20 {

namespace datatypes {

enum class OperatingMode : std::uint8_t {
    GridFollowing,
    GridForming,
};

enum class GridConnectionMode : std::uint8_t {
    GridConnected,
    GridIslanded
};

enum class CurveDataPointsUnit : std::uint8_t {
    V,
    Hz,
    W,
    s,
    var,
};

enum class LockValueUnit : std::uint8_t {
    V,
    Hz,
    W,
    s,
    var,
};

enum class PowerReference : std::uint8_t {
    MaximumDischargePower,
    MomentaryPower
};

struct ReactivePowerLimits {
    RationalNumber max_charge_reactive_power;
    std::optional<RationalNumber> max_charge_reactive_power_L2;
    std::optional<RationalNumber> max_charge_reactive_power_L3;
    RationalNumber min_charge_reactive_power;
    std::optional<RationalNumber> min_charge_reactive_power_L2;
    std::optional<RationalNumber> min_charge_reactive_power_L3;
    RationalNumber max_discharge_reactive_power;
    std::optional<RationalNumber> max_discharge_reactive_power_L2;
    std::optional<RationalNumber> max_discharge_reactive_power_L3;
    std::optional<RationalNumber> min_discharge_reactive_power;
    std::optional<RationalNumber> min_discharge_reactive_power_L2;
    std::optional<RationalNumber> min_discharge_reactive_power_L3;
};

struct FaultRideThrough {
    RationalNumber voltage_limit_start_frt;
    std::optional<RationalNumber> voltage_limit_stop_frt;
    std::optional<RationalNumber> voltage_recovery_limit;
    std::optional<RationalNumber> voltage_ride_through_positive_curve_k_factor;
    std::optional<RationalNumber> voltage_ride_through_negative_curve_k_factor;
    bool pt1_response_active_power;
    RationalNumber step_response_time_constant_active_power;
    bool pt1_response_reactive_power;
    RationalNumber step_response_time_constant_reactive_power;
};

struct ZeroCurrent {
    std::optional<RationalNumber> over_voltage_limit;
    std::optional<RationalNumber> under_voltage_limit;
    std::optional<RationalNumber> over_voltage_recovery_limit;
    std::optional<RationalNumber> under_voltage_recovery_limit;
    bool pt1_response_active_power;
    RationalNumber step_response_time_constant_active_power;
    bool pt1_response_reactive_power;
    RationalNumber step_response_time_constant_reactive_power;
};

struct SetPointExcitation {
    RationalNumber set_point_value;
    std::optional<PowerFactorExcitation> excitation;
};

struct DataTuple {
    RationalNumber x_value;
    SetPointExcitation y_value;
};

using CurveDataPointsList = std::vector<DataTuple>;
constexpr auto CurveDataPointsMinLength = 2;
constexpr auto CurveDataPointsMaxLength = 10;

struct DerCurve {
    CurveDataPointsUnit x_unit;
    CurveDataPointsUnit y_unit;
    CurveDataPointsList curve_data_points;
    std::optional<RationalNumber> min_cos_phi;
    std::optional<LockValueUnit> lock_value_unit;
    std::optional<RationalNumber> lock_in_value;
    std::optional<RationalNumber> lock_out_value;
    bool pt1_response_reactive_power;
    RationalNumber step_response_time_constant_reactive_power;
    std::optional<RationalNumber> intentional_delay;
};

struct ReactivePowerSupport {
    DerCurve VoltVar;
    DerCurve WattVar;
    DerCurve WattCosPhi;
};

struct FrequencyWatt {
    RationalNumber f_start;
    RationalNumber f_stop;
    std::optional<uint16_t> intentional_delay_f_stop;
    RationalNumber slope;
    std::optional<uint16_t> deactivation_time;
    std::optional<uint16_t> intentional_delay_power_control;
    PowerReference power_reference;
    bool hysteresis_control;
    std::optional<uint16_t> power_up_ramp;
    bool pt1_response_active_power;
    RationalNumber step_response_time_constant_active_power;
};

struct VoltWatt {
    PowerReference power_reference;
    RationalNumber u_start;
    RationalNumber u_stop;
    bool pt1_response_active_power;
    RationalNumber step_response_time_constant_active_power;
    std::optional<uint32_t> intentional_delay_power_control;
};

struct ActivePowerSupport {
    std::optional<FrequencyWatt> under_frequency_watt;
    std::optional<FrequencyWatt> over_frequency_watt;
    std::optional<VoltWatt> volt_watt;
};

struct DerControl {
    std::optional<FaultRideThrough> over_voltage_fault_ride_through;
    std::optional<FaultRideThrough> under_voltage_fault_ride_through;
    std::optional<ZeroCurrent> zero_current;
    std::optional<ReactivePowerSupport> reactive_power_support;
    std::optional<ActivePowerSupport> active_power_support;
    std::optional<RationalNumber> max_level_dc_injection;
};

struct DER_AC_CPDReqEnergyTransferMode {
    Processing processing;
    RationalNumber max_discharge_power;
    std::optional<RationalNumber> max_discharge_power_L2;
    std::optional<RationalNumber> max_discharge_power_L3;
    RationalNumber min_discharge_power;
    std::optional<RationalNumber> min_discharge_power_L2;
    std::optional<RationalNumber> min_discharge_power_L3;
    std::optional<RationalNumber> session_total_discharge_energy_available;
    std::optional<ReactivePowerLimits> reactive_power_limits;
};

struct DER_AC_CPDResEnergyTransferMode {
    RationalNumber nominal_charge_power;
    std::optional<RationalNumber> nominal_charge_power_L2;
    std::optional<RationalNumber> nominal_charge_power_L3;
    RationalNumber nominal_discharge_power;
    std::optional<RationalNumber> nominal_discharge_power_L2;
    std::optional<RationalNumber> nominal_discharge_power_L3;
    RationalNumber max_discharge_power;
    std::optional<RationalNumber> max_discharge_power_L2;
    std::optional<RationalNumber> max_discharge_power_L3;
    OperatingMode operating_mode;
    GridConnectionMode grid_connection_mode;
    DerControl der_control;
};

} // namespace datatypes

struct DER_AC_ChargeParameterDiscoveryRequest {
    Header header;
    std::variant<datatypes::AC_CPDReqEnergyTransferMode, datatypes::BPT_AC_CPDReqEnergyTransferMode,
                 datatypes::DER_AC_CPDReqEnergyTransferMode>
        transfer_mode;
};

struct DER_AC_ChargeParameterDiscoveryResponse {
    Header header;
    datatypes::ResponseCode response_code;
    std::variant<datatypes::AC_CPDResEnergyTransferMode, datatypes::BPT_AC_CPDResEnergyTransferMode,
                 datatypes::DER_AC_CPDResEnergyTransferMode>
        transfer_mode = datatypes::AC_CPDResEnergyTransferMode();
};

} // namespace iso15118::message_20
