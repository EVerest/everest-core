// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <array>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "common.hpp"

namespace iso15118::message_20 {

struct ScheduleExchangeRequest {

    struct Dynamic_SEReqControlMode {
        uint32_t departure_time;
        std::optional<PercentValue> minimum_soc;
        std::optional<PercentValue> target_soc;
        RationalNumber target_energy;
        RationalNumber max_energy;
        RationalNumber min_energy;
        std::optional<RationalNumber> max_v2x_energy;
        std::optional<RationalNumber> min_v2x_energy;
    };

    struct EVPowerScheduleEntry {
        uint32_t duration;
        RationalNumber power;
    };

    struct EVPowerSchedule {
        uint64_t time_anchor;
        std::vector<EVPowerScheduleEntry> entries; // max 1024
    };

    struct EVPriceRule {
        RationalNumber energy_fee;
        RationalNumber power_range_start;
    };

    struct EVPriceRuleStack {
        uint32_t duration;
        std::vector<EVPriceRule> price_rules; // max 8
    };

    struct EVAbsolutePriceSchedule {
        uint64_t time_anchor;
        std::string currency;
        std::string price_algorithm;
        std::vector<EVPriceRuleStack> price_rule_stacks; // max 1024
    };

    struct EVEnergyOffer {
        EVPowerSchedule power_schedule;
        EVAbsolutePriceSchedule absolute_price_schedule;
    };

    struct Scheduled_SEReqControlMode {
        std::optional<uint32_t> departure_time;
        std::optional<RationalNumber> target_energy;
        std::optional<RationalNumber> max_energy;
        std::optional<RationalNumber> min_energy;
        std::optional<EVEnergyOffer> energy_offer;
    };

    Header header;

    uint16_t max_supporting_points; // needs to be [12 - 1024]
    std::variant<Dynamic_SEReqControlMode, Scheduled_SEReqControlMode> control_mode;
};

struct ScheduleExchangeResponse {

    static constexpr auto SCHEDULED_POWER_DURATION_S = 86400;

    Header header;
    ResponseCode response_code;

    ScheduleExchangeResponse() :
        processing(Processing::Finished), control_mode(std::in_place_type<Dynamic_SEResControlMode>){};

    Processing processing;
    std::optional<bool> go_to_pause;

    struct TaxRule {
        uint32_t tax_rule_id;
        std::optional<std::string> tax_rule_name;
        RationalNumber tax_rate;
        std::optional<bool> tax_included_in_price;
        bool applies_to_energy_fee;
        bool applies_to_parking_fee;
        bool applies_to_overstay_fee;
        bool applies_to_minimum_maximum_cost;
    };

    struct PriceRule {
        RationalNumber energy_fee;
        std::optional<RationalNumber> parking_fee;
        std::optional<uint32_t> parking_fee_period;
        std::optional<uint16_t> carbon_dioxide_emission;
        std::optional<uint8_t> renewable_generation_percentage;
        RationalNumber power_range_start;
    };

    struct PriceRuleStack {
        uint32_t duration;
        std::vector<PriceRule> price_rule;
    };

    struct OverstayRule {
        std::optional<std::string> overstay_rule_description;
        uint32_t start_time;
        RationalNumber overstay_fee;
        uint32_t overstay_fee_period;
    };

    struct OverstayRulesList {
        std::optional<uint32_t> overstay_time_threshold;
        std::optional<RationalNumber> overstay_power_threshold;
        std::vector<OverstayRule> overstay_rule;
    };

    struct AdditionalService {
        std::string service_name;
        RationalNumber service_fee;
    };

    struct AbsolutePriceSchedule {
        std::optional<std::string> id;
        uint64_t time_anchor;
        uint32_t price_schedule_id;
        std::optional<std::string> price_schedule_description;
        std::string currency;
        std::string language;
        std::string price_algorithm;
        std::optional<RationalNumber> minimum_cost;
        std::optional<RationalNumber> maximum_cost;
        std::optional<std::vector<TaxRule>> tax_rules;
        std::vector<PriceRuleStack> price_rule_stacks;
        std::optional<OverstayRulesList> overstay_rules;
        std::optional<std::vector<AdditionalService>> additional_selected_services;
    };

    struct PriceLevelScheduleEntry {
        uint32_t duration;
        uint8_t price_level;
    };

    struct PriceLevelSchedule {
        std::optional<std::string> id;
        uint64_t time_anchor;
        uint32_t price_schedule_id;
        std::optional<std::string> price_schedule_description;
        uint8_t number_of_price_levels;
        std::vector<PriceLevelScheduleEntry> price_level_schedule_entries;
    };

    struct Dynamic_SEResControlMode {
        std::optional<uint32_t> departure_time;
        std::optional<uint8_t> minimum_soc;
        std::optional<uint8_t> target_soc;
        std::variant<std::monostate, AbsolutePriceSchedule, PriceLevelSchedule> price_schedule;
    };

    struct PowerScheduleEntry {
        uint32_t duration;
        RationalNumber power;
        std::optional<RationalNumber> power_l2;
        std::optional<RationalNumber> power_l3;
    };

    struct PowerSchedule {
        uint64_t time_anchor;
        std::optional<RationalNumber> available_energy;
        std::optional<RationalNumber> power_tolerance;
        std::vector<PowerScheduleEntry> entries;
    };

    struct ChargingSchedule {
        PowerSchedule power_schedule;
        std::variant<std::monostate, AbsolutePriceSchedule, PriceLevelSchedule> price_schedule;
    };

    struct ScheduleTuple {
        NumericID schedule_tuple_id; // 1 - 255
        ChargingSchedule charging_schedule;
        std::optional<ChargingSchedule> discharging_schedule;
    };

    struct Scheduled_SEResControlMode {
        std::vector<ScheduleTuple> schedule_tuple;
    };

    std::variant<Dynamic_SEResControlMode, Scheduled_SEResControlMode> control_mode;
};

} // namespace iso15118::message_20
