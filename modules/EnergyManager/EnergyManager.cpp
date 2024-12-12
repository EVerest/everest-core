// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest
#include "EnergyManager.hpp"
#include "Broker.hpp"
#include "BrokerFastCharging.hpp"
#include "Market.hpp"
#include <fmt/core.h>
#include <optional>

namespace module {

void EnergyManager::init() {

    EnergyManagerConfig energy_manager_config;
    energy_manager_config.nominal_ac_voltage = config.nominal_ac_voltage;
    energy_manager_config.update_interval = config.update_interval;
    energy_manager_config.schedule_interval_duration = config.schedule_interval_duration;
    energy_manager_config.schedule_total_duration = config.schedule_total_duration;
    energy_manager_config.slice_ampere = config.slice_ampere;
    energy_manager_config.slice_watt = config.slice_watt;
    energy_manager_config.debug = config.debug;
    energy_manager_config.switch_3ph1ph_while_charging_mode = config.switch_3ph1ph_while_charging_mode;
    energy_manager_config.switch_3ph1ph_max_nr_of_switches_per_session =
        config.switch_3ph1ph_max_nr_of_switches_per_session;
    energy_manager_config.switch_3ph1ph_switch_limit_stickyness = config.switch_3ph1ph_switch_limit_stickyness;
    energy_manager_config.switch_3ph1ph_power_hysteresis_W = config.switch_3ph1ph_power_hysteresis_W;
    energy_manager_config.switch_3ph1ph_time_hysteresis_s = config.switch_3ph1ph_time_hysteresis_s;

    const auto enforce_limits_callback = [this](const std::vector<types::energy::EnforcedLimits>& limits) {
        const types::energy::NumberWithSource nonumber = {-9999.0};
        const types::energy::IntegerWithSource noint = {-9999};
        for (const auto& it : limits) {
            if (globals.debug)
                EVLOG_info << fmt::format("\033[1;92m{} Enforce limits {}A {}W {} ph\033[1;0m", it.uuid,
                                          it.limits_root_side.ac_max_current_A.value_or(nonumber).value,
                                          it.limits_root_side.total_power_W.value_or(nonumber).value,
                                          it.limits_root_side.ac_max_phase_count.value_or(noint).value);
            r_energy_trunk->call_enforce_limits(it);
        }
    };

    this->impl = std::make_unique<EnergyManagerImpl>(energy_manager_config, enforce_limits_callback);

    r_energy_trunk->subscribe_energy_flow_request(
        [this](types::energy::EnergyFlowRequest e) { this->impl->on_energy_flow_request(e); });

    invoke_init(*p_main);
}

void EnergyManager::ready() {
    invoke_ready(*p_main);

    this->impl->start();
}

} // namespace module
