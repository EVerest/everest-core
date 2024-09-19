// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/ac_charge_loop.hpp>

#include <type_traits>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_20/iso20_AC_Decoder.h>
#include <cbv2g/iso_20/iso20_AC_Encoder.h>

namespace iso15118::message_20 {

template <> void convert(const struct iso20_ac_DisplayParametersType& in, DisplayParameters& out) {

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

// Todo(sl): this should go to common.cpp
template <typename InType> void convert(const InType& in, Scheduled_CLReqControlMode& out) {
    CB2CPP_CONVERT_IF_USED(in.EVTargetEnergyRequest, out.target_energy_request);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumEnergyRequest, out.max_energy_request);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumEnergyRequest, out.min_energy_request);
}

template <typename InType> void convert(const InType& in, AC_ChargeLoopRequest::Scheduled_AC_CLReqControlMode& out) {
    static_assert(std::is_same_v<InType, iso20_ac_Scheduled_AC_CLReqControlModeType> or
                  std::is_same_v<InType, iso20_ac_BPT_Scheduled_AC_CLReqControlModeType>);
    convert(in, static_cast<Scheduled_CLReqControlMode&>(out));

    CB2CPP_CONVERT_IF_USED(in.EVMaximumChargePower, out.max_charge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumChargePower_L2, out.max_charge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumChargePower_L3, out.max_charge_power_L3);

    CB2CPP_CONVERT_IF_USED(in.EVMinimumChargePower, out.min_charge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumChargePower_L2, out.min_charge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumChargePower_L3, out.min_charge_power_L3);

    convert(in.EVPresentActivePower, out.present_active_power);
    CB2CPP_CONVERT_IF_USED(in.EVPresentActivePower_L2, out.present_active_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVPresentActivePower_L3, out.present_active_power_L3);

    CB2CPP_CONVERT_IF_USED(in.EVPresentReactivePower, out.present_reactive_power);
    CB2CPP_CONVERT_IF_USED(in.EVPresentReactivePower_L2, out.present_reactive_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVPresentReactivePower_L3, out.present_reactive_power_L3);
}

template <>
void convert(const struct iso20_ac_BPT_Scheduled_AC_CLReqControlModeType& in,
             AC_ChargeLoopRequest::BPT_Scheduled_AC_CLReqControlMode& out) {
    convert(in, static_cast<AC_ChargeLoopRequest::Scheduled_AC_CLReqControlMode&>(out));

    CB2CPP_CONVERT_IF_USED(in.EVMaximumDischargePower, out.max_discharge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumDischargePower_L2, out.max_discharge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumDischargePower_L3, out.max_discharge_power_L3);

    CB2CPP_CONVERT_IF_USED(in.EVMinimumDischargePower, out.min_discharge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumDischargePower_L2, out.min_discharge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumDischargePower_L3, out.min_discharge_power_L3);
}

// Todo(sl): this should go to common.cpp
template <typename InType> void convert(const InType& in, Dynamic_CLReqControlMode& out) {
    CB2CPP_ASSIGN_IF_USED(in.DepartureTime, out.departure_time);
    convert(in.EVTargetEnergyRequest, out.target_energy_request);
    convert(in.EVMaximumEnergyRequest, out.max_energy_request);
    convert(in.EVMinimumEnergyRequest, out.min_energy_request);
}

template <typename InType> void convert(const InType& in, AC_ChargeLoopRequest::Dynamic_AC_CLReqControlMode& out) {
    static_assert(std::is_same_v<InType, iso20_ac_Dynamic_AC_CLReqControlModeType> or
                  std::is_same_v<InType, iso20_ac_BPT_Dynamic_AC_CLReqControlModeType>);
    convert(in, static_cast<Dynamic_CLReqControlMode&>(out));

    convert(in.EVMaximumChargePower, out.max_charge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumChargePower_L2, out.max_charge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumChargePower_L3, out.max_charge_power_L3);

    convert(in.EVMinimumChargePower, out.min_charge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumChargePower_L2, out.min_charge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumChargePower_L3, out.min_charge_power_L3);

    convert(in.EVPresentActivePower, out.present_active_power);
    CB2CPP_CONVERT_IF_USED(in.EVPresentActivePower_L2, out.present_active_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVPresentActivePower_L3, out.present_active_power_L3);

    convert(in.EVPresentReactivePower, out.present_reactive_power);
    CB2CPP_CONVERT_IF_USED(in.EVPresentReactivePower_L2, out.present_reactive_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVPresentReactivePower_L3, out.present_reactive_power_L3);
}

template <>
void convert(const struct iso20_ac_BPT_Dynamic_AC_CLReqControlModeType& in,
             AC_ChargeLoopRequest::BPT_Dynamic_AC_CLReqControlMode& out) {
    convert(in, static_cast<AC_ChargeLoopRequest::Dynamic_AC_CLReqControlMode&>(out));

    convert(in.EVMaximumDischargePower, out.max_discharge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumDischargePower_L2, out.max_discharge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumDischargePower_L3, out.max_discharge_power_L3);

    convert(in.EVMinimumDischargePower, out.min_discharge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumDischargePower_L2, out.min_discharge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumDischargePower_L3, out.min_discharge_power_L3);

    CB2CPP_CONVERT_IF_USED(in.EVMaximumV2XEnergyRequest, out.max_v2x_energy_request);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumV2XEnergyRequest, out.min_v2x_energy_request);
}

template <> void convert(const struct iso20_ac_AC_ChargeLoopReqType& in, AC_ChargeLoopRequest& out) {
    convert(in.Header, out.header);

    CB2CPP_CONVERT_IF_USED(in.DisplayParameters, out.display_parameters);

    out.meter_info_requested = in.MeterInfoRequested;

    if (in.Scheduled_AC_CLReqControlMode_isUsed) {
        convert(in.Scheduled_AC_CLReqControlMode,
                out.control_mode.emplace<AC_ChargeLoopRequest::Scheduled_AC_CLReqControlMode>());
    } else if (in.BPT_Scheduled_AC_CLReqControlMode_isUsed) {
        convert(in.BPT_Scheduled_AC_CLReqControlMode,
                out.control_mode.emplace<AC_ChargeLoopRequest::BPT_Scheduled_AC_CLReqControlMode>());
    } else if (in.Dynamic_AC_CLReqControlMode_isUsed) {
        convert(in.Dynamic_AC_CLReqControlMode,
                out.control_mode.emplace<AC_ChargeLoopRequest::Dynamic_AC_CLReqControlMode>());
    } else if (in.BPT_Dynamic_AC_CLReqControlMode_isUsed) {
        convert(in.BPT_Dynamic_AC_CLReqControlMode,
                out.control_mode.emplace<AC_ChargeLoopRequest::BPT_Dynamic_AC_CLReqControlMode>());
    } else {
        // should not happen
        assert(false);
    }
}

template <> void insert_type(VariantAccess& va, const struct iso20_ac_AC_ChargeLoopReqType& in) {
    va.insert_type<AC_ChargeLoopRequest>(in);
}

template <> void convert(const DetailedCost& in, struct iso20_ac_DetailedCostType& out) {
    init_iso20_ac_DetailedCostType(&out);
    convert(in.amount, out.Amount);
    convert(in.cost_per_unit, out.CostPerUnit);
}

template <> void convert(const DetailedTax& in, struct iso20_ac_DetailedTaxType& out) {
    init_iso20_ac_DetailedTaxType(&out);
    out.TaxRuleID = in.tax_rule_id;
    convert(in.amount, out.Amount);
}

template <> void convert(const Receipt& in, struct iso20_ac_ReceiptType& out) {
    init_iso20_ac_ReceiptType(&out);

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

template <typename cb_Type> void convert(const AC_ChargeLoopResponse::Scheduled_AC_CLResControlMode& in, cb_Type& out) {
    CPP2CB_CONVERT_IF_USED(in.target_active_power, out.EVSETargetActivePower);
    CPP2CB_CONVERT_IF_USED(in.target_active_power_L2, out.EVSETargetActivePower_L2);
    CPP2CB_CONVERT_IF_USED(in.target_active_power_L2, out.EVSETargetActivePower_L3);

    CPP2CB_CONVERT_IF_USED(in.target_reactive_power, out.EVSETargetReactivePower);
    CPP2CB_CONVERT_IF_USED(in.target_reactive_power_L2, out.EVSETargetReactivePower_L2);
    CPP2CB_CONVERT_IF_USED(in.target_reactive_power_L2, out.EVSETargetReactivePower_L3);

    CPP2CB_CONVERT_IF_USED(in.present_active_power, out.EVSEPresentActivePower);
    CPP2CB_CONVERT_IF_USED(in.present_active_power_L2, out.EVSEPresentActivePower_L2);
    CPP2CB_CONVERT_IF_USED(in.present_active_power_L3, out.EVSEPresentActivePower_L3);
}

template <>
void convert(const AC_ChargeLoopResponse::BPT_Scheduled_AC_CLResControlMode& in,
             struct iso20_ac_BPT_Scheduled_AC_CLResControlModeType& out) {
    convert(static_cast<const AC_ChargeLoopResponse::Scheduled_AC_CLResControlMode&>(in), out);
}

template <typename cb_Type> void convert(const Dynamic_CLResControlMode& in, cb_Type& out) {
    CPP2CB_ASSIGN_IF_USED(in.departure_time, out.DepartureTime);
    CPP2CB_ASSIGN_IF_USED(in.minimum_soc, out.MinimumSOC);
    CPP2CB_ASSIGN_IF_USED(in.target_soc, out.TargetSOC);
    CPP2CB_ASSIGN_IF_USED(in.ack_max_delay, out.AckMaxDelay);
}

template <typename cb_Type> void convert(const AC_ChargeLoopResponse::Dynamic_AC_CLResControlMode& in, cb_Type& out) {
    convert(static_cast<const Dynamic_CLResControlMode&>(in), out);

    convert(in.target_active_power, out.EVSETargetActivePower);
    CPP2CB_CONVERT_IF_USED(in.target_active_power_L2, out.EVSETargetActivePower_L2);
    CPP2CB_CONVERT_IF_USED(in.target_active_power_L3, out.EVSETargetActivePower_L3);

    CPP2CB_CONVERT_IF_USED(in.target_reactive_power, out.EVSETargetReactivePower);
    CPP2CB_CONVERT_IF_USED(in.target_reactive_power_L2, out.EVSETargetReactivePower_L2);
    CPP2CB_CONVERT_IF_USED(in.target_reactive_power_L3, out.EVSETargetReactivePower_L3);

    CPP2CB_CONVERT_IF_USED(in.present_active_power, out.EVSEPresentActivePower);
    CPP2CB_CONVERT_IF_USED(in.present_active_power_L2, out.EVSEPresentActivePower_L2);
    CPP2CB_CONVERT_IF_USED(in.present_active_power_L3, out.EVSEPresentActivePower_L3);
}

template <>
void convert(const AC_ChargeLoopResponse::BPT_Dynamic_AC_CLResControlMode& in,
             struct iso20_ac_BPT_Dynamic_AC_CLResControlModeType& out) {
    convert(static_cast<const AC_ChargeLoopResponse::Dynamic_AC_CLResControlMode&>(in), out);
}

struct ControlModeVisitor {
    using ScheduledCM = AC_ChargeLoopResponse::Scheduled_AC_CLResControlMode;
    using BPT_ScheduledCM = AC_ChargeLoopResponse::BPT_Scheduled_AC_CLResControlMode;
    using DynamicCM = AC_ChargeLoopResponse::Dynamic_AC_CLResControlMode;
    using BPT_DynamicCM = AC_ChargeLoopResponse::BPT_Dynamic_AC_CLResControlMode;

    ControlModeVisitor(iso20_ac_AC_ChargeLoopResType& res_) : res(res_){};

    void operator()(const ScheduledCM& in) {
        auto& out = res.Scheduled_AC_CLResControlMode;
        init_iso20_ac_Scheduled_AC_CLResControlModeType(&out);
        convert(in, out);
        CB_SET_USED(res.Scheduled_AC_CLResControlMode);
    }

    void operator()(const BPT_ScheduledCM& in) {
        auto& out = res.BPT_Scheduled_AC_CLResControlMode;
        init_iso20_ac_BPT_Scheduled_AC_CLResControlModeType(&out);
        convert(in, out);
        CB_SET_USED(res.BPT_Scheduled_AC_CLResControlMode);
    }

    void operator()(const DynamicCM& in) {
        auto& out = res.Dynamic_AC_CLResControlMode;
        init_iso20_ac_Dynamic_AC_CLResControlModeType(&out);
        convert(in, out);
        CB_SET_USED(res.Dynamic_AC_CLResControlMode);
    }

    void operator()(const BPT_DynamicCM& in) {
        auto& out = res.BPT_Dynamic_AC_CLResControlMode;
        init_iso20_ac_BPT_Dynamic_AC_CLResControlModeType(&out);
        convert(in, out);
        CB_SET_USED(res.BPT_Dynamic_AC_CLResControlMode);
    }

private:
    iso20_ac_AC_ChargeLoopResType& res;
};

template <> void convert(const AC_ChargeLoopResponse& in, struct iso20_ac_AC_ChargeLoopResType& out) {
    init_iso20_ac_AC_ChargeLoopResType(&out);

    convert(in.header, out.Header);
    cb_convert_enum(in.response_code, out.ResponseCode);

    CPP2CB_CONVERT_IF_USED(in.status, out.EVSEStatus);
    CPP2CB_CONVERT_IF_USED(in.meter_info, out.MeterInfo);
    CPP2CB_CONVERT_IF_USED(in.receipt, out.Receipt);

    CPP2CB_CONVERT_IF_USED(in.target_frequency, out.EVSETargetFrequency);

    std::visit(ControlModeVisitor(out), in.control_mode);
}

template <> int serialize_to_exi(const AC_ChargeLoopResponse& in, exi_bitstream_t& out) {
    iso20_ac_exiDocument doc;
    init_iso20_ac_exiDocument(&doc);

    CB_SET_USED(doc.AC_ChargeLoopRes);

    convert(in, doc.AC_ChargeLoopRes);

    return encode_iso20_ac_exiDocument(&out, &doc);
}

template <> size_t serialize(const AC_ChargeLoopResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace iso15118::message_20
