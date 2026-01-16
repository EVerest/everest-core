// SPDX-License-Identifier: Apache-2.0
// Copyright 2026 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/d2/charge_parameter_discovery.hpp>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_2/iso2_msgDefDecoder.h>
#include <cbv2g/iso_2/iso2_msgDefEncoder.h>

#include <iso15118/detail/helper.hpp>

namespace iso15118::d2::msg {

template <> void convert(const struct iso2_AC_EVChargeParameterType& in, data_types::AcEvChargeParameter& out) {
    CB2CPP_ASSIGN_IF_USED(in.DepartureTime, out.departure_time);
    convert(in.EAmount, out.e_amount);
    convert(in.EVMaxVoltage, out.ev_max_voltage);
    convert(in.EVMaxCurrent, out.ev_max_current);
    convert(in.EVMinCurrent, out.ev_min_current);
}

template <> void convert(const struct iso2_DC_EVChargeParameterType& in, data_types::DcEvChargeParameter& out) {
    CB2CPP_ASSIGN_IF_USED(in.DepartureTime, out.departure_time);
    convert(in.DC_EVStatus, out.dc_ev_status);
    convert(in.EVMaximumCurrentLimit, out.ev_maximum_current_limit);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumPowerLimit, out.ev_maximum_power_limit);
    convert(in.EVMaximumVoltageLimit, out.ev_maximum_voltage_limit);
    CB2CPP_CONVERT_IF_USED(in.EVEnergyCapacity, out.ev_energy_capacity);
    CB2CPP_CONVERT_IF_USED(in.EVEnergyRequest, out.ev_energy_request);
    CB2CPP_ASSIGN_IF_USED(in.FullSOC, out.full_soc);
    CB2CPP_ASSIGN_IF_USED(in.BulkSOC, out.bulk_soc);
}

template <> void convert(const data_types::AcEvseChargeParameter& in, struct iso2_AC_EVSEChargeParameterType& out) {
    init_iso2_AC_EVSEChargeParameterType(&out);
    convert(in.ac_evse_status, out.AC_EVSEStatus);
    convert(in.evse_nominal_voltage, out.EVSENominalVoltage);
    convert(in.evse_max_current, out.EVSEMaxCurrent);
}

template <> void convert(const data_types::DcEvseChargeParameter& in, struct iso2_DC_EVSEChargeParameterType& out) {
    init_iso2_DC_EVSEChargeParameterType(&out);
    convert(in.dc_evse_status, out.DC_EVSEStatus);
    convert(in.evse_maximum_current_limit, out.EVSEMaximumCurrentLimit);
    convert(in.evse_maximum_power_limit, out.EVSEMaximumPowerLimit);
    convert(in.evse_maximum_voltage_limit, out.EVSEMaximumVoltageLimit);
    convert(in.evse_minimum_current_limit, out.EVSEMinimumCurrentLimit);
    convert(in.evse_minimum_voltage_limit, out.EVSEMinimumVoltageLimit);
    CPP2CB_CONVERT_IF_USED(in.evse_current_regulation_tolerance, out.EVSECurrentRegulationTolerance);
    convert(in.evse_peak_current_ripple, out.EVSEPeakCurrentRipple);
    CPP2CB_CONVERT_IF_USED(in.evse_energy_to_be_delivered, out.EVSEEnergyToBeDelivered);
}

template <> void convert(const data_types::RelativeTimeInterval& in, struct iso2_RelativeTimeIntervalType& out) {
    init_iso2_RelativeTimeIntervalType(&out);
    out.start = in.start;
    CPP2CB_ASSIGN_IF_USED(in.duration, out.duration);
}

template <> void convert(const data_types::ConsumptionCost& in, struct iso2_ConsumptionCostType& out) {
    init_iso2_ConsumptionCostType(&out);
    convert(in.start_value, out.startValue);

    int idx = 0;
    for (const auto& entry : in.cost) {
        if (idx >= iso2_CostType_3_ARRAY_SIZE) {
            // TODO(kd): report error?
            break;
        }
        auto& entry_out = out.Cost.array[idx++];
        init_iso2_CostType(&entry_out);
        cb_convert_enum(entry.cost_kind, entry_out.costKind);
        entry_out.amount = entry.amount;
        CPP2CB_ASSIGN_IF_USED(entry.amount_multiplier, entry_out.amountMultiplier);
    }
    out.Cost.arrayLen = in.cost.size();
}

template <> void convert(const data_types::SalesTariffEntry& in, struct iso2_SalesTariffEntryType& out) {
    init_iso2_SalesTariffEntryType(&out);
    convert(in.time_interval, out.RelativeTimeInterval);
    out.RelativeTimeInterval_isUsed = true;
    CPP2CB_ASSIGN_IF_USED(in.e_price_level, out.EPriceLevel);

    int cost_idx = 0;
    for (const auto& cost : in.consumption_cost) {
        if (cost_idx >= iso2_ConsumptionCostType_3_ARRAY_SIZE) {
            // TODO(kd): report error?
            break;
        }
        auto& cost_out = out.ConsumptionCost.array[cost_idx++];
        convert(cost, cost_out);
    }
    out.ConsumptionCost.arrayLen = in.consumption_cost.size();
}

template <> void convert(const data_types::SaScheduleTuple& in, struct iso2_SAScheduleTupleType& out) {
    init_iso2_SAScheduleTupleType(&out);
    out.SAScheduleTupleID = in.sa_schedule_tuple_id;

    int entry_idx = 0;
    for (const auto& entry : in.pmax_schedule) {
        if (entry_idx >= iso2_PMaxScheduleEntryType_12_ARRAY_SIZE) {
            // TODO(kd): report error?
            break;
        }
        auto& entry_out = out.PMaxSchedule.PMaxScheduleEntry.array[entry_idx++];
        init_iso2_PMaxScheduleEntryType(&entry_out);
        convert(entry.time_interval, entry_out.RelativeTimeInterval);
        entry_out.RelativeTimeInterval_isUsed = true;
        convert(entry.p_max, entry_out.PMax);
    }
    out.PMaxSchedule.PMaxScheduleEntry.arrayLen = in.pmax_schedule.size();

    if (in.sales_tariff.has_value()) {
        init_iso2_SalesTariffType(&out.SalesTariff);
        out.SalesTariff_isUsed = true;
        CPP2CB_STRING_IF_USED(in.sales_tariff->id, out.SalesTariff.Id);
        out.SalesTariff.SalesTariffID = in.sales_tariff->sales_tariff_id;
        CPP2CB_STRING_IF_USED(in.sales_tariff->sales_tariff_description, out.SalesTariff.SalesTariffDescription);
        CPP2CB_ASSIGN_IF_USED(in.sales_tariff->num_e_price_levels, out.SalesTariff.NumEPriceLevels);

        entry_idx = 0;
        for (const auto& entry : in.sales_tariff->sales_tariff_entry) {
            if (entry_idx >= iso2_SalesTariffEntryType_12_ARRAY_SIZE) {
                // TODO(kd): report error?
                break;
            }
            auto& entry_out = out.SalesTariff.SalesTariffEntry.array[entry_idx++];
            convert(entry, entry_out);
        }
        out.SalesTariff.SalesTariffEntry.arrayLen = in.sales_tariff->sales_tariff_entry.size();
    }
}

template <> void convert(const struct iso2_ChargeParameterDiscoveryReqType& in, ChargeParameterDiscoveryRequest& out) {
    CB2CPP_ASSIGN_IF_USED(in.MaxEntriesSAScheduleTuple, out.max_entries_sa_schedule_tuple);
    cb_convert_enum(in.RequestedEnergyTransferMode, out.requested_energy_transfer_mode);
    CB2CPP_CONVERT_IF_USED(in.AC_EVChargeParameter, out.ac_ev_charge_parameter);
    CB2CPP_CONVERT_IF_USED(in.DC_EVChargeParameter, out.dc_ev_charge_parameter);
}

template <>
void insert_type(VariantAccess& va, const struct iso2_ChargeParameterDiscoveryReqType& in,
                 const struct iso2_MessageHeaderType& header) {
    va.insert_type<ChargeParameterDiscoveryRequest>(in, header);
}

template <> void convert(const ChargeParameterDiscoveryResponse& in, struct iso2_ChargeParameterDiscoveryResType& out) {
    init_iso2_ChargeParameterDiscoveryResType(&out);

    cb_convert_enum(in.response_code, out.ResponseCode);
    cb_convert_enum(in.evse_processing, out.EVSEProcessing);

    if (in.sa_schedule_list.has_value()) {
        int tuple_idx = 0;
        for (auto& schedule : in.sa_schedule_list.value()) {
            if (tuple_idx >= iso2_SAScheduleTupleType_3_ARRAY_SIZE) {
                // TODO(kd): report error?
                break;
            }
            auto& schedule_out = out.SAScheduleList.SAScheduleTuple.array[tuple_idx++];
            convert(schedule, schedule_out);
        }
        out.SAScheduleList.SAScheduleTuple.arrayLen = in.sa_schedule_list->size();
        out.SAScheduleList_isUsed = true;
    }

    CPP2CB_CONVERT_IF_USED(in.ac_evse_charge_parameter, out.AC_EVSEChargeParameter);
    CPP2CB_CONVERT_IF_USED(in.dc_evse_charge_parameter, out.DC_EVSEChargeParameter);
}

template <> int serialize_to_exi(const ChargeParameterDiscoveryResponse& in, exi_bitstream_t& out) {

    iso2_exiDocument doc;
    init_iso2_exiDocument(&doc);
    init_iso2_BodyType(&doc.V2G_Message.Body);

    convert(in.header, doc.V2G_Message.Header);

    CB_SET_USED(doc.V2G_Message.Body.ChargeParameterDiscoveryRes);
    convert(in, doc.V2G_Message.Body.ChargeParameterDiscoveryRes);

    return encode_iso2_exiDocument(&out, &doc);
}

template <> size_t serialize(const ChargeParameterDiscoveryResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace iso15118::d2::msg
