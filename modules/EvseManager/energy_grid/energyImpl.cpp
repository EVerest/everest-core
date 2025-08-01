// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "energyImpl.hpp"
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <date/date.h>
#include <date/tz.h>
#include <fmt/core.h>
#include <string>
#include <utils/date.hpp>
#include <utils/formatter.hpp>

namespace module {
namespace energy_grid {

// helper to find out if voltage changed (more then noise)
static bool voltage_changed(float a, float b) {
    constexpr float noise_voltage = 1;
    return (fabs(a - b) > noise_voltage);
}

void energyImpl::init() {

    charger_state = Charger::EvseState::Disabled;

    source_base = mod->info.id;
    source_bsp_caps = source_base + "/evse_board_support_caps";
    source_psu_caps = source_base + "/powersupply_dc_caps";

    std::srand((unsigned)time(0));

    // UUID must be unique also beyond this charging station -> will be handled on framework level and above later
    energy_flow_request.uuid = mod->info.id;
    energy_flow_request.node_type = types::energy::NodeType::Evse;

    if (mod->r_powermeter_car_side.size()) {
        mod->r_powermeter_car_side[0]->subscribe_powermeter([this](types::powermeter::Powermeter p) {
            // Received new power meter values, update our energy object.
            std::lock_guard<std::mutex> lock(this->energy_mutex);
            energy_flow_request.energy_usage_leaves = p;
        });
    }

    if (mod->r_powermeter_grid_side.size()) {
        mod->r_powermeter_grid_side[0]->subscribe_powermeter([this](types::powermeter::Powermeter p) {
            // Received new power meter values, update our energy object.
            std::lock_guard<std::mutex> lock(this->energy_mutex);
            energy_flow_request.energy_usage_root = p;
        });
    }
}

void energyImpl::clear_import_request_schedule() {
    types::energy::ScheduleReqEntry entry_import;
    const auto tpnow = date::utc_clock::now();
    const auto tp =
        Everest::Date::to_rfc3339(date::floor<std::chrono::hours>(tpnow) + date::get_leap_second_info(tpnow).elapsed);

    entry_import.timestamp = tp;
    entry_import.limits_to_root.ac_max_phase_count = {hw_caps.max_phase_count_import};
    entry_import.limits_to_root.ac_min_phase_count = {hw_caps.min_phase_count_import};
    entry_import.limits_to_root.ac_max_current_A = {hw_caps.max_current_A_import};
    entry_import.limits_to_root.ac_min_current_A = {hw_caps.min_current_A_import};
    entry_import.limits_to_root.ac_supports_changing_phases_during_charging =
        hw_caps.supports_changing_phases_during_charging;
    entry_import.limits_to_root.ac_number_of_active_phases = mod->ac_nr_phases_active;

    if (mod->config.charge_mode == "DC") {
        // For DC, apply our power supply capabilities as limit on leaves side
        const auto caps = mod->get_powersupply_capabilities();
        entry_import.limits_to_leaves.total_power_W = {caps.max_export_power_W,
                                                       source_base + "/clear_import_request_schedule"};
        // Note both sides are optionals
        entry_import.conversion_efficiency = caps.conversion_efficiency_export;
    }

    energy_flow_request.schedule_import = std::vector<types::energy::ScheduleReqEntry>({entry_import});
}

void energyImpl::clear_export_request_schedule() {
    types::energy::ScheduleReqEntry entry_export;
    const auto tpnow = date::utc_clock::now();
    const auto tp =
        Everest::Date::to_rfc3339(date::floor<std::chrono::hours>(tpnow) + date::get_leap_second_info(tpnow).elapsed);

    entry_export.timestamp = tp;
    entry_export.limits_to_root.ac_max_phase_count = {hw_caps.max_phase_count_export, source_bsp_caps};
    entry_export.limits_to_root.ac_min_phase_count = {hw_caps.min_phase_count_export, source_bsp_caps};
    entry_export.limits_to_root.ac_max_current_A = {hw_caps.max_current_A_export, source_bsp_caps};
    entry_export.limits_to_root.ac_min_current_A = {hw_caps.min_current_A_export, source_bsp_caps};
    entry_export.limits_to_root.ac_supports_changing_phases_during_charging =
        hw_caps.supports_changing_phases_during_charging;
    entry_export.limits_to_root.ac_number_of_active_phases = mod->ac_nr_phases_active;

    if (mod->config.charge_mode == "DC") {
        // For DC, apply our power supply capabilities as limit on leaves side
        const auto caps = mod->get_powersupply_capabilities();
        entry_export.limits_to_leaves.total_power_W = {caps.max_import_power_W.value_or(0), source_psu_caps};
        // Note that both sides are optionals
        entry_export.conversion_efficiency = caps.conversion_efficiency_import;
    }

    energy_flow_request.schedule_export = std::vector<types::energy::ScheduleReqEntry>({entry_export});
}

void energyImpl::clear_request_schedules() {
    this->clear_import_request_schedule();
    this->clear_export_request_schedule();
}

void energyImpl::ready() {
    hw_caps = mod->get_hw_capabilities();
    clear_request_schedules();

    // request energy now
    request_energy_from_energy_manager(true);

    // request energy every second
    std::thread([this] {
        while (true) {
            hw_caps = mod->get_hw_capabilities();
            request_energy_from_energy_manager(false);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }).detach();

    // request energy at the start and end of a charging session
    mod->charger->signal_state.connect([this](Charger::EvseState s) {
        charger_state = s;
        if (s == Charger::EvseState::WaitingForAuthentication || s == Charger::EvseState::Finished) {
            std::thread request_energy_thread([this]() { request_energy_from_energy_manager(true); });
            request_energy_thread.detach();
        }
    });
}

types::energy::EvseState to_energy_evse_state(const Charger::EvseState charger_state) {
    switch (charger_state) {
    case Charger::EvseState::Disabled:
        return types::energy::EvseState::Disabled;
        break;
    case Charger::EvseState::Idle:
        return types::energy::EvseState::Unplugged;
        break;
    case Charger::EvseState::WaitingForAuthentication:
        return types::energy::EvseState::WaitForAuth;
        break;
    case Charger::EvseState::PrepareCharging:
        return types::energy::EvseState::PrepareCharging;
        break;
    case Charger::EvseState::WaitingForEnergy:
        return types::energy::EvseState::WaitForEnergy;
        break;
    case Charger::EvseState::Charging:
        return types::energy::EvseState::Charging;
        break;
    case Charger::EvseState::ChargingPausedEV:
        return types::energy::EvseState::PausedEV;
        break;
    case Charger::EvseState::ChargingPausedEVSE:
        return types::energy::EvseState::PausedEVSE;
        break;
    case Charger::EvseState::StoppingCharging:
        return types::energy::EvseState::Finished;
        break;
    case Charger::EvseState::Finished:
        return types::energy::EvseState::Finished;
        break;
    case Charger::EvseState::T_step_EF:
        return types::energy::EvseState::PrepareCharging;
        break;
    case Charger::EvseState::T_step_X1:
        return types::energy::EvseState::PrepareCharging;
        break;
    case Charger::EvseState::SwitchPhases:
        return types::energy::EvseState::Charging;
        break;
    case Charger::EvseState::Replug:
        return types::energy::EvseState::PrepareCharging;
        break;
    }
    return types::energy::EvseState::Disabled;
}

void energyImpl::request_energy_from_energy_manager(bool priority_request) {
    std::lock_guard<std::mutex> lock(this->energy_mutex);
    clear_import_request_schedule();
    clear_export_request_schedule();

    // If we need energy, copy local limit schedules to energy_flow_request.
    if (charger_state == Charger::EvseState::Charging || charger_state == Charger::EvseState::PrepareCharging ||
        charger_state == Charger::EvseState::WaitingForEnergy ||
        charger_state == Charger::EvseState::WaitingForAuthentication ||
        charger_state == Charger::EvseState::ChargingPausedEV || !mod->config.request_zero_power_in_idle) {

        // copy complete external limit schedules for import
        if (not mod->get_local_energy_limits().schedule_import.empty()) {
            energy_flow_request.schedule_import = mod->get_local_energy_limits().schedule_import;

            if (mod->config.charge_mode == "DC") {
                // For DC, apply our power supply capabilities as an additional limit on leaves side
                const auto caps = mod->get_powersupply_capabilities();
                for (auto& entry : energy_flow_request.schedule_import) {
                    // Apply caps limit if we request a leaves side watt value and it is larger than the capabilities
                    if (entry.limits_to_leaves.total_power_W.has_value() and
                        entry.limits_to_leaves.total_power_W.value().value > caps.max_export_power_W) {
                        entry.limits_to_leaves.total_power_W = {caps.max_export_power_W, source_psu_caps};
                    }
                }
            }
        }

        // apply our local hardware limits on root side
        for (auto& e : energy_flow_request.schedule_import) {
            if (!e.limits_to_root.ac_max_current_A.has_value() ||
                e.limits_to_root.ac_max_current_A.value().value > hw_caps.max_current_A_import) {
                e.limits_to_root.ac_max_current_A = {hw_caps.max_current_A_import, source_bsp_caps};

                // are we in EV pause mode? -> Reduce requested current to minimum just to see when car
                // wants to start charging again. The energy manager may pause us externally to reduce to
                // zero
                if (charger_state == Charger::EvseState::ChargingPausedEV && mod->config.request_zero_power_in_idle) {
                    e.limits_to_root.ac_max_current_A = {hw_caps.min_current_A_import, source_bsp_caps};
                }
            }

            if (!e.limits_to_root.ac_max_phase_count.has_value() ||
                e.limits_to_root.ac_max_phase_count.value().value > hw_caps.max_phase_count_import)
                e.limits_to_root.ac_max_phase_count = {hw_caps.max_phase_count_import, source_bsp_caps};

            // copy remaining hw limits on root side
            e.limits_to_root.ac_min_phase_count = {hw_caps.min_phase_count_import, source_bsp_caps};
            e.limits_to_root.ac_min_current_A = {hw_caps.min_current_A_import, source_bsp_caps};
            e.limits_to_root.ac_supports_changing_phases_during_charging =
                hw_caps.supports_changing_phases_during_charging;
            e.limits_to_root.ac_number_of_active_phases = mod->ac_nr_phases_active;
        }

        // copy complete external limit schedules for export
        if (not mod->get_local_energy_limits().schedule_export.empty()) {
            energy_flow_request.schedule_export = mod->get_local_energy_limits().schedule_export;

            if (mod->config.charge_mode == "DC") {
                // For DC, apply our power supply capabilities as an additional limit on leaves side
                const auto caps = mod->get_powersupply_capabilities();
                for (auto& entry : energy_flow_request.schedule_export) {
                    if (entry.limits_to_leaves.total_power_W.has_value() and caps.max_import_power_W.has_value() and
                        entry.limits_to_leaves.total_power_W.value().value > caps.max_import_power_W.value()) {
                        entry.limits_to_leaves.total_power_W = {caps.max_import_power_W.value(), source_bsp_caps};
                    }
                }
            }
        }

        // apply our local hardware limits on root side
        for (auto& e : energy_flow_request.schedule_export) {
            if (!e.limits_to_root.ac_max_current_A.has_value() ||
                e.limits_to_root.ac_max_current_A.value().value > hw_caps.max_current_A_export) {
                e.limits_to_root.ac_max_current_A = {hw_caps.max_current_A_export, source_bsp_caps};

                // are we in EV pause mode? -> Reduce requested current to minimum just to see when car
                // wants to start discharging again. The energy manager may pause us externally to reduce to
                // zero
                if (charger_state == Charger::EvseState::ChargingPausedEV) {
                    e.limits_to_root.ac_max_current_A = {hw_caps.min_current_A_export, source_bsp_caps + "_pause"};
                }
            }

            if (!e.limits_to_root.ac_max_phase_count.has_value() ||
                e.limits_to_root.ac_max_phase_count.value().value > hw_caps.max_phase_count_export)
                e.limits_to_root.ac_max_phase_count = {hw_caps.max_phase_count_export, source_bsp_caps};

            // copy remaining hw limits on root side
            e.limits_to_root.ac_min_phase_count = {hw_caps.min_phase_count_export, source_bsp_caps};
            e.limits_to_root.ac_min_current_A = {hw_caps.min_current_A_export, source_bsp_caps};
            e.limits_to_root.ac_supports_changing_phases_during_charging =
                hw_caps.supports_changing_phases_during_charging;
            e.limits_to_root.ac_number_of_active_phases = mod->ac_nr_phases_active;
        }

        if (mod->config.charge_mode == "DC") {
            // For DC mode remove amp limit on leave side if any
            for (auto& e : energy_flow_request.schedule_import) {
                e.limits_to_leaves.ac_max_current_A.reset();
            }
            for (auto& e : energy_flow_request.schedule_export) {
                e.limits_to_leaves.ac_max_current_A.reset();
            }
        }
    } else {
        if (mod->config.charge_mode == "DC") {
            // we dont need power at the moment
            energy_flow_request.schedule_import[0].limits_to_leaves.total_power_W = {0., "Idle"};
            energy_flow_request.schedule_export[0].limits_to_leaves.total_power_W = {0., "Idle"};
        } else {
            energy_flow_request.schedule_import[0].limits_to_leaves.ac_max_current_A = {0., "Idle"};
            energy_flow_request.schedule_export[0].limits_to_leaves.ac_max_current_A = {0., "Idle"};
        }
    }

    if (priority_request) {
        energy_flow_request.priority_request = true;
    } else {
        energy_flow_request.priority_request = false;
    }

    // Attach our state
    energy_flow_request.evse_state = to_energy_evse_state(charger_state);

    publish_energy_flow_request(energy_flow_request);
    // EVLOG_info << "Outgoing request " << energy_flow_request;
}

static bool almost_eq(float a, float b) {
    return a > b - 0.1 and a < b + 0.1;
}

// This is the decision logic when limits are changing.
bool energyImpl::random_delay_needed(float last_limit, float limit) {

    if (mod->config.uk_smartcharging_random_delay_at_any_change) {
        if (not almost_eq(last_limit, limit)) {
            return true;
        }
    } else {
        if (almost_eq(last_limit, 0.) and limit > 0.1) {
            return true;
        } else if (last_limit > 0.1 and almost_eq(limit, 0.)) {
            return true;
        }
    }

    // Are we starting up with a car attached? This will need a random delay as well
    if ((charger_state == Charger::EvseState::PrepareCharging or charger_state == Charger::EvseState::Charging or
         charger_state == Charger::EvseState::WaitingForAuthentication or
         charger_state == Charger::EvseState::WaitingForEnergy) and
        std::chrono::steady_clock::now() - mod->timepoint_ready_for_charging.load() <
            detect_startup_with_ev_attached_duration) {
        last_enforced_limit = 0.;
        return true;
    }

    return false;
}

void energyImpl::handle_enforce_limits(types::energy::EnforcedLimits& value) {
    if (value.uuid == energy_flow_request.uuid) {
        // EVLOG_info << "Incoming enforce limits" << value;

        // publish for e.g. OCPP module
        mod->p_evse->publish_enforced_limits(value);

        //   set hardware limit
        float limit = 0.;
        int active_phasecount = mod->ac_nr_phases_active;

        // apply enforced limits

        // set enforced AC current limit
        if (value.limits_root_side.ac_max_current_A.has_value()) {
            limit = value.limits_root_side.ac_max_current_A.value().value;
        }

        // apply number of phase limit
        if (value.limits_root_side.ac_max_phase_count.has_value() &&
            value.limits_root_side.ac_max_phase_count.value().value not_eq active_phasecount) {
            if (mod->get_hw_capabilities().supports_changing_phases_during_charging) {
                if (mod->charger->switch_three_phases_while_charging(
                        value.limits_root_side.ac_max_phase_count.value().value == 3)) {
                    mod->ac_nr_phases_active = value.limits_root_side.ac_max_phase_count.value().value;
                    EVLOG_info << fmt::format("3ph/1ph: Switching #ph from {} to {}", active_phasecount,
                                              value.limits_root_side.ac_max_phase_count.value().value);
                } else {
                    EVLOG_warning << fmt::format(
                        "3ph/1ph: Energymanager requests switching #ph from {} to {}, ignored.", active_phasecount,
                        value.limits_root_side.ac_max_phase_count.value().value);
                }
            } else {
                EVLOG_error << fmt::format(
                    "Energy manager requests switching #ph from {} to {}, but switching phases during "
                    "charging is not supported by HW.",
                    active_phasecount, value.limits_root_side.ac_max_phase_count.value().value);
            }
        }

        // apply watt limit
        if (value.limits_root_side.total_power_W.has_value()) {
            mod->mqtt.publish(fmt::format("everest_external/nodered/{}/state/max_watt", mod->config.connector_id),
                              value.limits_root_side.total_power_W.value().value);

            float a = value.limits_root_side.total_power_W.value().value / mod->config.ac_nominal_voltage /
                      mod->ac_nr_phases_active;
            if (a < limit) {
                limit = a;
            }
        }

        auto enforced_limit = limit;

        // check if we need to add a random delay for UK smart charging regs
        if (mod->random_delay_enabled) {

            // Are we in a state where a random delay makes sense?
            if (not(charger_state == Charger::EvseState::PrepareCharging or
                    charger_state == Charger::EvseState::Charging or
                    charger_state == Charger::EvseState::WaitingForAuthentication or
                    charger_state == Charger::EvseState::WaitingForEnergy)) {
                mod->random_delay_running = false;
            }

            // Do we need to start a new random delay?
            // Ignore changes of less then 0.1 amps
            if (not mod->random_delay_running and random_delay_needed(last_enforced_limit, limit)) {
                mod->random_delay_running = true;
                mod->random_delay_start_time = date::utc_clock::now();
                auto random_delay_s = std::rand() % mod->random_delay_max_duration.load().count();
                mod->random_delay_end_time = std::chrono::steady_clock::now() + std::chrono::seconds(random_delay_s);
                EVLOG_info << "UK Smart Charging regulations: Starting random delay of " << random_delay_s << "s";
                limit_when_random_delay_started = last_enforced_limit;
            }

            // If a delay is running, replace the current limit with the stored value
            if (mod->random_delay_running) {
                // use limit from the time point when the random delay started
                limit = limit_when_random_delay_started;
                // publish the current random delay timer
                auto seconds_left = std::chrono::duration_cast<std::chrono::seconds>(mod->random_delay_end_time -
                                                                                     std::chrono::steady_clock::now())
                                        .count();
                types::uk_random_delay::CountDown c;
                c.current_limit_after_delay_A = enforced_limit;
                c.current_limit_during_delay_A = limit_when_random_delay_started;
                if (seconds_left <= 0) {
                    EVLOG_info << "UK Smart Charging regulations: Random delay elapsed.";
                    c.countdown_s = 0;
                    mod->random_delay_running = false;
                } else {
                    EVLOG_debug << "Random delay running, " << seconds_left
                                << "s left. Applying the limit before the random delay ("
                                << limit_when_random_delay_started << "A) instead of requested limit ("
                                << enforced_limit << "A)";
                    c.countdown_s = seconds_left;
                    c.start_time = Everest::Date::to_rfc3339(mod->random_delay_start_time);
                }
                mod->p_random_delay->publish_countdown(c);
            } else {
                types::uk_random_delay::CountDown c;
                c.countdown_s = 0;
                c.current_limit_after_delay_A = enforced_limit;
                c.current_limit_during_delay_A = limit_when_random_delay_started;
                mod->p_random_delay->publish_countdown(c);
            }
        }

        last_enforced_limit = enforced_limit;

        // update limit at the charger
        if (limit >= 0) {
            // import
            mod->charger->set_max_current(limit, Everest::Date::from_rfc3339(value.valid_until));
        } else {
            // export
            // FIXME: we cannot discharge on PWM charging or with -2, so we fake a charging current here.
            // this needs to be fixed in transition to -20 AC bidirectional.
            mod->charger->set_max_current(0, Everest::Date::from_rfc3339(value.valid_until));
        }

        if (limit > 1e-5 || limit < -1e-5)
            mod->charger->resume_charging_power_available();

        if (mod->config.charge_mode == "DC") {
            // DC mode apply limit at the leave side, we get root side limits here from EnergyManager on ACDC!
            // FIXME: multiply by conversion_efficiency here!
            if (value.limits_root_side.total_power_W.has_value() and
                value.limits_root_side.ac_max_current_A.has_value()) {
                float watt_leave_side = value.limits_root_side.total_power_W.value().value;
                float ampere_root_side = value.limits_root_side.ac_max_current_A.value().value;

                auto ev_info = mod->get_ev_info();
                float target_voltage = ev_info.target_voltage.value_or(0.);
                float actual_voltage = ev_info.present_voltage.value_or(0.);

                bool values_changed = true;

                // did the values change since the last call?
                if (almost_eq(last_enforced_limits_watt, watt_leave_side) and
                    almost_eq(last_enforced_limits_ampere, ampere_root_side) and
                    almost_eq(target_voltage, last_target_voltage) and
                    not voltage_changed(actual_voltage, last_actual_voltage)) {
                    values_changed = false;
                }

                if (values_changed) {
                    last_enforced_limits_ampere = ampere_root_side;
                    last_enforced_limits_watt = watt_leave_side;
                    last_target_voltage = target_voltage;
                    last_actual_voltage = actual_voltage;

                    auto powersupply_capabilities = mod->get_powersupply_capabilities();

                    // tell car our new limits
                    types::iso15118::DcEvseMaximumLimits evse_max_limits;

                    if (target_voltage > 10) {
                        // we use target_voltage here to calculate current limit.
                        // If target_voltage is a lot higher then the actual voltage the
                        // current limit is too low, i.e. charging will not reach the actual watt value.
                        // FIXME: we could use some magic here that involves actual measured voltage as well.
                        if (actual_voltage > 10) {
                            evse_max_limits.evse_maximum_current_limit =
                                value.limits_root_side.total_power_W.value().value / actual_voltage;
                        } else {
                            evse_max_limits.evse_maximum_current_limit =
                                value.limits_root_side.total_power_W.value().value / target_voltage;
                        }
                    } else {
                        evse_max_limits.evse_maximum_current_limit = powersupply_capabilities.max_export_current_A;
                    }

                    if (evse_max_limits.evse_maximum_current_limit > powersupply_capabilities.max_export_current_A)
                        evse_max_limits.evse_maximum_current_limit = powersupply_capabilities.max_export_current_A;

                    if (powersupply_capabilities.max_import_current_A.has_value() &&
                        evse_max_limits.evse_maximum_current_limit <
                            -powersupply_capabilities.max_import_current_A.value())
                        evse_max_limits.evse_maximum_current_limit =
                            -powersupply_capabilities.max_import_current_A.value();

                    // now evse_max_limits.evse_maximum_current_limit is between
                    // -max_import_current_A ... +max_export_current_A

                    evse_max_limits.evse_maximum_power_limit = value.limits_root_side.total_power_W.value().value;
                    if (evse_max_limits.evse_maximum_power_limit > powersupply_capabilities.max_export_power_W)
                        evse_max_limits.evse_maximum_power_limit = powersupply_capabilities.max_export_power_W;

                    if (powersupply_capabilities.max_import_power_W.has_value() &&
                        evse_max_limits.evse_maximum_power_limit < -powersupply_capabilities.max_import_power_W.value())
                        evse_max_limits.evse_maximum_power_limit = -powersupply_capabilities.max_import_power_W.value();

                    // now evse_max_limits.evse_maximum_power_limit is between
                    // -max_import_power_W ... +max_export_power_W

                    evse_max_limits.evse_maximum_voltage_limit = powersupply_capabilities.max_export_voltage_V;

                    // FIXME: we tell the ISO stack positive numbers for DIN spec and ISO-2 here in case of exporting to
                    // grid. This needs to be fixed in the transition to -20 for BPT.
                    if (evse_max_limits.evse_maximum_power_limit < 0 ||
                        evse_max_limits.evse_maximum_current_limit < 0) {
                        // we are exporting power back to the grid
                        if (mod->config.hack_allow_bpt_with_iso2) {
                            if (evse_max_limits.evse_maximum_power_limit < 0) {
                                evse_max_limits.evse_maximum_power_limit = -evse_max_limits.evse_maximum_power_limit;
                                mod->is_actually_exporting_to_grid = true;
                            } else {
                                mod->is_actually_exporting_to_grid = false;
                            }

                            if (evse_max_limits.evse_maximum_current_limit < 0) {
                                evse_max_limits.evse_maximum_current_limit =
                                    -evse_max_limits.evse_maximum_current_limit;
                                mod->is_actually_exporting_to_grid = true;
                            } else {
                                mod->is_actually_exporting_to_grid = false;
                            }
                        } else if (mod->sae_bidi_active) {
                            if (evse_max_limits.evse_maximum_power_limit < 0) {
                                mod->is_actually_exporting_to_grid = true;
                            } else {
                                mod->is_actually_exporting_to_grid = false;
                            }
                            if (evse_max_limits.evse_maximum_current_limit < 0) {
                                mod->is_actually_exporting_to_grid = true;
                            } else {
                                mod->is_actually_exporting_to_grid = false;
                            }
                        } else {
                            EVLOG_error << "Bidirectional export back to grid requested, but not supported. Enable "
                                           "ISO-20 or set hack_allow_bpt_with_iso2 config option.";
                            evse_max_limits.evse_maximum_power_limit = 0.;
                            evse_max_limits.evse_maximum_current_limit = 0.;
                        }
                    } else {
                        mod->is_actually_exporting_to_grid = false;
                    }

                    session_log.evse(
                        true, fmt::format(
                                  "Change HLC Limits: {}W/{}A, target_voltage {}, actual_voltage {}, hack_bpt {}",
                                  evse_max_limits.evse_maximum_power_limit, evse_max_limits.evse_maximum_current_limit,
                                  target_voltage, actual_voltage, mod->is_actually_exporting_to_grid));
                    mod->r_hlc[0]->call_update_dc_maximum_limits(evse_max_limits);
                    mod->charger->inform_new_evse_max_hlc_limits(evse_max_limits);

                    // This is just neccessary to switch between charging and discharging
                    if (target_voltage > 0) {
                        mod->apply_new_target_voltage_current();
                    }

                    // Note: If the limits are lower then before, we could tell the DC power supply to
                    // ramp down already here instead of waiting for the car to request less power.
                    // Some cars may not like it, so we wait for the car to request less for now.
                }
            }
        }
    }
}

} // namespace energy_grid
} // namespace module
