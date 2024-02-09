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
    const auto hw_caps = mod->get_hw_capabilities();
    const auto tpnow = date::utc_clock::now();
    const auto tp =
        Everest::Date::to_rfc3339(date::floor<std::chrono::hours>(tpnow) + date::get_leap_second_info(tpnow).elapsed);

    entry_import.timestamp = tp;
    entry_import.limits_to_root.ac_max_phase_count = hw_caps.max_phase_count_import;
    entry_import.limits_to_root.ac_min_phase_count = hw_caps.min_phase_count_import;
    entry_import.limits_to_root.ac_max_current_A = hw_caps.max_current_A_import;
    entry_import.limits_to_root.ac_min_current_A = hw_caps.min_current_A_import;
    entry_import.limits_to_root.ac_supports_changing_phases_during_charging =
        hw_caps.supports_changing_phases_during_charging;
    entry_import.conversion_efficiency = mod->powersupply_capabilities.conversion_efficiency_export;
    energy_flow_request.schedule_import.emplace(std::vector<types::energy::ScheduleReqEntry>({entry_import}));
}

void energyImpl::clear_export_request_schedule() {
    types::energy::ScheduleReqEntry entry_export;
    const auto hw_caps = mod->get_hw_capabilities();
    const auto tpnow = date::utc_clock::now();
    const auto tp =
        Everest::Date::to_rfc3339(date::floor<std::chrono::hours>(tpnow) + date::get_leap_second_info(tpnow).elapsed);

    entry_export.timestamp = tp;
    entry_export.limits_to_root.ac_max_phase_count = hw_caps.max_phase_count_export;
    entry_export.limits_to_root.ac_min_phase_count = hw_caps.min_phase_count_export;
    entry_export.limits_to_root.ac_max_current_A = hw_caps.max_current_A_export;
    entry_export.limits_to_root.ac_min_current_A = hw_caps.min_current_A_export;
    entry_export.limits_to_root.ac_supports_changing_phases_during_charging =
        hw_caps.supports_changing_phases_during_charging;
    entry_export.conversion_efficiency = mod->powersupply_capabilities.conversion_efficiency_import;
    energy_flow_request.schedule_export.emplace(std::vector<types::energy::ScheduleReqEntry>({entry_export}));
}

void energyImpl::clear_request_schedules() {
    this->clear_import_request_schedule();
    this->clear_export_request_schedule();
}

void energyImpl::ready() {
    hw_caps = mod->get_hw_capabilities();
    clear_request_schedules();

    // request energy now
    request_energy_from_energy_manager();

    // request energy every second
    std::thread([this] {
        while (true) {
            request_energy_from_energy_manager();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }).detach();

    // request energy at the start and end of a charging session
    mod->charger->signal_state.connect([this](Charger::EvseState s) {
        if (s == Charger::EvseState::WaitingForAuthentication || s == Charger::EvseState::Finished) {
            std::thread request_energy_thread([this]() { request_energy_from_energy_manager(); });
            request_energy_thread.detach();
        }
    });
}

void energyImpl::request_energy_from_energy_manager() {
    auto s = mod->charger->get_current_state();
    {
        std::lock_guard<std::mutex> lock(this->energy_mutex);
        clear_import_request_schedule();
        clear_export_request_schedule();

        // If we need energy, copy local limit schedules to energy_flow_request.
        if (s == Charger::EvseState::Charging || s == Charger::EvseState::PrepareCharging ||
            s == Charger::EvseState::WaitingForEnergy || s == Charger::EvseState::WaitingForAuthentication ||
            s == Charger::EvseState::ChargingPausedEV || !mod->config.request_zero_power_in_idle) {

            // copy complete external limit schedules
            if (mod->getLocalEnergyLimits().schedule_import.has_value() &&
                !mod->getLocalEnergyLimits().schedule_import.value().empty()) {
                energy_flow_request.schedule_import = mod->getLocalEnergyLimits().schedule_import;
            }

            // apply our local hardware limits on root side
            for (auto& e : energy_flow_request.schedule_import.value()) {
                if (!e.limits_to_root.ac_max_current_A.has_value() ||
                    e.limits_to_root.ac_max_current_A.value() > hw_caps.max_current_A_import) {
                    e.limits_to_root.ac_max_current_A = hw_caps.max_current_A_import;

                    // are we in EV pause mode? -> Reduce requested current to minimum just to see when car
                    // wants to start charging again. The energy manager may pause us externally to reduce to
                    // zero
                    if (s == Charger::EvseState::ChargingPausedEV && mod->config.request_zero_power_in_idle) {
                        e.limits_to_root.ac_max_current_A = hw_caps.min_current_A_import;
                    }
                }

                if (!e.limits_to_root.ac_max_phase_count.has_value() ||
                    e.limits_to_root.ac_max_phase_count.value() > hw_caps.max_phase_count_import)
                    e.limits_to_root.ac_max_phase_count = hw_caps.max_phase_count_import;

                // copy remaining hw limits on root side
                e.limits_to_root.ac_min_phase_count = hw_caps.min_phase_count_import;
                e.limits_to_root.ac_min_current_A = hw_caps.min_current_A_import;
                e.limits_to_root.ac_supports_changing_phases_during_charging =
                    hw_caps.supports_changing_phases_during_charging;
            }

            if (mod->getLocalEnergyLimits().schedule_export.has_value() &&
                !mod->getLocalEnergyLimits().schedule_export.value().empty()) {
                energy_flow_request.schedule_export = mod->getLocalEnergyLimits().schedule_export;
            }

            // apply our local hardware limits on root side
            for (auto& e : energy_flow_request.schedule_export.value()) {
                if (!e.limits_to_root.ac_max_current_A.has_value() ||
                    e.limits_to_root.ac_max_current_A.value() > hw_caps.max_current_A_export) {
                    e.limits_to_root.ac_max_current_A = hw_caps.max_current_A_export;

                    // are we in EV pause mode? -> Reduce requested current to minimum just to see when car
                    // wants to start discharging again. The energy manager may pause us externally to reduce to
                    // zero
                    if (s == Charger::EvseState::ChargingPausedEV) {
                        e.limits_to_root.ac_max_current_A = hw_caps.min_current_A_export;
                    }
                }

                if (!e.limits_to_root.ac_max_phase_count.has_value() ||
                    e.limits_to_root.ac_max_phase_count.value() > hw_caps.max_phase_count_export)
                    e.limits_to_root.ac_max_phase_count = hw_caps.max_phase_count_export;

                // copy remaining hw limits on root side
                e.limits_to_root.ac_min_phase_count = hw_caps.min_phase_count_export;
                e.limits_to_root.ac_min_current_A = hw_caps.min_current_A_export;
                e.limits_to_root.ac_supports_changing_phases_during_charging =
                    hw_caps.supports_changing_phases_during_charging;
            }

            if (mod->config.charge_mode == "DC") {
                // For DC mode remove amp limit on leave side if any
                for (auto& e : energy_flow_request.schedule_import.value()) {
                    e.limits_to_leaves.ac_max_current_A.reset();
                }
                for (auto& e : energy_flow_request.schedule_export.value()) {
                    e.limits_to_leaves.ac_max_current_A.reset();
                }
            }

        } else {
            if (mod->config.charge_mode == "DC") {
                // we dont need power at the moment
                energy_flow_request.schedule_import.value()[0].limits_to_leaves.total_power_W = 0.;
                energy_flow_request.schedule_export.value()[0].limits_to_leaves.total_power_W = 0.;
            } else {
                energy_flow_request.schedule_import.value()[0].limits_to_leaves.ac_max_current_A = 0.;
                energy_flow_request.schedule_export.value()[0].limits_to_leaves.ac_max_current_A = 0.;
            }
        }

        publish_energy_flow_request(energy_flow_request);
        // EVLOG_info << "Outgoing request " << energy_flow_request;
    }
}

void energyImpl::handle_enforce_limits(types::energy::EnforcedLimits& value) {
    if (value.uuid == energy_flow_request.uuid) {
        // EVLOG_info << "Incoming enforce limits" << value;

        // publish for e.g. OCPP module
        mod->p_evse->publish_enforced_limits(value);

        //   set hardware limit
        float limit = 0.;
        int phasecount = (mod->getLocalThreePhases() ? 3 : 1);

        // apply enforced limits
        if (value.limits_root_side.has_value()) {
            // set enforced AC current limit
            if (value.limits_root_side.value().ac_max_current_A.has_value()) {
                limit = value.limits_root_side.value().ac_max_current_A.value();
            }

            // apply number of phase limit
            if (value.limits_root_side.value().ac_max_phase_count.has_value() &&
                value.limits_root_side.value().ac_max_phase_count.value() < phasecount) {
                if (mod->get_hw_capabilities().supports_changing_phases_during_charging) {
                    phasecount = value.limits_root_side.value().ac_max_phase_count.value();
                    mod->charger->switch_three_phases_while_charging(phasecount == 3);
                } else {
                    EVLOG_error << fmt::format("Energy manager limits #ph to {}, but switching phases during "
                                               "charging is not supported by HW.",
                                               value.limits_root_side.value().ac_max_phase_count.value());
                }
            }

            // apply watt limit
            if (value.limits_root_side.value().total_power_W.has_value()) {
                mod->mqtt.publish(fmt::format("everest_external/nodered/{}/state/max_watt", mod->config.connector_id),
                                  value.limits_root_side.value().total_power_W.value());

                float a =
                    value.limits_root_side.value().total_power_W.value() / mod->config.ac_nominal_voltage / phasecount;
                if (a < limit) {
                    limit = a;
                }
            }
        }

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
            if (value.limits_root_side.has_value() && value.limits_root_side.value().total_power_W.has_value()) {
                float watt_leave_side = value.limits_root_side.value().total_power_W.value();
                float ampere_root_side = value.limits_root_side.value().ac_max_current_A.value();

                auto ev_info = mod->get_ev_info();
                float target_voltage = ev_info.target_voltage.value_or(0.);
                float actual_voltage = ev_info.present_voltage.value_or(0.);

                // did the values change since the last call?
                if (last_enforced_limits.total_power_W != watt_leave_side ||
                    last_enforced_limits.ac_max_current_A != ampere_root_side ||
                    target_voltage != last_target_voltage || voltage_changed(actual_voltage, last_actual_voltage)) {

                    last_enforced_limits = value.limits_root_side.value();
                    last_target_voltage = target_voltage;
                    last_actual_voltage = actual_voltage;

                    // tell car our new limits
                    types::iso15118_charger::DC_EVSEMaximumLimits evseMaxLimits;

                    if (target_voltage > 10) {
                        // we use target_voltage here to calculate current limit.
                        // If target_voltage is a lot higher then the actual voltage the
                        // current limit is too low, i.e. charging will not reach the actual watt value.
                        // FIXME: we could use some magic here that involves actual measured voltage as well.
                        if (actual_voltage > 10) {
                            evseMaxLimits.EVSEMaximumCurrentLimit =
                                value.limits_root_side.value().total_power_W.value() / actual_voltage;
                        } else {
                            evseMaxLimits.EVSEMaximumCurrentLimit =
                                value.limits_root_side.value().total_power_W.value() / target_voltage;
                        }
                    } else {
                        evseMaxLimits.EVSEMaximumCurrentLimit = mod->powersupply_capabilities.max_export_current_A;
                    }

                    if (evseMaxLimits.EVSEMaximumCurrentLimit > mod->powersupply_capabilities.max_export_current_A)
                        evseMaxLimits.EVSEMaximumCurrentLimit = mod->powersupply_capabilities.max_export_current_A;

                    if (mod->powersupply_capabilities.max_import_current_A.has_value() &&
                        evseMaxLimits.EVSEMaximumCurrentLimit <
                            -mod->powersupply_capabilities.max_import_current_A.value())
                        evseMaxLimits.EVSEMaximumCurrentLimit =
                            -mod->powersupply_capabilities.max_import_current_A.value();

                    // now evseMaxLimits.EVSEMaximumCurrentLimit is between
                    // -max_import_current_A ... +max_export_current_A

                    evseMaxLimits.EVSEMaximumPowerLimit = value.limits_root_side.value().total_power_W.value();
                    if (evseMaxLimits.EVSEMaximumPowerLimit > mod->powersupply_capabilities.max_export_power_W)
                        evseMaxLimits.EVSEMaximumPowerLimit = mod->powersupply_capabilities.max_export_power_W;

                    if (mod->powersupply_capabilities.max_import_power_W.has_value() &&
                        evseMaxLimits.EVSEMaximumPowerLimit < -mod->powersupply_capabilities.max_import_power_W.value())
                        evseMaxLimits.EVSEMaximumPowerLimit = -mod->powersupply_capabilities.max_import_power_W.value();

                    // now evseMaxLimits.EVSEMaximumPowerLimit is between
                    // -max_import_power_W ... +max_export_power_W

                    evseMaxLimits.EVSEMaximumVoltageLimit = mod->powersupply_capabilities.max_export_voltage_V;

                    // FIXME: we tell the ISO stack positive numbers for DIN spec and ISO-2 here in case of exporting to
                    // grid. This needs to be fixed in the transition to -20 for BPT.
                    if (evseMaxLimits.EVSEMaximumPowerLimit < 0 || evseMaxLimits.EVSEMaximumCurrentLimit < 0) {
                        // we are exporting power back to the grid
                        if (mod->config.hack_allow_bpt_with_iso2) {
                            if (evseMaxLimits.EVSEMaximumPowerLimit < 0) {
                                evseMaxLimits.EVSEMaximumPowerLimit = -evseMaxLimits.EVSEMaximumPowerLimit;
                                mod->is_actually_exporting_to_grid = true;
                            } else {
                                mod->is_actually_exporting_to_grid = false;
                            }

                            if (evseMaxLimits.EVSEMaximumCurrentLimit < 0) {
                                evseMaxLimits.EVSEMaximumCurrentLimit = -evseMaxLimits.EVSEMaximumCurrentLimit;
                                mod->is_actually_exporting_to_grid = true;
                            } else {
                                mod->is_actually_exporting_to_grid = false;
                            }
                        } else if (mod->sae_bidi_active) {
                            if (evseMaxLimits.EVSEMaximumPowerLimit < 0) {
                                mod->is_actually_exporting_to_grid = true;
                            } else {
                                mod->is_actually_exporting_to_grid = false;
                            }
                            if (evseMaxLimits.EVSEMaximumCurrentLimit < 0) {
                                mod->is_actually_exporting_to_grid = true;
                            } else {
                                mod->is_actually_exporting_to_grid = false;
                            }
                        } else {
                            EVLOG_error << "Bidirectional export back to grid requested, but not supported. Enable "
                                           "ISO-20 or set hack_allow_bpt_with_iso2 config option.";
                            evseMaxLimits.EVSEMaximumPowerLimit = 0.;
                            evseMaxLimits.EVSEMaximumCurrentLimit = 0.;
                        }
                    } else {
                        mod->is_actually_exporting_to_grid = false;
                    }

                    session_log.evse(
                        true,
                        fmt::format("Change HLC Limits: {}W/{}A, target_voltage {}, actual_voltage {}, hack_bpt {}",
                                    evseMaxLimits.EVSEMaximumPowerLimit, evseMaxLimits.EVSEMaximumCurrentLimit,
                                    target_voltage, actual_voltage, mod->is_actually_exporting_to_grid));
                    mod->r_hlc[0]->call_update_dc_maximum_limits(evseMaxLimits);
                    mod->charger->inform_new_evse_max_hlc_limits(evseMaxLimits);

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
