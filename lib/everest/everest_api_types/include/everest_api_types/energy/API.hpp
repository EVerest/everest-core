// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <everest_api_types/powermeter/API.hpp>
#include <optional>
#include <string>
#include <vector>

namespace everest::lib::API::V1_0::types::energy {

struct NumberWithSource {
    float value;
    std::string source;
};

struct IntegerWithSource {
    int32_t value;
    std::string source;
};

struct FrequencyWattPoint {
    float frequency_Hz;
    float total_power_W;
};

struct SetpointType {
    int32_t priority;
    std::string source;
    std::optional<float> ac_current_A;
    std::optional<float> total_power_W;
    std::optional<std::vector<types::energy::FrequencyWattPoint>> frequency_table;
};

struct PricePerkWh {
    std::string timestamp;
    float value;
    std::string currency;
};

struct LimitsReq {
    std::optional<NumberWithSource> total_power_W;
    std::optional<NumberWithSource> ac_max_current_A;
    std::optional<NumberWithSource> ac_min_current_A;
    std::optional<IntegerWithSource> ac_max_phase_count;
    std::optional<IntegerWithSource> ac_min_phase_count;
    std::optional<bool> ac_supports_changing_phases_during_charging;
    std::optional<int32_t> ac_number_of_active_phases;
};

struct LimitsRes {
    std::optional<NumberWithSource> total_power_W;
    std::optional<NumberWithSource> ac_max_current_A;
    std::optional<IntegerWithSource> ac_max_phase_count;
};

struct ScheduleReqEntry {
    std::string timestamp;
    LimitsReq limits_to_root;
    LimitsReq limits_to_leaves;
    std::optional<float> conversion_efficiency;
    std::optional<PricePerkWh> price_per_kwh;
};

struct ScheduleResEntry {
    std::string timestamp;
    LimitsRes limits_to_root;
    std::optional<PricePerkWh> price_per_kwh;
};

struct ScheduleSetpointEntry {
    std::string timestamp;
    std::optional<SetpointType> setpoint;
};

struct ExternalLimits {
    std::vector<ScheduleReqEntry> schedule_import;
    std::vector<ScheduleReqEntry> schedule_export;
    std::vector<ScheduleSetpointEntry> schedule_setpoints;
};

struct EnforcedLimits {
    std::string uuid;
    std::string valid_until;
    LimitsRes limits_root_side;
    std::vector<ScheduleResEntry> schedule;
};

} // namespace everest::lib::API::V1_0::types::energy
