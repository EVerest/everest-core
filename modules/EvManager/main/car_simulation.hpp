// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include "simulation_data.hpp"

#include <generated/interfaces/ISO15118_ev/Interface.hpp>
#include <generated/interfaces/ev_board_support/Interface.hpp>
#include <generated/interfaces/ev_slac/Interface.hpp>
#include <generated/types/ev_board_support.hpp>

using CmdArguments = std::vector<std::string>;

class CarSimulation {
public:
    CarSimulation(const std::unique_ptr<ev_board_supportIntf>& r_ev_board_support_,
                  const std::vector<std::unique_ptr<ISO15118_evIntf>>& r_ev_,
                  const std::vector<std::unique_ptr<ev_slacIntf>>& r_slac_) :
        r_ev_board_support(r_ev_board_support_), r_ev(r_ev_), r_slac(r_slac_){};
    ~CarSimulation() = default;

    void reset() {
        sim_data = SimulationData();
    }

    const SimState& get_state() const {
        return sim_data.state;
    }

    std::optional<std::string>& get_modify_charging_session_cmds() {
        return sim_data.modify_charging_session_cmds;
    }

    void set_state(SimState state) {
        sim_data.state = state;
    }

    void set_bsp_event(types::board_support_common::Event event) {
        sim_data.actual_bsp_event = event;
    }

    void set_pp(types::board_support_common::Ampacity pp) {
        sim_data.pp = pp;
    }

    void set_rcd_current(float rcd_current) {
        sim_data.rcd_current_ma = rcd_current;
    }

    void set_pwm_duty_cycle(float pwm_duty_cycle) {
        sim_data.pwm_duty_cycle = pwm_duty_cycle;
    }

    void set_slac_state(std::string slac_state) {
        sim_data.slac_state = std::move(slac_state);
    }

    void set_iso_pwr_ready(bool iso_pwr_ready) {
        sim_data.iso_pwr_ready = iso_pwr_ready;
    }

    void set_evse_max_current(size_t evse_max_current) {
        sim_data.evse_maxcurrent = evse_max_current;
    }

    void set_iso_stopped(bool iso_stopped) {
        sim_data.iso_stopped = iso_stopped;
    }

    void set_iso_d20_paused(bool iso_d20_paused) {
        sim_data.iso_d20_paused = iso_d20_paused;
    }

    void set_v2g_finished(bool v2g_finished) {
        sim_data.v2g_finished = v2g_finished;
    }

    void set_dc_power_on(bool dc_power_on) {
        sim_data.dc_power_on = dc_power_on;
    }

    void state_machine();
    bool sleep(const CmdArguments&, size_t);
    bool iec_wait_pwr_ready(const CmdArguments&);
    bool iso_wait_pwm_is_running(const CmdArguments&);
    bool draw_power_regulated(const CmdArguments&);
    bool draw_power_fixed(const CmdArguments&);
    bool pause(const CmdArguments&);
    bool unplug(const CmdArguments&);
    bool error_e(const CmdArguments&);
    bool diode_fail(const CmdArguments&);
    bool rcd_current(const CmdArguments&);
    bool iso_wait_slac_matched(const CmdArguments&);
    bool iso_wait_pwr_ready(const CmdArguments&);
    bool iso_dc_power_on(const CmdArguments&);
    bool iso_start_v2g_session(const CmdArguments&, bool);
    bool iso_draw_power_regulated(const CmdArguments&);
    bool iso_stop_charging(const CmdArguments&);
    bool iso_wait_for_stop(const CmdArguments&, size_t);
    bool iso_wait_v2g_session_stopped(const CmdArguments&);
    bool iso_pause_charging(const CmdArguments&);
    bool iso_wait_for_resume(const CmdArguments&);
    bool iso_start_bcb_toggle(const CmdArguments&);
    bool wait_for_real_plugin(const CmdArguments&);

private:
    SimulationData sim_data;

    const std::unique_ptr<ev_board_supportIntf>& r_ev_board_support;
    const std::vector<std::unique_ptr<ISO15118_evIntf>>& r_ev;
    const std::vector<std::unique_ptr<ev_slacIntf>>& r_slac;
};
