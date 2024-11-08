// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/dc_charge_loop.hpp>

#include <type_traits>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_20/iso20_DC_Decoder.h>
#include <cbv2g/iso_20/iso20_DC_Encoder.h>

namespace iso15118::message_20 {

template <> void convert(const struct iso20_dc_DisplayParametersType& in, DisplayParameters& out) {
    CB2CPP_ASSIGN_IF_USED(in.PresentSOC, out.present_soc);
    CB2CPP_ASSIGN_IF_USED(in.MinimumSOC, out.min_soc);
    CB2CPP_ASSIGN_IF_USED(in.TargetSOC, out.target_soc);
    CB2CPP_ASSIGN_IF_USED(in.MaximumSOC, out.max_soc);

    CB2CPP_ASSIGN_IF_USED(in.RemainingTimeToMinimumSOC, out.remaining_time_to_min_soc);
    CB2CPP_ASSIGN_IF_USED(in.RemainingTimeToTargetSOC, out.remaining_time_to_target_soc);
    CB2CPP_ASSIGN_IF_USED(in.RemainingTimeToMaximumSOC, out.remaining_time_to_max_soc);

    CB2CPP_ASSIGN_IF_USED(in.ChargingComplete, out.charging_complete);
    CB2CPP_CONVERT_IF_USED(in.BatteryEnergyCapacity, out.battery_energy_capacity);
    CB2CPP_ASSIGN_IF_USED(in.InletHot, out.inlet_hot);
}

// FIXME (aw): this should go to common.cpp
template <typename InType> void convert(const InType& in, Scheduled_CLReqControlMode& out) {
    CB2CPP_CONVERT_IF_USED(in.EVTargetEnergyRequest, out.target_energy_request);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumEnergyRequest, out.max_energy_request);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumEnergyRequest, out.min_energy_request);
}

template <typename InType> void convert(const InType& in, DC_ChargeLoopRequest::Scheduled_DC_CLReqControlMode& out) {
    static_assert(std::is_same_v<InType, iso20_dc_Scheduled_DC_CLReqControlModeType> or
                  std::is_same_v<InType, iso20_dc_BPT_Scheduled_DC_CLReqControlModeType>);
    convert(in, static_cast<Scheduled_CLReqControlMode&>(out));

    convert(in.EVTargetCurrent, out.target_current);
    convert(in.EVTargetVoltage, out.target_voltage);

    CB2CPP_CONVERT_IF_USED(in.EVMaximumChargePower, out.max_charge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumChargePower, out.min_charge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumChargeCurrent, out.max_charge_current);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumVoltage, out.max_voltage);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumVoltage, out.min_voltage);
}

template <>
void convert(const struct iso20_dc_BPT_Scheduled_DC_CLReqControlModeType& in,
             DC_ChargeLoopRequest::BPT_Scheduled_DC_CLReqControlMode& out) {
    convert(in, static_cast<DC_ChargeLoopRequest::Scheduled_DC_CLReqControlMode&>(out));

    CB2CPP_CONVERT_IF_USED(in.EVMaximumDischargePower, out.max_discharge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumDischargePower, out.min_discharge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumDischargeCurrent, out.max_discharge_current);
}

// FIXME (aw): this should go to common.cpp
template <typename InType> void convert(const InType& in, Dynamic_CLReqControlMode& out) {
    CB2CPP_ASSIGN_IF_USED(in.DepartureTime, out.departure_time);
    convert(in.EVTargetEnergyRequest, out.target_energy_request);
    convert(in.EVMaximumEnergyRequest, out.max_energy_request);
    convert(in.EVMinimumEnergyRequest, out.min_energy_request);
}

template <typename InType> void convert(const InType& in, DC_ChargeLoopRequest::Dynamic_DC_CLReqControlMode& out) {
    static_assert(std::is_same_v<InType, iso20_dc_Dynamic_DC_CLReqControlModeType> or
                  std::is_same_v<InType, iso20_dc_BPT_Dynamic_DC_CLReqControlModeType>);
    convert(in, static_cast<Dynamic_CLReqControlMode&>(out));

    convert(in.EVMaximumChargePower, out.max_charge_power);
    convert(in.EVMinimumChargePower, out.min_charge_power);
    convert(in.EVMaximumChargeCurrent, out.max_charge_current);
    convert(in.EVMaximumVoltage, out.max_voltage);
    convert(in.EVMinimumVoltage, out.min_voltage);
}

template <>
void convert(const struct iso20_dc_BPT_Dynamic_DC_CLReqControlModeType& in,
             DC_ChargeLoopRequest::BPT_Dynamic_DC_CLReqControlMode& out) {
    convert(in, static_cast<DC_ChargeLoopRequest::Dynamic_DC_CLReqControlMode&>(out));

    convert(in.EVMaximumDischargePower, out.max_discharge_power);
    convert(in.EVMinimumDischargePower, out.min_discharge_power);
    convert(in.EVMaximumDischargeCurrent, out.max_discharge_current);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumV2XEnergyRequest, out.max_v2x_energy_request);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumV2XEnergyRequest, out.min_v2x_energy_request);
}

template <> void convert(const struct iso20_dc_DC_ChargeLoopReqType& in, DC_ChargeLoopRequest& out) {
    convert(in.Header, out.header);

    CB2CPP_CONVERT_IF_USED(in.DisplayParameters, out.display_parameters);

    out.meter_info_requested = in.MeterInfoRequested;

    convert(in.EVPresentVoltage, out.present_voltage);

    if (in.Scheduled_DC_CLReqControlMode_isUsed) {
        convert(in.Scheduled_DC_CLReqControlMode,
                out.control_mode.emplace<DC_ChargeLoopRequest::Scheduled_DC_CLReqControlMode>());
    } else if (in.BPT_Scheduled_DC_CLReqControlMode_isUsed) {
        convert(in.BPT_Scheduled_DC_CLReqControlMode,
                out.control_mode.emplace<DC_ChargeLoopRequest::BPT_Scheduled_DC_CLReqControlMode>());
    } else if (in.Dynamic_DC_CLReqControlMode_isUsed) {
        convert(in.Dynamic_DC_CLReqControlMode,
                out.control_mode.emplace<DC_ChargeLoopRequest::Dynamic_DC_CLReqControlMode>());
    } else if (in.BPT_Dynamic_DC_CLReqControlMode_isUsed) {
        convert(in.BPT_Dynamic_DC_CLReqControlMode,
                out.control_mode.emplace<DC_ChargeLoopRequest::BPT_Dynamic_DC_CLReqControlMode>());
    } else {
        // should not happen
        assert(false);
    }
}

template <> void insert_type(VariantAccess& va, const struct iso20_dc_DC_ChargeLoopReqType& in) {
    va.insert_type<DC_ChargeLoopRequest>(in);
}

template <> void convert(const DetailedCost& in, struct iso20_dc_DetailedCostType& out) {
    init_iso20_dc_DetailedCostType(&out);
    convert(in.amount, out.Amount);
    convert(in.cost_per_unit, out.CostPerUnit);
}

template <> void convert(const DetailedTax& in, struct iso20_dc_DetailedTaxType& out) {
    init_iso20_dc_DetailedTaxType(&out);
    out.TaxRuleID = in.tax_rule_id;
    convert(in.amount, out.Amount);
}

template <> void convert(const Receipt& in, struct iso20_dc_ReceiptType& out) {
    init_iso20_dc_ReceiptType(&out);

    out.TimeAnchor = in.time_anchor;
    CPP2CB_CONVERT_IF_USED(in.energy_costs, out.EnergyCosts);
    CPP2CB_CONVERT_IF_USED(in.occupany_costs, out.OccupancyCosts);
    CPP2CB_CONVERT_IF_USED(in.additional_service_costs, out.AdditionalServicesCosts);
    CPP2CB_CONVERT_IF_USED(in.overstay_costs, out.OverstayCosts);

    if (sizeof(out.TaxCosts.array) < in.tax_costs.size()) {
        throw std::runtime_error("tax costs array is too large");
    }
    for (std::size_t i = 0; i < in.tax_costs.size(); ++i) {
        convert(in.tax_costs[i], out.TaxCosts.array[i]);
    }
    out.TaxCosts.arrayLen = in.tax_costs.size();
}

template <typename cb_Type> void convert(const DC_ChargeLoopResponse::Scheduled_DC_CLResControlMode& in, cb_Type& out) {
    CPP2CB_CONVERT_IF_USED(in.max_charge_power, out.EVSEMaximumChargePower);
    CPP2CB_CONVERT_IF_USED(in.min_charge_power, out.EVSEMinimumChargePower);
    CPP2CB_CONVERT_IF_USED(in.max_charge_current, out.EVSEMaximumChargeCurrent);
    CPP2CB_CONVERT_IF_USED(in.max_voltage, out.EVSEMaximumVoltage);
}

template <typename cb_Type> void convert(const Dynamic_CLResControlMode& in, cb_Type& out) {
    CPP2CB_ASSIGN_IF_USED(in.departure_time, out.DepartureTime);
    CPP2CB_ASSIGN_IF_USED(in.minimum_soc, out.MinimumSOC);
    CPP2CB_ASSIGN_IF_USED(in.target_soc, out.TargetSOC);
    CPP2CB_ASSIGN_IF_USED(in.ack_max_delay, out.AckMaxDelay);
}

template <>
void convert(const DC_ChargeLoopResponse::BPT_Scheduled_DC_CLResControlMode& in,
             struct iso20_dc_BPT_Scheduled_DC_CLResControlModeType& out) {
    convert(static_cast<const DC_ChargeLoopResponse::Scheduled_DC_CLResControlMode&>(in), out);

    CPP2CB_CONVERT_IF_USED(in.max_discharge_power, out.EVSEMaximumDischargePower);
    CPP2CB_CONVERT_IF_USED(in.min_discharge_power, out.EVSEMinimumDischargePower);
    CPP2CB_CONVERT_IF_USED(in.max_discharge_current, out.EVSEMaximumDischargePower);
    CPP2CB_CONVERT_IF_USED(in.min_voltage, out.EVSEMinimumVoltage);
}

template <typename cb_Type> void convert(const DC_ChargeLoopResponse::Dynamic_DC_CLResControlMode& in, cb_Type& out) {
    convert(static_cast<const Dynamic_CLResControlMode&>(in), out);

    convert(in.max_charge_power, out.EVSEMaximumChargePower);
    convert(in.min_charge_power, out.EVSEMinimumChargePower);
    convert(in.max_charge_current, out.EVSEMaximumChargePower);
    convert(in.max_voltage, out.EVSEMaximumVoltage);
}

template <>
void convert(const DC_ChargeLoopResponse::BPT_Dynamic_DC_CLResControlMode& in,
             struct iso20_dc_BPT_Dynamic_DC_CLResControlModeType& out) {
    convert(static_cast<const DC_ChargeLoopResponse::Dynamic_DC_CLResControlMode&>(in), out);

    convert(in.max_discharge_power, out.EVSEMaximumDischargePower);
    convert(in.min_discharge_power, out.EVSEMinimumDischargePower);
    convert(in.max_discharge_current, out.EVSEMaximumDischargeCurrent);
    convert(in.min_voltage, out.EVSEMinimumVoltage);
}

struct ControlModeVisitor {
    using ScheduledCM = DC_ChargeLoopResponse::Scheduled_DC_CLResControlMode;
    using BPT_ScheduledCM = DC_ChargeLoopResponse::BPT_Scheduled_DC_CLResControlMode;
    using DynamicCM = DC_ChargeLoopResponse::Dynamic_DC_CLResControlMode;
    using BPT_DynamicCM = DC_ChargeLoopResponse::BPT_Dynamic_DC_CLResControlMode;

    ControlModeVisitor(iso20_dc_DC_ChargeLoopResType& res_) : res(res_){};
    void operator()(const ScheduledCM& in) {
        auto& out = res.Scheduled_DC_CLResControlMode;
        init_iso20_dc_Scheduled_DC_CLResControlModeType(&out);
        convert(in, out);
        CB_SET_USED(res.Scheduled_DC_CLResControlMode);
    }
    void operator()(const BPT_ScheduledCM& in) {
        auto& out = res.BPT_Scheduled_DC_CLResControlMode;
        init_iso20_dc_BPT_Scheduled_DC_CLResControlModeType(&out);
        convert(in, out);
        CB_SET_USED(res.BPT_Scheduled_DC_CLResControlMode);
    }
    void operator()(const DynamicCM& in) {
        auto& out = res.Dynamic_DC_CLResControlMode;
        init_iso20_dc_Dynamic_DC_CLResControlModeType(&out);
        convert(in, out);
        CB_SET_USED(res.Dynamic_DC_CLResControlMode);
    }
    void operator()(const BPT_DynamicCM& in) {
        auto& out = res.BPT_Dynamic_DC_CLResControlMode;
        init_iso20_dc_BPT_Dynamic_DC_CLResControlModeType(&out);
        convert(in, out);
        CB_SET_USED(res.BPT_Dynamic_DC_CLResControlMode);
    }

private:
    iso20_dc_DC_ChargeLoopResType& res;
};

template <> void convert(const DC_ChargeLoopResponse& in, struct iso20_dc_DC_ChargeLoopResType& out) {
    init_iso20_dc_DC_ChargeLoopResType(&out);
    convert(in.header, out.Header);
    cb_convert_enum(in.response_code, out.ResponseCode);

    CPP2CB_CONVERT_IF_USED(in.status, out.EVSEStatus);
    CPP2CB_CONVERT_IF_USED(in.meter_info, out.MeterInfo);
    CPP2CB_CONVERT_IF_USED(in.receipt, out.Receipt);

    convert(in.present_current, out.EVSEPresentCurrent);
    convert(in.present_voltage, out.EVSEPresentVoltage);

    out.EVSEPowerLimitAchieved = in.power_limit_achieved;
    out.EVSECurrentLimitAchieved = in.current_limit_achieved;
    out.EVSEVoltageLimitAchieved = in.voltage_limit_achieved;

    std::visit(ControlModeVisitor(out), in.control_mode);
}

template <> int serialize_to_exi(const DC_ChargeLoopResponse& in, exi_bitstream_t& out) {
    iso20_dc_exiDocument doc;
    init_iso20_dc_exiDocument(&doc);

    CB_SET_USED(doc.DC_ChargeLoopRes);

    convert(in, doc.DC_ChargeLoopRes);

    return encode_iso20_dc_exiDocument(&out, &doc);
}

template <> size_t serialize(const DC_ChargeLoopResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace iso15118::message_20
