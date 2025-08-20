// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef TESTS_ENERGY_MANAGER_CONFIG_JSON_HPP
#define TESTS_ENERGY_MANAGER_CONFIG_JSON_HPP

#include "EnergyManagerImpl.hpp"
#include <nlohmann/json.hpp>

NLOHMANN_JSON_NAMESPACE_BEGIN

template <> struct adl_serializer<module::EnergyManagerConfig> {
    static void to_json(json& j, const module::EnergyManagerConfig& config) {
        j = {
            {"nominal_ac_voltage", config.nominal_ac_voltage},
            {"update_interval", config.update_interval},
            {"schedule_interval_duration", config.schedule_interval_duration},
            {"schedule_total_duration", config.schedule_total_duration},
            {"slice_ampere", config.slice_ampere},
            {"slice_watt", config.slice_watt},
            {"debug", config.debug},
            {"switch_3ph1ph_while_charging_mode", config.switch_3ph1ph_while_charging_mode},
            {"switch_3ph1ph_max_nr_of_switches_per_session", config.switch_3ph1ph_max_nr_of_switches_per_session},
            {"switch_3ph1ph_switch_limit_stickyness", config.switch_3ph1ph_switch_limit_stickyness},
            {"switch_3ph1ph_power_hysteresis_W", config.switch_3ph1ph_power_hysteresis_W},
            {"switch_3ph1ph_time_hysteresis_s", config.switch_3ph1ph_time_hysteresis_s},
        };
    }
    static module::EnergyManagerConfig from_json(const json& j) {
        return {
            j.at("nominal_ac_voltage"),
            j.at("update_interval"),
            j.at("schedule_interval_duration"),
            j.at("schedule_total_duration"),
            j.at("slice_ampere"),
            j.at("slice_watt"),
            j.at("debug"),
            j.at("switch_3ph1ph_while_charging_mode"),
            j.at("switch_3ph1ph_max_nr_of_switches_per_session"),
            j.at("switch_3ph1ph_switch_limit_stickyness"),
            j.at("switch_3ph1ph_power_hysteresis_W"),
            j.at("switch_3ph1ph_time_hysteresis_s"),
        };
    }
};

NLOHMANN_JSON_NAMESPACE_END
#endif // TESTS_ENERGY_MANAGER_CONFIG_JSON_HPP
