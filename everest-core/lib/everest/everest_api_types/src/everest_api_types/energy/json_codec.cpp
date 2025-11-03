// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "energy/json_codec.hpp"
#include "energy/API.hpp"
#include "nlohmann/json.hpp"

namespace everest::lib::API::V1_0::types::energy {

void to_json(json& j, NumberWithSource const& k) noexcept {
    j = json{
        {"value", k.value},
        {"source", k.source},
    };
}

void from_json(const json& j, NumberWithSource& k) {
    k.value = j.at("value");
    k.source = j.at("source");
}

void to_json(json& j, IntegerWithSource const& k) noexcept {
    j = json{
        {"value", k.value},
        {"source", k.source},
    };
}

void from_json(const json& j, IntegerWithSource& k) {
    k.value = j.at("value");
    k.source = j.at("source");
}

void to_json(json& j, FrequencyWattPoint const& k) noexcept {
    j = json{
        {"frequency_Hz", k.frequency_Hz},
        {"total_power_W", k.total_power_W},
    };
}

void from_json(const json& j, FrequencyWattPoint& k) {
    k.frequency_Hz = j.at("frequency_Hz");
    k.total_power_W = j.at("total_power_W");
}

void to_json(json& j, SetpointType const& k) noexcept {
    j = json{
        {"priority", k.priority},
        {"source", k.source},
    };
    if (k.ac_current_A) {
        j["ac_current_A"] = k.ac_current_A.value();
    }
    if (k.total_power_W) {
        j["total_power_W"] = k.total_power_W.value();
    }
    if (k.frequency_table) {
        j["frequency_table"] = json::array();
        for (auto val : k.frequency_table.value()) {
            j["frequency_table"].push_back(val);
        }
    }
}

void from_json(const json& j, SetpointType& k) {
    k.priority = j.at("priority");
    k.source = j.at("source");

    if (j.contains("ac_current_A")) {
        k.ac_current_A.emplace(j.at("ac_current_A"));
    }
    if (j.contains("total_power_W")) {
        k.total_power_W.emplace(j.at("total_power_W"));
    }
    if (j.contains("frequency_table")) {
        json arr = j.at("frequency_table");
        std::vector<types::energy::FrequencyWattPoint> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.frequency_table.emplace(vec);
    }
}

void to_json(json& j, PricePerkWh const& k) noexcept {
    j = json{
        {"timestamp", k.timestamp},
        {"value", k.value},
        {"currency", k.currency},
    };
}

void from_json(const json& j, PricePerkWh& k) {
    k.timestamp = j.at("timestamp");
    k.value = j.at("value");
    k.currency = j.at("currency");
}

void to_json(json& j, LimitsReq const& k) noexcept {
    j = json({});
    // the optional parts of the type
    if (k.total_power_W) {
        j["total_power_W"] = k.total_power_W.value();
    }
    if (k.ac_max_current_A) {
        j["ac_max_current_A"] = k.ac_max_current_A.value();
    }
    if (k.ac_min_current_A) {
        j["ac_min_current_A"] = k.ac_min_current_A.value();
    }
    if (k.ac_max_phase_count) {
        j["ac_max_phase_count"] = k.ac_max_phase_count.value();
    }
    if (k.ac_min_phase_count) {
        j["ac_min_phase_count"] = k.ac_min_phase_count.value();
    }
    if (k.ac_supports_changing_phases_during_charging) {
        j["ac_supports_changing_phases_during_charging"] = k.ac_supports_changing_phases_during_charging.value();
    }
    if (k.ac_number_of_active_phases) {
        j["ac_number_of_active_phases"] = k.ac_number_of_active_phases.value();
    }
}

void from_json(const json& j, LimitsReq& k) {
    if (j.contains("total_power_W")) {
        k.total_power_W.emplace(j.at("total_power_W"));
    }
    if (j.contains("ac_max_current_A")) {
        k.ac_max_current_A.emplace(j.at("ac_max_current_A"));
    }
    if (j.contains("ac_min_current_A")) {
        k.ac_min_current_A.emplace(j.at("ac_min_current_A"));
    }
    if (j.contains("ac_max_phase_count")) {
        k.ac_max_phase_count.emplace(j.at("ac_max_phase_count"));
    }
    if (j.contains("ac_min_phase_count")) {
        k.ac_min_phase_count.emplace(j.at("ac_min_phase_count"));
    }
    if (j.contains("ac_supports_changing_phases_during_charging")) {
        k.ac_supports_changing_phases_during_charging.emplace(j.at("ac_supports_changing_phases_during_charging"));
    }
    if (j.contains("ac_number_of_active_phases")) {
        k.ac_number_of_active_phases.emplace(j.at("ac_number_of_active_phases"));
    }
}

void to_json(json& j, LimitsRes const& k) noexcept {
    j = json({});
    if (k.total_power_W) {
        j["total_power_W"] = k.total_power_W.value();
    }
    if (k.ac_max_current_A) {
        j["ac_max_current_A"] = k.ac_max_current_A.value();
    }
    if (k.ac_max_phase_count) {
        j["ac_max_phase_count"] = k.ac_max_phase_count.value();
    }
}

void from_json(const json& j, LimitsRes& k) {
    if (j.contains("total_power_W")) {
        k.total_power_W.emplace(j.at("total_power_W"));
    }
    if (j.contains("ac_max_current_A")) {
        k.ac_max_current_A.emplace(j.at("ac_max_current_A"));
    }
    if (j.contains("ac_max_phase_count")) {
        k.ac_max_phase_count.emplace(j.at("ac_max_phase_count"));
    }
}

void to_json(json& j, ScheduleReqEntry const& k) noexcept {
    j = json{
        {"timestamp", k.timestamp},
        {"limits_to_root", k.limits_to_root},
        {"limits_to_leaves", k.limits_to_leaves},
    };

    if (k.conversion_efficiency) {
        j["conversion_efficiency"] = k.conversion_efficiency.value();
    }
    if (k.price_per_kwh) {
        j["price_per_kwh"] = k.price_per_kwh.value();
    }
}

void from_json(const json& j, ScheduleReqEntry& k) {
    k.timestamp = j.at("timestamp");
    k.limits_to_root = j.at("limits_to_root");
    k.limits_to_leaves = j.at("limits_to_leaves");

    if (j.contains("conversion_efficiency")) {
        k.conversion_efficiency.emplace(j.at("conversion_efficiency"));
    }
    if (j.contains("price_per_kwh")) {
        k.price_per_kwh.emplace(j.at("price_per_kwh"));
    }
}

void to_json(json& j, ScheduleResEntry const& k) noexcept {
    j = json{
        {"timestamp", k.timestamp},
        {"limits_to_root", k.limits_to_root},
    };
    if (k.price_per_kwh) {
        j["price_per_kwh"] = k.price_per_kwh.value();
    }
}

void from_json(const json& j, ScheduleResEntry& k) {
    k.timestamp = j.at("timestamp");
    k.limits_to_root = j.at("limits_to_root");

    if (j.contains("price_per_kwh")) {
        k.price_per_kwh.emplace(j.at("price_per_kwh"));
    }
}

void to_json(json& j, ScheduleSetpointEntry const& k) noexcept {
    j = json{
        {"timestamp", k.timestamp},
    };
    if (k.setpoint) {
        j["setpoint"] = k.setpoint.value();
    }
}

void from_json(const json& j, ScheduleSetpointEntry& k) {
    k.timestamp = j.at("timestamp");
    if (j.contains("setpoint")) {
        k.setpoint.emplace(j.at("setpoint"));
    }
}

void to_json(json& j, ExternalLimits const& k) noexcept {
    j = json{
        {"schedule_import", k.schedule_import},
        {"schedule_export", k.schedule_export},
        {"schedule_setpoints", k.schedule_setpoints},
    };
}

void from_json(const json& j, ExternalLimits& k) {
    for (auto val : j.at("schedule_import")) {
        k.schedule_import.push_back(val);
    }
    for (auto val : j.at("schedule_export")) {
        k.schedule_export.push_back(val);
    }
    for (auto val : j.at("schedule_setpoints")) {
        k.schedule_setpoints.push_back(val);
    }
}

void to_json(json& j, EnforcedLimits const& k) noexcept {
    j = json{
        {"uuid", k.uuid},
        {"valid_for", k.valid_for},
        {"limits_root_side", k.limits_root_side},
        {"schedule", k.schedule},
    };
}

void from_json(const json& j, EnforcedLimits& k) {
    k.uuid = j.at("uuid");
    k.valid_for = j.at("valid_for");
    k.limits_root_side = j.at("limits_root_side");
    for (auto val : j.at("schedule")) {
        k.schedule.push_back(val);
    }
}

} // namespace everest::lib::API::V1_0::types::energy
