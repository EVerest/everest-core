// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
/*
 * Charger.cpp
 *
 *  Created on: 08.03.2021
 *      Author: cornelius
 *
 */

#include "Charger.hpp"
#include <algorithm>
#include <generated/types/powermeter.hpp>
#include <math.h>
#include <string.h>
#include <thread>
#include <type_traits>

#include <fmt/core.h>

#include "everest/logging.hpp"
#include "scoped_lock_timeout.hpp"
#include "utils.hpp"

namespace module {

Charger::Charger(const std::unique_ptr<IECStateMachine>& bsp, const std::unique_ptr<ErrorHandling>& error_handling,
                 const std::vector<std::unique_ptr<powermeterIntf>>& r_powermeter_billing,
                 const std::unique_ptr<PersistentStore>& _store,
                 const types::evse_board_support::Connector_type& connector_type, const std::string& evse_id) :
    bsp(bsp),
    error_handling(error_handling),
    r_powermeter_billing(r_powermeter_billing),
    store(_store),
    connector_type(connector_type),
    evse_id(evse_id) {

#ifdef EVEREST_USE_BACKTRACES
    Everest::install_backtrace_handler();
#endif

    shared_context.connector_enabled = true;
    shared_context.max_current = 6.0;
    if (connector_type == types::evse_board_support::Connector_type::IEC62196Type2Socket) {
        shared_context.max_current_cable = bsp->read_pp_ampacity();
    }
    shared_context.authorized = false;

    internal_context.update_pwm_last_dc = 0.;

    shared_context.current_state = EvseState::Idle;
    internal_context.last_state = EvseState::Disabled;

    shared_context.current_drawn_by_vehicle[0] = 0.;
    shared_context.current_drawn_by_vehicle[1] = 0.;
    shared_context.current_drawn_by_vehicle[2] = 0.;

    internal_context.t_step_EF_return_state = EvseState::Idle;
    internal_context.t_step_X1_return_state = EvseState::Idle;

    shared_context.matching_started = false;

    shared_context.transaction_active = false;
    shared_context.session_active = false;

    hlc_use_5percent_current_session = false;

    // create thread for processing errors/error clearings
    error_thread_handle = std::thread(&Charger::error_thread, this);

    // Register callbacks for errors/error clearings
    error_handling->signal_error.connect([this](const bool prevent_charging) {
        if (prevent_charging) {
            // raise external error to signal we cannot charge anymore
            error_handling_event_queue.push(ErrorHandlingEvents::PreventCharging);
        } else {
            EVLOG_info << "All errors cleared that prevented charging";
            error_handling_event_queue.push(ErrorHandlingEvents::AllErrorsPreventingChargingCleared);
        }
    });

    error_handling->signal_all_errors_cleared.connect([this]() {
        EVLOG_info << "All errors cleared";
        error_handling_event_queue.push(ErrorHandlingEvents::AllErrorCleared);
    });
}

Charger::~Charger() {
    pwm_F();
    // need to send an event to wake up processing
    error_handling_event_queue.push(ErrorHandlingEvents::PreventCharging);
    error_thread_handle.stop();
}

void Charger::main_thread() {

    // Enable CP output
    bsp->enable(true);

    // publish initial values
    signal_max_current(get_max_current_internal());
    signal_state(shared_context.current_state);

    while (true) {
        if (main_thread_handle.shouldExit()) {
            break;
        }

        std::this_thread::sleep_for(MAINLOOP_UPDATE_RATE);

        {
            Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::Charger_mainloop);
            // update power limits
            power_available();
            // Run our own state machine update (i.e. run everything that needs
            // to be done on regular intervals independent from events)
            run_state_machine();
        }
    }
}

void Charger::error_thread() {
    for (;;) {
        if (error_thread_handle.shouldExit()) {
            break;
        }
        auto events = error_handling_event_queue.wait();
        if (!events.empty()) {
            Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::Charger_signal_loop);
            for (auto& event : events) {
                switch (event) {
                case ErrorHandlingEvents::PreventCharging:
                    shared_context.error_prevent_charging_flag = true;
                    break;
                case ErrorHandlingEvents::AllErrorsPreventingChargingCleared:
                    shared_context.error_prevent_charging_flag = false;
                    break;
                case ErrorHandlingEvents::AllErrorCleared:
                    shared_context.error_prevent_charging_flag = false;
                    break;
                default:
                    EVLOG_error << "ErrorHandlingEvents invalid value: "
                                << static_cast<std::underlying_type_t<ErrorHandlingEvents>>(event);
                    break;
                }
            }
        }
    }
}

void Charger::run_state_machine() {

    constexpr int max_mainloop_runs = 10;
    int mainloop_runs = 0;

    // run over state machine loop until current_state does not change anymore
    do {
        mainloop_runs++;
        // If a state change happened or an error recovered during a state we reinitialize the state
        bool initialize_state =
            (internal_context.last_state_detect_state_change not_eq shared_context.current_state) or
            (internal_context.last_error_prevent_charging_flag not_eq shared_context.error_prevent_charging_flag);

        if (initialize_state) {
            session_log.evse(false, fmt::format("Charger state: {}->{}",
                                                evse_state_to_string(internal_context.last_state_detect_state_change),
                                                evse_state_to_string(shared_context.current_state)));
        }

        internal_context.last_state = internal_context.last_state_detect_state_change;
        internal_context.last_state_detect_state_change = shared_context.current_state;
        internal_context.last_error_prevent_charging_flag = shared_context.error_prevent_charging_flag;

        auto now = std::chrono::system_clock::now();

        if (shared_context.ac_with_soc_timeout and (shared_context.ac_with_soc_timer -= 50) < 0) {
            shared_context.ac_with_soc_timeout = false;
            shared_context.ac_with_soc_timer = 3600000;
            signal_ac_with_soc_timeout();
            return;
        }

        if (initialize_state) {
            internal_context.current_state_started = now;
            signal_state(shared_context.current_state);
        }

        auto time_in_current_state =
            std::chrono::duration_cast<std::chrono::milliseconds>(now - internal_context.current_state_started).count();

        // Clean up ongoing 1ph 3ph switching operation?
        if (internal_context.last_state == EvseState::SwitchPhases and
            shared_context.current_state not_eq EvseState::SwitchPhases and
            shared_context.switch_3ph1ph_threephase_ongoing) {
            bsp->switch_three_phases_while_charging(shared_context.switch_3ph1ph_threephase);
            shared_context.switch_3ph1ph_threephase_ongoing = false;
        }

        switch (shared_context.current_state) {
        case EvseState::Disabled:
            if (initialize_state) {
                signal_simple_event(types::evse_manager::SessionEventEnum::Disabled);
                pwm_F();
            }
            break;

        case EvseState::Replug:
            if (initialize_state) {
                signal_simple_event(types::evse_manager::SessionEventEnum::ReplugStarted);
                // start timer in case we need to
                if (shared_context.ac_with_soc_timeout) {
                    shared_context.ac_with_soc_timer = 120000;
                }
            }
            // simply wait here until BSP informs us that replugging was finished
            break;

        case EvseState::Idle:
            // make sure we signal availability to potential new cars
            if (initialize_state) {
                bcb_toggle_reset();
                shared_context.iec_allow_close_contactor = false;
                shared_context.hlc_charging_active = false;
                shared_context.hlc_allow_close_contactor = false;
                shared_context.max_current_cable = 0;
                shared_context.hlc_charging_terminate_pause = HlcTerminatePause::Unknown;
                shared_context.legacy_wakeup_done = false;
                pwm_off();
                deauthorize_internal();
                shared_context.transaction_active = false;
                clear_errors_on_unplug();
            }
            break;

        case EvseState::WaitingForAuthentication:

            // Explicitly do not allow to be powered on. This is important
            // to make sure control_pilot does not switch on relais even if
            // we start PWM here
            if (initialize_state) {
                internal_context.pp_warning_printed = false;
                internal_context.no_energy_warning_printed = false;

                bsp->allow_power_on(false, types::evse_board_support::Reason::PowerOff);

                if (internal_context.last_state == EvseState::Replug) {
                    signal_simple_event(types::evse_manager::SessionEventEnum::ReplugFinished);
                } else {
                    // First user interaction was plug in of car? Start session here.
                    if (not shared_context.session_active) {
                        start_session(false);
                    }
                    // External signal on MQTT
                    signal_simple_event(types::evse_manager::SessionEventEnum::AuthRequired);
                }
                hlc_use_5percent_current_session = false;

                // switch on HLC if configured. May be switched off later on after retries for this session only.
                if (config_context.charge_mode == ChargeMode::AC) {
                    ac_hlc_enabled_current_session = config_context.ac_hlc_enabled;
                    if (ac_hlc_enabled_current_session) {
                        hlc_use_5percent_current_session = config_context.ac_hlc_use_5percent;
                    }
                } else if (config_context.charge_mode == ChargeMode::DC) {
                    hlc_use_5percent_current_session = true;
                } else {
                    // unsupported charging mode, give up here.
                    error_handling->raise_internal_error("Unsupported charging mode.");
                }

                if (hlc_use_5percent_current_session) {
                    // FIXME: wait for SLAC to be ready. Teslas are really fast with sending the first slac packet after
                    // enabling PWM.
                    std::this_thread::sleep_for(
                        std::chrono::milliseconds(config_context.sleep_before_enabling_pwm_hlc_mode_ms));
                    update_pwm_now(PWM_5_PERCENT);
                    stopwatch.mark("HLC_PWM_5%_ON");
                }
            }

            // Read PP value in case of AC socket
            if (connector_type == types::evse_board_support::Connector_type::IEC62196Type2Socket and
                shared_context.max_current_cable == 0) {
                shared_context.max_current_cable = bsp->read_pp_ampacity();
                // retry if the value is not yet available. Some BSPs may take some time to measure the PP.
                if (shared_context.max_current_cable == 0) {
                    if (not internal_context.pp_warning_printed) {
                        EVLOG_warning << "PP ampacity is zero, still retrying to read PP ampacity...";
                        internal_context.pp_warning_printed = true;
                    }
                    break;
                }
            }

            // Wait for Energy Manager to supply some power, otherwise wait here.
            // If we have zero power, some cars will not like the ChargingParameter message.
            if (config_context.charge_mode == ChargeMode::DC) {
                // Create a copy of the atomic struct
                types::iso15118::DcEvseMaximumLimits evse_limit = shared_context.current_evse_max_limits;
                if (not(evse_limit.evse_maximum_current_limit > 0 and evse_limit.evse_maximum_power_limit > 0)) {
                    if (not internal_context.no_energy_warning_printed) {
                        EVLOG_warning << "No energy available, still retrying...";
                        internal_context.no_energy_warning_printed = true;
                    }
                    break;
                }
            }

            // SLAC is running in the background trying to setup a PLC connection.

            // we get Auth (maybe before SLAC matching or during matching)
            // FIXME: getAuthorization needs to distinguish between EIM and PnC in Auth mananger

            // FIXME: Note Fig 7. is not fully supported here yet (AC Auth before plugin - this overides PnC and always
            // starts with nominal PWM). We need to support this as this is the only way a user can
            // skip PnC if he does NOT want to use it for this charging session.

            // FIXME: In case V2G is not successfull after all, it needs
            // to send a dlink_error request, which then needs to:
            // AC mode: fall back to nominal PWM charging etc (not
            // implemented yet!), it can do so because it is already authorized.
            // DC mode: go to error_hlc and give up

            // FIXME: if slac reports a dlink_ready=false while we are still waiting for auth we should:
            // in AC mode: go back to non HLC nominal PWM mode
            // in DC mode: go to error_slac for this session

            if (shared_context.authorized and not shared_context.authorized_pnc) {
                session_log.evse(false, "EIM Authorization received");

                // If we are restarting, the transaction may already be active
                if (not shared_context.transaction_active) {
                    if (!start_transaction()) {
                        break;
                    }
                }

                const EvseState target_state(EvseState::PrepareCharging);

                // EIM done and matching process not started -> we need to go through t_step_EF and fall back to nominal
                // PWM. This is a complete waste of 4 precious seconds.
                if (config_context.charge_mode == ChargeMode::AC) {
                    if (ac_hlc_enabled_current_session) {
                        if (config_context.ac_enforce_hlc) {
                            // non standard compliant mode: we just keep 5 percent running all the time like in DC
                            session_log.evse(
                                false, "AC mode, HLC enabled(ac_enforce_hlc), keeping 5 percent on until a dlink error "
                                       "is signalled.");
                            hlc_use_5percent_current_session = true;
                            shared_context.current_state = target_state;
                        } else {
                            if (not shared_context.matching_started) {
                                // SLAC matching was not started when EIM arrived

                                session_log.evse(
                                    false,
                                    fmt::format(
                                        "AC mode, HLC enabled, matching not started yet. Go through t_step_EF and "
                                        "disable 5 percent if it was enabled before: {}",
                                        (bool)hlc_use_5percent_current_session));

                                // Figure 3 of ISO15118-3: 5 percent start, PnC and EIM
                                // Figure 4 of ISO15118-3: X1 start, PnC and EIM
                                internal_context.t_step_EF_return_state = target_state;
                                internal_context.t_step_EF_return_pwm = 0.;
                                internal_context.t_step_EF_return_ampere = 0.;
                                // fall back to nominal PWM after the t_step_EF break. Note that
                                // ac_hlc_enabled_current_session remains untouched as HLC can still start later in
                                // nominal PWM mode
                                hlc_use_5percent_current_session = false;
                                shared_context.current_state = EvseState::T_step_EF;
                            } else {
                                // SLAC matching was started already when EIM arrived
                                if (hlc_use_5percent_current_session) {
                                    // Figure 5 of ISO15118-3: 5 percent start, PnC and EIM, matching already started
                                    // when EIM was done
                                    session_log.evse(
                                        false, "AC mode, HLC enabled(5percent), matching already started. Go through "
                                               "t_step_X1 and disable 5 percent.");
                                    internal_context.t_step_X1_return_state = target_state;
                                    internal_context.t_step_X1_return_pwm = 0.;
                                    internal_context.t_step_EF_return_ampere = 0.;
                                    hlc_use_5percent_current_session = false;
                                    shared_context.current_state = EvseState::T_step_X1;
                                } else {
                                    // Figure 6 of ISO15118-3: X1 start, PnC and EIM, matching already started when EIM
                                    // was done. We can go directly to PrepareCharging, as we do not need to switch from
                                    // 5 percent to nominal first
                                    session_log.evse(
                                        false,
                                        "AC mode, HLC enabled(X1), matching already started. We are in X1 so we can "
                                        "go directly to nominal PWM.");
                                    shared_context.current_state = target_state;
                                }
                            }
                        }

                    } else {
                        // HLC is disabled for this session.
                        // simply proceed to PrepareCharging, as we are fully authorized to start charging whenever car
                        // wants to.
                        session_log.evse(false, "AC mode, HLC disabled. We are in X1 so we can "
                                                "go directly to nominal PWM.");
                        shared_context.current_state = target_state;
                    }
                } else if (config_context.charge_mode == ChargeMode::DC) {
                    // Figure 8 of ISO15118-3: DC with EIM before or after plugin or PnC
                    // simple here as we always stay within 5 percent mode anyway.
                    session_log.evse(false,
                                     "DC mode. We are in 5percent mode so we can continue without further action.");
                    shared_context.current_state = target_state;
                } else {
                    // unsupported charging mode, give up here.
                    error_handling->raise_internal_error("Unsupported charging mode.");
                }
            } else if (shared_context.authorized and shared_context.authorized_pnc) {

                if (!start_transaction()) {
                    break;
                }

                const EvseState target_state(EvseState::PrepareCharging);

                // We got authorization by Plug and Charge
                session_log.evse(false, "PnC Authorization received");
                if (config_context.charge_mode == ChargeMode::AC) {
                    // Figures 3,4,5,6 of ISO15118-3: Independent on how we started we can continue with 5 percent
                    // signalling once we got PnC authorization without going through t_step_EF or t_step_X1.

                    session_log.evse(
                        false, "AC mode, HLC enabled, PnC auth received. We will continue with 5percent independent on "
                               "how we started.");
                    hlc_use_5percent_current_session = true;
                    shared_context.current_state = target_state;

                } else if (config_context.charge_mode == ChargeMode::DC) {
                    // Figure 8 of ISO15118-3: DC with EIM before or after plugin or PnC
                    // simple here as we always stay within 5 percent mode anyway.
                    session_log.evse(false,
                                     "DC mode. We are in 5percent mode so we can continue without further action.");
                    shared_context.current_state = target_state;
                } else {
                    // unsupported charging mode, give up here.
                    error_handling->raise_internal_error("Unsupported charging mode.");
                }
            }

            break;

        case EvseState::SwitchPhases:
            if (initialize_state) {
                session_log.evse(false, "Start switching phases");
                signal_simple_event(types::evse_manager::SessionEventEnum::SwitchingPhases);
                if (config_context.switch_3ph1ph_cp_state_F) {
                    pwm_F();
                } else {
                    pwm_off();
                }
            }
            if (time_in_current_state >= config_context.switch_3ph1ph_delay_s * 1000) {
                session_log.evse(false, "Exit switching phases");
                bsp->switch_three_phases_while_charging(shared_context.switch_3ph1ph_threephase);
                shared_context.switch_3ph1ph_threephase_ongoing = false;
                shared_context.current_state = internal_context.switching_phases_return_state;
            }
            break;

        case EvseState::T_step_EF:
            if (initialize_state) {
                session_log.evse(false, "Enter T_step_EF");
                internal_context.t_step_ef_x1_pause = false;
                pwm_F();
            }
            if (time_in_current_state >= T_STEP_EF + STAY_IN_X1_AFTER_TSTEP_EF_MS) {
                session_log.evse(false, "Exit T_step_EF");
                if (internal_context.t_step_EF_return_pwm == 0.) {
                    pwm_off();
                } else {
                    update_pwm_now(internal_context.t_step_EF_return_pwm);
                    internal_context.pwm_set_last_ampere = internal_context.t_step_EF_return_ampere;
                }
                shared_context.current_state = internal_context.t_step_EF_return_state;
            } else if (time_in_current_state >= T_STEP_EF and not internal_context.t_step_ef_x1_pause) {
                internal_context.t_step_ef_x1_pause = true;
                // stay in X1 for a little while as required by EV READY regulations
                session_log.evse(false, "Pause in X1 for EV READY regulations");
                pwm_off();
            }
            break;

        case EvseState::T_step_X1:
            if (initialize_state) {
                session_log.evse(false, "Enter T_step_X1");
                pwm_off();
            }
            if (time_in_current_state >= T_STEP_X1) {
                session_log.evse(false, "Exit T_step_X1");
                if (internal_context.t_step_X1_return_pwm == 0.) {
                    pwm_off();
                } else {
                    update_pwm_now(internal_context.t_step_X1_return_pwm);
                    internal_context.pwm_set_last_ampere = internal_context.t_step_EF_return_ampere;
                }
                shared_context.current_state = internal_context.t_step_X1_return_state;
            }
            break;

        case EvseState::PrepareCharging:
            if (initialize_state) {
                signal_simple_event(types::evse_manager::SessionEventEnum::PrepareCharging);
                bcb_toggle_reset();
            }

            if (config_context.charge_mode == ChargeMode::DC) {
                if (shared_context.hlc_allow_close_contactor and shared_context.iec_allow_close_contactor) {
                    bsp->allow_power_on(true, types::evse_board_support::Reason::DCCableCheck);
                }
            }

            // Wait here until all errors are cleared
            if (stop_charging_on_fatal_error_internal()) {
                // reset the time counter for the wake-up sequence if we are blocked by errors
                internal_context.current_state_started = now;
                break;
            }

            // make sure we are enabling PWM
            if (not hlc_use_5percent_current_session) {
                auto m = get_max_current_internal();
                update_pwm_now_if_changed_ampere(m);
            } else {
                update_pwm_now_if_changed(PWM_5_PERCENT);
            }

            if (config_context.charge_mode == ChargeMode::AC) {
                // In AC mode BASIC, iec_allow is sufficient.  The same is true for HLC mode when nominal PWM is
                // used as the car can do BASIC and HLC charging any time. In AC HLC with 5 percent mode, we need to
                // wait for both iec_allow and hlc_allow.

                if (not power_available()) {
                    shared_context.current_state = EvseState::WaitingForEnergy;
                } else {
                    // Power is available, PWM is already enabled. Check if we can go to charging
                    if ((shared_context.iec_allow_close_contactor and not hlc_use_5percent_current_session) or
                        (shared_context.iec_allow_close_contactor and shared_context.hlc_allow_close_contactor and
                         hlc_use_5percent_current_session)) {

                        signal_simple_event(types::evse_manager::SessionEventEnum::ChargingStarted);
                        shared_context.current_state = EvseState::Charging;
                    } else {
                        // We have power and PWM is on, but EV did not proceed to state C yet (and/or HLC is not
                        // ready)
                        if (not shared_context.hlc_charging_active and not shared_context.legacy_wakeup_done and
                            time_in_current_state > LEGACY_WAKEUP_TIMEOUT) {
                            session_log.evse(false, "EV did not transition to state C, trying one legacy wakeup "
                                                    "according to IEC61851-1 A.5.3");
                            shared_context.legacy_wakeup_done = true;
                            internal_context.t_step_EF_return_state = EvseState::PrepareCharging;
                            internal_context.t_step_EF_return_pwm = ampere_to_duty_cycle(get_max_current_internal());
                            internal_context.t_step_EF_return_ampere = get_max_current_internal();
                            shared_context.current_state = EvseState::T_step_EF;
                        } else if (not shared_context.hlc_charging_active and shared_context.legacy_wakeup_done and
                                   time_in_current_state > PREPARING_TIMEOUT_PAUSED_BY_EV) {
                            // We are still here after the wakeup plus some extra delay, so probably the EV really does
                            // not want to charge. Switch to ChargingPausedEV state.
                            shared_context.current_state = EvseState::ChargingPausedEV;
                        }
                    }
                }
            }

            // if (charge_mode == ChargeMode::DC) {
            //  DC: wait until car requests power on CP (B->C/D).
            //  Then we close contactor and wait for instructions from HLC.
            //  HLC will perform CableCheck, PreCharge, and PowerDelivery.
            //  These are all controlled from the handlers directly, so there is nothing we need to do here.
            //  Once HLC informs us about CurrentDemand has started, we will go to Charging in the handler.
            //}

            break;

        case EvseState::Charging:
            if (initialize_state) {
                shared_context.hlc_charging_terminate_pause = HlcTerminatePause::Unknown;
                stopwatch.mark("Charging started");
                stopwatch.report_phase();
                auto report = stopwatch.report_all_phases();
                if (config_context.charge_mode == ChargeMode::DC) {
                    EVLOG_info << "Timing statistics (Plugin to CurrentDemand)";
                    EVLOG_info << "-------------------------------------------";
                    for (const auto& r : report) {
                        EVLOG_info << r;
                    }
                }
            }

            if (config_context.state_F_after_fault_ms > 0 and not shared_context.hlc_charging_active) {
                // First time we see that a fatal error became active, signal F for a short time.
                // Only use in basic charging mode.
                if (entered_fatal_error_state()) {
                    pwm_F();
                }

                if (internal_context.pwm_F_active and
                    time_in_fatal_error_state_ms() > config_context.state_F_after_fault_ms) {
                    pwm_off();
                }
            }

            // Wait here until all errors are cleared
            if (stop_charging_on_fatal_error_internal()) {
                break;
            }

            if (config_context.charge_mode == ChargeMode::DC) {
                if (initialize_state) {
                    bsp->allow_power_on(true, types::evse_board_support::Reason::FullPowerCharging);
                }
            } else {
                check_soft_over_current();

                if (not power_available()) {
                    pause_charging_wait_for_power_internal();
                    break;
                }

                if (initialize_state) {
                    if (internal_context.last_state not_eq EvseState::PrepareCharging) {
                        signal_simple_event(types::evse_manager::SessionEventEnum::ChargingResumed);
                    }

                    // Allow another wake-up sequence
                    shared_context.legacy_wakeup_done = false;

                    bsp->allow_power_on(true, types::evse_board_support::Reason::FullPowerCharging);
                    // make sure we are enabling PWM
                    if (hlc_use_5percent_current_session) {
                        update_pwm_now_if_changed(PWM_5_PERCENT);
                    } else {
                        update_pwm_now_if_changed_ampere(get_max_current_internal());
                    }
                } else {
                    // update PWM if it has changed and 5 seconds have passed since last update
                    if (not hlc_use_5percent_current_session) {
                        update_pwm_max_every_5seconds_ampere(get_max_current_internal());
                    }
                }
            }
            break;

        case EvseState::ChargingPausedEV:

            if (config_context.charge_mode == ChargeMode::AC) {
                check_soft_over_current();
            }

            // A pause issued by the EV needs to be handled differently for the different charging modes

            // 1) BASIC AC charging: Nominal PWM needs be running, so the EV can actually resume charging when it wants
            // to

            // 2) HLC charging: [V2G3-M07-19] requires the EV to switch to state B, so we will end up here in this state
            //    [V2G3-M07-20] forces us to switch off PWM.
            //    This is also true for nominal PWM AC HLC charging, so an EV that does HLC AC and pauses can only
            //    resume in HLC mode and not in BASIC charging.

            if (shared_context.hlc_charging_active) {
                // This is for HLC charging (both AC and DC)
                if (initialize_state) {
                    bcb_toggle_reset();
                    bsp->allow_power_on(false, types::evse_board_support::Reason::PowerOff);
                    if (config_context.charge_mode == ChargeMode::DC) {
                        signal_dc_supply_off();
                    }
                    signal_simple_event(types::evse_manager::SessionEventEnum::ChargingPausedEV);
                }

                if (bcb_toggle_detected()) {
                    shared_context.current_state = EvseState::PrepareCharging;
                }

                // We come here by a state C->B transition but the ISO message may not have arrived yet,
                // so we wait here until we know wether it is Terminate or Pause. Until we leave PWM on (should not
                // be shut down before SessionStop.req)

                if (shared_context.hlc_charging_terminate_pause == HlcTerminatePause::Terminate) {
                    // EV wants to terminate session
                    shared_context.current_state = EvseState::StoppingCharging;
                    if (shared_context.pwm_running) {
                        pwm_off();
                    }
                } else if (shared_context.hlc_charging_terminate_pause == HlcTerminatePause::Pause) {
                    // EV wants an actual pause
                    if (shared_context.pwm_running) {
                        pwm_off();
                    }
                }

            } else {
                // This is for BASIC charging only

                // Normally power should be available, since we request a minimum power also during EV pause.
                // In case the energy manager gives us no energy, we effectivly switch to a pause by EVSE here.
                if (not power_available()) {
                    pause_charging_wait_for_power_internal();
                    break;
                }

                if (initialize_state) {
                    signal_simple_event(types::evse_manager::SessionEventEnum::ChargingPausedEV);
                } else {
                    // update PWM if it has changed and 5 seconds have passed since last update
                    if (not stop_charging_on_fatal_error_internal()) {
                        update_pwm_max_every_5seconds_ampere(get_max_current_internal());
                    }
                }
            }
            break;

        case EvseState::ChargingPausedEVSE:
            if (initialize_state) {
                signal_simple_event(types::evse_manager::SessionEventEnum::ChargingPausedEVSE);
                if (shared_context.hlc_charging_active) {
                    // currentState = EvseState::StoppingCharging;
                    shared_context.last_stop_transaction_reason = types::evse_manager::StopTransactionReason::Local;
                    // tell HLC stack to stop the session
                    signal_hlc_stop_charging();
                    pwm_off();
                } else {
                    pwm_off();
                }
            }
            break;

        case EvseState::WaitingForEnergy:
            if (initialize_state) {
                signal_simple_event(types::evse_manager::SessionEventEnum::WaitingForEnergy);
                if (not hlc_use_5percent_current_session) {
                    pwm_off();
                }
            }
            break;

        case EvseState::StoppingCharging:
            if (initialize_state) {
                bcb_toggle_reset();
                if (shared_context.transaction_active) {
                    signal_simple_event(types::evse_manager::SessionEventEnum::StoppingCharging);
                }

                if (shared_context.hlc_charging_active) {
                    if (config_context.charge_mode == ChargeMode::DC) {
                        // DC supply off - actually this is after relais switched off
                        // this is a backup switch off, normally it should be switched off earlier by ISO protocol.
                        signal_dc_supply_off();
                    }
                    // Car is maybe not unplugged yet, so for HLC(AC/DC) wait in this state. We will go to Finished
                    // once car is unplugged.
                } else {
                    // For AC BASIC charging, we reached StoppingCharging because an unplug happend.
                    pwm_off();
                    shared_context.current_state = EvseState::Finished;
                }
            }

            // Allow session restart after SessionStop.terminate (full restart including new SLAC).
            // Only allow that if the transaction is still running. If it was cancelled externally with
            // cancel_transaction(), we do not allow restart. If OCPP cancels a transaction it assumes it cannot be
            // restarted. In all other cases, e.g. the EV stopping the transaction it may resume with a BCB toggle.
            if (shared_context.hlc_charging_active and bcb_toggle_detected()) {
                if (shared_context.transaction_active) {
                    shared_context.current_state = EvseState::PrepareCharging;
                    // wake up SLAC as well
                    signal_slac_start();
                } else {
                    session_log.car(false, "Car requested restarting with BCB toggle. Ignored, since we were cancelled "
                                           "externally before.");
                }
            }
            break;

        case EvseState::Finished:

            if (initialize_state) {
                // Transaction may already be stopped when it was cancelled earlier.
                // In that case, do not sent a second transactionFinished event.
                if (shared_context.transaction_active) {
                    stop_transaction();
                }

                // We may come here from an error state, so a session was maybe not active.
                if (shared_context.session_active) {
                    stop_session();
                }

                if (config_context.charge_mode == ChargeMode::DC) {
                    signal_dc_supply_off();
                }
            }

            shared_context.current_state = EvseState::Idle;
            break;
        }

        if (mainloop_runs > max_mainloop_runs) {
            EVLOG_warning << "Charger main loop exceeded maximum number of runs, last_state "
                          << evse_state_to_string(internal_context.last_state_detect_state_change)
                          << " current_state: " << evse_state_to_string(shared_context.current_state);
        }

    } while (internal_context.last_state_detect_state_change not_eq shared_context.current_state);
}

void Charger::process_event(CPEvent cp_event) {
    switch (cp_event) {
    case CPEvent::CarPluggedIn:
    case CPEvent::CarRequestedPower:
    case CPEvent::CarRequestedStopPower:
    case CPEvent::CarUnplugged:
    case CPEvent::BCDtoEF:
    case CPEvent::BCDtoE:
    case CPEvent::EFtoBCD:
        session_log.car(false, fmt::format("Event {}", cpevent_to_string(cp_event)));
        break;
    default:
        session_log.evse(false, fmt::format("Event {}", cpevent_to_string(cp_event)));
        break;
    }

    if (cp_event == CPEvent::PowerOn) {
        contactors_closed = true;
    } else if (cp_event == CPEvent::PowerOff) {
        contactors_closed = false;
    }

    Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::Charger_process_event);

    run_state_machine();

    // Process all event actions that are independent of the current state
    process_cp_events_independent(cp_event);

    run_state_machine();

    // Process all events that depend on the current state
    process_cp_events_state(cp_event);

    run_state_machine();
}

void Charger::process_cp_events_state(CPEvent cp_event) {
    switch (shared_context.current_state) {

    case EvseState::Idle:
        if (cp_event == CPEvent::CarPluggedIn) {
            stopwatch.reset();
            stopwatch.mark_phase("ConnSetup");
            shared_context.current_state = EvseState::WaitingForAuthentication;
        }
        break;

    case EvseState::WaitingForEnergy:
        [[fallthrough]];
    case EvseState::WaitingForAuthentication:
        if (cp_event == CPEvent::CarRequestedPower) {
            session_log.car(false, "B->C transition before PWM is enabled at this stage violates IEC61851-1");
            shared_context.iec_allow_close_contactor = true;
        } else if (cp_event == CPEvent::CarRequestedStopPower) {
            shared_context.iec_allow_close_contactor = false;
        }
        break;

    case EvseState::PrepareCharging:
        if (cp_event == CPEvent::CarRequestedPower) {
            shared_context.iec_allow_close_contactor = true;
        } else if (cp_event == CPEvent::CarRequestedStopPower) {
            shared_context.iec_allow_close_contactor = false;
            signal_dc_supply_off();
        }
        break;

    case EvseState::Charging:
        if (cp_event == CPEvent::CarRequestedStopPower) {
            shared_context.iec_allow_close_contactor = false;
            shared_context.current_state = EvseState::ChargingPausedEV;
            // Tell HLC stack to stop the session. Normally the session should have already been stopped by the EV, but
            // if this is not the case, we have to do it here.
            if (shared_context.hlc_charging_active) {
                signal_hlc_stop_charging();
                session_log.evse(false, "CP state transition C->B at this stage violates ISO15118-2");
            }
        } else if (cp_event == CPEvent::BCDtoE) {
            shared_context.iec_allow_close_contactor = false;
            shared_context.current_state = EvseState::StoppingCharging;
            // Tell HLC stack to stop the session in case of an E event while charging.
            if (shared_context.hlc_charging_active) {
                signal_hlc_stop_charging();
                session_log.evse(false, "CP state transition C->E/F at this stage violates ISO15118-2");
            }
        }
        break;

    case EvseState::ChargingPausedEV:
        if (cp_event == CPEvent::CarRequestedPower) {
            shared_context.iec_allow_close_contactor = true;
            // For BASIC charging we can simply switch back to Charging
            if (config_context.charge_mode == ChargeMode::AC and not shared_context.hlc_charging_active) {
                shared_context.current_state = EvseState::Charging;
            } else if (not shared_context.pwm_running) {
                bcb_toggle_detect_start_pulse();
            }
        }

        if (cp_event == CPEvent::CarRequestedStopPower and not shared_context.pwm_running and
            shared_context.hlc_charging_active) {
            bcb_toggle_detect_stop_pulse();
        }
        break;

    case EvseState::StoppingCharging:
        // Allow session restart from EV after SessionStop.terminate with BCB toggle
        if (shared_context.hlc_charging_active and not shared_context.pwm_running) {
            if (cp_event == CPEvent::CarRequestedPower) {
                bcb_toggle_detect_start_pulse();
            } else if (cp_event == CPEvent::CarRequestedStopPower) {
                bcb_toggle_detect_stop_pulse();
            }
        }
        break;

    default:
        break;
    }
}

void Charger::process_cp_events_independent(CPEvent cp_event) {
    switch (cp_event) {
    case CPEvent::EvseReplugStarted:
        shared_context.current_state = EvseState::Replug;
        break;
    case CPEvent::EvseReplugFinished:
        shared_context.current_state = EvseState::WaitingForAuthentication;
        break;
    case CPEvent::CarRequestedStopPower:
        shared_context.iec_allow_close_contactor = false;
        break;
    case CPEvent::CarUnplugged:
        if (not shared_context.hlc_charging_active) {
            shared_context.current_state = EvseState::StoppingCharging;
        } else {
            shared_context.current_state = EvseState::Finished;
        }
        break;

    default:
        break;
    }
}

void Charger::update_pwm_max_every_5seconds_ampere(float ampere) {
    float dc = ampere_to_duty_cycle(ampere);
    if (dc not_eq internal_context.update_pwm_last_dc) {
        auto now = std::chrono::steady_clock::now();
        auto time_since_last_update =
            std::chrono::duration_cast<std::chrono::milliseconds>(now - internal_context.last_pwm_update).count();
        if (time_since_last_update >= IEC_PWM_MAX_UPDATE_INTERVAL) {
            update_pwm_now(dc);
            internal_context.pwm_set_last_ampere = ampere;
        }
    }
}

void Charger::update_pwm_now(float dc) {
    auto start = std::chrono::steady_clock::now();
    internal_context.update_pwm_last_dc = dc;
    shared_context.pwm_running = true;

    session_log.evse(
        false,
        fmt::format(
            "Set PWM On ({}%) took {} ms", dc * 100.,
            (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start)).count()));
    internal_context.last_pwm_update = std::chrono::steady_clock::now();
    internal_context.pwm_F_active = false;
    bsp->set_pwm(dc);
}

void Charger::update_pwm_now_if_changed(float dc) {
    if (internal_context.update_pwm_last_dc not_eq dc) {
        update_pwm_now(dc);
    }
}

void Charger::update_pwm_now_if_changed_ampere(float ampere) {
    float dc = ampere_to_duty_cycle(ampere);
    if (internal_context.update_pwm_last_dc not_eq dc) {
        update_pwm_now(dc);
        internal_context.pwm_set_last_ampere = ampere;
    }
}

void Charger::pwm_off() {
    session_log.evse(false, "Set PWM Off");
    shared_context.pwm_running = false;
    internal_context.update_pwm_last_dc = 1.;
    internal_context.pwm_set_last_ampere = 0.;
    internal_context.pwm_F_active = false;
    bsp->set_pwm_off();
}

void Charger::pwm_F() {
    session_log.evse(false, "Set PWM F");
    shared_context.pwm_running = false;
    internal_context.update_pwm_last_dc = 0.;
    internal_context.pwm_set_last_ampere = 0.;
    internal_context.pwm_F_active = true;
    bsp->set_pwm_F();
}

void Charger::run() {
    // spawn new thread and return
    main_thread_handle = std::thread(&Charger::main_thread, this);
}

float Charger::ampere_to_duty_cycle(float ampere) {
    float dc = 0;

    // calculate max current
    if (ampere < 5.9) {
        // Invalid argument, switch to error
        dc = 1.0;
    } else if (ampere <= 6.1) {
        dc = 0.1;
    } else if (ampere < 52.5) {
        if (ampere > 51.)
            ampere = 51; // Weird gap in norm: 51A .. 52.5A has no defined PWM.
        dc = ampere / 0.6 / 100.;
    } else if (ampere <= 80) {
        dc = ((ampere / 2.5) + 64) / 100.;
    } else if (ampere <= 80.1) {
        dc = 0.97;
    } else {
        // Invalid argument, switch to error
        dc = 1.0;
    }

    return dc;
}

bool Charger::set_max_current(float c, std::chrono::time_point<date::utc_clock> validUntil) {
    if (c >= 0.0 and c <= CHARGER_ABSOLUTE_MAX_CURRENT) {

        // is it still valid?
        if (validUntil > date::utc_clock::now()) {
            {
                Everest::scoped_lock_timeout lock(state_machine_mutex,
                                                  Everest::MutexDescription::Charger_pause_charging);
                shared_context.max_current = c;
                shared_context.max_current_valid_until = validUntil;
            }
            bsp->set_overcurrent_limit(c);
            signal_max_current(c);
            return true;
        }
    }
    return false;
}

// pause if currently charging, else do nothing.
bool Charger::pause_charging() {
    Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::Charger_pause_charging);
    if (shared_context.current_state == EvseState::Charging) {
        shared_context.legacy_wakeup_done = false;
        shared_context.current_state = EvseState::ChargingPausedEVSE;
        return true;
    }
    return false;
}

bool Charger::resume_charging() {
    Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::Charger_resume_charging);

    if (shared_context.hlc_charging_active and shared_context.transaction_active and
        shared_context.current_state == EvseState::ChargingPausedEVSE) {
        shared_context.current_state = EvseState::PrepareCharging;
        // wake up SLAC as well
        signal_slac_start();
        return true;
    } else if (shared_context.transaction_active and shared_context.current_state == EvseState::ChargingPausedEVSE) {
        shared_context.current_state = EvseState::WaitingForEnergy;
        return true;
    }

    return false;
}

// pause charging since no power is available at the moment
bool Charger::pause_charging_wait_for_power() {
    Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::Charger_waiting_for_power);
    return pause_charging_wait_for_power_internal();
}

// pause charging since no power is available at the moment
bool Charger::pause_charging_wait_for_power_internal() {
    if (shared_context.current_state == EvseState::Charging or
        shared_context.current_state == EvseState::ChargingPausedEV) {
        shared_context.current_state = EvseState::WaitingForEnergy;
        return true;
    }
    return false;
}

// resume charging since power became available. Does not resume if user paused charging.
bool Charger::resume_charging_power_available() {
    Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::Charger_resume_power_available);

    if (shared_context.transaction_active and shared_context.current_state == EvseState::WaitingForEnergy and
        power_available()) {
        shared_context.current_state = EvseState::PrepareCharging;
        return true;
    }
    return false;
}

// pause charging since we run through replug sequence
bool Charger::evse_replug() {
    // call BSP to start the replug sequence. It BSP actually does it,
    // it will emit a EvseReplugStarted event which will then modify our state.
    // If BSP never executes the replug, we also never change state and nothing happens.
    // After replugging finishes, BSP will emit EvseReplugFinished event and we will go back to WaitingForAuth
    EVLOG_info << fmt::format("Calling evse_replug({})...", T_REPLUG_MS);
    bsp->evse_replug(T_REPLUG_MS);
    return true;
}

// Cancel transaction/charging from external EvseManager interface (e.g. via OCPP)
bool Charger::cancel_transaction(const types::evse_manager::StopTransactionRequest& request) {
    Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::Charger_cancel_transaction);

    if (shared_context.transaction_active) {

        if (shared_context.hlc_charging_active) {

            if (request.reason == types::evse_manager::StopTransactionReason::EmergencyStop) {
                signal_hlc_error(types::iso15118::EvseError::Error_EmergencyShutdown);
            } else if (request.reason == types::evse_manager::StopTransactionReason::PowerLoss) {
                signal_hlc_error(types::iso15118::EvseError::Error_UtilityInterruptEvent);
            }

            shared_context.current_state = EvseState::StoppingCharging;
            signal_hlc_stop_charging();
        } else {
            shared_context.current_state = EvseState::ChargingPausedEVSE;
            pwm_off();
        }

        shared_context.transaction_active = false;
        shared_context.last_stop_transaction_reason = request.reason;
        shared_context.stop_transaction_id_token = request.id_tag;

        for (const auto& meter : r_powermeter_billing) {
            const auto response = meter->call_stop_transaction(shared_context.session_uuid);
            // If we fail to stop the transaction, we ignore since there is no
            // path to recovery. Its also not clear what to do
            if (response.status == types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR) {
                EVLOG_error << "Failed to stop a transaction on the power meter " << response.error.value_or("");
                break;
            } else if (response.status == types::powermeter::TransactionRequestStatus::OK) {
                shared_context.start_signed_meter_value = response.start_signed_meter_value;
                shared_context.stop_signed_meter_value = response.signed_meter_value;
                break;
            }
        }

        signal_simple_event(types::evse_manager::SessionEventEnum::ChargingFinished);
        signal_transaction_finished_event(shared_context.last_stop_transaction_reason,
                                          shared_context.stop_transaction_id_token);
        return true;
    }
    return false;
}

void Charger::start_session(bool authfirst) {
    shared_context.session_active = true;
    shared_context.authorized = false;
    shared_context.session_uuid = utils::generate_session_uuid();
    std::optional<types::authorization::ProvidedIdToken> provided_id_token;
    if (authfirst) {
        shared_context.last_start_session_reason = types::evse_manager::StartSessionReason::Authorized;
        provided_id_token = shared_context.id_token;
    } else {
        shared_context.last_start_session_reason = types::evse_manager::StartSessionReason::EVConnected;
    }
    signal_session_started_event(shared_context.last_start_session_reason, provided_id_token);
}

void Charger::stop_session() {
    shared_context.session_active = false;
    shared_context.authorized = false;
    signal_simple_event(types::evse_manager::SessionEventEnum::SessionFinished);
    shared_context.session_uuid.clear();
}

bool Charger::start_transaction() {
    shared_context.stop_transaction_id_token.reset();
    shared_context.transaction_active = true;

    types::powermeter::TransactionReq req;
    req.evse_id = evse_id;
    req.transaction_id = shared_context.session_uuid;
    req.identification_status =
        types::powermeter::OCMFUserIdentificationStatus::ASSIGNED; // currently user is always assigned
    req.identification_flags = {};                                 // TODO: Collect IF. Not all known in EVerest
    req.identification_type = utils::convert_to_ocmf_identification_type(shared_context.id_token.id_token.type);
    req.identification_level = std::nullopt; // TODO: Not yet known to EVerest
    req.identification_data = shared_context.id_token.id_token.value;
    req.tariff_text = std::nullopt; // TODO: Not yet known to EVerest

    for (const auto& meter : r_powermeter_billing) {
        const auto response = meter->call_start_transaction(req);
        // If we want to start the session but the meter fail, we stop the charging since
        // we can't bill the customer.
        if (response.status == types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR) {
            EVLOG_error << "Failed to start a transaction on the power meter " << response.error.value_or("");
            if (true == config_context.fail_on_powermeter_errors) {
                error_handling->raise_powermeter_transaction_start_failed_error(
                    "Failed to start transaction on the power meter");
                return false;
            }
        }
    }

    store->store_session(shared_context.session_uuid);
    signal_transaction_started_event(shared_context.id_token);
    return true;
}

void Charger::stop_transaction() {
    shared_context.transaction_active = false;
    shared_context.last_stop_transaction_reason = types::evse_manager::StopTransactionReason::EVDisconnected;

    for (const auto& meter : r_powermeter_billing) {
        const auto response = meter->call_stop_transaction(shared_context.session_uuid);
        // If we fail to stop the transaction, we ignore since there is no
        // path to recovery. Its also not clear what to do
        if (response.status == types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR) {
            EVLOG_error << "Failed to stop a transaction on the power meter " << response.error.value_or("");
            break;
        } else if (response.status == types::powermeter::TransactionRequestStatus::OK) {
            shared_context.start_signed_meter_value = response.start_signed_meter_value;
            shared_context.stop_signed_meter_value = response.signed_meter_value;
            break;
        }
    }

    store->clear_session();

    signal_simple_event(types::evse_manager::SessionEventEnum::ChargingFinished);
    signal_transaction_finished_event(shared_context.last_stop_transaction_reason,
                                      shared_context.stop_transaction_id_token);
}

void Charger::cleanup_transactions_on_startup() {
    // See if we have an open transaction in persistent storage
    auto session_uuid = store->get_session();
    if (not session_uuid.empty()) {
        EVLOG_info << "Cleaning up transaction with UUID " << session_uuid << " on start up";
        store->clear_session();

        // If yes, try to close nicely with the ID we remember and trigger a transaction finished event on success
        for (const auto& meter : r_powermeter_billing) {
            const auto response = meter->call_stop_transaction(session_uuid);
            // If we fail to stop the transaction, it was probably just not active anymore
            if (response.status == types::powermeter::TransactionRequestStatus::UNEXPECTED_ERROR) {
                EVLOG_warning << "Failed to stop a transaction on the power meter " << response.error.value_or("");
                break;
            } else if (response.status == types::powermeter::TransactionRequestStatus::OK) {
                // Fill in OCMF from the recovered transaction
                shared_context.start_signed_meter_value = response.start_signed_meter_value;
                shared_context.stop_signed_meter_value = response.signed_meter_value;
                break;
            }
        }

        // Send out event to inform OCPP et al
        std::optional<types::authorization::ProvidedIdToken> id_token;
        signal_transaction_finished_event(types::evse_manager::StopTransactionReason::PowerLoss, id_token);
    }

    // Now we did what we could to clean up, so if there are still transactions going on in the power meter close them
    // anyway. In this case we cannot generate a transaction finished event for OCPP et al since we cannot match it to
    // our transaction anymore.
    EVLOG_info << "Cleaning up any other transaction on start up";
    for (const auto& meter : r_powermeter_billing) {
        meter->call_stop_transaction("");
    }
}

std::optional<types::units_signed::SignedMeterValue>
Charger::take_signed_meter_data(std::optional<types::units_signed::SignedMeterValue>& in) {
    std::optional<types::units_signed::SignedMeterValue> out;
    std::swap(out, in);
    return out;
}

std::optional<types::units_signed::SignedMeterValue> Charger::get_stop_signed_meter_value() {
    // This is used only inside of the state machine, so we do not need to lock here.
    return take_signed_meter_data(shared_context.stop_signed_meter_value);
}

std::optional<types::units_signed::SignedMeterValue> Charger::get_start_signed_meter_value() {
    // This is used only inside of the state machine, so we do not need to lock here.
    return take_signed_meter_data(shared_context.start_signed_meter_value);
}

bool Charger::switch_three_phases_while_charging(bool n) {
    if (shared_context.hlc_charging_active) {
        return false;
    }

    if (shared_context.current_state == EvseState::Charging) {
        // In charging state, we need to go via a helper state for the delay
        shared_context.switch_3ph1ph_threephase = n;
        shared_context.switch_3ph1ph_threephase_ongoing = true;
        internal_context.switching_phases_return_state = EvseState::PrepareCharging;
        shared_context.current_state = EvseState::SwitchPhases;
    } else if (shared_context.current_state == EvseState::SwitchPhases) {
        shared_context.switch_3ph1ph_threephase = n;
    } else if (shared_context.current_state == EvseState::WaitingForEnergy) {
        shared_context.switch_3ph1ph_threephase = n;
        shared_context.switch_3ph1ph_threephase_ongoing = true;
        internal_context.switching_phases_return_state = EvseState::WaitingForEnergy;
        shared_context.current_state = EvseState::SwitchPhases;
    } else {
        // In all other states we can tell the bsp directly.
        bsp->switch_three_phases_while_charging(n);
    }
    return true;
}

void Charger::setup(bool has_ventilation, const ChargeMode _charge_mode, bool _ac_hlc_enabled,
                    bool _ac_hlc_use_5percent, bool _ac_enforce_hlc, bool _ac_with_soc_timeout,
                    float _soft_over_current_tolerance_percent, float _soft_over_current_measurement_noise_A,
                    const int _switch_3ph1ph_delay_s, const std::string _switch_3ph1ph_cp_state,
                    const int _soft_over_current_timeout_ms, const int _state_F_after_fault_ms,
                    const bool fail_on_powermeter_errors, const bool raise_mrec9,
                    const int sleep_before_enabling_pwm_hlc_mode_ms) {
    // set up board support package
    bsp->setup(has_ventilation);

    Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::Charger_setup);
    // cache our config variables
    config_context.charge_mode = _charge_mode;
    ac_hlc_enabled_current_session = config_context.ac_hlc_enabled = _ac_hlc_enabled;
    config_context.ac_hlc_use_5percent = _ac_hlc_use_5percent;
    config_context.ac_enforce_hlc = _ac_enforce_hlc;
    config_context.soft_over_current_timeout_ms = _soft_over_current_timeout_ms;
    shared_context.ac_with_soc_timeout = _ac_with_soc_timeout;
    shared_context.ac_with_soc_timer = 3600000;
    soft_over_current_tolerance_percent = _soft_over_current_tolerance_percent;
    soft_over_current_measurement_noise_A = _soft_over_current_measurement_noise_A;

    config_context.switch_3ph1ph_delay_s = _switch_3ph1ph_delay_s;
    config_context.switch_3ph1ph_cp_state_F = _switch_3ph1ph_cp_state == "F";

    config_context.state_F_after_fault_ms = _state_F_after_fault_ms;
    config_context.fail_on_powermeter_errors = fail_on_powermeter_errors;
    config_context.raise_mrec9 = raise_mrec9;
    config_context.sleep_before_enabling_pwm_hlc_mode_ms = sleep_before_enabling_pwm_hlc_mode_ms;

    if (config_context.charge_mode == ChargeMode::AC and config_context.ac_hlc_enabled)
        EVLOG_info << "AC HLC mode enabled.";
}

Charger::EvseState Charger::get_current_state() {
    Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::Charger_get_current_state);
    return shared_context.current_state;
}

bool Charger::get_authorized_pnc() {
    Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::Charger_get_authorized_pnc);
    return (shared_context.authorized and shared_context.authorized_pnc);
}

bool Charger::get_authorized_eim() {
    Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::Charger_get_authorized_eim);
    return (shared_context.authorized and not shared_context.authorized_pnc);
}

bool Charger::get_authorized_pnc_ready_for_hlc() {
    bool auth = false, ready = false;
    Everest::scoped_lock_timeout lock(state_machine_mutex,
                                      Everest::MutexDescription::Charger_get_authorized_pnc_ready_for_hlc);
    auth = (shared_context.authorized and shared_context.authorized_pnc);
    ready = (shared_context.current_state == EvseState::ChargingPausedEV) or
            (shared_context.current_state == EvseState::ChargingPausedEVSE) or
            (shared_context.current_state == EvseState::Charging) or
            (shared_context.current_state == EvseState::WaitingForEnergy);
    return (auth and ready);
}

bool Charger::get_authorized_eim_ready_for_hlc() {
    bool auth = false, ready = false;
    Everest::scoped_lock_timeout lock(state_machine_mutex,
                                      Everest::MutexDescription::Charger_get_authorized_eim_ready_for_hlc);
    auth = (shared_context.authorized and not shared_context.authorized_pnc);
    ready = (shared_context.current_state == EvseState::ChargingPausedEV) or
            (shared_context.current_state == EvseState::ChargingPausedEVSE) or
            (shared_context.current_state == EvseState::Charging) or
            (shared_context.current_state == EvseState::WaitingForEnergy);
    return (auth and ready);
}

std::string Charger::get_session_id() const {
    return shared_context.session_uuid;
}

void Charger::authorize(bool a, const types::authorization::ProvidedIdToken& token) {
    Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::Charger_authorize);
    if (a) {
        shared_context.id_token = token;
        // First user interaction was auth? Then start session already here and not at plug in
        if (not shared_context.session_active) {
            start_session(true);
        }
        signal_simple_event(types::evse_manager::SessionEventEnum::Authorized);
        shared_context.authorized = true;
        shared_context.authorized_pnc =
            token.authorization_type == types::authorization::AuthorizationType::PlugAndCharge;
    } else {
        if (shared_context.session_active) {
            stop_session();
        }
        shared_context.authorized = false;
    }
}

bool Charger::deauthorize() {
    Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::Charger_deauthorize);
    return deauthorize_internal();
}

bool Charger::deauthorize_internal() {
    if (shared_context.session_active) {
        signal_simple_event(types::evse_manager::SessionEventEnum::Deauthorized);
        auto s = shared_context.current_state;

        if (s == EvseState::Disabled or s == EvseState::Idle or s == EvseState::WaitingForAuthentication) {

            // We can safely remove auth as it is not in use right now
            if (not shared_context.authorized) {
                signal_simple_event(types::evse_manager::SessionEventEnum::PluginTimeout);
                if (config_context.raise_mrec9) {
                    error_handling->raise_authorization_timeout_error("No authorization was provided within timeout.");
                }
                return false;
            }
            shared_context.authorized = false;
            stop_session();
            return true;
        }
    }
    return false;
}

void Charger::enable_disable_initial_state_publish() {
    Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::Charger_disable);
    types::evse_manager::EnableDisableSource source{types::evse_manager::Enable_source::Unspecified,
                                                    types::evse_manager::Enable_state::Unassigned, 10000};

    // check the startup state and publish it
    if (shared_context.current_state == EvseState::Disabled) {
        signal_simple_event(types::evse_manager::SessionEventEnum::Disabled);
        source.enable_state = types::evse_manager::Enable_state::Disable;
    } else {
        signal_simple_event(types::evse_manager::SessionEventEnum::Enabled);
        source.enable_state = types::evse_manager::Enable_state::Enable;
    }

    // ensure the state is in the table
    enable_disable_source_table_update(source);
    active_enable_disable_source = source;
}

bool Charger::enable_disable(int connector_id, const types::evse_manager::EnableDisableSource& source) {
    Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::Charger_disable);

    const auto last = active_enable_disable_source;

    // add/update enable_disable_source_table with new source information
    enable_disable_source_table_update(source);

    // process the table to calculate the current state
    // updates/recalculates active_enable_disable_source
    const bool is_enabled = parse_enable_disable_source_table();

    // update state to Idle only when applied to non-zero connector ID
    // note Disable changes the state regardless of the connnector ID
    if (connector_id != 0) {
        shared_context.connector_enabled = is_enabled;
    }

    if (shared_context.current_state == EvseState::Disabled && shared_context.connector_enabled) {
        // note this can change state when connector_id = 0 when the previous
        // state is enabled
        shared_context.current_state = EvseState::Idle;
    }

    // check for state change
    // for this check Unassigned and Enabled are equivalent
    // previous   current    result
    // ========   =======    ======
    // Unassigned Unassigned false
    // Unassigned Enable     false
    // Unassigned Disable    true
    // Enable     Unassigned false
    // Enable     Enable     false
    // Enable     Disable    true
    // Disable    Unassigned true
    // Disable    Enable     true
    // Disable    Disable    false
    //
    // simplifies to:
    // (previous == Disable or current == Disable) and (previous != current)

    using namespace types::evse_manager;
    if ((active_enable_disable_source.enable_state == Enable_state::Disable ||
         last.enable_state == Enable_state::Disable) &&
        active_enable_disable_source.enable_state != last.enable_state) {
        // the state has changed so process events
        if (is_enabled) {
            signal_simple_event(types::evse_manager::SessionEventEnum::Enabled);
        } else {
            shared_context.current_state = EvseState::Disabled;
            signal_simple_event(types::evse_manager::SessionEventEnum::Disabled);
        }
    }

    return is_enabled;
}

bool Charger::parse_enable_disable_source_table() {
    /* Decide if we are enabled or disabled from the current table.
     The logic is as follows:

     The source is an enum of the following source types :

        - Unspecified
        - LocalAPI
        - LocalKeyLock
        - ServiceTechnician
        - RemoteKeyLock
        - MobileApp
        - FirmwareUpdate
        - CSMS

    The state can be either "enable", "disable", or "unassigned".

    "enable" and "disable" enforce the state to be enable/disable, while unassigned means that the source does not care
    about the state and other sources may decide.

    Each call to this command will update an internal table that looks like this:
    void enable_disable_source_table_update(const types::evse_manager::EnableDisableSource& update);

    Evaluation will be done based on priorities. 0 is the highest priority, 10000 the lowest, so in this example the
    connector will be enabled regardless of what other sources say. Imagine LocalKeyLock sends a "unassigned, prio 0",
    the table will then look like this:

    | Source       | State         | Priority |
    | ------------ | ------------- | -------- |
    | Unspecified  | unassigned    |   10000  |
    | LocalAPI     | disable       |      42  |
    | LocalKeyLock | unassigned    |       0  |

    So now the connector will be disabled, because the second highest priority (42) sets it to disabled.

    If all sources are unassigned, the connector is enabled.
    If two sources have the same priority, "disabled" has priority over "enabled".
    */

    bool is_enabled = true; // By default, it is enabled.
    types::evse_manager::EnableDisableSource winning_source{types::evse_manager::Enable_source::Unspecified,
                                                            types::evse_manager::Enable_state::Unassigned, 10000};

    // Walk through table
    for (const auto& entry : enable_disable_source_table) {
        // Ignore unassigned entries
        if (entry.enable_state == types::evse_manager::Enable_state::Unassigned) {
            continue;
        }

        if (winning_source.enable_state == types::evse_manager::Enable_state::Unassigned) {
            // Use the first entry that is not Unassigned
            winning_source = entry;
            if (entry.enable_state == types::evse_manager::Enable_state::Disable) {
                is_enabled = false;
            } else {
                is_enabled = true;
            }
        } else if (entry.enable_priority == winning_source.enable_priority) {
            // At the same priority, disable has higher priority then enable
            if (entry.enable_state == types::evse_manager::Enable_state::Disable) {
                is_enabled = false;
                winning_source = entry;
            }

        } else if (entry.enable_priority < winning_source.enable_priority) {
            winning_source = entry;
            if (entry.enable_state == types::evse_manager::Enable_state::Disable) {
                is_enabled = false;
            } else {
                is_enabled = true;
            }
        }
    }

    active_enable_disable_source = winning_source;
    return is_enabled;
}

void Charger::enable_disable_source_table_update(const types::evse_manager::EnableDisableSource& source) {
    // already locked

    // add/update enable_disable_source_table with new source information
    const auto enable_source = source.enable_source;
    const auto fn = [enable_source](const auto& entry) { return entry.enable_source == enable_source; };
    if (auto it = std::find_if(enable_disable_source_table.begin(), enable_disable_source_table.end(), fn);
        it == enable_disable_source_table.end()) {
        // add to table
        enable_disable_source_table.push_back(source);
    } else {
        // update in table
        *it = source;
    }
}

void Charger::set_faulted() {
    Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::Charger_set_faulted);
    shared_context.error_prevent_charging_flag = true;
}

std::string Charger::evse_state_to_string(EvseState s) {
    switch (s) {
    case EvseState::Disabled:
        return ("Disabled");
        break;
    case EvseState::Idle:
        return ("Idle");
        break;
    case EvseState::WaitingForAuthentication:
        return ("Wait for Auth");
        break;
    case EvseState::Charging:
        return ("Charging");
        break;
    case EvseState::ChargingPausedEV:
        return ("Car Paused");
        break;
    case EvseState::ChargingPausedEVSE:
        return ("EVSE Paused");
        break;
    case EvseState::WaitingForEnergy:
        return ("Wait for energy");
        break;
    case EvseState::Finished:
        return ("Finished");
        break;
    case EvseState::T_step_EF:
        return ("T_step_EF");
        break;
    case EvseState::T_step_X1:
        return ("T_step_X1");
        break;
    case EvseState::Replug:
        return ("Replug");
        break;
    case EvseState::PrepareCharging:
        return ("PrepareCharging");
        break;
    case EvseState::StoppingCharging:
        return ("StoppingCharging");
        break;
    case EvseState::SwitchPhases:
        return ("SwitchPhases");
        break;
    }
    return "Invalid";
}

float Charger::get_max_current() {
    Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::Charger_get_max_current);
    return get_max_current_internal();
}

float Charger::get_max_current_internal() {
    auto maxc = shared_context.max_current;

    if (connector_type == types::evse_board_support::Connector_type::IEC62196Type2Socket and
        shared_context.max_current_cable < maxc and shared_context.current_state not_eq EvseState::Idle) {
        maxc = shared_context.max_current_cable;
    }

    return maxc;
}

float Charger::get_max_current_signalled_to_ev_internal() {
    // For basic charging, the max current signalled to the EV may be different from the actual current limit
    // for up to 5 seconds as the PWM may only be updated every 5 seconds according to IEC61851-1.
    if (not shared_context.hlc_charging_active) {
        return internal_context.pwm_set_last_ampere;
    } else {
        return get_max_current_internal();
    }
}

void Charger::set_current_drawn_by_vehicle(float l1, float l2, float l3) {
    Everest::scoped_lock_timeout lock(state_machine_mutex,
                                      Everest::MutexDescription::Charger_set_current_drawn_by_vehicle);
    shared_context.current_drawn_by_vehicle[0] = l1;
    shared_context.current_drawn_by_vehicle[1] = l2;
    shared_context.current_drawn_by_vehicle[2] = l3;
}

void Charger::check_soft_over_current() {

    // Allow some tolerance
    float limit = (get_max_current_signalled_to_ev_internal() + soft_over_current_measurement_noise_A) *
                  (1. + soft_over_current_tolerance_percent / 100.);

    if (shared_context.current_drawn_by_vehicle[0] > limit or shared_context.current_drawn_by_vehicle[1] > limit or
        shared_context.current_drawn_by_vehicle[2] > limit) {
        if (not internal_context.over_current) {
            internal_context.over_current = true;
            // timestamp when over current happend first
            internal_context.last_over_current_event = std::chrono::steady_clock::now();
            session_log.evse(false,
                             fmt::format("Soft overcurrent event (L1:{}, L2:{}, L3:{}, limit {}), starting timer.",
                                         shared_context.current_drawn_by_vehicle[0],
                                         shared_context.current_drawn_by_vehicle[1],
                                         shared_context.current_drawn_by_vehicle[2], limit));
        }
    } else {
        internal_context.over_current = false;
    }
    auto now = std::chrono::steady_clock::now();
    auto time_since_over_current_started =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - internal_context.last_over_current_event).count();
    if (internal_context.over_current and
        time_since_over_current_started >= config_context.soft_over_current_timeout_ms) {
        auto errstr =
            fmt::format("Soft overcurrent event (L1:{}, L2:{}, L3:{}, limit {}) triggered",
                        shared_context.current_drawn_by_vehicle[0], shared_context.current_drawn_by_vehicle[1],
                        shared_context.current_drawn_by_vehicle[2], limit);
        session_log.evse(false, errstr);
        // raise the OC error
        error_handling->raise_overcurrent_error(errstr);
    }
}

// returns whether power is actually available from EnergyManager
// i.e. max_current is in valid range
bool Charger::power_available() {
    if (shared_context.max_current_valid_until < date::utc_clock::now()) {
        EVLOG_warning << "Power budget expired, falling back to 0. Last update: "
                      << Everest::Date::to_rfc3339(shared_context.max_current_valid_until)
                      << " Now:" << Everest::Date::to_rfc3339(date::utc_clock::now());
        if (shared_context.max_current > 0.) {
            shared_context.max_current = 0.;
            signal_max_current(shared_context.max_current);
        }
    }
    return (get_max_current_internal() > 5.9);
}

void Charger::request_error_sequence() {
    Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::Charger_request_error_sequence);
    if (shared_context.current_state == EvseState::WaitingForAuthentication or
        shared_context.current_state == EvseState::PrepareCharging) {
        internal_context.t_step_EF_return_state = shared_context.current_state;
        internal_context.t_step_EF_return_ampere = 0.;
        shared_context.current_state = EvseState::T_step_EF;
        signal_slac_reset();
        if (hlc_use_5percent_current_session) {
            internal_context.t_step_EF_return_pwm = PWM_5_PERCENT;
        } else {
            internal_context.t_step_EF_return_pwm = 0.;
        }
    }
}

void Charger::set_matching_started(bool m) {
    Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::Charger_set_matching_started);
    shared_context.matching_started = m;
}

void Charger::notify_currentdemand_started() {
    Everest::scoped_lock_timeout lock(state_machine_mutex,
                                      Everest::MutexDescription::Charger_notify_currentdemand_started);
    if (shared_context.current_state == EvseState::PrepareCharging) {
        signal_simple_event(types::evse_manager::SessionEventEnum::ChargingStarted);
        shared_context.current_state = EvseState::Charging;
    }
}

void Charger::inform_new_evse_max_hlc_limits(const types::iso15118::DcEvseMaximumLimits& _currentEvseMaxLimits) {
    Everest::scoped_lock_timeout lock(state_machine_mutex,
                                      Everest::MutexDescription::Charger_inform_new_evse_max_hlc_limits);
    shared_context.current_evse_max_limits = _currentEvseMaxLimits;
}

types::iso15118::DcEvseMaximumLimits Charger::get_evse_max_hlc_limits() {
    Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::Charger_get_evse_max_hlc_limits);
    return shared_context.current_evse_max_limits;
}

// HLC stack signalled a pause request for the lower layers.
void Charger::dlink_pause() {
    Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::Charger_dlink_pause);
    shared_context.hlc_allow_close_contactor = false;
    pwm_off();
    shared_context.hlc_charging_terminate_pause = HlcTerminatePause::Pause;
}

// HLC requested end of charging session, so we can stop the 5% PWM
void Charger::dlink_terminate() {
    Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::Charger_dlink_terminate);
    shared_context.hlc_allow_close_contactor = false;
    pwm_off();
    shared_context.hlc_charging_terminate_pause = HlcTerminatePause::Terminate;
}

void Charger::dlink_error() {
    Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::Charger_dlink_error);

    shared_context.hlc_allow_close_contactor = false;

    // Is PWM on at the moment?
    if (not shared_context.pwm_running) {
        // [V2G3-M07-04]: With receiving a D-LINK_ERROR.request from HLE in X1 state, the EVSE’s
        // communication node shall perform a state X1 to state E/F to state X1 or X2 transition.
    } else {
        // [V2G3-M07-05]: With receiving a D-LINK_ERROR.request in X2 state from HLE, the EVSE’s
        // communication node shall perform a state X2 to X1 to state E/F to state X1 or X2 transition.

        // Are we in 5% mode or not?
        if (hlc_use_5percent_current_session) {
            // [V2G3-M07-06] Within the control pilot state X1, the communication node shall leave the
            // logical network and change the matching state to "Unmatched". [V2G3-M07-07] With reaching the
            // state “Unmatched”, the EVSE shall switch to state E/F.

            // FIXME: We don't wait for SLAC to go to UNMATCHED in X1 for now but just do a normal 3 seconds
            // t_step_X1 instead. This should be more then sufficient for the SLAC module to reset.

            // Do t_step_X1 with a t_step_EF afterwards
            // [V2G3-M07-08] The state E/F shall be applied at least T_step_EF: This is already handled in
            // the t_step_EF state.
            internal_context.t_step_X1_return_state = EvseState::T_step_EF;
            internal_context.t_step_X1_return_pwm = 0.;
            internal_context.t_step_EF_return_ampere = 0.;
            shared_context.current_state = EvseState::T_step_X1;

            // After returning from T_step_EF, go to Waiting for Auth (We are restarting the session)
            internal_context.t_step_EF_return_state = EvseState::WaitingForAuthentication;
            // [V2G3-M07-09] After applying state E/F, the EVSE shall switch to contol pilot state X1 or X2
            // as soon as the EVSE is ready control for pilot incoming duty matching cycle requests: This is
            // already handled in the Auth step.

            // [V2G3-M07-05] says we need to go through X1 at the end of the sequence
            internal_context.t_step_EF_return_pwm = 0.;
            internal_context.t_step_EF_return_ampere = 0.;
        }
        // else {
        // [V2G3-M07-10] Gives us two options for nominal PWM mode and HLC in case of error: We choose
        // [V2G3-M07-12] (Don't interrupt basic AC charging just because an error in HLC happend) So we
        // don't do anything here, SLAC will be notified anyway to reset
        //}
    }
}

void Charger::set_hlc_charging_active() {
    Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::Charger_set_hlc_charging_active);
    shared_context.hlc_charging_active = true;
}

void Charger::set_hlc_allow_close_contactor(bool on) {
    Everest::scoped_lock_timeout lock(state_machine_mutex,
                                      Everest::MutexDescription::Charger_set_hlc_allow_close_contactor);
    shared_context.hlc_allow_close_contactor = on;
}

void Charger::set_hlc_error() {
    Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::Charger_set_hlc_error);
    shared_context.error_prevent_charging_flag = true;
}

// this resets the BCB sequence (which may contain 1-3 toggle pulses)
void Charger::bcb_toggle_reset() {
    internal_context.hlc_ev_pause_bcb_count = 0;
    internal_context.hlc_bcb_sequence_started = false;
}

// call this B->C transitions
void Charger::bcb_toggle_detect_start_pulse() {
    // For HLC charging, PWM already off: This is probably a BCB Toggle to wake us up from sleep mode.
    // Remember start of BCB toggle.
    internal_context.hlc_ev_pause_start_of_bcb = std::chrono::steady_clock::now();
    if (internal_context.hlc_ev_pause_bcb_count == 0) {
        // remember sequence start
        internal_context.hlc_ev_pause_start_of_bcb_sequence = std::chrono::steady_clock::now();
        internal_context.hlc_bcb_sequence_started = true;
    }
}

// call this on C->B transitions
void Charger::bcb_toggle_detect_stop_pulse() {
    if (not internal_context.hlc_bcb_sequence_started) {
        return;
    }

    // This is probably and end of BCB toggle, verify it was not too long or too short
    auto pulse_length = std::chrono::steady_clock::now() - internal_context.hlc_ev_pause_start_of_bcb;

    if (pulse_length > TP_EV_VALD_STATE_DURATION_MIN and pulse_length < TP_EV_VALD_STATE_DURATION_MAX) {

        // enable PWM again. ISO stack should have been ready for the whole time.
        // FIXME where do we go from here? Auth?
        internal_context.hlc_ev_pause_bcb_count++;

        session_log.car(false, fmt::format("BCB toggle ({} ms), #{} in sequence",
                                           std::chrono::duration_cast<std::chrono::milliseconds>(pulse_length).count(),
                                           internal_context.hlc_ev_pause_bcb_count));

    } else {
        internal_context.hlc_ev_pause_bcb_count = 0;
        EVLOG_warning << "BCB toggle with invalid duration detected: "
                      << std::chrono::duration_cast<std::chrono::milliseconds>(pulse_length).count();
    }
}

// Query if a BCB sequence (of 1-3 pulses) was detected and is finished. If that is true, PWM can be enabled again
// etc
bool Charger::bcb_toggle_detected() {
    auto sequence_length = std::chrono::steady_clock::now() - internal_context.hlc_ev_pause_start_of_bcb_sequence;
    if (internal_context.hlc_bcb_sequence_started and
        (sequence_length > TT_EVSE_VALD_TOGGLE or internal_context.hlc_ev_pause_bcb_count >= 3)) {
        // no need to wait for further BCB toggles
        internal_context.hlc_ev_pause_bcb_count = 0;
        return true;
    }
    return false;
}

bool Charger::stop_charging_on_fatal_error() {
    Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::Charger_errors_prevent_charging);
    return stop_charging_on_fatal_error_internal();
}

bool Charger::entered_fatal_error_state() {
    return shared_context.error_prevent_charging_flag and not shared_context.last_error_prevent_charging_flag;
}

int Charger::time_in_fatal_error_state_ms() {
    if (not internal_context.fatal_error_timer_running) {
        return 0;
    } else {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() -
                                                                     internal_context.fatal_error_became_active)
            .count();
    }
}

bool Charger::stop_charging_on_fatal_error_internal() {
    bool err = false;
    if (shared_context.error_prevent_charging_flag) {
        if (not shared_context.last_error_prevent_charging_flag) {
            internal_context.fatal_error_became_active = std::chrono::steady_clock::now();

            graceful_stop_charging();
        }
        err = true;
    }
    internal_context.fatal_error_timer_running = err;
    shared_context.last_error_prevent_charging_flag = shared_context.error_prevent_charging_flag;
    return err;
}

void Charger::graceful_stop_charging() {
    if (shared_context.pwm_running) {
        pwm_off();
    }

    // Shutdown DC power supplies
    if (config_context.charge_mode == ChargeMode::DC) {
        signal_dc_supply_off();
    }

    // open contactors
    bsp->allow_power_on(false, types::evse_board_support::Reason::PowerOff);
}

void Charger::clear_errors_on_unplug() {
    error_handling->clear_overcurrent_error();
    error_handling->clear_internal_error();
    error_handling->clear_powermeter_transaction_start_failed_error();
    error_handling->clear_authorization_timeout_error();
}

types::evse_manager::EnableDisableSource Charger::get_last_enable_disable_source() {
    return active_enable_disable_source;
}

} // namespace module
