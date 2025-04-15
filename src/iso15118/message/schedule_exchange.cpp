// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/message/schedule_exchange.hpp>

#include <type_traits>

#include <iso15118/detail/cb_exi.hpp>
#include <iso15118/detail/variant_access.hpp>

#include <cbv2g/iso_20/iso20_CommonMessages_Decoder.h>
#include <cbv2g/iso_20/iso20_CommonMessages_Encoder.h>

namespace iso15118::message_20 {

// Begin conversion for deserializing a ScheduleExchangeRequest (EVSEside)
template <> void convert(const struct iso20_EVPowerScheduleEntryType& in, datatypes::EVPowerScheduleEntry& out) {
    out.duration = in.Duration;
    convert(in.Power, out.power);
}

template <> void convert(const struct iso20_EVPowerScheduleType& in, datatypes::EVPowerSchedule& out) {
    out.time_anchor = in.TimeAnchor;
    const auto& entries_in = in.EVPowerScheduleEntries.EVPowerScheduleEntry;
    out.entries.reserve(entries_in.arrayLen);
    for (auto i = 0; i < entries_in.arrayLen; ++i) {
        const auto& entry_in = entries_in.array[i];
        auto& entry_out = out.entries.emplace_back();
        convert(entry_in, entry_out);
    }
}

template <> void convert(const struct iso20_EVPriceRuleType& in, datatypes::EVPriceRule& out) {
    convert(in.EnergyFee, out.energy_fee);
    convert(in.PowerRangeStart, out.power_range_start);
}

template <> void convert(const struct iso20_EVPriceRuleStackType& in, datatypes::EVPriceRuleStack& out) {
    out.duration = in.Duration;

    const auto& rules_in = in.EVPriceRule;
    out.price_rules.reserve(rules_in.arrayLen);
    for (auto i = 0; i < rules_in.arrayLen; ++i) {
        const auto& rule_in = rules_in.array[i];
        auto& rule_out = out.price_rules.emplace_back();
        convert(rule_in, rule_out);
    }
}

template <> void convert(const struct iso20_EVAbsolutePriceScheduleType& in, datatypes::EVAbsolutePriceSchedule& out) {
    out.time_anchor = in.TimeAnchor;
    out.currency = CB2CPP_STRING(in.Currency);
    out.price_algorithm = CB2CPP_STRING(in.PriceAlgorithm);

    const auto& stacks_in = in.EVPriceRuleStacks.EVPriceRuleStack;
    out.price_rule_stacks.reserve(stacks_in.arrayLen);
    for (auto i = 0; i < stacks_in.arrayLen; ++i) {
        const auto& stack_in = stacks_in.array[i];
        auto& stack_out = out.price_rule_stacks.emplace_back();
        convert(stack_in, stack_out);
    }
}

template <> void convert(const struct iso20_EVEnergyOfferType& in, datatypes::EVEnergyOffer& out) {
    convert(in.EVPowerSchedule, out.power_schedule);
    convert(in.EVAbsolutePriceSchedule, out.absolute_price_schedule);
}

template <>
void convert(const struct iso20_Scheduled_SEReqControlModeType& in, datatypes::Scheduled_SEReqControlMode& out) {
    CB2CPP_ASSIGN_IF_USED(in.DepartureTime, out.departure_time);
    CB2CPP_CONVERT_IF_USED(in.EVTargetEnergyRequest, out.target_energy);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumEnergyRequest, out.max_energy);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumEnergyRequest, out.min_energy);
    CB2CPP_CONVERT_IF_USED(in.EVEnergyOffer, out.energy_offer);
}

template <>
void convert(const struct iso20_Dynamic_SEReqControlModeType& in, datatypes::Dynamic_SEReqControlMode& out) {
    out.departure_time = in.DepartureTime;
    CB2CPP_ASSIGN_IF_USED(in.MinimumSOC, out.minimum_soc);
    CB2CPP_ASSIGN_IF_USED(in.TargetSOC, out.target_soc);
    convert(in.EVTargetEnergyRequest, out.target_energy);
    convert(in.EVMaximumEnergyRequest, out.max_energy);
    convert(in.EVMinimumEnergyRequest, out.min_energy);
    CB2CPP_CONVERT_IF_USED(in.EVMaximumV2XEnergyRequest, out.max_v2x_energy);
    CB2CPP_CONVERT_IF_USED(in.EVMinimumV2XEnergyRequest, out.min_v2x_energy);
}

template <> void convert(const struct iso20_ScheduleExchangeReqType& in, ScheduleExchangeRequest& out) {
    convert(in.Header, out.header);

    out.max_supporting_points = in.MaximumSupportingPoints;

    if (in.Dynamic_SEReqControlMode_isUsed) {
        auto& mode_out = out.control_mode.emplace<datatypes::Dynamic_SEReqControlMode>();
        convert(in.Dynamic_SEReqControlMode, mode_out);
    } else if (in.Scheduled_SEReqControlMode_isUsed) {
        auto& mode_out = out.control_mode.emplace<datatypes::Scheduled_SEReqControlMode>();
        convert(in.Scheduled_SEReqControlMode, mode_out);
    } else {
        throw std::runtime_error("No control mode selected in iso20_ScheduleExchangeReqType");
    }
}
// End of conversion for deserializing a ScheduleExchangeRequest (EVSEside)

// Begin conversion for deserializing a ScheduleExchangeResponse (EVCCSide)
template <> void convert(const struct iso20_PriceRuleStackType& in, datatypes::PriceRuleStack& out) {
    out.duration = in.Duration;
    for (auto i = 0; i < in.PriceRule.arrayLen; ++i) {
        const auto& rule_in = in.PriceRule.array[i];
        auto& rule_out = out.price_rule[i];
        convert(rule_in.EnergyFee, rule_out.energy_fee);
        CB2CPP_CONVERT_IF_USED(rule_in.ParkingFee, rule_out.parking_fee);
        CB2CPP_ASSIGN_IF_USED(rule_in.ParkingFeePeriod, rule_out.parking_fee_period);
        CB2CPP_ASSIGN_IF_USED(rule_in.RenewableGenerationPercentage, rule_out.renewable_generation_percentage);
        CB2CPP_ASSIGN_IF_USED(rule_in.CarbonDioxideEmission, rule_out.carbon_dioxide_emission);
        convert(rule_in.PowerRangeStart, rule_out.power_range_start);
    }
}

template <> void convert(const struct iso20_PriceLevelScheduleType& in, datatypes::PriceLevelSchedule& out) {
    out.time_anchor = in.TimeAnchor;
    out.price_schedule_id = in.PriceScheduleID;
    out.number_of_price_levels = in.NumberOfPriceLevels;
    CB2CPP_STRING_IF_USED(in.Id, out.id);
    CB2CPP_STRING_IF_USED(in.PriceScheduleDescription, out.price_schedule_description);
    out.price_level_schedule_entries.reserve(in.PriceLevelScheduleEntries.PriceLevelScheduleEntry.arrayLen);
    for (auto i = 0; i < in.PriceLevelScheduleEntries.PriceLevelScheduleEntry.arrayLen; ++i) {
        const auto& entry_in = in.PriceLevelScheduleEntries.PriceLevelScheduleEntry.array[i];
        auto& entry_out = out.price_level_schedule_entries.emplace_back();
        entry_out.duration = entry_in.Duration;
        entry_out.price_level = entry_in.PriceLevel;
    }
}

template <> void convert(const struct iso20_PriceRuleType& in, datatypes::PriceRule& out) {
    convert(in.EnergyFee, out.energy_fee);
    CB2CPP_CONVERT_IF_USED(in.ParkingFee, out.parking_fee);
    CB2CPP_ASSIGN_IF_USED(in.ParkingFeePeriod, out.parking_fee_period);
    CB2CPP_ASSIGN_IF_USED(in.RenewableGenerationPercentage, out.renewable_generation_percentage);

    CB2CPP_ASSIGN_IF_USED(in.CarbonDioxideEmission, out.carbon_dioxide_emission);
    convert(in.PowerRangeStart, out.power_range_start);
}

template <> void convert(const struct iso20_TaxRuleType& in, datatypes::TaxRule& out) {
    out.tax_rule_id = in.TaxRuleID;
    CB2CPP_STRING_IF_USED(in.TaxRuleName, out.tax_rule_name);
    convert(in.TaxRate, out.tax_rate);
    CB2CPP_ASSIGN_IF_USED(in.TaxIncludedInPrice, out.tax_included_in_price);
    out.applies_to_energy_fee = in.AppliesToEnergyFee;
    out.applies_to_parking_fee = in.AppliesToParkingFee;
    out.applies_to_overstay_fee = in.AppliesToOverstayFee;
    out.applies_to_minimum_maximum_cost = in.AppliesMinimumMaximumCost;
}

template <> void convert(const struct iso20_OverstayRuleType& in, datatypes::OverstayRule& out) {
    out.start_time = in.StartTime;
    CB2CPP_STRING_IF_USED(in.OverstayRuleDescription, out.overstay_rule_description);
    convert(in.OverstayFee, out.overstay_fee);
    out.overstay_fee_period = in.OverstayFeePeriod;
    convert(in.OverstayFee, out.overstay_fee);
}

template <> void convert(const struct iso20_AbsolutePriceScheduleType& in, datatypes::AbsolutePriceSchedule& out) {
    out.time_anchor = in.TimeAnchor;
    CB2CPP_STRING_IF_USED(in.Id, out.id);
    out.price_schedule_id = in.PriceScheduleID;
    CB2CPP_STRING_IF_USED(in.PriceScheduleDescription, out.price_schedule_description);
    out.currency = CB2CPP_STRING(in.Currency);
    out.language = CB2CPP_STRING(in.Language);
    out.price_algorithm = CB2CPP_STRING(in.PriceAlgorithm);
    CB2CPP_CONVERT_IF_USED(in.MaximumCost, out.maximum_cost);
    CB2CPP_CONVERT_IF_USED(in.MinimumCost, out.minimum_cost);

    const auto& stacks_in = in.PriceRuleStacks.PriceRuleStack;
    auto& stacks_out = out.price_rule_stacks;
    for (auto i = 0; i < stacks_in.arrayLen; ++i) {
        const auto& stack_in = stacks_in.array[i];
        auto& stack_out = stacks_out[i];
        convert(stack_in, stack_out);
    }

    if (in.TaxRules_isUsed) {
        const auto& tax_rules_in = in.TaxRules.TaxRule;
        out.tax_rules.emplace();
        for (auto i = 0; i < tax_rules_in.arrayLen; ++i) {
            const auto& tax_rule_in = tax_rules_in.array[i];
            auto& tax_rule_out = out.tax_rules->at(i);
            convert(tax_rule_in, tax_rule_out);
        }
    }

    if (in.OverstayRules_isUsed) {
        const auto& in_overstay_rules = in.OverstayRules;
        out.overstay_rules.emplace();
        out.overstay_rules->overstay_time_threshold = in_overstay_rules.OverstayTimeThreshold;
        CB2CPP_CONVERT_IF_USED(in_overstay_rules.OverstayPowerThreshold, out.overstay_rules->overstay_power_threshold);

        const auto& overstay_rules_in = in_overstay_rules.OverstayRule;
        out.overstay_rules->overstay_rule.reserve(overstay_rules_in.arrayLen);
        for (auto i = 0; i < overstay_rules_in.arrayLen; ++i) {
            const auto& overstay_rule_in = overstay_rules_in.array[i];
            auto& overstay_rule_out = out.overstay_rules->overstay_rule.emplace_back();
            convert(overstay_rule_in, overstay_rule_out);
        }
    }

    if (in.AdditionalSelectedServices_isUsed) {
        const auto& in_add_services = in.AdditionalSelectedServices.AdditionalService;
        out.additional_selected_services.emplace();
        for (auto i = 0; i < in_add_services.arrayLen; ++i) {
            const auto& add_service_in = in_add_services.array[i];
            auto& add_service_out = out.additional_selected_services->at(i);
            add_service_out.service_name = CB2CPP_STRING(add_service_in.ServiceName);
            convert(add_service_in.ServiceFee, add_service_out.service_fee);
        }
    }
}

template <> void convert(const struct iso20_PowerScheduleType& in, datatypes::PowerSchedule& out) {
    out.time_anchor = in.TimeAnchor;
    CB2CPP_CONVERT_IF_USED(in.AvailableEnergy, out.available_energy);
    CB2CPP_CONVERT_IF_USED(in.PowerTolerance, out.power_tolerance);
    out.entries.reserve(in.PowerScheduleEntries.PowerScheduleEntry.arrayLen);
    for (auto i = 0; i < in.PowerScheduleEntries.PowerScheduleEntry.arrayLen; ++i) {
        const auto& entry_in = in.PowerScheduleEntries.PowerScheduleEntry.array[i];
        auto& entry_out = out.entries.emplace_back();
        convert(entry_in, entry_out);
    }
}

template <> void convert(const struct iso20_ChargingScheduleType& in, datatypes::ChargingSchedule& out) {
    if (in.AbsolutePriceSchedule_isUsed) {
        auto& out_absolute_price_schedule = out.price_schedule.emplace<datatypes::AbsolutePriceSchedule>();
        convert(in.AbsolutePriceSchedule, out_absolute_price_schedule);
    } else if (in.PriceLevelSchedule_isUsed) {
        auto& out_price_level_schedule = out.price_schedule.emplace<datatypes::PriceLevelSchedule>();
        convert(in.PriceLevelSchedule, out_price_level_schedule);
    } else {
        out.price_schedule.emplace<std::monostate>();
    }
    convert(in.PowerSchedule, out.power_schedule);
}

template <> void convert(const struct iso20_ScheduleTupleType& in, datatypes::ScheduleTuple& out) {
    out.schedule_tuple_id = in.ScheduleTupleID;
    convert(in.ChargingSchedule, out.charging_schedule);
    if (in.DischargingSchedule_isUsed) {
        auto& out_discharge_schedule = out.discharging_schedule.emplace();
        convert(in.DischargingSchedule, out_discharge_schedule);
    }
}

template <>
void convert(const struct iso20_Scheduled_SEResControlModeType& in, datatypes::Scheduled_SEResControlMode& out) {
    out.schedule_tuple.reserve(in.ScheduleTuple.arrayLen);
    for (auto i = 0; i < in.ScheduleTuple.arrayLen; ++i) {
        const auto& tuple_in = in.ScheduleTuple.array[i];
        auto& tuple_out = out.schedule_tuple.emplace_back();
        convert(tuple_in, tuple_out);
    }
}

template <>
void convert(const struct iso20_Dynamic_SEResControlModeType& in, datatypes::Dynamic_SEResControlMode& out) {

    CB2CPP_ASSIGN_IF_USED(in.DepartureTime, out.departure_time);
    CB2CPP_ASSIGN_IF_USED(in.MinimumSOC, out.minimum_soc);
    CB2CPP_ASSIGN_IF_USED(in.TargetSOC, out.target_soc);
    if (in.AbsolutePriceSchedule_isUsed) {
        auto& out_absolute_price_schedule = out.price_schedule.emplace<datatypes::AbsolutePriceSchedule>();
        convert(in.AbsolutePriceSchedule, out_absolute_price_schedule);
    } else if (in.PriceLevelSchedule_isUsed) {
        auto& out_price_level_schedule = out.price_schedule.emplace<datatypes::PriceLevelSchedule>();
        convert(in.PriceLevelSchedule, out_price_level_schedule);
    } else {
        out.price_schedule.emplace<std::monostate>();
    }
}

template <> void convert(const struct iso20_ScheduleExchangeResType& in, ScheduleExchangeResponse& out) {
    convert(in.Header, out.header);
    cb_convert_enum(in.ResponseCode, out.response_code);
    cb_convert_enum(in.EVSEProcessing, out.processing);
    CB2CPP_ASSIGN_IF_USED(in.GoToPause, out.go_to_pause);

    if (in.Dynamic_SEResControlMode_isUsed) {
        auto& mode_out = out.control_mode.emplace<datatypes::Dynamic_SEResControlMode>();
        convert(in.Dynamic_SEResControlMode, mode_out);

    } else if (in.Scheduled_SEResControlMode_isUsed) {
        auto& mode_out = out.control_mode.emplace<datatypes::Scheduled_SEResControlMode>();
        convert(in.Scheduled_SEResControlMode, mode_out);
    } else {
        throw std::runtime_error("No control mode selected in iso20_ScheduleExchangeResType");
    }
}

template <> void insert_type(VariantAccess& va, const struct iso20_ScheduleExchangeResType& in) {
    va.insert_type<ScheduleExchangeResponse>(in);
};
// End of conversion for deserializing a ScheduleExchangeResponse (EVCCSide)

// Begin conversion for serializing a ScheduleExchangeResponse (EVSESide)
template <> void convert(const datatypes::PowerSchedule& in, struct iso20_PowerScheduleType& out) {
    init_iso20_PowerScheduleType(&out);

    out.TimeAnchor = in.time_anchor;
    CPP2CB_CONVERT_IF_USED(in.available_energy, out.AvailableEnergy);
    CPP2CB_CONVERT_IF_USED(in.power_tolerance, out.PowerTolerance);

    if ((sizeof(out.PowerScheduleEntries.PowerScheduleEntry.array) /
         sizeof(out.PowerScheduleEntries.PowerScheduleEntry.array[0])) < in.entries.size()) {
        throw std::runtime_error("array is too large"); // FIXME(SL): Change error message
    }

    for (std::size_t i = 0; i < in.entries.size(); i++) {
        auto& out_entry = out.PowerScheduleEntries.PowerScheduleEntry.array[i];
        const auto& in_entry = in.entries[i];

        out_entry.Duration = in_entry.duration;
        convert(in_entry.power, out_entry.Power);
        CPP2CB_CONVERT_IF_USED(in_entry.power_l2, out_entry.Power_L2);
        CPP2CB_CONVERT_IF_USED(in_entry.power_l3, out_entry.Power_L3);
    }
    out.PowerScheduleEntries.PowerScheduleEntry.arrayLen = in.entries.size();
}

template <> void convert(const datatypes::TaxRule& in, struct iso20_TaxRuleType& out) {
    out.TaxRuleID = in.tax_rule_id;
    CPP2CB_STRING_IF_USED(in.tax_rule_name, out.TaxRuleName);
    convert(in.tax_rate, out.TaxRate);
    CPP2CB_ASSIGN_IF_USED(in.tax_included_in_price, out.TaxIncludedInPrice);
    out.AppliesToEnergyFee = in.applies_to_energy_fee;
    out.AppliesToParkingFee = in.applies_to_parking_fee;
    out.AppliesToOverstayFee = in.applies_to_overstay_fee;
    out.AppliesMinimumMaximumCost = in.applies_to_minimum_maximum_cost;
}

template <> void convert(const datatypes::PriceRule& in, struct iso20_PriceRuleType& out) {
    convert(in.energy_fee, out.EnergyFee);
    CPP2CB_CONVERT_IF_USED(in.parking_fee, out.ParkingFee);
    CPP2CB_ASSIGN_IF_USED(in.parking_fee_period, out.ParkingFeePeriod);
    CPP2CB_ASSIGN_IF_USED(in.carbon_dioxide_emission, out.CarbonDioxideEmission);
    CPP2CB_ASSIGN_IF_USED(in.renewable_generation_percentage, out.RenewableGenerationPercentage);
    convert(in.power_range_start, out.PowerRangeStart);
}

template <> void convert(const datatypes::PriceRuleStack& in, struct iso20_PriceRuleStackType& out) {
    out.Duration = in.duration;

    if ((sizeof(out.PriceRule.array) / sizeof(out.PriceRule.array[0])) < in.price_rule.size()) {
        throw std::runtime_error("array is too large"); // FIXME(SL): Change error message
    }
    for (std::size_t i = 0; i < in.price_rule.size(); i++) {
        convert(in.price_rule.at(i), out.PriceRule.array[i]);
    }
    out.PriceRule.arrayLen = in.price_rule.size();
}

template <> void convert(const datatypes::OverstayRule& in, struct iso20_OverstayRuleType& out) {
    CPP2CB_STRING_IF_USED(in.overstay_rule_description, out.OverstayRuleDescription);
    out.StartTime = in.start_time;
    convert(in.overstay_fee, out.OverstayFee);
    out.OverstayFeePeriod = in.overstay_fee_period;
}

template <> void convert(const datatypes::AbsolutePriceSchedule& in, struct iso20_AbsolutePriceScheduleType& out) {

    CPP2CB_STRING_IF_USED(in.id, out.Id);
    out.TimeAnchor = in.time_anchor;
    out.PriceScheduleID = in.price_schedule_id;
    CPP2CB_STRING_IF_USED(in.price_schedule_description, out.PriceScheduleDescription);
    CPP2CB_STRING(in.currency, out.Currency);
    CPP2CB_STRING(in.language, out.Language);
    CPP2CB_STRING(in.price_algorithm, out.PriceAlgorithm);
    CPP2CB_CONVERT_IF_USED(in.minimum_cost, out.MaximumCost);
    CPP2CB_CONVERT_IF_USED(in.maximum_cost, out.MaximumCost);

    if (in.tax_rules.has_value()) {
        out.TaxRules_isUsed = true;
        const auto& in_tax_rules = in.tax_rules.value();

        if ((sizeof(out.TaxRules.TaxRule.array) / sizeof(out.TaxRules.TaxRule.array[0])) < in_tax_rules.size()) {
            throw std::runtime_error("array is too large"); // FIXME(SL): Change error message
        }

        for (std::size_t i = 0; i < in_tax_rules.size(); i++) {
            convert(in_tax_rules.at(i), out.TaxRules.TaxRule.array[i]);
        }
        out.TaxRules.TaxRule.arrayLen = in_tax_rules.size();
    }

    if ((sizeof(out.PriceRuleStacks.PriceRuleStack.array) / sizeof(out.PriceRuleStacks.PriceRuleStack.array[0])) <
        in.price_rule_stacks.size()) {
        throw std::runtime_error("array is too large"); // FIXME(SL): Change error message
    }
    for (std::size_t i = 0; i < in.price_rule_stacks.size(); i++) {
        convert(in.price_rule_stacks.at(i), out.PriceRuleStacks.PriceRuleStack.array[i]);
    }
    out.PriceRuleStacks.PriceRuleStack.arrayLen = in.price_rule_stacks.size();

    if (in.overstay_rules.has_value()) {
        out.OverstayRules_isUsed = true;
        const auto& in_overstay_rules = in.overstay_rules.value();

        CPP2CB_ASSIGN_IF_USED(in_overstay_rules.overstay_time_threshold, out.OverstayRules.OverstayTimeThreshold);
        CPP2CB_CONVERT_IF_USED(in_overstay_rules.overstay_power_threshold, out.OverstayRules.OverstayPowerThreshold);

        if ((sizeof(out.OverstayRules.OverstayRule.array) / sizeof(out.OverstayRules.OverstayRule.array[0])) <
            in_overstay_rules.overstay_rule.size()) {
            throw std::runtime_error("array is too large"); // FIXME(SL): Change error message
        }

        for (std::size_t i = 0; i < in_overstay_rules.overstay_rule.size(); i++) {
            convert(in_overstay_rules.overstay_rule.at(i), out.OverstayRules.OverstayRule.array[i]);
        }
        out.OverstayRules.OverstayRule.arrayLen = in_overstay_rules.overstay_rule.size();
    }

    if (in.additional_selected_services.has_value()) {
        out.AdditionalSelectedServices_isUsed = true;
        const auto& in_add_services = in.additional_selected_services.value();

        if ((sizeof(out.AdditionalSelectedServices.AdditionalService.array) /
             sizeof(out.AdditionalSelectedServices.AdditionalService.array[0])) < in_add_services.size()) {
            throw std::runtime_error("array is too large"); // FIXME(SL): Change error message
        }

        for (std::size_t i = 0; i < in_add_services.size(); i++) {
            CPP2CB_STRING(in_add_services.at(i).service_name,
                          out.AdditionalSelectedServices.AdditionalService.array[i].ServiceName);
            convert(in_add_services.at(i).service_fee,
                    out.AdditionalSelectedServices.AdditionalService.array[i].ServiceFee);
        }
        out.AdditionalSelectedServices.AdditionalService.arrayLen = in_add_services.size();
    }
}

template <> void convert(const datatypes::PriceLevelSchedule& in, struct iso20_PriceLevelScheduleType& out) {

    CPP2CB_STRING_IF_USED(in.id, out.Id);
    out.TimeAnchor = in.time_anchor;
    out.PriceScheduleID = in.price_schedule_id;
    CPP2CB_STRING_IF_USED(in.price_schedule_description, out.PriceScheduleDescription);
    out.NumberOfPriceLevels = in.number_of_price_levels;

    if ((sizeof(out.PriceLevelScheduleEntries.PriceLevelScheduleEntry.array) /
         sizeof(out.PriceLevelScheduleEntries.PriceLevelScheduleEntry.array[0])) <
        in.price_level_schedule_entries.size()) {
        throw std::runtime_error("array is too large"); // FIXME(SL): Change error message
    }

    for (std::size_t i = 0; i < in.price_level_schedule_entries.size(); i++) {
        out.PriceLevelScheduleEntries.PriceLevelScheduleEntry.array[i].Duration =
            in.price_level_schedule_entries.at(i).duration;
        out.PriceLevelScheduleEntries.PriceLevelScheduleEntry.array[i].PriceLevel =
            in.price_level_schedule_entries.at(i).price_level;
    }

    out.PriceLevelScheduleEntries.PriceLevelScheduleEntry.arrayLen = in.price_level_schedule_entries.size();
}

using PriceSchedule = std::variant<std::monostate, datatypes::AbsolutePriceSchedule, datatypes::PriceLevelSchedule>;
template <typename CbMessageType> void convert_price_schedule(const PriceSchedule& in, CbMessageType& out) {

    if (const auto* absolute_price = std::get_if<datatypes::AbsolutePriceSchedule>(&in)) {
        convert(*absolute_price, out.AbsolutePriceSchedule);
        out.AbsolutePriceSchedule_isUsed = true;
    } else if (const auto* price_level_schedule = std::get_if<datatypes::PriceLevelSchedule>(&in)) {
        convert(*price_level_schedule, out.PriceLevelSchedule);
        out.PriceLevelSchedule_isUsed = true;
    } else {
        out.AbsolutePriceSchedule_isUsed = false;
        out.PriceLevelSchedule_isUsed = false;
    }
}

template <> void convert(const PriceSchedule& in, struct iso20_ChargingScheduleType& out) {
    convert_price_schedule(in, out);
}
template <> void convert(const PriceSchedule& in, struct iso20_Dynamic_SEResControlModeType& out) {
    convert_price_schedule(in, out);
}

template <> void convert(const datatypes::ChargingSchedule& in, struct iso20_ChargingScheduleType& out) {
    init_iso20_ChargingScheduleType(&out);

    convert(in.power_schedule, out.PowerSchedule);
    convert(in.price_schedule, out);
}

template <> void convert(const datatypes::ScheduleTuple& in, struct iso20_ScheduleTupleType& out) {
    init_iso20_ScheduleTupleType(&out);

    out.ScheduleTupleID = in.schedule_tuple_id;
    convert(in.charging_schedule, out.ChargingSchedule);
    CPP2CB_CONVERT_IF_USED(in.discharging_schedule, out.DischargingSchedule);
}

struct ModeResponseVisitor {
    ModeResponseVisitor(iso20_ScheduleExchangeResType& res_) : res(res_){};
    void operator()(const datatypes::Dynamic_SEResControlMode& in) {
        init_iso20_Dynamic_SEResControlModeType(&res.Dynamic_SEResControlMode);
        CB_SET_USED(res.Dynamic_SEResControlMode);

        auto& out = res.Dynamic_SEResControlMode;

        CPP2CB_ASSIGN_IF_USED(in.departure_time, out.DepartureTime)
        CPP2CB_ASSIGN_IF_USED(in.minimum_soc, out.MinimumSOC);
        CPP2CB_ASSIGN_IF_USED(in.target_soc, out.TargetSOC);

        convert(in.price_schedule, out);
    }

    void operator()(const datatypes::Scheduled_SEResControlMode& in) {
        init_iso20_Scheduled_SEResControlModeType(&res.Scheduled_SEResControlMode);
        CB_SET_USED(res.Scheduled_SEResControlMode);

        auto& out = res.Scheduled_SEResControlMode;

        if ((sizeof(out.ScheduleTuple.array) / sizeof(out.ScheduleTuple.array[0])) < in.schedule_tuple.size()) {
            throw std::runtime_error("array is too large"); // FIXME(SL): Change error message
        }

        for (std::size_t i = 0; i < in.schedule_tuple.size(); i++) {
            convert(in.schedule_tuple[i], out.ScheduleTuple.array[i]);
        }
        out.ScheduleTuple.arrayLen = in.schedule_tuple.size();
    }

private:
    iso20_ScheduleExchangeResType& res;
};

template <> void convert(const ScheduleExchangeResponse& in, struct iso20_ScheduleExchangeResType& out) {
    init_iso20_ScheduleExchangeResType(&out);

    convert(in.header, out.Header);
    cb_convert_enum(in.response_code, out.ResponseCode);

    cb_convert_enum(in.processing, out.EVSEProcessing);

    CPP2CB_ASSIGN_IF_USED(in.go_to_pause, out.GoToPause);

    std::visit(ModeResponseVisitor(out), in.control_mode);
}

template <> void insert_type(VariantAccess& va, const struct iso20_ScheduleExchangeReqType& in) {
    va.insert_type<ScheduleExchangeRequest>(in);
};

template <> int serialize_to_exi(const ScheduleExchangeResponse& in, exi_bitstream_t& out) {
    iso20_exiDocument doc;
    init_iso20_exiDocument(&doc);

    CB_SET_USED(doc.ScheduleExchangeRes);

    convert(in, doc.ScheduleExchangeRes);

    return encode_iso20_exiDocument(&out, &doc);
}

template <> size_t serialize(const ScheduleExchangeResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}
// End conversion for serializing a ScheduleExchangeResponse (EVSESide)

// Begin conversion for serializing a ScheduleExchangeRequest (EVCCSide)
template <> void convert(const datatypes::EVPriceRule& in, struct iso20_EVPriceRuleType& out) {
    convert(in.energy_fee, out.EnergyFee);
    convert(in.power_range_start, out.PowerRangeStart);
}
template <> void convert(const datatypes::EVPriceRuleStack& in, struct iso20_EVPriceRuleStackType& out) {
    out.Duration = in.duration;

    if ((sizeof(out.EVPriceRule.array) / sizeof(out.EVPriceRule.array[0])) < in.price_rules.size()) {
        throw std::runtime_error("array is too large"); // FIXME(SL): Change error message
    }
    for (std::size_t i = 0; i < in.price_rules.size(); i++) {
        convert(in.price_rules.at(i), out.EVPriceRule.array[i]);
    }
    out.EVPriceRule.arrayLen = in.price_rules.size();
}

template <> void convert(const datatypes::EVAbsolutePriceSchedule& in, struct iso20_EVAbsolutePriceScheduleType& out) {
    out.TimeAnchor = in.time_anchor;
    CPP2CB_STRING(in.currency, out.Currency);
    CPP2CB_STRING(in.price_algorithm, out.PriceAlgorithm);

    if ((sizeof(out.EVPriceRuleStacks.EVPriceRuleStack.array) /
         sizeof(out.EVPriceRuleStacks.EVPriceRuleStack.array[0])) < in.price_rule_stacks.size()) {
        throw std::runtime_error("array is too large"); // FIXME(SL): Change error message
    }
    for (std::size_t i = 0; i < in.price_rule_stacks.size(); i++) {
        convert(in.price_rule_stacks.at(i), out.EVPriceRuleStacks.EVPriceRuleStack.array[i]);
    }
    out.EVPriceRuleStacks.EVPriceRuleStack.arrayLen = in.price_rule_stacks.size();
}

template <> void convert(const datatypes::EVPowerScheduleEntry& in, struct iso20_EVPowerScheduleEntryType& out) {
    out.Duration = in.duration;
    convert(in.power, out.Power);
}

template <> void convert(const datatypes::EVPowerSchedule& in, struct iso20_EVPowerScheduleType& out) {
    out.TimeAnchor = in.time_anchor;

    if ((sizeof(out.EVPowerScheduleEntries.EVPowerScheduleEntry.array) /
         sizeof(out.EVPowerScheduleEntries.EVPowerScheduleEntry.array[0])) < in.entries.size()) {
        throw std::runtime_error("array is too large"); // FIXME(SL): Change error message
    }
    for (std::size_t i = 0; i < in.entries.size(); i++) {
        convert(in.entries.at(i), out.EVPowerScheduleEntries.EVPowerScheduleEntry.array[i]);
    }
    out.EVPowerScheduleEntries.EVPowerScheduleEntry.arrayLen = in.entries.size();
}

template <> void convert(const datatypes::EVEnergyOffer& in, struct iso20_EVEnergyOfferType& out) {
    convert(in.power_schedule, out.EVPowerSchedule);
    convert(in.absolute_price_schedule, out.EVAbsolutePriceSchedule);
}

struct ModeRequestVisitor {
    ModeRequestVisitor(iso20_ScheduleExchangeReqType& req_) : req(req_){};
    void operator()(const datatypes::Dynamic_SEReqControlMode& in) {
        init_iso20_Dynamic_SEReqControlModeType(&req.Dynamic_SEReqControlMode);
        CB_SET_USED(req.Dynamic_SEReqControlMode);

        auto& out = req.Dynamic_SEReqControlMode;

        out.DepartureTime = in.departure_time;
        convert(in.max_energy, out.EVMaximumEnergyRequest);
        convert(in.min_energy, out.EVMinimumEnergyRequest);
        convert(in.target_energy, out.EVTargetEnergyRequest);
        CPP2CB_CONVERT_IF_USED(in.max_v2x_energy, out.EVMaximumV2XEnergyRequest);
        CPP2CB_CONVERT_IF_USED(in.min_v2x_energy, out.EVMinimumV2XEnergyRequest);
        CPP2CB_ASSIGN_IF_USED(in.minimum_soc, out.MinimumSOC);
        CPP2CB_ASSIGN_IF_USED(in.target_soc, out.TargetSOC);
    }

    void operator()(const datatypes::Scheduled_SEReqControlMode& in) {
        init_iso20_Scheduled_SEReqControlModeType(&req.Scheduled_SEReqControlMode);
        CB_SET_USED(req.Scheduled_SEReqControlMode);

        auto& out = req.Scheduled_SEReqControlMode;

        CPP2CB_ASSIGN_IF_USED(in.departure_time, out.DepartureTime);
        CPP2CB_CONVERT_IF_USED(in.target_energy, out.EVTargetEnergyRequest);
        CPP2CB_CONVERT_IF_USED(in.max_energy, out.EVMaximumEnergyRequest);
        CPP2CB_CONVERT_IF_USED(in.min_energy, out.EVMinimumEnergyRequest);

        CPP2CB_CONVERT_IF_USED(in.energy_offer, out.EVEnergyOffer);
    }

private:
    iso20_ScheduleExchangeReqType& req;
};

template <> void convert(const ScheduleExchangeRequest& in, iso20_ScheduleExchangeReqType& out) {
    init_iso20_ScheduleExchangeReqType(&out);

    out.MaximumSupportingPoints = in.max_supporting_points;

    std::visit(ModeRequestVisitor(out), in.control_mode);

    convert(in.header, out.Header);
}

template <> int serialize_to_exi(const ScheduleExchangeRequest& in, exi_bitstream_t& out) {
    iso20_exiDocument doc;
    init_iso20_exiDocument(&doc);

    CB_SET_USED(doc.ScheduleExchangeReq);

    convert(in, doc.ScheduleExchangeReq);

    return encode_iso20_exiDocument(&out, &doc);
}

template <> size_t serialize(const ScheduleExchangeRequest& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}
// End conversion for serializing a ScheduleExchangeRequest (EVCCSide)

} // namespace iso15118::message_20
