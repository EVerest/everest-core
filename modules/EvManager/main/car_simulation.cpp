// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "car_simulation.hpp"

#include "constants.hpp"

#include <everest/logging.hpp>

void CarSimulation::state_machine() {
    using types::ev_board_support::EvCpState;

    const auto state_has_changed = sim_data.state != sim_data.last_state;
    sim_data.last_state = sim_data.state;

    switch (sim_data.state) {
    case SimState::UNPLUGGED:
        if (state_has_changed) {

            r_ev_board_support->call_set_cp_state(EvCpState::A);
            r_ev_board_support->call_allow_power_on(false);
            // Wait for physical plugin (ev BSP sees state A on CP and not Disconnected)

            sim_data.slac_state = "UNMATCHED";
            if (!r_ev.empty()) {
                r_ev[0]->call_stop_charging();
            }
        }
        break;
    case SimState::PLUGGED_IN:
        if (state_has_changed) {
            r_ev_board_support->call_set_cp_state(EvCpState::B);
            r_ev_board_support->call_allow_power_on(false);
        }
        break;
    case SimState::CHARGING_REGULATED:
        if (state_has_changed || sim_data.pwm_duty_cycle != sim_data.last_pwm_duty_cycle) {
            sim_data.last_pwm_duty_cycle = sim_data.pwm_duty_cycle;
            // do not draw power if EVSE paused by stopping PWM
            if (sim_data.pwm_duty_cycle > 7.0 && sim_data.pwm_duty_cycle < 97.0) {
                r_ev_board_support->call_set_cp_state(EvCpState::C);
                r_ev_board_support->call_allow_power_on(true);
            } else {
                r_ev_board_support->call_set_cp_state(EvCpState::B);
                r_ev_board_support->call_allow_power_on(false);
            }
        }
        break;
    case SimState::CHARGING_FIXED:
        // Todo(sl): What to do here
        if (state_has_changed) {
            // Also draw power if EVSE stopped PWM - this is a break the rules simulator->mode to test the charging
            // implementation!
            r_ev_board_support->call_set_cp_state(EvCpState::C);
            r_ev_board_support->call_allow_power_on(true);
        }
        break;

    case SimState::ERROR_E:
        if (state_has_changed) {
            r_ev_board_support->call_set_cp_state(EvCpState::E);
            r_ev_board_support->call_allow_power_on(false);
        }
        break;
    case SimState::DIODE_FAIL:
        if (state_has_changed) {
            r_ev_board_support->call_diode_fail(true);
            r_ev_board_support->call_allow_power_on(false);
        }
        break;
    case SimState::ISO_POWER_READY:
        if (state_has_changed) {
            r_ev_board_support->call_set_cp_state(EvCpState::C);
        }
        break;
    case SimState::ISO_CHARGING_REGULATED:
        if (state_has_changed) {
            r_ev_board_support->call_set_cp_state(EvCpState::C);
            r_ev_board_support->call_allow_power_on(true);
        }
        break;
    case SimState::BCB_TOGGLE:
        if (sim_data.bcb_toggle_C) {
            r_ev_board_support->call_set_cp_state(EvCpState::C);
            sim_data.bcb_toggle_C = false;
        } else {
            r_ev_board_support->call_set_cp_state(EvCpState::B);
            sim_data.bcb_toggle_C = true;
            ++sim_data.bcb_toggles;
        }
        break;
    default:
        sim_data.state = SimState::UNPLUGGED;
        break;
    }
};

bool CarSimulation::sleep(const CmdArguments& arguments, size_t loop_interval_ms) {
    if (not sim_data.sleep_ticks_left.has_value()) {
        const auto sleep_time = std::stold(arguments[0]);
        const auto sleep_time_ms = sleep_time * 1000;
        sim_data.sleep_ticks_left = static_cast<long long>(sleep_time_ms / loop_interval_ms) + 1;
    }
    auto& sleep_ticks_left = sim_data.sleep_ticks_left.value();
    sleep_ticks_left -= 1;
    if (not(sleep_ticks_left > 0)) {
        sim_data.sleep_ticks_left.reset();
        return true;
    } else {
        return false;
    }
}

bool CarSimulation::iec_wait_pwr_ready(const CmdArguments& arguments) {
    sim_data.state = SimState::PLUGGED_IN;
    return (sim_data.pwm_duty_cycle > 7.0f && sim_data.pwm_duty_cycle < 97.0f);
}

bool CarSimulation::iso_wait_pwm_is_running(const CmdArguments& arguments) {
    sim_data.state = SimState::PLUGGED_IN;
    return (sim_data.pwm_duty_cycle > 4.0f && sim_data.pwm_duty_cycle < 97.0f);
}

bool CarSimulation::draw_power_regulated(const CmdArguments& arguments) {
    r_ev_board_support->call_set_ac_max_current(std::stod(arguments[0]));
    if (arguments[1] == constants::THREE_PHASES) {
        r_ev_board_support->call_set_three_phases(true);
    } else {
        r_ev_board_support->call_set_three_phases(false);
    }
    sim_data.state = SimState::CHARGING_REGULATED;
    return true;
}

bool CarSimulation::draw_power_fixed(const CmdArguments& arguments) {
    r_ev_board_support->call_set_ac_max_current(std::stod(arguments[0]));
    if (arguments[1] == constants::THREE_PHASES) {
        r_ev_board_support->call_set_three_phases(true);
    } else {
        r_ev_board_support->call_set_three_phases(false);
    }
    sim_data.state = SimState::CHARGING_FIXED;
    return true;
}

bool CarSimulation::pause(const CmdArguments& arguments) {
    sim_data.state = SimState::PLUGGED_IN;
    return true;
}

bool CarSimulation::unplug(const CmdArguments& arguments) {
    sim_data.state = SimState::UNPLUGGED;
    return true;
}

bool CarSimulation::error_e(const CmdArguments& arguments) {
    sim_data.state = SimState::ERROR_E;
    return true;
}

bool CarSimulation::diode_fail(const CmdArguments& arguments) {
    sim_data.state = SimState::DIODE_FAIL;
    return true;
}

bool CarSimulation::rcd_current(const CmdArguments& arguments) {
    sim_data.rcd_current_ma = std::stof(arguments[0]);
    r_ev_board_support->call_set_rcd_error(sim_data.rcd_current_ma);
    return true;
}

bool CarSimulation::plugin(const CmdArguments& arguments) {
    sim_data.state = SimState::PLUGGED_IN;
    return true;
}

bool CarSimulation::iso_wait_slac_matched(const CmdArguments& arguments) {
    sim_data.state = SimState::PLUGGED_IN;

    if (sim_data.slac_state == "UNMATCHED") {
        EVLOG_debug << "Slac UNMATCHED";
        if (!r_slac.empty()) {
            EVLOG_debug << "Slac trigger matching";
            r_slac[0]->call_reset();
            r_slac[0]->call_trigger_matching();
            sim_data.slac_state = "TRIGGERED";
        }
    }
    if (sim_data.slac_state == "MATCHED") {
        EVLOG_debug << "Slac Matched";
        return true;
    }
    return false;
}

bool CarSimulation::iso_wait_pwr_ready(const CmdArguments& arguments) {
    if (sim_data.iso_pwr_ready) {
        sim_data.state = SimState::ISO_POWER_READY;
        return true;
    }
    return false;
}

bool CarSimulation::iso_dc_power_on(const CmdArguments& arguments) {
    sim_data.state = SimState::ISO_POWER_READY;
    if (sim_data.dc_power_on) {
        sim_data.state = SimState::ISO_CHARGING_REGULATED;
        r_ev_board_support->call_allow_power_on(true);
        return true;
    }
    return false;
}

bool CarSimulation::iso_start_v2g_session(const CmdArguments& arguments, bool three_phases) {
    const auto& energy_mode = arguments[0];

    if (energy_mode == constants::AC) {
        if (three_phases == false) {
            r_ev[0]->call_start_charging(types::iso15118_ev::EnergyTransferMode::AC_single_phase_core);
        } else {
            r_ev[0]->call_start_charging(types::iso15118_ev::EnergyTransferMode::AC_three_phase_core);
        }
    } else if (energy_mode == constants::DC) {
        r_ev[0]->call_start_charging(types::iso15118_ev::EnergyTransferMode::DC_extended);
    } else {
        return false;
    }
    return true;
}

bool CarSimulation::iso_draw_power_regulated(const CmdArguments& arguments) {
    r_ev_board_support->call_set_ac_max_current(std::stod(arguments[0]));
    if (arguments[1] == constants::THREE_PHASES) {
        r_ev_board_support->call_set_three_phases(true);
    } else {
        r_ev_board_support->call_set_three_phases(false);
    }
    sim_data.state = SimState::ISO_CHARGING_REGULATED;
    return true;
}

bool CarSimulation::iso_stop_charging(const CmdArguments& arguments) {
    r_ev[0]->call_stop_charging();
    r_ev_board_support->call_allow_power_on(false);
    sim_data.state = SimState::PLUGGED_IN;
    return true;
}

bool CarSimulation::iso_wait_for_stop(const CmdArguments& arguments, size_t loop_interval_ms) {
    if (not sim_data.sleep_ticks_left.has_value()) {
        const auto sleep_time_ms = std::stold(arguments[0]) * 1000;
        sim_data.sleep_ticks_left = static_cast<long long>(sleep_time_ms / loop_interval_ms) + 1;
    }
    auto& sleep_ticks_left = sim_data.sleep_ticks_left.value();
    sleep_ticks_left -= 1;
    if (not(sleep_ticks_left > 0)) {
        r_ev[0]->call_stop_charging();
        r_ev_board_support->call_allow_power_on(false);
        sim_data.state = SimState::PLUGGED_IN;
        sim_data.sleep_ticks_left.reset();
        return true;
    }
    if (sim_data.iso_stopped) {
        EVLOG_info << "POWER OFF iso stopped";
        r_ev_board_support->call_allow_power_on(false);
        sim_data.state = SimState::PLUGGED_IN;
        sim_data.sleep_ticks_left.reset();
        return true;
    }
    return false;
}

bool CarSimulation::iso_wait_v2g_session_stopped(const CmdArguments& arguments) {
    if (sim_data.v2g_finished) {
        return true;
    }
    return false;
}

bool CarSimulation::iso_pause_charging(const CmdArguments& arguments) {
    r_ev[0]->call_pause_charging();
    sim_data.state = SimState::PLUGGED_IN;
    sim_data.iso_pwr_ready = false;
    return true;
}

bool CarSimulation::iso_wait_for_resume(const CmdArguments& arguments) {
    return false;
}

bool CarSimulation::iso_start_bcb_toggle(const CmdArguments& arguments) {
    sim_data.v2g_finished = false;
    sim_data.state = SimState::BCB_TOGGLE;
    if (sim_data.bcb_toggles >= std::stoul(arguments[0]) || sim_data.bcb_toggles == 3) {
        sim_data.bcb_toggles = 0;
        sim_data.state = SimState::PLUGGED_IN;
        return true;
    }
    return false;
}

bool CarSimulation::wait_for_real_plugin(const CmdArguments& arguments) {
    using types::board_support_common::Event;
    if (sim_data.actual_bsp_event == Event::A) {
        EVLOG_info << "Real plugin detected";
        sim_data.state = SimState::PLUGGED_IN;
        return true;
    }
    return false;
}
