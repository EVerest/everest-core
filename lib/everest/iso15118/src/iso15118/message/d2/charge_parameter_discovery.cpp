// SPDX-License-Identifier: Apache-2.0
// Copyright 2026 Pionix GmbH and Contributors to EVerest
#include <cstddef>
#include <iso15118/message/d2/charge_parameter_discovery.hpp>

#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_2/iso2_msgDefDecoder.h>
#include <cbv2g/iso_2/iso2_msgDefEncoder.h>

#include <iso15118/detail/helper.hpp>
#include <variant>

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

    const auto cost_type_max_length = std::min(static_cast<size_t>(iso2_CostType_3_ARRAY_SIZE), in.cost.size());
    for (size_t i = 0; i < cost_type_max_length; i++) {
        const auto& entry_in = in.cost.at(i);
        auto& entry_out = out.Cost.array[i];
        init_iso2_CostType(&entry_out);
        cb_convert_enum(entry_in.cost_kind, entry_out.costKind);
        entry_out.amount = entry_in.amount;
        CPP2CB_ASSIGN_IF_USED(entry_in.amount_multiplier, entry_out.amountMultiplier);
    }
    out.Cost.arrayLen = cost_type_max_length;
}

template <> void convert(const data_types::SalesTariffEntry& in, struct iso2_SalesTariffEntryType& out) {
    init_iso2_SalesTariffEntryType(&out);
    convert(in.time_interval, out.RelativeTimeInterval);
    out.RelativeTimeInterval_isUsed = true;
    CPP2CB_ASSIGN_IF_USED(in.e_price_level, out.EPriceLevel);

    const auto consumption_cost_max_length =
        std::min(static_cast<size_t>(iso2_ConsumptionCostType_3_ARRAY_SIZE), in.consumption_cost.size());
    for (size_t i = 0; i < consumption_cost_max_length; i++) {
        const auto& cost_in = in.consumption_cost.at(i);
        auto& cost_out = out.ConsumptionCost.array[i];
        convert(cost_in, cost_out);
    }
    out.ConsumptionCost.arrayLen = consumption_cost_max_length;
}

template <> void convert(const data_types::SaScheduleTuple& in, struct iso2_SAScheduleTupleType& out) {
    init_iso2_SAScheduleTupleType(&out);
    out.SAScheduleTupleID = in.sa_schedule_tuple_id;

    const auto pmax_schedule_max_length =
        std::min(static_cast<size_t>(iso2_PMaxScheduleEntryType_12_ARRAY_SIZE), in.pmax_schedule.size());
    for (size_t i = 0; i < pmax_schedule_max_length; i++) {
        const auto& entry_in = in.pmax_schedule.at(i);
        auto& entry_out = out.PMaxSchedule.PMaxScheduleEntry.array[i];
        init_iso2_PMaxScheduleEntryType(&entry_out);
        convert(entry_in.time_interval, entry_out.RelativeTimeInterval);
        entry_out.RelativeTimeInterval_isUsed = true;
        convert(entry_in.p_max, entry_out.PMax);
    }
    out.PMaxSchedule.PMaxScheduleEntry.arrayLen = pmax_schedule_max_length;

    if (in.sales_tariff.has_value()) {
        init_iso2_SalesTariffType(&out.SalesTariff);
        out.SalesTariff_isUsed = true;
        CPP2CB_STRING_IF_USED(in.sales_tariff->id, out.SalesTariff.Id);
        out.SalesTariff.SalesTariffID = in.sales_tariff->sales_tariff_id;
        CPP2CB_STRING_IF_USED(in.sales_tariff->sales_tariff_description, out.SalesTariff.SalesTariffDescription);
        CPP2CB_ASSIGN_IF_USED(in.sales_tariff->num_e_price_levels, out.SalesTariff.NumEPriceLevels);

        const auto sales_tariff_entry_max_length = std::min(
            static_cast<size_t>(iso2_SalesTariffEntryType_12_ARRAY_SIZE), in.sales_tariff->sales_tariff_entry.size());
        for (size_t i = 0; i < sales_tariff_entry_max_length; i++) {
            const auto& entry_in = in.sales_tariff->sales_tariff_entry.at(i);
            auto& entry_out = out.SalesTariff.SalesTariffEntry.array[i];
            convert(entry_in, entry_out);
        }
        out.SalesTariff.SalesTariffEntry.arrayLen = sales_tariff_entry_max_length;
    }
}

template <> void convert(const struct iso2_ChargeParameterDiscoveryReqType& in, ChargeParameterDiscoveryRequest& out) {
    CB2CPP_ASSIGN_IF_USED(in.MaxEntriesSAScheduleTuple, out.max_entries_sa_schedule_tuple);
    cb_convert_enum(in.RequestedEnergyTransferMode, out.requested_energy_transfer_mode);
    if (in.AC_EVChargeParameter_isUsed) {
        data_types::AcEvChargeParameter param;
        convert(in.AC_EVChargeParameter, param);
        out.ev_charge_parameter = param;
    } else if (in.DC_EVChargeParameter_isUsed) {
        data_types::DcEvChargeParameter param;
        convert(in.DC_EVChargeParameter, param);
        out.ev_charge_parameter = param;
    }
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
        const auto sa_schedule_list_max_length =
            std::min(static_cast<size_t>(iso2_SAScheduleTupleType_3_ARRAY_SIZE), in.sa_schedule_list->size());
        for (size_t i = 0; i < sa_schedule_list_max_length; i++) {
            const auto& schedule_in = in.sa_schedule_list->at(i);
            auto& schedule_out = out.SAScheduleList.SAScheduleTuple.array[i];
            convert(schedule_in, schedule_out);
        }
        out.SAScheduleList.SAScheduleTuple.arrayLen = sa_schedule_list_max_length;
        out.SAScheduleList_isUsed = true;
    }

    if (std::holds_alternative<data_types::AcEvseChargeParameter>(in.evse_charge_parameter)) {
        const auto& param = std::get<data_types::AcEvseChargeParameter>(in.evse_charge_parameter);
        convert(param, out.AC_EVSEChargeParameter);
        CB_SET_USED(out.AC_EVSEChargeParameter);
    } else if (std::holds_alternative<data_types::DcEvseChargeParameter>(in.evse_charge_parameter)) {
        const auto& param = std::get<data_types::DcEvseChargeParameter>(in.evse_charge_parameter);
        convert(param, out.DC_EVSEChargeParameter);
        CB_SET_USED(out.DC_EVSEChargeParameter);
    }
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
