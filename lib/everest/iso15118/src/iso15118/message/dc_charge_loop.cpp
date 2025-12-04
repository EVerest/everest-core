// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/dc_charge_loop.hpp>

#include <type_traits>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_20/iso20_CommonMessages_Decoder.h>
#include <cbv2g/iso_20/iso20_CommonMessages_Encoder.h>

#include <cbv2g/iso_20/iso20_DC_Decoder.h>
#include <cbv2g/iso_20/iso20_DC_Encoder.h>

namespace iso15118::message_20 {

// Begin DC_ChargeLoopRequest Deserialization (EVSEside)
template <> void convert(const struct iso20_dc_DisplayParametersType& in, datatypes::DisplayParameters& out) {
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
template <typename InType> void convert(const InType& in, datatypes::Scheduled_CLReqControlMode& out) {
    CB2CPP_CONVERT_IF_USED(in.EVTargetEnergyRequest, out.target_energy_request);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumEnergyRequest, out.max_energy_request);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumEnergyRequest, out.min_energy_request);
}

template <typename InType> void convert(const InType& in, datatypes::Scheduled_DC_CLReqControlMode& out) {
    static_assert(std::is_same_v<InType, iso20_dc_Scheduled_DC_CLReqControlModeType> or
                  std::is_same_v<InType, iso20_dc_BPT_Scheduled_DC_CLReqControlModeType>);
    convert(in, static_cast<datatypes::Scheduled_CLReqControlMode&>(out));

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
             datatypes::BPT_Scheduled_DC_CLReqControlMode& out) {
    convert(in, static_cast<datatypes::Scheduled_DC_CLReqControlMode&>(out));

    CB2CPP_CONVERT_IF_USED(in.EVMaximumDischargePower, out.max_discharge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumDischargePower, out.min_discharge_power);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumDischargeCurrent, out.max_discharge_current);
}

// FIXME (aw): this should go to common.cpp
template <typename InType> void convert(const InType& in, datatypes::Dynamic_CLReqControlMode& out) {
    CB2CPP_ASSIGN_IF_USED(in.DepartureTime, out.departure_time);
    convert(in.EVTargetEnergyRequest, out.target_energy_request);
    convert(in.EVMaximumEnergyRequest, out.max_energy_request);
    convert(in.EVMinimumEnergyRequest, out.min_energy_request);
}

template <typename InType> void convert(const InType& in, datatypes::Dynamic_DC_CLReqControlMode& out) {
    static_assert(std::is_same_v<InType, iso20_dc_Dynamic_DC_CLReqControlModeType> or
                  std::is_same_v<InType, iso20_dc_BPT_Dynamic_DC_CLReqControlModeType>);
    convert(in, static_cast<datatypes::Dynamic_CLReqControlMode&>(out));

    convert(in.EVMaximumChargePower, out.max_charge_power);
    convert(in.EVMinimumChargePower, out.min_charge_power);
    convert(in.EVMaximumChargeCurrent, out.max_charge_current);
    convert(in.EVMaximumVoltage, out.max_voltage);
    convert(in.EVMinimumVoltage, out.min_voltage);
}

template <>
void convert(const struct iso20_dc_BPT_Dynamic_DC_CLReqControlModeType& in,
             datatypes::BPT_Dynamic_DC_CLReqControlMode& out) {
    convert(in, static_cast<datatypes::Dynamic_DC_CLReqControlMode&>(out));

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
        convert(in.Scheduled_DC_CLReqControlMode, out.control_mode.emplace<datatypes::Scheduled_DC_CLReqControlMode>());
    } else if (in.BPT_Scheduled_DC_CLReqControlMode_isUsed) {
        convert(in.BPT_Scheduled_DC_CLReqControlMode,
                out.control_mode.emplace<datatypes::BPT_Scheduled_DC_CLReqControlMode>());
    } else if (in.Dynamic_DC_CLReqControlMode_isUsed) {
        convert(in.Dynamic_DC_CLReqControlMode, out.control_mode.emplace<datatypes::Dynamic_DC_CLReqControlMode>());
    } else if (in.BPT_Dynamic_DC_CLReqControlMode_isUsed) {
        convert(in.BPT_Dynamic_DC_CLReqControlMode,
                out.control_mode.emplace<datatypes::BPT_Dynamic_DC_CLReqControlMode>());
    } else {
        // should not happen
        assert(false);
    }
}
// End DC_ChargeLoopRequest Deserialization (EVSEside)

// Begin DC_ChargeLoopResponse Deserialization (EVside)
template <typename InType> void convert(const InType& in, datatypes::Scheduled_DC_CLResControlMode& out) {

    CB2CPP_CONVERT_IF_USED(in.EVSEMaximumChargePower, out.max_charge_power);
    CB2CPP_CONVERT_IF_USED(in.EVSEMinimumChargePower, out.min_charge_power);
    CB2CPP_CONVERT_IF_USED(in.EVSEMaximumChargeCurrent, out.max_charge_current);
    CB2CPP_CONVERT_IF_USED(in.EVSEMaximumVoltage, out.max_voltage);
}

template <>
void convert(const struct iso20_dc_BPT_Scheduled_DC_CLResControlModeType& in,
             datatypes::BPT_Scheduled_DC_CLResControlMode& out) {
    convert(in, static_cast<datatypes::Scheduled_DC_CLResControlMode&>(out));
    CB2CPP_CONVERT_IF_USED(in.EVSEMaximumDischargePower, out.max_discharge_power);
    CB2CPP_CONVERT_IF_USED(in.EVSEMinimumDischargePower, out.min_discharge_power);
    CB2CPP_CONVERT_IF_USED(in.EVSEMaximumDischargeCurrent, out.max_discharge_current);
    CB2CPP_CONVERT_IF_USED(in.EVSEMinimumVoltage, out.min_voltage);
}

// FIXME (rb): this should go to common.cpp
template <typename InType> void convert(const InType& in, datatypes::Dynamic_CLResControlMode& out) {
    CB2CPP_ASSIGN_IF_USED(in.DepartureTime, out.departure_time);
    CB2CPP_ASSIGN_IF_USED(in.MinimumSOC, out.minimum_soc);
    CB2CPP_ASSIGN_IF_USED(in.TargetSOC, out.target_soc);
    CB2CPP_ASSIGN_IF_USED(in.AckMaxDelay, out.ack_max_delay);
}

template <typename InType> void convert(const InType& in, datatypes::Dynamic_DC_CLResControlMode& out) {
    static_assert(std::is_same_v<InType, iso20_dc_Dynamic_DC_CLResControlModeType> or
                  std::is_same_v<InType, iso20_dc_BPT_Dynamic_DC_CLResControlModeType>);
    convert(in, static_cast<datatypes::Dynamic_CLResControlMode&>(out));
    convert(in.EVSEMaximumChargePower, out.max_charge_power);
    convert(in.EVSEMinimumChargePower, out.min_charge_power);
    convert(in.EVSEMaximumChargeCurrent, out.max_charge_current);
    convert(in.EVSEMaximumVoltage, out.max_voltage);
}

template <>
void convert(const struct iso20_dc_BPT_Dynamic_DC_CLResControlModeType& in,
             datatypes::BPT_Dynamic_DC_CLResControlMode& out) {
    convert(in, static_cast<datatypes::Dynamic_DC_CLResControlMode&>(out));
    convert(in.EVSEMaximumDischargePower, out.max_discharge_power);
    convert(in.EVSEMinimumDischargePower, out.min_discharge_power);
    convert(in.EVSEMaximumDischargeCurrent, out.max_discharge_current);
    convert(in.EVSEMinimumVoltage, out.min_voltage);
}

template <> void convert(const struct iso20_dc_DetailedCostType& in, datatypes::DetailedCost& out) {
    convert(in.Amount, out.amount);
    convert(in.CostPerUnit, out.cost_per_unit);
}

template <> void convert(const struct iso20_dc_DetailedTaxType& in, datatypes::DetailedTax& out) {
    out.tax_rule_id = in.TaxRuleID;
    convert(in.Amount, out.amount);
}

template <> void convert(const struct iso20_dc_ReceiptType& in, datatypes::Receipt& out) {
    out.time_anchor = in.TimeAnchor;
    CB2CPP_CONVERT_IF_USED(in.EnergyCosts, out.energy_costs);
    CB2CPP_CONVERT_IF_USED(in.OccupancyCosts, out.occupancy_costs);
    CB2CPP_CONVERT_IF_USED(in.AdditionalServicesCosts, out.additional_service_costs);
    CB2CPP_CONVERT_IF_USED(in.OverstayCosts, out.overstay_costs);

    // todo (rb): TaxCosts should be optional as far as I can tell.
    if (in.TaxCosts.arrayLen > 10) {
        throw std::runtime_error("tax costs array is too large");
    }
    out.tax_costs.resize(in.TaxCosts.arrayLen);
    for (std::size_t i = 0; i < in.TaxCosts.arrayLen; ++i) {
        convert(in.TaxCosts.array[i], out.tax_costs[i]);
    }
}

template <> void convert(const struct iso20_dc_DC_ChargeLoopResType& in, DC_ChargeLoopResponse& out) {
    cb_convert_enum(in.ResponseCode, out.response_code);
    convert(in.EVSEPresentVoltage, out.present_voltage);
    convert(in.EVSEPresentCurrent, out.present_current);
    out.current_limit_achieved = in.EVSECurrentLimitAchieved;
    out.power_limit_achieved = in.EVSEPowerLimitAchieved;
    out.voltage_limit_achieved = in.EVSEVoltageLimitAchieved;
    CB2CPP_CONVERT_IF_USED(in.EVSEStatus, out.status);
    CB2CPP_CONVERT_IF_USED(in.MeterInfo, out.meter_info);
    CB2CPP_CONVERT_IF_USED(in.Receipt, out.receipt);
    convert(in.Header, out.header);
    if (in.Scheduled_DC_CLResControlMode_isUsed) {
        convert(in.Scheduled_DC_CLResControlMode, out.control_mode.emplace<datatypes::Scheduled_DC_CLResControlMode>());
    } else if (in.BPT_Scheduled_DC_CLResControlMode_isUsed) {
        convert(in.BPT_Scheduled_DC_CLResControlMode,
                out.control_mode.emplace<datatypes::BPT_Scheduled_DC_CLResControlMode>());
    } else if (in.Dynamic_DC_CLResControlMode_isUsed) {
        convert(in.Dynamic_DC_CLResControlMode, out.control_mode.emplace<datatypes::Dynamic_DC_CLResControlMode>());
    } else if (in.BPT_Dynamic_DC_CLResControlMode_isUsed) {
        convert(in.BPT_Dynamic_DC_CLResControlMode,
                out.control_mode.emplace<datatypes::BPT_Dynamic_DC_CLResControlMode>());
    } else {
        // should not happen
        assert(false);
    }
}
// End DC_ChargeLoopResponse Deserialization (EVside)

// Begin DC_ChargeLoopResponse Serialization (EVSEside)
template <> void insert_type(VariantAccess& va, const struct iso20_dc_DC_ChargeLoopResType& in) {
    va.insert_type<DC_ChargeLoopResponse>(in);
};

template <> void convert(const datatypes::DetailedCost& in, struct iso20_dc_DetailedCostType& out) {
    init_iso20_dc_DetailedCostType(&out);
    convert(in.amount, out.Amount);
    convert(in.cost_per_unit, out.CostPerUnit);
}

template <> void convert(const datatypes::DetailedTax& in, struct iso20_dc_DetailedTaxType& out) {
    init_iso20_dc_DetailedTaxType(&out);
    out.TaxRuleID = in.tax_rule_id;
    convert(in.amount, out.Amount);
}

template <> void convert(const datatypes::Receipt& in, struct iso20_dc_ReceiptType& out) {
    init_iso20_dc_ReceiptType(&out);

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

template <typename OutType> void convert(const datatypes::Scheduled_DC_CLResControlMode& in, OutType& out) {
    CPP2CB_CONVERT_IF_USED(in.max_charge_power, out.EVSEMaximumChargePower);
    CPP2CB_CONVERT_IF_USED(in.min_charge_power, out.EVSEMinimumChargePower);
    CPP2CB_CONVERT_IF_USED(in.max_charge_current, out.EVSEMaximumChargeCurrent);
    CPP2CB_CONVERT_IF_USED(in.max_voltage, out.EVSEMaximumVoltage);
}

template <typename OutType> void convert(const datatypes::Dynamic_CLResControlMode& in, OutType& out) {
    CPP2CB_ASSIGN_IF_USED(in.departure_time, out.DepartureTime);
    CPP2CB_ASSIGN_IF_USED(in.minimum_soc, out.MinimumSOC);
    CPP2CB_ASSIGN_IF_USED(in.target_soc, out.TargetSOC);
    CPP2CB_ASSIGN_IF_USED(in.ack_max_delay, out.AckMaxDelay);
}

template <>
void convert(const datatypes::BPT_Scheduled_DC_CLResControlMode& in,
             struct iso20_dc_BPT_Scheduled_DC_CLResControlModeType& out) {
    convert(static_cast<const datatypes::Scheduled_DC_CLResControlMode&>(in), out);

    CPP2CB_CONVERT_IF_USED(in.max_discharge_power, out.EVSEMaximumDischargePower);
    CPP2CB_CONVERT_IF_USED(in.min_discharge_power, out.EVSEMinimumDischargePower);
    CPP2CB_CONVERT_IF_USED(in.max_discharge_current, out.EVSEMaximumDischargePower);
    CPP2CB_CONVERT_IF_USED(in.min_voltage, out.EVSEMinimumVoltage);
}

template <typename OutType> void convert(const datatypes::Dynamic_DC_CLResControlMode& in, OutType& out) {
    convert(static_cast<const datatypes::Dynamic_CLResControlMode&>(in), out);

    convert(in.max_charge_power, out.EVSEMaximumChargePower);
    convert(in.min_charge_power, out.EVSEMinimumChargePower);
    convert(in.max_charge_current, out.EVSEMaximumChargeCurrent);
    convert(in.max_voltage, out.EVSEMaximumVoltage);
}

template <>
void convert(const datatypes::BPT_Dynamic_DC_CLResControlMode& in,
             struct iso20_dc_BPT_Dynamic_DC_CLResControlModeType& out) {
    convert(static_cast<const datatypes::Dynamic_DC_CLResControlMode&>(in), out);

    convert(in.max_discharge_power, out.EVSEMaximumDischargePower);
    convert(in.min_discharge_power, out.EVSEMinimumDischargePower);
    convert(in.max_discharge_current, out.EVSEMaximumDischargeCurrent);
    convert(in.min_voltage, out.EVSEMinimumVoltage);
}

struct ControlModeVisitor {
    using ScheduledCM = datatypes::Scheduled_DC_CLResControlMode;
    using BPT_ScheduledCM = datatypes::BPT_Scheduled_DC_CLResControlMode;
    using DynamicCM = datatypes::Dynamic_DC_CLResControlMode;
    using BPT_DynamicCM = datatypes::BPT_Dynamic_DC_CLResControlMode;

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
// End DC_ChargeLoopResponse Serialization (EVSEside)

// Begin DC_ChargeLoopRequest Serialization (EVside)
template <> void insert_type(VariantAccess& va, const struct iso20_dc_DC_ChargeLoopReqType& in) {
    va.insert_type<DC_ChargeLoopRequest>(in);
}

template <> void convert(const datatypes::DisplayParameters& in, struct iso20_dc_DisplayParametersType& out) {
    init_iso20_dc_DisplayParametersType(&out);
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

template <typename OutType> void convert(const datatypes::Scheduled_CLReqControlMode& in, OutType& out) {
    CPP2CB_CONVERT_IF_USED(in.target_energy_request, out.EVTargetEnergyRequest);
    CPP2CB_CONVERT_IF_USED(in.max_energy_request, out.EVMaximumEnergyRequest);
    CPP2CB_CONVERT_IF_USED(in.min_energy_request, out.EVMinimumEnergyRequest);
}

template <typename OutType> void convert(const datatypes::Scheduled_DC_CLReqControlMode& in, OutType& out) {
    static_assert(std::is_same_v<OutType, iso20_dc_Scheduled_DC_CLReqControlModeType> or
                  std::is_same_v<OutType, iso20_dc_BPT_Scheduled_DC_CLReqControlModeType>);
    convert(static_cast<const datatypes::Scheduled_CLReqControlMode&>(in), out);
    CPP2CB_CONVERT_IF_USED(in.max_charge_current, out.EVMaximumChargeCurrent);
    CPP2CB_CONVERT_IF_USED(in.max_charge_power, out.EVMaximumChargePower);
    CPP2CB_CONVERT_IF_USED(in.min_charge_power, out.EVMinimumChargePower);
    convert(in.target_current, out.EVTargetCurrent);
    convert(in.target_voltage, out.EVTargetVoltage);
    CPP2CB_CONVERT_IF_USED(in.max_voltage, out.EVMaximumVoltage);
    CPP2CB_CONVERT_IF_USED(in.min_voltage, out.EVMinimumVoltage);
}

template <>
void convert(const datatypes::BPT_Scheduled_DC_CLReqControlMode& in,
             struct iso20_dc_BPT_Scheduled_DC_CLReqControlModeType& out) {
    convert(static_cast<const datatypes::Scheduled_DC_CLReqControlMode&>(in), out);
    CPP2CB_CONVERT_IF_USED(in.max_discharge_power, out.EVMaximumDischargePower);
    CPP2CB_CONVERT_IF_USED(in.min_discharge_power, out.EVMinimumDischargePower);
    CPP2CB_CONVERT_IF_USED(in.max_discharge_current, out.EVMaximumDischargeCurrent);
}

template <typename OutType> void convert(const datatypes::Dynamic_CLReqControlMode& in, OutType& out) {
    convert(in.target_energy_request, out.EVTargetEnergyRequest);
    convert(in.max_energy_request, out.EVMaximumEnergyRequest);
    convert(in.min_energy_request, out.EVMinimumEnergyRequest);
    CPP2CB_ASSIGN_IF_USED(in.departure_time, out.DepartureTime);
}

template <typename OutType> void convert(const datatypes::Dynamic_DC_CLReqControlMode& in, OutType& out) {
    static_assert(std::is_same_v<OutType, iso20_dc_Dynamic_DC_CLReqControlModeType> or
                  std::is_same_v<OutType, iso20_dc_BPT_Dynamic_DC_CLReqControlModeType>);
    convert(static_cast<const datatypes::Dynamic_CLReqControlMode&>(in), out);
    convert(in.max_charge_power, out.EVMaximumChargePower);
    convert(in.min_charge_power, out.EVMinimumChargePower);
    convert(in.max_charge_current, out.EVMaximumChargeCurrent);
    convert(in.max_voltage, out.EVMaximumVoltage);
    convert(in.min_voltage, out.EVMinimumVoltage);
}

template <>
void convert(const datatypes::BPT_Dynamic_DC_CLReqControlMode& in,
             struct iso20_dc_BPT_Dynamic_DC_CLReqControlModeType& out) {
    convert(static_cast<const datatypes::Dynamic_DC_CLReqControlMode&>(in), out);
    convert(in.max_discharge_power, out.EVMaximumDischargePower);
    convert(in.min_discharge_power, out.EVMinimumDischargePower);
    convert(in.max_discharge_current, out.EVMaximumDischargeCurrent);
    CPP2CB_CONVERT_IF_USED(in.max_v2x_energy_request, out.EVMaximumV2XEnergyRequest);
    CPP2CB_CONVERT_IF_USED(in.min_v2x_energy_request, out.EVMinimumV2XEnergyRequest);
}

struct RequestControlModeVisitor {
    using ScheduledCM = datatypes::Scheduled_DC_CLReqControlMode;
    using BPT_ScheduledCM = datatypes::BPT_Scheduled_DC_CLReqControlMode;
    using DynamicCM = datatypes::Dynamic_DC_CLReqControlMode;
    using BPT_DynamicCM = datatypes::BPT_Dynamic_DC_CLReqControlMode;

    RequestControlModeVisitor(iso20_dc_DC_ChargeLoopReqType& req_) : req(req_){};
    void operator()(const ScheduledCM& in) {
        auto& out = req.Scheduled_DC_CLReqControlMode;
        init_iso20_dc_Scheduled_DC_CLReqControlModeType(&out);
        convert(in, out);
        CB_SET_USED(req.Scheduled_DC_CLReqControlMode);
    }
    void operator()(const BPT_ScheduledCM& in) {
        auto& out = req.BPT_Scheduled_DC_CLReqControlMode;
        init_iso20_dc_BPT_Scheduled_DC_CLReqControlModeType(&out);
        convert(in, out);
        CB_SET_USED(req.BPT_Scheduled_DC_CLReqControlMode);
    }
    void operator()(const DynamicCM& in) {
        auto& out = req.Dynamic_DC_CLReqControlMode;
        init_iso20_dc_Dynamic_DC_CLReqControlModeType(&out);
        convert(in, out);
        CB_SET_USED(req.Dynamic_DC_CLReqControlMode);
    }
    void operator()(const BPT_DynamicCM& in) {
        auto& out = req.BPT_Dynamic_DC_CLReqControlMode;
        init_iso20_dc_BPT_Dynamic_DC_CLReqControlModeType(&out);
        convert(in, out);
        CB_SET_USED(req.BPT_Dynamic_DC_CLReqControlMode);
    }

private:
    iso20_dc_DC_ChargeLoopReqType& req;
};

template <> void convert(const DC_ChargeLoopRequest& in, iso20_dc_DC_ChargeLoopReqType& out) {
    init_iso20_dc_DC_ChargeLoopReqType(&out);

    out.MeterInfoRequested = in.meter_info_requested;
    convert(in.present_voltage, out.EVPresentVoltage);
    CPP2CB_CONVERT_IF_USED(in.display_parameters, out.DisplayParameters);
    std::visit(RequestControlModeVisitor{out}, in.control_mode);
    convert(in.header, out.Header);
}

template <> int serialize_to_exi(const DC_ChargeLoopRequest& in, exi_bitstream_t& out) {
    iso20_dc_exiDocument doc;
    init_iso20_dc_exiDocument(&doc);

    CB_SET_USED(doc.DC_ChargeLoopReq);

    convert(in, doc.DC_ChargeLoopReq);

    return encode_iso20_dc_exiDocument(&out, &doc);
}

template <> size_t serialize(const DC_ChargeLoopRequest& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

// End DC_ChargeLoopRequest Serialization (EVside)

} // namespace iso15118::message_20
