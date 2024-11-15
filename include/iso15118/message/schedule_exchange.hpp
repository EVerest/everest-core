// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <array>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "common_types.hpp"

namespace iso15118::message_20 {

namespace datatypes {

static constexpr auto TAX_RULE_LENGTH = 10;
static constexpr auto PRICE_RULE_STACK_LENGTH = 1024;
static constexpr auto PRICE_RULE_LENGTH = 8;
static constexpr auto OVERSTAY_RULE_LENGTH = 5;
static constexpr auto ADDITIONAL_SERVICE_LENGTH = 5;
static constexpr auto PRICE_LEVEL_SCHEDULE_LENGTH = 1024;
static constexpr auto SCHEDULED_POWER_DURATION_S = 86400;

using MaxSupportingPointsScheduleTuple = uint16_t; // needs to be [12 - 1024]

using Curreny = std::string;  // MaxLength: 3
using Language = std::string; // MaxLength: 3

struct TaxRule {
    NumericId tax_rule_id;
    std::optional<Name> tax_rule_name;
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
    std::array<PriceRule, PRICE_RULE_LENGTH> price_rule;
};

struct AdditionalService {
    Name service_name;
    RationalNumber service_fee;
};

using TaxRuleList = std::array<TaxRule, TAX_RULE_LENGTH>;
using PriceRuleStackList = std::array<PriceRuleStack, PRICE_RULE_STACK_LENGTH>;
using AdditionalServiceList = std::array<AdditionalService, ADDITIONAL_SERVICE_LENGTH>;

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
    Curreny currency;
    Identifier price_algorithm;
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

struct OverstayRule {
    std::optional<Description> overstay_rule_description;
    uint32_t start_time;
    RationalNumber overstay_fee;
    uint32_t overstay_fee_period;
};

struct OverstayRulesList {
    std::optional<uint32_t> overstay_time_threshold;
    std::optional<RationalNumber> overstay_power_threshold;
    std::vector<OverstayRule> overstay_rule;
};

struct AbsolutePriceSchedule {
    std::optional<std::string> id;
    uint64_t time_anchor;
    NumericId price_schedule_id;
    std::optional<Description> price_schedule_description;
    Curreny currency;
    Language language;
    Identifier price_algorithm;
    std::optional<RationalNumber> minimum_cost;
    std::optional<RationalNumber> maximum_cost;
    std::optional<TaxRuleList> tax_rules;
    PriceRuleStackList price_rule_stacks;
    std::optional<OverstayRulesList> overstay_rules;
    std::optional<AdditionalServiceList> additional_selected_services;
};

struct PriceLevelScheduleEntry {
    uint32_t duration;
    uint8_t price_level;
};

struct PriceLevelSchedule {
    std::optional<std::string> id;
    uint64_t time_anchor;
    NumericId price_schedule_id;
    std::optional<Description> price_schedule_description;
    uint8_t number_of_price_levels;
    std::vector<PriceLevelScheduleEntry> price_level_schedule_entries;
};

struct Dynamic_SEResControlMode {
    std::optional<uint32_t> departure_time;
    std::optional<PercentValue> minimum_soc;
    std::optional<PercentValue> target_soc;
    std::variant<std::monostate, AbsolutePriceSchedule, PriceLevelSchedule> price_schedule;
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
    NumericId schedule_tuple_id; // 1 - 255
    ChargingSchedule charging_schedule;
    std::optional<ChargingSchedule> discharging_schedule;
};

struct Scheduled_SEResControlMode {
    std::vector<ScheduleTuple> schedule_tuple;
};

}; // namespace datatypes

struct ScheduleExchangeRequest {
    Header header;
    datatypes::MaxSupportingPointsScheduleTuple max_supporting_points;
    std::variant<datatypes::Dynamic_SEReqControlMode, datatypes::Scheduled_SEReqControlMode> control_mode;
};

struct ScheduleExchangeResponse {
    Header header;
    datatypes::ResponseCode response_code;
    datatypes::Processing processing{datatypes::Processing::Finished};
    std::optional<bool> go_to_pause;

    std::variant<datatypes::Dynamic_SEResControlMode, datatypes::Scheduled_SEResControlMode> control_mode =
        datatypes::Dynamic_SEResControlMode();
};

} // namespace iso15118::message_20
