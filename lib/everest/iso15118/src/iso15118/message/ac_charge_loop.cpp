// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/ac_charge_loop.hpp>

#include <type_traits>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_20/iso20_AC_Decoder.h>
#include <cbv2g/iso_20/iso20_AC_Encoder.h>

namespace iso15118::message_20 {

// Begin conversion for deserializing an ACChargeLoopRequest (EVSEside)
template <> void convert(const struct iso20_ac_DisplayParametersType& in, datatypes::DisplayParameters& out) {

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
template <typename InType> void convert(const InType& in, datatypes::Scheduled_CLReqControlMode& out) {
    CB2CPP_CONVERT_IF_USED(in.EVTargetEnergyRequest, out.target_energy_request);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumEnergyRequest, out.max_energy_request);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumEnergyRequest, out.min_energy_request);
}

template <typename InType> void convert(const InType& in, datatypes::Scheduled_AC_CLReqControlMode& out) {
    static_assert(std::is_same_v<InType, iso20_ac_Scheduled_AC_CLReqControlModeType> or
                  std::is_same_v<InType, iso20_ac_BPT_Scheduled_AC_CLReqControlModeType>);
    convert(in, static_cast<datatypes::Scheduled_CLReqControlMode&>(out));

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
             datatypes::BPT_Scheduled_AC_CLReqControlMode& out) {
    convert(in, static_cast<datatypes::Scheduled_AC_CLReqControlMode&>(out));

    CB2CPP_CONVERT_IF_USED(in.EVMaximumDischargePower, out.max_discharge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumDischargePower_L2, out.max_discharge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumDischargePower_L3, out.max_discharge_power_L3);

    CB2CPP_CONVERT_IF_USED(in.EVMinimumDischargePower, out.min_discharge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumDischargePower_L2, out.min_discharge_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumDischargePower_L3, out.min_discharge_power_L3);
}

// Todo(sl): this should go to common.cpp
template <typename InType> void convert(const InType& in, datatypes::Dynamic_CLReqControlMode& out) {
    CB2CPP_ASSIGN_IF_USED(in.DepartureTime, out.departure_time);
    convert(in.EVTargetEnergyRequest, out.target_energy_request);
    convert(in.EVMaximumEnergyRequest, out.max_energy_request);
    convert(in.EVMinimumEnergyRequest, out.min_energy_request);
}

template <typename InType> void convert(const InType& in, datatypes::Dynamic_AC_CLReqControlMode& out) {
    static_assert(std::is_same_v<InType, iso20_ac_Dynamic_AC_CLReqControlModeType> or
                  std::is_same_v<InType, iso20_ac_BPT_Dynamic_AC_CLReqControlModeType>);
    convert(in, static_cast<datatypes::Dynamic_CLReqControlMode&>(out));

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
             datatypes::BPT_Dynamic_AC_CLReqControlMode& out) {
    convert(in, static_cast<datatypes::Dynamic_AC_CLReqControlMode&>(out));

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
        convert(in.Scheduled_AC_CLReqControlMode, out.control_mode.emplace<datatypes::Scheduled_AC_CLReqControlMode>());
    } else if (in.BPT_Scheduled_AC_CLReqControlMode_isUsed) {
        convert(in.BPT_Scheduled_AC_CLReqControlMode,
                out.control_mode.emplace<datatypes::BPT_Scheduled_AC_CLReqControlMode>());
    } else if (in.Dynamic_AC_CLReqControlMode_isUsed) {
        convert(in.Dynamic_AC_CLReqControlMode, out.control_mode.emplace<datatypes::Dynamic_AC_CLReqControlMode>());
    } else if (in.BPT_Dynamic_AC_CLReqControlMode_isUsed) {
        convert(in.BPT_Dynamic_AC_CLReqControlMode,
                out.control_mode.emplace<datatypes::BPT_Dynamic_AC_CLReqControlMode>());
    } else {
        // should not happen
        assert(false);
    }
}

template <> void insert_type(VariantAccess& va, const struct iso20_ac_AC_ChargeLoopReqType& in) {
    va.insert_type<AC_ChargeLoopRequest>(in);
}
// End conversion for deserializing an ACChargeLoopRequest (EVSEside)

// Begin conversion for deserializing an ACChargeLoopResponse (EVside)
template <> void convert(const struct iso20_ac_DetailedCostType& in, datatypes::DetailedCost& out) {
    convert(in.Amount, out.amount);
    convert(in.CostPerUnit, out.cost_per_unit);
}

template <> void convert(const struct iso20_ac_DetailedTaxType& in, datatypes::DetailedTax& out) {
    out.tax_rule_id = in.TaxRuleID;
    convert(in.Amount, out.amount);
}

template <> void convert(const struct iso20_ac_ReceiptType& in, datatypes::Receipt& out) {
    out.time_anchor = in.TimeAnchor;

    CB2CPP_CONVERT_IF_USED(in.EnergyCosts, out.energy_costs);
    CB2CPP_CONVERT_IF_USED(in.OccupancyCosts, out.occupancy_costs);
    CB2CPP_CONVERT_IF_USED(in.AdditionalServicesCosts, out.additional_service_costs);
    CB2CPP_CONVERT_IF_USED(in.OverstayCosts, out.overstay_costs);

    if (in.TaxCosts.arrayLen > 10) {
        throw std::runtime_error("tax costs array is too large");
    }
    out.tax_costs.resize(in.TaxCosts.arrayLen);
    for (std::size_t i = 0; i < in.TaxCosts.arrayLen; ++i) {
        convert(in.TaxCosts.array[i], out.tax_costs[i]);
    }
}

template <typename InType> void convert(const InType& in, datatypes::Scheduled_AC_CLResControlMode& out) {

    CB2CPP_CONVERT_IF_USED(in.EVSETargetActivePower, out.target_active_power);
    CB2CPP_CONVERT_IF_USED(in.EVSETargetActivePower_L2, out.target_active_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVSETargetActivePower_L3, out.target_active_power_L3);

    CB2CPP_CONVERT_IF_USED(in.EVSETargetReactivePower, out.target_reactive_power);
    CB2CPP_CONVERT_IF_USED(in.EVSETargetReactivePower_L2, out.target_reactive_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVSETargetReactivePower_L3, out.target_reactive_power_L3);

    CB2CPP_CONVERT_IF_USED(in.EVSEPresentActivePower, out.present_active_power);
    CB2CPP_CONVERT_IF_USED(in.EVSEPresentActivePower_L2, out.present_active_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVSEPresentActivePower_L3, out.present_active_power_L3);
}

template <>
void convert(const iso20_ac_BPT_Scheduled_AC_CLResControlModeType& in,
             struct datatypes::BPT_Scheduled_AC_CLResControlMode& out) {
    convert(in, static_cast<datatypes::Scheduled_AC_CLResControlMode&>(out));
}

template <typename cb_Type> void convert(const cb_Type& in, datatypes::Dynamic_CLResControlMode& out) {
    static_assert(std::is_same_v<cb_Type, iso20_ac_Dynamic_AC_CLResControlModeType> or
                  std::is_same_v<cb_Type, iso20_ac_BPT_Dynamic_AC_CLResControlModeType>);

    CB2CPP_ASSIGN_IF_USED(in.DepartureTime, out.departure_time);
    CB2CPP_ASSIGN_IF_USED(in.MinimumSOC, out.minimum_soc);
    CB2CPP_ASSIGN_IF_USED(in.TargetSOC, out.target_soc);
    CB2CPP_ASSIGN_IF_USED(in.AckMaxDelay, out.ack_max_delay);
}

template <typename cb_Type> void convert(const cb_Type& in, datatypes::Dynamic_AC_CLResControlMode& out) {
    convert(in, static_cast<datatypes::Dynamic_CLResControlMode&>(out));

    convert(in.EVSETargetActivePower, out.target_active_power);
    CB2CPP_CONVERT_IF_USED(in.EVSETargetActivePower_L2, out.target_active_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVSETargetActivePower_L3, out.target_active_power_L3);

    CB2CPP_CONVERT_IF_USED(in.EVSETargetReactivePower, out.target_reactive_power);
    CB2CPP_CONVERT_IF_USED(in.EVSETargetReactivePower_L2, out.target_reactive_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVSETargetReactivePower_L3, out.target_reactive_power_L3);

    CB2CPP_CONVERT_IF_USED(in.EVSEPresentActivePower, out.present_active_power);
    CB2CPP_CONVERT_IF_USED(in.EVSEPresentActivePower_L2, out.present_active_power_L2);
    CB2CPP_CONVERT_IF_USED(in.EVSEPresentActivePower_L3, out.present_active_power_L3);
}

template <>
void convert(const struct iso20_ac_BPT_Dynamic_AC_CLResControlModeType& in,
             datatypes::BPT_Dynamic_AC_CLResControlMode& out) {
    convert(in, static_cast<datatypes::Dynamic_AC_CLResControlMode&>(out));
}

template <> void convert(const struct iso20_ac_AC_ChargeLoopResType& in, AC_ChargeLoopResponse& out) {
    convert(in.Header, out.header);
    cb_convert_enum(in.ResponseCode, out.response_code);
    CB2CPP_CONVERT_IF_USED(in.MeterInfo, out.meter_info);
    CB2CPP_CONVERT_IF_USED(in.Receipt, out.receipt);
    CB2CPP_CONVERT_IF_USED(in.EVSEStatus, out.status);
    CB2CPP_CONVERT_IF_USED(in.EVSETargetFrequency, out.target_frequency);

    if (in.Scheduled_AC_CLResControlMode_isUsed) {
        convert(in.Scheduled_AC_CLResControlMode, out.control_mode.emplace<datatypes::Scheduled_AC_CLResControlMode>());
    } else if (in.BPT_Scheduled_AC_CLResControlMode_isUsed) {
        convert(in.BPT_Scheduled_AC_CLResControlMode,
                out.control_mode.emplace<datatypes::BPT_Scheduled_AC_CLResControlMode>());
    } else if (in.Dynamic_AC_CLResControlMode_isUsed) {
        convert(in.Dynamic_AC_CLResControlMode, out.control_mode.emplace<datatypes::Dynamic_AC_CLResControlMode>());
    } else if (in.BPT_Dynamic_AC_CLResControlMode_isUsed) {
        convert(in.BPT_Dynamic_AC_CLResControlMode,
                out.control_mode.emplace<datatypes::BPT_Dynamic_AC_CLResControlMode>());
    } else {
        // should not happen
        assert(false);
    }
}

template <> void insert_type(VariantAccess& va, const struct iso20_ac_AC_ChargeLoopResType& in) {
    va.insert_type<AC_ChargeLoopResponse>(in);
}

// End conversion for deserializing an ACChargeLoopResponse (EVside)

// Begin conversion for serializing an ACChargeLoopResponse (EVSEside)
template <> void convert(const datatypes::DetailedCost& in, struct iso20_ac_DetailedCostType& out) {
    init_iso20_ac_DetailedCostType(&out);
    convert(in.amount, out.Amount);
    convert(in.cost_per_unit, out.CostPerUnit);
}

template <> void convert(const datatypes::DetailedTax& in, struct iso20_ac_DetailedTaxType& out) {
    init_iso20_ac_DetailedTaxType(&out);
    out.TaxRuleID = in.tax_rule_id;
    convert(in.amount, out.Amount);
}

template <> void convert(const datatypes::Receipt& in, struct iso20_ac_ReceiptType& out) {
    init_iso20_ac_ReceiptType(&out);

    out.TimeAnchor = in.time_anchor;
    CPP2CB_CONVERT_IF_USED(in.energy_costs, out.EnergyCosts);
    CPP2CB_CONVERT_IF_USED(in.occupancy_costs, out.OccupancyCosts);
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

template <typename cb_Type> void convert(const datatypes::Scheduled_AC_CLResControlMode& in, cb_Type& out) {
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
void convert(const datatypes::BPT_Scheduled_AC_CLResControlMode& in,
             struct iso20_ac_BPT_Scheduled_AC_CLResControlModeType& out) {
    convert(static_cast<const datatypes::Scheduled_AC_CLResControlMode&>(in), out);
}

template <typename cb_Type> void convert(const datatypes::Dynamic_CLResControlMode& in, cb_Type& out) {
    CPP2CB_ASSIGN_IF_USED(in.departure_time, out.DepartureTime);
    CPP2CB_ASSIGN_IF_USED(in.minimum_soc, out.MinimumSOC);
    CPP2CB_ASSIGN_IF_USED(in.target_soc, out.TargetSOC);
    CPP2CB_ASSIGN_IF_USED(in.ack_max_delay, out.AckMaxDelay);
}

template <typename cb_Type> void convert(const datatypes::Dynamic_AC_CLResControlMode& in, cb_Type& out) {
    convert(static_cast<const datatypes::Dynamic_CLResControlMode&>(in), out);

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
void convert(const datatypes::BPT_Dynamic_AC_CLResControlMode& in,
             struct iso20_ac_BPT_Dynamic_AC_CLResControlModeType& out) {
    convert(static_cast<const datatypes::Dynamic_AC_CLResControlMode&>(in), out);
}

struct ControlModeVisitor {
    using ScheduledCM = datatypes::Scheduled_AC_CLResControlMode;
    using BPT_ScheduledCM = datatypes::BPT_Scheduled_AC_CLResControlMode;
    using DynamicCM = datatypes::Dynamic_AC_CLResControlMode;
    using BPT_DynamicCM = datatypes::BPT_Dynamic_AC_CLResControlMode;

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
// End conversion for serializing an ACChargeLoopResponse (EVSEside)

// Begin conversion for serializing an ACChargeLoopRequest (EVside)
template <> void convert(const datatypes::DisplayParameters& in, struct iso20_ac_DisplayParametersType& out) {
    init_iso20_ac_DisplayParametersType(&out);

    CPP2CB_ASSIGN_IF_USED(in.present_soc, out.PresentSOC);
    CPP2CB_ASSIGN_IF_USED(in.min_soc, out.MinimumSOC);
    CPP2CB_ASSIGN_IF_USED(in.target_soc, out.TargetSOC);
    CPP2CB_ASSIGN_IF_USED(in.max_soc, out.MaximumSOC);

    CPP2CB_ASSIGN_IF_USED(in.remaining_time_to_min_soc, out.RemainingTimeToMinimumSOC);
    CPP2CB_ASSIGN_IF_USED(in.remaining_time_to_target_soc, out.RemainingTimeToTargetSOC);
    CPP2CB_ASSIGN_IF_USED(in.remaining_time_to_max_soc, out.RemainingTimeToMaximumSOC);

    CPP2CB_ASSIGN_IF_USED(in.charging_complete, out.ChargingComplete);
    CPP2CB_CONVERT_IF_USED(in.battery_energy_capacity, out.BatteryEnergyCapacity);
    CPP2CB_ASSIGN_IF_USED(in.inlet_hot, out.InletHot);
}

struct ModeRequestVisitor {
    using ScheduledCM = datatypes::Scheduled_AC_CLReqControlMode;
    using BPT_ScheduledCM = datatypes::BPT_Scheduled_AC_CLReqControlMode;
    using DynamicCM = datatypes::Dynamic_AC_CLReqControlMode;
    using BPT_DynamicCM = datatypes::BPT_Dynamic_AC_CLReqControlMode;

public:
    ModeRequestVisitor(iso20_ac_AC_ChargeLoopReqType& req_) : req(req_){};

    void operator()(const ScheduledCM in) {
        init_iso20_ac_Scheduled_AC_CLReqControlModeType(&req.Scheduled_AC_CLReqControlMode);
        CB_SET_USED(req.Scheduled_AC_CLReqControlMode);
        convert_scheduled_common(in, req.Scheduled_AC_CLReqControlMode);
    }

    void operator()(const BPT_ScheduledCM& in) {
        init_iso20_ac_BPT_Scheduled_AC_CLReqControlModeType(&req.BPT_Scheduled_AC_CLReqControlMode);
        CB_SET_USED(req.BPT_Scheduled_AC_CLReqControlMode);
        auto& out = req.BPT_Scheduled_AC_CLReqControlMode;
        convert_scheduled_common(in, out);
        CPP2CB_CONVERT_IF_USED(in.max_discharge_power, out.EVMaximumDischargePower);
        CPP2CB_CONVERT_IF_USED(in.max_discharge_power_L2, out.EVMaximumDischargePower_L2);
        CPP2CB_CONVERT_IF_USED(in.max_discharge_power_L3, out.EVMaximumDischargePower_L3);
        CPP2CB_CONVERT_IF_USED(in.min_discharge_power, out.EVMinimumDischargePower);
        CPP2CB_CONVERT_IF_USED(in.min_discharge_power_L2, out.EVMinimumDischargePower_L2);
        CPP2CB_CONVERT_IF_USED(in.min_discharge_power_L3, out.EVMinimumDischargePower_L3);
    }

    template <typename ModeReqTypeIn, typename ModeReqTypeOut>
    static void convert_scheduled_common(const ModeReqTypeIn& in, ModeReqTypeOut& out) {
        CPP2CB_CONVERT_IF_USED(in.target_energy_request, out.EVTargetEnergyRequest);
        CPP2CB_CONVERT_IF_USED(in.max_energy_request, out.EVMaximumEnergyRequest);
        CPP2CB_CONVERT_IF_USED(in.min_energy_request, out.EVMinimumEnergyRequest);
        CPP2CB_CONVERT_IF_USED(in.max_charge_power, out.EVMaximumChargePower);
        CPP2CB_CONVERT_IF_USED(in.max_charge_power_L2, out.EVMaximumChargePower_L2);
        CPP2CB_CONVERT_IF_USED(in.max_charge_power_L3, out.EVMaximumChargePower_L3);
        CPP2CB_CONVERT_IF_USED(in.min_charge_power, out.EVMinimumChargePower);
        CPP2CB_CONVERT_IF_USED(in.min_charge_power_L2, out.EVMinimumChargePower_L2);
        CPP2CB_CONVERT_IF_USED(in.min_charge_power_L3, out.EVMinimumChargePower_L3);
        convert(in.present_active_power, out.EVPresentActivePower);
        CPP2CB_CONVERT_IF_USED(in.present_active_power_L2, out.EVPresentActivePower_L2);
        CPP2CB_CONVERT_IF_USED(in.present_active_power_L3, out.EVPresentActivePower_L3);
        CPP2CB_CONVERT_IF_USED(in.present_reactive_power, out.EVPresentReactivePower);
        CPP2CB_CONVERT_IF_USED(in.present_reactive_power_L2, out.EVPresentReactivePower_L2);
        CPP2CB_CONVERT_IF_USED(in.present_reactive_power_L3, out.EVPresentReactivePower_L3);
    }

    void operator()(const DynamicCM& in) {
        init_iso20_ac_Dynamic_AC_CLReqControlModeType(&req.Dynamic_AC_CLReqControlMode);
        CB_SET_USED(req.Dynamic_AC_CLReqControlMode);
        convert_dynamic_common(in, req.Dynamic_AC_CLReqControlMode);
    }
    void operator()(const BPT_DynamicCM& in) {
        init_iso20_ac_BPT_Dynamic_AC_CLReqControlModeType(&req.BPT_Dynamic_AC_CLReqControlMode);
        CB_SET_USED(req.BPT_Dynamic_AC_CLReqControlMode);
        auto& out = req.BPT_Dynamic_AC_CLReqControlMode;
        convert_dynamic_common(in, out);
        convert(in.max_discharge_power, out.EVMaximumDischargePower);
        CPP2CB_CONVERT_IF_USED(in.max_discharge_power_L2, out.EVMaximumDischargePower_L2);
        CPP2CB_CONVERT_IF_USED(in.max_discharge_power_L3, out.EVMaximumDischargePower_L3);
        convert(in.min_discharge_power, out.EVMinimumDischargePower);
        CPP2CB_CONVERT_IF_USED(in.min_discharge_power_L2, out.EVMinimumDischargePower_L2);
        CPP2CB_CONVERT_IF_USED(in.min_discharge_power_L3, out.EVMinimumDischargePower_L3);
        CPP2CB_CONVERT_IF_USED(in.max_v2x_energy_request, out.EVMaximumV2XEnergyRequest);
        CPP2CB_CONVERT_IF_USED(in.min_v2x_energy_request, out.EVMinimumV2XEnergyRequest);
    }
    template <typename ModeReqTypeIn, typename ModeReqTypeOut>
    static void convert_dynamic_common(const ModeReqTypeIn& in, ModeReqTypeOut& out) {
        CPP2CB_ASSIGN_IF_USED(in.departure_time, out.DepartureTime);
        convert(in.target_energy_request, out.EVTargetEnergyRequest);
        convert(in.max_energy_request, out.EVMaximumEnergyRequest);
        convert(in.min_energy_request, out.EVMinimumEnergyRequest);
        convert(in.max_charge_power, out.EVMaximumChargePower);
        CPP2CB_CONVERT_IF_USED(in.max_charge_power_L2, out.EVMaximumChargePower_L2);
        CPP2CB_CONVERT_IF_USED(in.max_charge_power_L3, out.EVMaximumChargePower_L3);
        convert(in.min_charge_power, out.EVMinimumChargePower);
        CPP2CB_CONVERT_IF_USED(in.min_charge_power_L2, out.EVMinimumChargePower_L2);
        CPP2CB_CONVERT_IF_USED(in.min_charge_power_L3, out.EVMinimumChargePower_L3);
        convert(in.present_active_power, out.EVPresentActivePower);
        CPP2CB_CONVERT_IF_USED(in.present_active_power_L2, out.EVPresentActivePower_L2);
        CPP2CB_CONVERT_IF_USED(in.present_active_power_L3, out.EVPresentActivePower_L3);
        convert(in.present_reactive_power, out.EVPresentReactivePower);
        CPP2CB_CONVERT_IF_USED(in.present_reactive_power_L2, out.EVPresentReactivePower_L2);
        CPP2CB_CONVERT_IF_USED(in.present_reactive_power_L3, out.EVPresentReactivePower_L3);
    }

private:
    iso20_ac_AC_ChargeLoopReqType& req;
};

template <> void convert(const AC_ChargeLoopRequest& in, struct iso20_ac_AC_ChargeLoopReqType& out) {
    init_iso20_ac_AC_ChargeLoopReqType(&out);

    convert(in.header, out.Header);
    CPP2CB_CONVERT_IF_USED(in.display_parameters, out.DisplayParameters);
    out.MeterInfoRequested = in.meter_info_requested;

    std::visit(ModeRequestVisitor(out), in.control_mode);
}

template <> int serialize_to_exi(const AC_ChargeLoopRequest& in, exi_bitstream_t& out) {
    iso20_ac_exiDocument doc;
    init_iso20_ac_exiDocument(&doc);

    CB_SET_USED(doc.AC_ChargeLoopReq);

    convert(in, doc.AC_ChargeLoopReq);

    return encode_iso20_ac_exiDocument(&out, &doc);
}

template <> size_t serialize(const AC_ChargeLoopRequest& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}
// End conversion for serializing an ACChargeLoopRequest (EVside)

} // namespace iso15118::message_20
