// SPDX-License-Identifier: Apache-2.0
// Copyright 2026 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/ac_der_iec_charge_parameter_discovery.hpp>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_20/iso20_AC_DER_IEC_Decoder.h>
#include <cbv2g/iso_20/iso20_AC_DER_IEC_Encoder.h>

namespace iso15118::message_20 {

using AC_ModeReq = datatypes::AC_CPDReqEnergyTransferMode;
using BPT_AC_ModeReq = datatypes::BPT_AC_CPDReqEnergyTransferMode;
using DER_AC_ModeReq = datatypes::DER_AC_CPDReqEnergyTransferMode;

using AC_ModeRes = datatypes::AC_CPDResEnergyTransferMode;
using BPT_AC_ModeRes = datatypes::BPT_AC_CPDResEnergyTransferMode;
using DER_AC_ModeRes = datatypes::DER_AC_CPDResEnergyTransferMode;

template <>
void convert(const struct iso20_ac_der_iec_EVReactivePowerLimitsType& in, datatypes::ReactivePowerLimits& out) {
    convert(in.EVMaximumChargeReactivePower, out.max_charge_reactive_power);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumChargeReactivePower_L2, out.max_charge_reactive_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumChargeReactivePower_L3, out.max_charge_reactive_power_L3);
    convert(in.EVMinimumChargeReactivePower, out.min_charge_reactive_power);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumChargeReactivePower_L2, out.min_charge_reactive_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumChargeReactivePower_L3, out.min_charge_reactive_power_L3);
    convert(in.EVMaximumDischargeReactivePower, out.max_discharge_reactive_power);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumDischargeReactivePower_L2, out.max_discharge_reactive_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumDischargeReactivePower_L3, out.max_discharge_reactive_power_L3);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumDischargeReactivePower, out.min_discharge_reactive_power);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumDischargeReactivePower_L2, out.min_discharge_reactive_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumDischargeReactivePower_L3, out.min_discharge_reactive_power_L3);
}

template <>
void convert(const datatypes::ReactivePowerLimits& in, struct iso20_ac_der_iec_EVReactivePowerLimitsType& out) {
    convert(in.max_charge_reactive_power, out.EVMaximumChargeReactivePower);
    CPP2CB_CONVERT_IF_USED(in.max_charge_reactive_power_L2, out.EVMaximumChargeReactivePower_L2);
    CPP2CB_CONVERT_IF_USED(in.max_charge_reactive_power_L3, out.EVMaximumChargeReactivePower_L3);
    convert(in.min_charge_reactive_power, out.EVMinimumChargeReactivePower);
    CPP2CB_CONVERT_IF_USED(in.min_charge_reactive_power_L2, out.EVMinimumChargeReactivePower_L2);
    CPP2CB_CONVERT_IF_USED(in.min_charge_reactive_power_L3, out.EVMinimumChargeReactivePower_L3);
    convert(in.max_discharge_reactive_power, out.EVMaximumDischargeReactivePower);
    CPP2CB_CONVERT_IF_USED(in.max_discharge_reactive_power_L2, out.EVMaximumDischargeReactivePower_L2);
    CPP2CB_CONVERT_IF_USED(in.max_discharge_reactive_power_L3, out.EVMaximumDischargeReactivePower_L3);
    CPP2CB_CONVERT_IF_USED(in.min_discharge_reactive_power, out.EVMinimumDischargeReactivePower);
    CPP2CB_CONVERT_IF_USED(in.min_discharge_reactive_power_L2, out.EVMinimumDischargeReactivePower_L2);
    CPP2CB_CONVERT_IF_USED(in.min_discharge_reactive_power_L3, out.EVMinimumDischargeReactivePower_L3);
}

template <typename InType> void convert(const InType& in, AC_ModeReq& out) {
    convert(in.EVMaximumChargePower, out.max_charge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumChargePower_L2, out.max_charge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumChargePower_L3, out.max_charge_power_L3);
    convert(in.EVMinimumChargePower, out.min_charge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumChargePower_L2, out.min_charge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumChargePower_L3, out.min_charge_power_L3);
}

template <> void convert(const struct iso20_ac_der_iec_BPT_AC_CPDReqEnergyTransferModeType& in, BPT_AC_ModeReq& out) {
    convert(in, static_cast<AC_ModeReq&>(out));
    convert(in.EVMaximumDischargePower, out.max_discharge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumDischargePower_L2, out.max_discharge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumDischargePower_L3, out.max_discharge_power_L3);
    convert(in.EVMinimumDischargePower, out.min_discharge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumDischargePower_L2, out.min_discharge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumDischargePower_L3, out.min_discharge_power_L3);
}

template <> void convert(const struct iso20_ac_der_iec_DER_AC_CPDReqEnergyTransferModeType& in, DER_AC_ModeReq& out) {
    convert(in, static_cast<AC_ModeReq&>(out));
    cb_convert_enum(in.EVProcessing, out.processing);
    convert(in.EVMaximumDischargePower, out.max_discharge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumDischargePower_L2, out.max_discharge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumDischargePower_L3, out.max_discharge_power_L3);
    convert(in.EVMinimumDischargePower, out.min_discharge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumDischargePower_L2, out.min_discharge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumDischargePower_L3, out.min_discharge_power_L3);
    CB2CPP_CONVERT_IF_USED(in.EVSessionTotalDischargeEnergyAvailable, out.session_total_discharge_energy_available);
    CB2CPP_CONVERT_IF_USED(in.EVReactivePowerLimits, out.reactive_power_limits);
}

template <>
void convert(const struct iso20_ac_der_iec_AC_ChargeParameterDiscoveryReqType& in,
             DER_AC_ChargeParameterDiscoveryRequest& out) {
    convert(in.Header, out.header);
    if (in.AC_CPDReqEnergyTransferMode_isUsed) {
        auto& mode_out = out.transfer_mode.emplace<AC_ModeReq>();
        convert(in.AC_CPDReqEnergyTransferMode, mode_out);
    } else if (in.BPT_AC_CPDReqEnergyTransferMode_isUsed) {
        auto& mode_out = out.transfer_mode.emplace<BPT_AC_ModeReq>();
        convert(in.BPT_AC_CPDReqEnergyTransferMode, mode_out);
    } else if (in.DER_AC_CPDReqEnergyTransferMode_isUsed) {
        auto& mode_out = out.transfer_mode.emplace<DER_AC_ModeReq>();
        convert(in.DER_AC_CPDReqEnergyTransferMode, mode_out);
    } else {
        // FIXME (aw): fail, should not happen!
    }
}

template <> void insert_type(VariantAccess& va, const struct iso20_ac_der_iec_AC_ChargeParameterDiscoveryReqType& in) {
    va.insert_type<DER_AC_ChargeParameterDiscoveryRequest>(in);
}

struct ModeRequestVisitor {
public:
    ModeRequestVisitor(iso20_ac_der_iec_AC_ChargeParameterDiscoveryReqType& req_) : req(req_) {};
    void operator()(const AC_ModeReq& in) {
        init_iso20_ac_der_iec_AC_CPDReqEnergyTransferModeType(&req.AC_CPDReqEnergyTransferMode);
        CB_SET_USED(req.AC_CPDReqEnergyTransferMode);
        convert_common(in, req.AC_CPDReqEnergyTransferMode);
    }

    void operator()(const BPT_AC_ModeReq& in) {
        init_iso20_ac_der_iec_BPT_AC_CPDReqEnergyTransferModeType(&req.BPT_AC_CPDReqEnergyTransferMode);
        CB_SET_USED(req.BPT_AC_CPDReqEnergyTransferMode);
        auto& out = req.BPT_AC_CPDReqEnergyTransferMode;
        convert_common(in, out);
        convert(in.max_discharge_power, out.EVMaximumDischargePower);
        CPP2CB_CONVERT_IF_USED(in.max_discharge_power_L2, out.EVMaximumDischargePower_L2);
        CPP2CB_CONVERT_IF_USED(in.max_discharge_power_L3, out.EVMaximumDischargePower_L3);
        convert(in.min_discharge_power, out.EVMinimumDischargePower);
        CPP2CB_CONVERT_IF_USED(in.min_discharge_power_L2, out.EVMinimumDischargePower_L2);
        CPP2CB_CONVERT_IF_USED(in.min_discharge_power_L3, out.EVMinimumDischargePower_L3);
    }

    void operator()(const DER_AC_ModeReq& in) {
        init_iso20_ac_der_iec_DER_AC_CPDReqEnergyTransferModeType(&req.DER_AC_CPDReqEnergyTransferMode);
        CB_SET_USED(req.DER_AC_CPDReqEnergyTransferMode);
        auto& out = req.DER_AC_CPDReqEnergyTransferMode;
        convert_common(in, out);
        cb_convert_enum(in.processing, out.EVProcessing);
        convert(in.max_discharge_power, out.EVMaximumDischargePower);
        CPP2CB_CONVERT_IF_USED(in.max_discharge_power_L2, out.EVMaximumDischargePower_L2);
        CPP2CB_CONVERT_IF_USED(in.max_discharge_power_L3, out.EVMaximumDischargePower_L3);
        convert(in.min_discharge_power, out.EVMinimumDischargePower);
        CPP2CB_CONVERT_IF_USED(in.min_discharge_power_L2, out.EVMinimumDischargePower_L2);
        CPP2CB_CONVERT_IF_USED(in.min_discharge_power_L3, out.EVMinimumDischargePower_L3);
        CPP2CB_CONVERT_IF_USED(in.session_total_discharge_energy_available, out.EVSessionTotalDischargeEnergyAvailable);
        CPP2CB_CONVERT_IF_USED(in.reactive_power_limits, out.EVReactivePowerLimits);
    }

    template <typename ModeReqTypeIn, typename ModeReqTypeOut>
    static void convert_common(const ModeReqTypeIn& in, ModeReqTypeOut& out) {
        convert(in.max_charge_power, out.EVMaximumChargePower);
        CPP2CB_CONVERT_IF_USED(in.max_charge_power_L2, out.EVMaximumChargePower_L2);
        CPP2CB_CONVERT_IF_USED(in.max_charge_power_L3, out.EVMaximumChargePower_L3);
        convert(in.min_charge_power, out.EVMinimumChargePower);
        CPP2CB_CONVERT_IF_USED(in.min_charge_power_L2, out.EVMinimumChargePower_L2);
        CPP2CB_CONVERT_IF_USED(in.min_charge_power_L3, out.EVMinimumChargePower_L3);
    }

private:
    iso20_ac_der_iec_AC_ChargeParameterDiscoveryReqType& req;
};

template <>
void convert(const DER_AC_ChargeParameterDiscoveryRequest& in,
             struct iso20_ac_der_iec_AC_ChargeParameterDiscoveryReqType& out) {
    init_iso20_ac_der_iec_AC_ChargeParameterDiscoveryReqType(&out);
    convert(in.header, out.Header);
    std::visit(ModeRequestVisitor(out), in.transfer_mode);
}

template <> int serialize_to_exi(const DER_AC_ChargeParameterDiscoveryRequest& in, exi_bitstream_t& out) {
    iso20_ac_der_iec_exiDocument doc{};
    init_iso20_ac_der_iec_exiDocument(&doc);

    CB_SET_USED(doc.AC_ChargeParameterDiscoveryReq);
    convert(in, doc.AC_ChargeParameterDiscoveryReq);
    return encode_iso20_ac_der_iec_exiDocument(&out, &doc);
}
template <> size_t serialize(const DER_AC_ChargeParameterDiscoveryRequest& in, const io::StreamOutputView& out) {
    auto rv = serialize_helper(in, out);
    return rv;
}

template <> void convert(const struct iso20_ac_der_iec_FaultRideThroughType& in, datatypes::FaultRideThrough& out) {
    convert(in.VoltageLimitStartFRT, out.voltage_limit_start_frt);
    CB2CPP_CONVERT_IF_USED(in.VoltageLimitStopFRT, out.voltage_limit_stop_frt);
    CB2CPP_CONVERT_IF_USED(in.VoltageRecoveryLimit, out.voltage_recovery_limit);
    CB2CPP_CONVERT_IF_USED(in.VoltageRideThroughPositiveCurveKFactor, out.voltage_ride_through_positive_curve_k_factor);
    CB2CPP_CONVERT_IF_USED(in.VoltageRideThroughNegativeCurveKFactor, out.voltage_ride_through_negative_curve_k_factor);
    out.pt1_response_active_power = in.PT1ResponseActivePower;
    convert(in.StepResponseTimeConstantActivePower, out.step_response_time_constant_active_power);
    out.pt1_response_reactive_power = in.PT1ResponseReactivePower;
    convert(in.StepResponseTimeConstantReactivePower, out.step_response_time_constant_reactive_power);
}

template <> void convert(const datatypes::FaultRideThrough& in, struct iso20_ac_der_iec_FaultRideThroughType& out) {
    convert(in.voltage_limit_start_frt, out.VoltageLimitStartFRT);
    CPP2CB_CONVERT_IF_USED(in.voltage_limit_stop_frt, out.VoltageLimitStopFRT);
    CPP2CB_CONVERT_IF_USED(in.voltage_recovery_limit, out.VoltageRecoveryLimit);
    CPP2CB_CONVERT_IF_USED(in.voltage_ride_through_positive_curve_k_factor, out.VoltageRideThroughPositiveCurveKFactor);
    CPP2CB_CONVERT_IF_USED(in.voltage_ride_through_negative_curve_k_factor, out.VoltageRideThroughNegativeCurveKFactor);
    out.PT1ResponseActivePower = in.pt1_response_active_power;
    convert(in.step_response_time_constant_active_power, out.StepResponseTimeConstantActivePower);
    out.PT1ResponseReactivePower = in.pt1_response_reactive_power;
    convert(in.step_response_time_constant_reactive_power, out.StepResponseTimeConstantReactivePower);
}

template <> void convert(const struct iso20_ac_der_iec_ZeroCurrentType& in, datatypes::ZeroCurrent& out) {
    CB2CPP_CONVERT_IF_USED(in.OverVoltageLimit, out.over_voltage_limit);
    CB2CPP_CONVERT_IF_USED(in.UnderVoltageLimit, out.under_voltage_limit);
    CB2CPP_CONVERT_IF_USED(in.OverVoltageRecoveryLimit, out.over_voltage_recovery_limit);
    CB2CPP_CONVERT_IF_USED(in.UnderVoltageRecoveryLimit, out.under_voltage_recovery_limit);
    out.pt1_response_active_power = in.PT1ResponseActivePower;
    convert(in.StepResponseTimeConstantActivePower, out.step_response_time_constant_active_power);
    out.pt1_response_reactive_power = in.PT1ResponseReactivePower;
    convert(in.StepResponseTimeConstantReactivePower, out.step_response_time_constant_reactive_power);
}

template <> void convert(const datatypes::ZeroCurrent& in, struct iso20_ac_der_iec_ZeroCurrentType& out) {
    CPP2CB_CONVERT_IF_USED(in.over_voltage_limit, out.OverVoltageLimit);
    CPP2CB_CONVERT_IF_USED(in.under_voltage_limit, out.UnderVoltageLimit);
    CPP2CB_CONVERT_IF_USED(in.over_voltage_recovery_limit, out.OverVoltageRecoveryLimit);
    CPP2CB_CONVERT_IF_USED(in.under_voltage_recovery_limit, out.UnderVoltageRecoveryLimit);
    out.PT1ResponseActivePower = in.pt1_response_active_power;
    convert(in.step_response_time_constant_active_power, out.StepResponseTimeConstantActivePower);
    out.PT1ResponseReactivePower = in.pt1_response_reactive_power;
    convert(in.step_response_time_constant_reactive_power, out.StepResponseTimeConstantReactivePower);
}

template <> void convert(const struct iso20_ac_der_iec_SetpointExcitationType& in, datatypes::SetPointExcitation& out) {
    convert(in.SetpointValue, out.set_point_value);
    if (in.Excitation_isUsed) {
        cb_convert_enum(in.Excitation, out.excitation.emplace());
    }
}

template <> void convert(const datatypes::SetPointExcitation& in, struct iso20_ac_der_iec_SetpointExcitationType& out) {
    convert(in.set_point_value, out.SetpointValue);
    if (in.excitation.has_value()) {
        cb_convert_enum(in.excitation.value(), out.Excitation);
        CB_SET_USED(out.Excitation);
    }
}

template <> void convert(const struct iso20_ac_der_iec_DataTupleType& in, datatypes::DataTuple& out) {
    convert(in.xValue, out.x_value);
    convert(in.yValue, out.y_value);
}

template <> void convert(const datatypes::DataTuple& in, struct iso20_ac_der_iec_DataTupleType& out) {
    convert(in.x_value, out.xValue);
    convert(in.y_value, out.yValue);
}

template <> void convert(const struct iso20_ac_der_iec_DERCurveType& in, datatypes::DerCurve& out) {
    cb_convert_enum(in.xUnit, out.x_unit);
    cb_convert_enum(in.yUnit, out.y_unit);
    for (size_t i = 0; i < in.CurveDataPoints.CurveDataPoint.arrayLen; i++) {
        datatypes::DataTuple tuple;
        convert(in.CurveDataPoints.CurveDataPoint.array[i], tuple);
        out.curve_data_points.push_back(tuple);
    }
    CB2CPP_CONVERT_IF_USED(in.MinCosPhi, out.min_cos_phi);
    if (in.LockValueUnit_isUsed) {
        cb_convert_enum(in.LockValueUnit, out.lock_value_unit.emplace());
    }
    CB2CPP_CONVERT_IF_USED(in.LockInValue, out.lock_in_value);
    CB2CPP_CONVERT_IF_USED(in.LockOutValue, out.lock_out_value);
    out.pt1_response_reactive_power = in.PT1ResponseReactivePower;
    convert(in.StepResponseTimeConstantReactivePower, out.step_response_time_constant_reactive_power);
    CB2CPP_CONVERT_IF_USED(in.IntentionalDelay, out.intentional_delay);
}

template <> void convert(const datatypes::DerCurve& in, struct iso20_ac_der_iec_DERCurveType& out) {
    cb_convert_enum(in.x_unit, out.xUnit);
    cb_convert_enum(in.y_unit, out.yUnit);
    // TODO(SL): Check vector size if its above c max elements
    for (size_t i = 0; i < in.curve_data_points.size(); i++) {
        convert(in.curve_data_points[i], out.CurveDataPoints.CurveDataPoint.array[i]);
    }
    out.CurveDataPoints.CurveDataPoint.arrayLen = in.curve_data_points.size();
    CPP2CB_CONVERT_IF_USED(in.min_cos_phi, out.MinCosPhi);
    if (in.lock_value_unit.has_value()) {
        cb_convert_enum(in.lock_value_unit.value(), out.LockValueUnit);
        CB_SET_USED(out.LockValueUnit);
    }
    CPP2CB_CONVERT_IF_USED(in.lock_in_value, out.LockInValue);
    CPP2CB_CONVERT_IF_USED(in.lock_out_value, out.LockOutValue);
    out.PT1ResponseReactivePower = in.pt1_response_reactive_power;
    convert(in.step_response_time_constant_reactive_power, out.StepResponseTimeConstantReactivePower);
    CPP2CB_CONVERT_IF_USED(in.intentional_delay, out.IntentionalDelay);
}

template <>
void convert(const struct iso20_ac_der_iec_ReactivePowerSupportType& in, datatypes::ReactivePowerSupport& out) {
    convert(in.VoltVar, out.VoltVar);
    convert(in.WattVar, out.WattVar);
    convert(in.WattCosPhi, out.WattCosPhi);
}

template <>
void convert(const datatypes::ReactivePowerSupport& in, struct iso20_ac_der_iec_ReactivePowerSupportType& out) {
    convert(in.VoltVar, out.VoltVar);
    convert(in.WattVar, out.WattVar);
    convert(in.WattCosPhi, out.WattCosPhi);
}

template <> void convert(const struct iso20_ac_der_iec_FrequencyWattType& in, datatypes::FrequencyWatt& out) {
    convert(in.Fstart, out.f_start);
    convert(in.Fstop, out.f_stop);
    CB2CPP_ASSIGN_IF_USED(in.IntentionalDelayFstop, out.intentional_delay_f_stop);
    convert(in.Slope, out.slope);
    CB2CPP_ASSIGN_IF_USED(in.DeactivationTime, out.deactivation_time);
    CB2CPP_ASSIGN_IF_USED(in.IntentionalDelayPowerControl, out.intentional_delay_power_control);
    cb_convert_enum(in.PowerReference, out.power_reference);
    out.hysteresis_control = in.HysteresisControl;
    CB2CPP_ASSIGN_IF_USED(in.PowerUpRamp, out.power_up_ramp);
    out.pt1_response_active_power = in.PT1ResponseActivePower;
    convert(in.StepResponseTimeConstantActivePower, out.step_response_time_constant_active_power);
}

template <> void convert(const datatypes::FrequencyWatt& in, struct iso20_ac_der_iec_FrequencyWattType& out) {
    convert(in.f_start, out.Fstart);
    convert(in.f_stop, out.Fstop);
    CPP2CB_ASSIGN_IF_USED(in.intentional_delay_f_stop, out.IntentionalDelayFstop);
    convert(in.slope, out.Slope);
    CPP2CB_ASSIGN_IF_USED(in.deactivation_time, out.DeactivationTime);
    CPP2CB_ASSIGN_IF_USED(in.intentional_delay_power_control, out.IntentionalDelayPowerControl);
    cb_convert_enum(in.power_reference, out.PowerReference);
    out.HysteresisControl = in.hysteresis_control;
    CPP2CB_ASSIGN_IF_USED(in.power_up_ramp, out.PowerUpRamp);
    out.PT1ResponseActivePower = in.pt1_response_active_power;
    convert(in.step_response_time_constant_active_power, out.StepResponseTimeConstantActivePower);
}

template <> void convert(const struct iso20_ac_der_iec_VoltWattType& in, datatypes::VoltWatt& out) {
    cb_convert_enum(in.PowerReference, out.power_reference);
    convert(in.UStart, out.u_start);
    convert(in.UStop, out.u_stop);
    out.pt1_response_active_power = in.PT1ResponseActivePower;
    convert(in.StepResponseTimeConstantActivePower, out.step_response_time_constant_active_power);
    CB2CPP_ASSIGN_IF_USED(in.IntentionalDelayPowerControl, out.intentional_delay_power_control);
}

template <> void convert(const datatypes::VoltWatt& in, struct iso20_ac_der_iec_VoltWattType& out) {
    cb_convert_enum(in.power_reference, out.PowerReference);
    convert(in.u_start, out.UStart);
    convert(in.u_stop, out.UStop);
    out.PT1ResponseActivePower = in.pt1_response_active_power;
    convert(in.step_response_time_constant_active_power, out.StepResponseTimeConstantActivePower);
    CPP2CB_ASSIGN_IF_USED(in.intentional_delay_power_control, out.IntentionalDelayPowerControl);
}

template <> void convert(const struct iso20_ac_der_iec_ActivePowerSupportType& in, datatypes::ActivePowerSupport& out) {
    CB2CPP_CONVERT_IF_USED(in.UnderFrequencyWatt, out.under_frequency_watt);
    CB2CPP_CONVERT_IF_USED(in.OverFrequencyWatt, out.over_frequency_watt);
    CB2CPP_CONVERT_IF_USED(in.VoltWatt, out.volt_watt);
}

template <> void convert(const datatypes::ActivePowerSupport& in, struct iso20_ac_der_iec_ActivePowerSupportType& out) {
    CPP2CB_CONVERT_IF_USED(in.under_frequency_watt, out.UnderFrequencyWatt);
    CPP2CB_CONVERT_IF_USED(in.over_frequency_watt, out.OverFrequencyWatt);
    CPP2CB_CONVERT_IF_USED(in.volt_watt, out.VoltWatt);
}

template <> void convert(const struct iso20_ac_der_iec_DERControlType& in, datatypes::DerControl& out) {
    CB2CPP_CONVERT_IF_USED(in.OvervoltageFaultRideThrough, out.over_voltage_fault_ride_through);
    CB2CPP_CONVERT_IF_USED(in.UndervoltageFaultRideThrough, out.under_voltage_fault_ride_through);
    CB2CPP_CONVERT_IF_USED(in.ZeroCurrent, out.zero_current);
    CB2CPP_CONVERT_IF_USED(in.ReactivePowerSupport, out.reactive_power_support);
    CB2CPP_CONVERT_IF_USED(in.ActivePowerSupport, out.active_power_support);
    CB2CPP_CONVERT_IF_USED(in.MaximumLevelDCInjection, out.max_level_dc_injection);
}

template <> void convert(const datatypes::DerControl& in, struct iso20_ac_der_iec_DERControlType& out) {
    CPP2CB_CONVERT_IF_USED(in.over_voltage_fault_ride_through, out.OvervoltageFaultRideThrough);
    CPP2CB_CONVERT_IF_USED(in.under_voltage_fault_ride_through, out.UndervoltageFaultRideThrough);
    CPP2CB_CONVERT_IF_USED(in.zero_current, out.ZeroCurrent);
    CPP2CB_CONVERT_IF_USED(in.reactive_power_support, out.ReactivePowerSupport);
    CPP2CB_CONVERT_IF_USED(in.active_power_support, out.ActivePowerSupport);
    CPP2CB_CONVERT_IF_USED(in.max_level_dc_injection, out.MaximumLevelDCInjection);
}

template <typename InType> void convert(const InType& in, AC_ModeRes& out) {
    convert(in.EVSEMaximumChargePower, out.max_charge_power);
    CB2CPP_CONVERT_IF_USED(in.EVSEMaximumChargePower_L2, out.max_charge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVSEMaximumChargePower_L3, out.max_charge_power_L3);
    convert(in.EVSEMinimumChargePower, out.min_charge_power);
    CB2CPP_CONVERT_IF_USED(in.EVSEMinimumChargePower_L2, out.min_charge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVSEMinimumChargePower_L3, out.min_charge_power_L3);
    convert(in.EVSENominalFrequency, out.nominal_frequency);
    CB2CPP_CONVERT_IF_USED(in.MaximumPowerAsymmetry, out.max_power_asymmetry);
    CB2CPP_CONVERT_IF_USED(in.EVSEPowerRampLimitation, out.power_ramp_limitation);
    CB2CPP_CONVERT_IF_USED(in.EVSEPresentActivePower, out.present_active_power);
    CB2CPP_CONVERT_IF_USED(in.EVSEPresentActivePower_L2, out.present_active_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVSEPresentActivePower_L3, out.present_active_power_L3);
}

template <> void convert(const struct iso20_ac_der_iec_BPT_AC_CPDResEnergyTransferModeType& in, BPT_AC_ModeRes& out) {
    convert(in, static_cast<AC_ModeRes&>(out));
    convert(in.EVSEMaximumDischargePower, out.max_discharge_power);
    CB2CPP_CONVERT_IF_USED(in.EVSEMaximumDischargePower_L2, out.max_discharge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVSEMaximumDischargePower_L3, out.max_discharge_power_L3);
    convert(in.EVSEMinimumDischargePower, out.min_discharge_power);
    CB2CPP_CONVERT_IF_USED(in.EVSEMinimumDischargePower_L2, out.min_discharge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVSEMinimumDischargePower_L3, out.min_discharge_power_L3);
}

template <> void convert(const struct iso20_ac_der_iec_DER_AC_CPDResEnergyTransferModeType& in, DER_AC_ModeRes& out) {
    convert(in, static_cast<AC_ModeRes&>(out));
    convert(in.EVSENominalChargePower, out.nominal_charge_power);
    CB2CPP_CONVERT_IF_USED(in.EVSENominalChargePower_L2, out.nominal_charge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVSENominalChargePower_L3, out.nominal_charge_power_L3);
    convert(in.EVSENominalDischargePower, out.nominal_discharge_power);
    CB2CPP_CONVERT_IF_USED(in.EVSENominalDischargePower_L2, out.nominal_discharge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVSENominalDischargePower_L3, out.nominal_discharge_power_L3);
    convert(in.EVSEMaximumDischargePower, out.max_discharge_power);
    CB2CPP_CONVERT_IF_USED(in.EVSEMaximumDischargePower_L2, out.max_discharge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVSEMaximumDischargePower_L3, out.max_discharge_power_L3);
    cb_convert_enum(in.EVOperatingMode, out.operating_mode);
    cb_convert_enum(in.GridConnectionMode, out.grid_connection_mode);
    convert(in.DERControl, out.der_control);
}

template <>
void convert(const struct iso20_ac_der_iec_AC_ChargeParameterDiscoveryResType& in,
             DER_AC_ChargeParameterDiscoveryResponse& out) {
    convert(in.Header, out.header);
    if (in.AC_CPDResEnergyTransferMode_isUsed) {
        auto& mode_out = out.transfer_mode.emplace<AC_ModeRes>();
        convert(in.AC_CPDResEnergyTransferMode, mode_out);
    } else if (in.BPT_AC_CPDResEnergyTransferMode_isUsed) {
        auto& mode_out = out.transfer_mode.emplace<BPT_AC_ModeRes>();
        convert(in.BPT_AC_CPDResEnergyTransferMode, mode_out);
    } else if (in.DER_AC_CPDResEnergyTransferMode_isUsed) {
        auto& mode_out = out.transfer_mode.emplace<DER_AC_ModeRes>();
        convert(in.DER_AC_CPDResEnergyTransferMode, mode_out);
    } else {
        // FIXME (aw): fail, should not happen!
    }
}

template <> void insert_type(VariantAccess& va, const struct iso20_ac_der_iec_AC_ChargeParameterDiscoveryResType& in) {
    va.insert_type<DER_AC_ChargeParameterDiscoveryResponse>(in);
}

struct ModeResponseVisitor {
public:
    ModeResponseVisitor(iso20_ac_der_iec_AC_ChargeParameterDiscoveryResType& res_) : res(res_) {};
    void operator()(const AC_ModeRes& in) {
        init_iso20_ac_der_iec_AC_CPDResEnergyTransferModeType(&res.AC_CPDResEnergyTransferMode);

        CB_SET_USED(res.AC_CPDResEnergyTransferMode);
        convert_common(in, res.AC_CPDResEnergyTransferMode);
    }

    void operator()(const BPT_AC_ModeRes& in) {
        init_iso20_ac_der_iec_BPT_AC_CPDResEnergyTransferModeType(&res.BPT_AC_CPDResEnergyTransferMode);

        CB_SET_USED(res.BPT_AC_CPDResEnergyTransferMode);

        auto& out = res.BPT_AC_CPDResEnergyTransferMode;
        convert_common(in, out);

        convert(in.max_discharge_power, out.EVSEMaximumDischargePower);
        CPP2CB_CONVERT_IF_USED(in.max_charge_power_L2, out.EVSEMaximumDischargePower_L2);
        CPP2CB_CONVERT_IF_USED(in.max_charge_power_L3, out.EVSEMaximumDischargePower_L3);

        convert(in.min_discharge_power, out.EVSEMinimumDischargePower);
        CPP2CB_CONVERT_IF_USED(in.min_charge_power_L2, out.EVSEMinimumDischargePower_L2);
        CPP2CB_CONVERT_IF_USED(in.min_charge_power_L3, out.EVSEMinimumDischargePower_L3);
    }

    void operator()(const DER_AC_ModeRes& in) {
        init_iso20_ac_der_iec_DER_AC_CPDResEnergyTransferModeType(&res.DER_AC_CPDResEnergyTransferMode);
        CB_SET_USED(res.DER_AC_CPDResEnergyTransferMode);

        auto& out = res.DER_AC_CPDResEnergyTransferMode;
        convert_common(in, out);

        convert(in.nominal_charge_power, out.EVSENominalChargePower);
        CPP2CB_CONVERT_IF_USED(in.nominal_charge_power_L2, out.EVSENominalChargePower_L2);
        CPP2CB_CONVERT_IF_USED(in.nominal_charge_power_L3, out.EVSENominalChargePower_L3);
        convert(in.nominal_discharge_power, out.EVSENominalDischargePower);

        CPP2CB_CONVERT_IF_USED(in.nominal_discharge_power_L2, out.EVSENominalDischargePower_L2);
        CPP2CB_CONVERT_IF_USED(in.nominal_discharge_power_L3, out.EVSENominalDischargePower_L3);

        convert(in.max_discharge_power, out.EVSEMaximumDischargePower);
        CPP2CB_CONVERT_IF_USED(in.max_charge_power_L2, out.EVSEMaximumDischargePower_L2);
        CPP2CB_CONVERT_IF_USED(in.max_charge_power_L3, out.EVSEMaximumDischargePower_L3);

        cb_convert_enum(in.operating_mode, out.EVOperatingMode);
        cb_convert_enum(in.grid_connection_mode, out.GridConnectionMode);
        convert(in.der_control, out.DERControl);
    }

    template <typename ModeResTypeIn, typename ModeResTypeOut>
    static void convert_common(const ModeResTypeIn& in, ModeResTypeOut& out) {
        convert(in.max_charge_power, out.EVSEMaximumChargePower);
        CPP2CB_CONVERT_IF_USED(in.max_charge_power_L2, out.EVSEMaximumChargePower_L2);
        CPP2CB_CONVERT_IF_USED(in.max_charge_power_L3, out.EVSEMaximumChargePower_L3);
        convert(in.min_charge_power, out.EVSEMinimumChargePower);
        CPP2CB_CONVERT_IF_USED(in.min_charge_power_L2, out.EVSEMinimumChargePower_L2);
        CPP2CB_CONVERT_IF_USED(in.min_charge_power_L3, out.EVSEMinimumChargePower_L3);
        convert(in.nominal_frequency, out.EVSENominalFrequency);
        CPP2CB_CONVERT_IF_USED(in.max_power_asymmetry, out.MaximumPowerAsymmetry);
        CPP2CB_CONVERT_IF_USED(in.power_ramp_limitation, out.EVSEPowerRampLimitation);
        CPP2CB_CONVERT_IF_USED(in.present_active_power, out.EVSEPresentActivePower);
        CPP2CB_CONVERT_IF_USED(in.present_active_power_L2, out.EVSEPresentActivePower_L2);
        CPP2CB_CONVERT_IF_USED(in.present_active_power_L3, out.EVSEPresentActivePower_L3);
    }

private:
    iso20_ac_der_iec_AC_ChargeParameterDiscoveryResType& res;
};

template <>
void convert(const DER_AC_ChargeParameterDiscoveryResponse& in,
             struct iso20_ac_der_iec_AC_ChargeParameterDiscoveryResType& out) {

    init_iso20_ac_der_iec_AC_ChargeParameterDiscoveryResType(&out);
    convert(in.header, out.Header);
    cb_convert_enum(in.response_code, out.ResponseCode);

    std::visit(ModeResponseVisitor(out), in.transfer_mode);
}

template <> int serialize_to_exi(const DER_AC_ChargeParameterDiscoveryResponse& in, exi_bitstream_t& out) {
    iso20_ac_der_iec_exiDocument doc{};
    init_iso20_ac_der_iec_exiDocument(&doc);
    CB_SET_USED(doc.AC_ChargeParameterDiscoveryRes);
    convert(in, doc.AC_ChargeParameterDiscoveryRes);

    return encode_iso20_ac_der_iec_exiDocument(&out, &doc);
}
template <> size_t serialize(const DER_AC_ChargeParameterDiscoveryResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace iso15118::message_20
