// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
/*
 * Charger.cpp
 *
 *  Created on: 08.03.2021
 *      Author: cornelius
 *
 * TODO:
 * - way too many calls to bsp (e.g. every 50msecs)!
 * - we need to publish state to external mqtt to make nodered happy
 * - overcurrent: works in uC. in low control mode uc can only verify hard limit, need to test soft limit here!
 *
 */

#include "Charger.hpp"

#include <math.h>
#include <string.h>

#include <fmt/core.h>

namespace module {

Charger::Charger(const std::unique_ptr<board_support_ACIntf>& r_bsp) : r_bsp(r_bsp) {
    maxCurrent = 6.0;
    authorized = false;
    r_bsp->call_enable(false);

    update_pwm_last_dc = 0.;
    cancelled = false;

    currentState = EvseState::Disabled;
    lastState = EvseState::Disabled;

    paused_by_user = false;

    currentDrawnByVehicle[0] = 0.;
    currentDrawnByVehicle[1] = 0.;
    currentDrawnByVehicle[2] = 0.;

    t_step_EF_returnState = EvseState::Faulted;
    t_step_X1_returnState = EvseState::Faulted;

    matching_started = false;

    hlc_use_5percent_current_session = false;
}

Charger::~Charger() {
    r_bsp->call_pwm_F();
}

void Charger::mainThread() {

    // Enable CP output
    r_bsp->call_enable(true);

    // publish initial values
    signalMaxCurrent(getMaxCurrent());
    signalState(currentState);
    signalError(errorState);

    while (true) {

        if (mainThreadHandle.shouldExit()) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        stateMutex.lock();

        // update power limits
        powerAvailable();

        // Run our own state machine update (i.e. run everything that needs
        // to be done on regular intervals independent from events)
        runStateMachine();

        stateMutex.unlock();
    }
}

void Charger::runStateMachine() {
    bool new_in_state = lastState != currentState;
    if (new_in_state) {
        session_log.evse(
            false, fmt::format("Charger state: {}->{}", evseStateToString(lastState), evseStateToString(currentState)));
    }
    lastState = currentState;
    auto now = std::chrono::system_clock::now();

    if (ac_with_soc_timeout && (ac_with_soc_timer -= 50) < 0) {
        ac_with_soc_timeout = false;
        ac_with_soc_timer = 3600000;
        signalACWithSoCTimeout();
	return;
    }

    if (new_in_state) {
        currentStateStarted = now;
        signalState(currentState);
        if (currentState == EvseState::Error) {
            signalError(errorState);
        }
    }

    auto timeInCurrentState = std::chrono::duration_cast<std::chrono::milliseconds>(now - currentStateStarted).count();

    switch (currentState) {
    case EvseState::Disabled:
        if (new_in_state) {
            signalEvent(EvseEvent::Disabled);
            pwm_F();
        }
        break;

    case EvseState::Replug:
        if (new_in_state) {
            signalEvent(EvseEvent::ReplugStarted);
            // start timer in case we need to
            if (ac_with_soc_timeout)
                ac_with_soc_timer = 120000;
        }
        // simply wait here until BSP informs us that replugging was finished
        break;

    case EvseState::Idle:
        // make sure we signal availability to potential new cars
        if (new_in_state) {
            pwm_off();
            Authorize(false, "", false);
            cancelled = false;
        }
        break;

    case EvseState::WaitingForAuthentication:

        // Explicitly do not allow to be powered on. This is important
        // to make sure control_pilot does not switch on relais even if
        // we start PWM here
        if (new_in_state) {
            r_bsp->call_allow_power_on(false);

            if (lastState == EvseState::Replug) {
                signalEvent(EvseEvent::ReplugFinished);
            } else {
                // External signal on MQTT
                signalEvent(EvseEvent::AuthRequired);
            }
            hlc_use_5percent_current_session = false;

            // switch on HLC if configured. May be switched off later on after retries for this session only.
            if (charge_mode == "AC") {
                ac_hlc_enabled_current_session = ac_hlc_enabled;
                if (ac_hlc_enabled_current_session) {
                    hlc_use_5percent_current_session = ac_hlc_use_5percent;
                }
            } else if (charge_mode == "DC") {
                hlc_use_5percent_current_session = true;
            } else {
                // unsupported charging mode, give up here.
                currentState = EvseState::Error;
                errorState = ErrorState::Error_Internal;
            }

            if (hlc_use_5percent_current_session) {
                update_pwm_now(PWM_5_PERCENT);
            }
        }

        // Internal Signal to main module class that we need auth
        if (!(AuthorizedEIM() || AuthorizedPnC())) {
            signalAuthRequired();
        }

        // SLAC is running in the background trying to setup a PLC connection.

        // we get Auth (maybe before SLAC matching or during matching)
        // FIXME: getAuthorization needs to distinguish between EIM and PnC in Auth mananger

        // FIXME: Note Fig 7. is not fully supported here yet (AC Auth before plugin - this overides PnC and always
        // starts with nominal PWM). Hard to do here at the moment since auth before plugin is cached in auth manager
        // and is similar to a very quick auth after plugin. We need to support this as this is the only way a user can
        // skip PnC if he does NOT want to use it for this charging session. This basically works as Auth comes very
        // fast if it was done before plugin, but we need to ensure that we do not double auth with PnC as well

        // FIXME: In case V2G is not successfull after all, it needs
        // to send a dlink_error request, which then needs to:
        // AC mode: fall back to nominal PWM charging etc (not
        // implemented yet!), it can do so because it is already authorized.
        // DC mode: go to error_hlc and give up

        // FIXME: if slac reports a dlink_ready=false while we are still waiting for auth we should:
        // in AC mode: go back to non HLC nominal PWM mode
        // in DC mode: go to error_slac for this session

        if (AuthorizedEIM()) {
            session_log.evse(false, "EIM Authorization received");

            signalEvent(EvseEvent::ChargingStarted);
            EvseState targetState;

            if (powerAvailable()) {
                targetState = EvseState::ChargingPausedEV;
            } else {
                targetState = EvseState::ChargingPausedEVSE;
            }
            // EIM done and matching process not started -> we need to go through t_step_EF and fall back to nominal
            // PWM. This is a complete waste of 4 precious seconds.
            if (charge_mode == "AC") {
                if (ac_hlc_enabled_current_session) {
                    if (ac_enforce_hlc) {
                        // non standard compliant mode: we just keep 5 percent running all the time like in DC
                        session_log.evse(
                            false, "AC mode, HLC enabled(ac_enforce_hlc), keeping 5 percent on until a dlink error "
                                   "is signalled.");
                        hlc_use_5percent_current_session = true;
                        currentState = targetState;
                    } else {
                        if (!getMatchingStarted()) {
                            // SLAC matching was not started when EIM arrived

                            session_log.evse(
                                false,
                                fmt::format("AC mode, HLC enabled, matching not started yet. Go through t_step_EF and "
                                            "disable 5 percent if it was enabled before: {}",
                                            hlc_use_5percent_current_session));

                            // Figure 3 of ISO15118-3: 5 percent start, PnC and EIM
                            // Figure 4 of ISO15118-3: X1 start, PnC and EIM
                            t_step_EF_returnState = targetState;
                            t_step_EF_returnPWM = 0.;
                            // fall back to nominal PWM after the t_step_EF break. Note that
                            // ac_hlc_enabled_current_session remains untouched as HLC can still start later in nominal
                            // PWM mode
                            hlc_use_5percent_current_session = false;
                            currentState = EvseState::T_step_EF;
                        } else {
                            // SLAC matching was started already when EIM arrived
                            if (hlc_use_5percent_current_session) {
                                // Figure 5 of ISO15118-3: 5 percent start, PnC and EIM, matching already started when
                                // EIM was done
                                session_log.evse(false,
                                                 "AC mode, HLC enabled(5percent), matching already started. Go through "
                                                 "t_step_X1 and disable 5 percent.");
                                t_step_X1_returnState = targetState;
                                t_step_X1_returnPWM = 0.;
                                hlc_use_5percent_current_session = false;
                                currentState = EvseState::T_step_X1;
                            } else {
                                // Figure 6 of ISO15118-3: X1 start, PnC and EIM, matching already started when EIM was
                                // done. We can go directly to ChargingPausedEV, as we do not need to switch from 5
                                // percent to nominal first
                                session_log.evse(
                                    false, "AC mode, HLC enabled(X1), matching already started. We are in X1 so we can "
                                           "go directly to nominal PWM.");
                                currentState = targetState;
                            }
                        }
                    }

                } else {
                    // HLC is disabled for this session.
                    // simply proceed to ChargingPausedEV, as we are fully authorized to start charging whenever car
                    // wants to.
                    session_log.evse(false, "AC mode, HLC disabled. We are in X1 so we can "
                                            "go directly to nominal PWM.");
                    currentState = targetState;
                }
            } else if (charge_mode == "DC") {
                // Figure 8 of ISO15118-3: DC with EIM before or after plugin or PnC
                // simple here as we always stay within 5 percent mode anyway.
                session_log.evse(false, "DC mode. We are in 5percent mode so we can continue without further action.");
                currentState = targetState;
            } else {
                // unsupported charging mode, give up here.
                EVLOG_error << "Unsupported charging mode.";
                currentState = EvseState::Error;
                errorState = ErrorState::Error_Internal;
            }
        } else if (AuthorizedPnC()) {

            signalEvent(EvseEvent::ChargingStarted);

            EvseState targetState;
            if (powerAvailable()) {
                targetState = EvseState::ChargingPausedEV;
            } else {
                targetState = EvseState::ChargingPausedEVSE;
            }
            // We got authorization by Plug and Charge
            session_log.evse(false, "PnC Authorization received");
            if (charge_mode == "AC") {
                // Figures 3,4,5,6 of ISO15118-3: Independent on how we started we can continue with 5 percent
                // signalling once we got PnC authorization without going through t_step_EF or t_step_X1.

                session_log.evse(
                    false, "AC mode, HLC enabled, PnC auth received. We will continue with 5percent independent on "
                           "how we started.");
                hlc_use_5percent_current_session = true;
                currentState = targetState;

            } else if (charge_mode == "DC") {
                // Figure 8 of ISO15118-3: DC with EIM before or after plugin or PnC
                // simple here as we always stay within 5 percent mode anyway.
                session_log.evse(false, "DC mode. We are in 5percent mode so we can continue without further action.");
                currentState = targetState;
            } else {
                // unsupported charging mode, give up here.
                EVLOG_error << "Unsupported charging mode.";
                currentState = EvseState::Error;
                errorState = ErrorState::Error_Internal;
            }
        }

        break;

    case EvseState::T_step_EF:
        if (new_in_state) {
            session_log.evse(false, "Enter T_step_EF");
            pwm_F();
        }
        if (timeInCurrentState >= t_step_EF) {
            session_log.evse(false, "Exit T_step_EF");
            if (t_step_EF_returnPWM == 0.)
                pwm_off();
            else
                update_pwm_now(t_step_EF_returnPWM);
            currentState = t_step_EF_returnState;
        }
        break;

    case EvseState::T_step_X1:
        if (new_in_state) {
            session_log.evse(false, "Enter T_step_X1");
            pwm_off();
        }
        if (timeInCurrentState >= t_step_X1) {
            session_log.evse(false, "Exit T_step_X1");
            if (t_step_X1_returnPWM == 0.)
                pwm_off();
            else
                update_pwm_now(t_step_X1_returnPWM);
            currentState = t_step_X1_returnState;
        }
        break;

    case EvseState::Charging:
        if (charge_mode == "DC") {
            if (new_in_state) {
                r_bsp->call_allow_power_on(false);
            }
        } else {
            checkSoftOverCurrent();

            if (!powerAvailable()) {
                pauseChargingWaitForPower();
                break;
            }

            if (new_in_state) {
                signalEvent(EvseEvent::ChargingResumed);
                r_bsp->call_allow_power_on(true);
                // make sure we are enabling PWM
                if (hlc_use_5percent_current_session)
                    update_pwm_now(PWM_5_PERCENT);
                else
                    update_pwm_now(ampereToDutyCycle(getMaxCurrent()));
            } else {
                // update PWM if it has changed and 5 seconds have passed since last update
                if (!hlc_use_5percent_current_session)
                    update_pwm_max_every_5seconds(ampereToDutyCycle(getMaxCurrent()));
            }
        }
        break;

    case EvseState::ChargingPausedEV:
        if (charge_mode == "DC") {
            if (new_in_state) {
                r_bsp->call_allow_power_on(false);
                EVLOG_info << "Charger reached ChargingPausedEV for DC. Stopping here.";
            }

        } else {
            checkSoftOverCurrent();

            if (!powerAvailable()) {
                pauseChargingWaitForPower();
                break;
            }

            if (new_in_state) {
                signalEvent(EvseEvent::ChargingPausedEV);
                r_bsp->call_allow_power_on(true);
                // make sure we are enabling PWM
                if (hlc_use_5percent_current_session) {
                    update_pwm_now(PWM_5_PERCENT);
                } else {
                    update_pwm_now(ampereToDutyCycle(getMaxCurrent()));
                }
            } else {
                // update PWM if it has changed and 5 seconds have passed since last update
                if (!hlc_use_5percent_current_session) {
                    update_pwm_max_every_5seconds(ampereToDutyCycle(getMaxCurrent()));
                }
            }
        }
        break;

    case EvseState::ChargingPausedEVSE:
        if (new_in_state) {
            signalEvent(EvseEvent::ChargingPausedEVSE);
            pwm_off();
        }
        break;

    case EvseState::Finished:
        // We are temporarily unavailable to avoid that new cars plug in
        // during the cleanup phase. Re-Enable once we are in idle.
        if (new_in_state) {
            signalEvent(EvseEvent::SessionFinished);
            pwm_F();
        }
        // FIXME: this should be done by higher layer.
        restart();
        break;

    case EvseState::Error:
        if (new_in_state) {
            signalEvent(EvseEvent::Error);
            pwm_off();
        }
        // Do not set F here, as F cannot detect unplugging of car!
        break;

    case EvseState::Faulted:
        if (new_in_state) {
            signalEvent(EvseEvent::PermanentFault);
            pwm_F();
        }
        break;
    }
}

ControlPilotEvent Charger::string_to_control_pilot_event(const types::board_support::Event& event) {
    if (event == types::board_support::Event::CarPluggedIn) {
        return ControlPilotEvent::CarPluggedIn;
    } else if (event == types::board_support::Event::CarRequestedPower) {
        return ControlPilotEvent::CarRequestedPower;
    } else if (event == types::board_support::Event::PowerOn) {
        return ControlPilotEvent::PowerOn;
    } else if (event == types::board_support::Event::PowerOff) {
        return ControlPilotEvent::PowerOff;
    } else if (event == types::board_support::Event::CarRequestedStopPower) {
        return ControlPilotEvent::CarRequestedStopPower;
    } else if (event == types::board_support::Event::CarUnplugged) {
        return ControlPilotEvent::CarUnplugged;
    } else if (event == types::board_support::Event::ErrorE) {
        return ControlPilotEvent::Error_E;
    } else if (event == types::board_support::Event::ErrorDF) {
        return ControlPilotEvent::Error_DF;
    } else if (event == types::board_support::Event::ErrorRelais) {
        return ControlPilotEvent::Error_Relais;
    } else if (event == types::board_support::Event::ErrorRCD) {
        return ControlPilotEvent::Error_RCD;
    } else if (event == types::board_support::Event::ErrorVentilationNotAvailable) {
        return ControlPilotEvent::Error_VentilationNotAvailable;
    } else if (event == types::board_support::Event::ErrorOverCurrent) {
        return ControlPilotEvent::Error_OverCurrent;
    } else if (event == types::board_support::Event::LeaveBCD) {
        return ControlPilotEvent::LeaveBCD;
    } else if (event == types::board_support::Event::EnterBCD) {
        return ControlPilotEvent::EnterBCD;
    } else if (event == types::board_support::Event::PermanentFault) {
        return ControlPilotEvent::PermanentFault;
    } else if (event == types::board_support::Event::EvseReplugStarted) {
        return ControlPilotEvent::EvseReplugStarted;
    } else if (event == types::board_support::Event::EvseReplugFinished) {
        return ControlPilotEvent::EvseReplugFinished;
    }

    return ControlPilotEvent::Invalid;
}

void Charger::processEvent(types::board_support::Event event) {
    auto cp_event = string_to_control_pilot_event(event);

    switch (cp_event) {
    case ControlPilotEvent::CarPluggedIn:
    case ControlPilotEvent::CarRequestedPower:
    case ControlPilotEvent::CarRequestedStopPower:
    case ControlPilotEvent::CarUnplugged:
    case ControlPilotEvent::Error_DF:
    case ControlPilotEvent::Error_E:
    case ControlPilotEvent::LeaveBCD:
    case ControlPilotEvent::EnterBCD:
        session_log.car(false, fmt::format("Event {}", types::board_support::event_to_string(event)));
        break;
    case ControlPilotEvent::Error_OverCurrent:
    case ControlPilotEvent::Error_RCD:
    case ControlPilotEvent::Error_Relais:
    case ControlPilotEvent::Error_VentilationNotAvailable:
    case ControlPilotEvent::PermanentFault:
    case ControlPilotEvent::PowerOff:
    case ControlPilotEvent::PowerOn:
    case ControlPilotEvent::EvseReplugStarted:
    case ControlPilotEvent::EvseReplugFinished:

    default:
        session_log.evse(false, fmt::format("Event {}", types::board_support::event_to_string(event)));
        break;
    }

    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    // Process all event actions that are independent of the current state
    processCPEventsIndependent(cp_event);

    // Process all events that depend on the current state
    processCPEventsState(cp_event);
}

void Charger::processCPEventsState(ControlPilotEvent cp_event) {
    switch (currentState) {

    case EvseState::Idle:
        if (cp_event == ControlPilotEvent::CarPluggedIn) {
            signalEvent(EvseEvent::SessionStarted);
            // FIXME: Set cable current limit from PP resistor value for this
            // session e.g. readPPAmpacity();
            currentState = EvseState::WaitingForAuthentication;
        }
        break;

    case EvseState::WaitingForAuthentication:
        // FIXME: in simplified mode, we get a car requested power here directly
        // even if PWM is not enabled yet... this should be fixed in control
        // pilot logic? i.e. car requests power only if pwm was enabled? Could
        // be a fix, but with 5% mode it will not work.
        // this is work arounded in cp logic -> needs testing.
        if (cp_event == ControlPilotEvent::CarRequestedPower) {
            // FIXME
        }
        break;

    case EvseState::Charging:
        if (cp_event == ControlPilotEvent::CarRequestedStopPower) {
            currentState = EvseState::ChargingPausedEV;
        }
        break;

    case EvseState::ChargingPausedEV:
        if (cp_event == ControlPilotEvent::CarRequestedPower) {
            currentState = EvseState::Charging;
        }
        break;
    default:
        break;
    }
}

void Charger::processCPEventsIndependent(ControlPilotEvent cp_event) {

    switch (cp_event) {
    case ControlPilotEvent::EvseReplugStarted:
        currentState = EvseState::Replug;
        break;
    case ControlPilotEvent::EvseReplugFinished:
        currentState = EvseState::WaitingForAuthentication;
        break;
    case ControlPilotEvent::CarUnplugged:
        currentState = EvseState::Finished;
        break;
    case ControlPilotEvent::Error_E:
        currentState = EvseState::Error;
        errorState = ErrorState::Error_E;
        break;
    case ControlPilotEvent::Error_DF:
        currentState = EvseState::Error;
        errorState = ErrorState::Error_DF;
        break;
    case ControlPilotEvent::Error_Relais:
        currentState = EvseState::Error;
        errorState = ErrorState::Error_Relais;
        break;
    case ControlPilotEvent::Error_RCD:
        currentState = EvseState::Error;
        errorState = ErrorState::Error_RCD;
        break;
    case ControlPilotEvent::Error_VentilationNotAvailable:
        currentState = EvseState::Error;
        errorState = ErrorState::Error_VentilationNotAvailable;
        break;
    case ControlPilotEvent::Error_OverCurrent:
        currentState = EvseState::Error;
        errorState = ErrorState::Error_OverCurrent;
        break;
    default:
        break;
    }
}

void Charger::update_pwm_max_every_5seconds(float dc) {
    if (dc != update_pwm_last_dc) {
        auto now = date::utc_clock::now();
        auto timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastPwmUpdate).count();
        if (timeSinceLastUpdate >= 5000) {
            update_pwm_now(dc);
        }
    }
}

void Charger::update_pwm_now(float dc) {

    session_log.evse(false, fmt::format("Set PWM On ({}%)", dc * 100.));

    update_pwm_last_dc = dc;
    r_bsp->call_pwm_on(dc);
    lastPwmUpdate = date::utc_clock::now();
}

void Charger::pwm_off() {
    session_log.evse(false, "Set PWM Off");
    r_bsp->call_pwm_off();
}

void Charger::pwm_F() {
    session_log.evse(false, "Set PWM F");
    r_bsp->call_pwm_F();
}

void Charger::run() {
    // spawn new thread and return
    mainThreadHandle = std::thread(&Charger::mainThread, this);
}

float Charger::ampereToDutyCycle(float ampere) {
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

bool Charger::setMaxCurrent(float c, std::chrono::time_point<date::utc_clock> validUntil) {
    if (c >= 0.0 && c <= CHARGER_ABSOLUTE_MAX_CURRENT) {
        std::lock_guard<std::recursive_mutex> lock(configMutex);
        // FIXME: limit to cable limit (PP reading) if that is smaller!
        // is it still valid?
        // FIXME this requires local clock to be UTC
        if (validUntil > date::utc_clock::now()) {
            maxCurrent = c;
            maxCurrentValidUntil = validUntil;
            signalMaxCurrent(c);
            return true;
        }
    }
    return false;
}

bool Charger::setMaxCurrent(float c) {
    if (c >= 0.0 && c <= 80) {
        std::lock_guard<std::recursive_mutex> lock(configMutex);
        // FIXME: limit to cable limit (PP reading) if that is smaller!
        // is it still valid?
        // FIXME this requires local clock to be UTC
        if (maxCurrentValidUntil > date::utc_clock::now()) {
            maxCurrent = c;
            signalMaxCurrent(c);
            return true;
        }
    }
    return false;
}

// pause if currently charging, else do nothing.
bool Charger::pauseCharging() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    if (currentState == EvseState::Charging) {
        paused_by_user = true;
        currentState = EvseState::ChargingPausedEVSE;
        return true;
    }
    return false;
}

bool Charger::resumeCharging() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    if (!cancelled && currentState == EvseState::ChargingPausedEVSE && powerAvailable()) {
        currentState = EvseState::Charging;
        return true;
    }
    return false;
}

// pause charging since no power is available at the moment
bool Charger::pauseChargingWaitForPower() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    if (currentState == EvseState::Charging) {
        paused_by_user = false;
        currentState = EvseState::ChargingPausedEVSE;
        return true;
    }
    return false;
}

// resume charging since power became available. Does not resume if user paused charging.
bool Charger::resumeChargingPowerAvailable() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    if (!cancelled && currentState == EvseState::ChargingPausedEVSE && powerAvailable() && !paused_by_user) {
        currentState = EvseState::Charging;
        return true;
    }
    return false;
}

// pause charging since we run through replug sequence
bool Charger::evseReplug() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    // call BSP to start the replug sequence. It BSP actually does it,
    // it will emit a EvseReplugStarted event which will then modify our state.
    // If BSP never executes the replug, we also never change state and nothing happens.
    // After replugging finishes, BSP will emit EvseReplugFinished event and we will go back to WaitingForAuth
    EVLOG_info << fmt::format("Calling evse_replug({})...", t_replug_ms);
    r_bsp->call_evse_replug(t_replug_ms);
    return true;
}

bool Charger::cancelCharging() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    if (currentState == EvseState::Charging || currentState == EvseState::ChargingPausedEVSE ||
        currentState == EvseState::ChargingPausedEV) {
        currentState = EvseState::ChargingPausedEVSE;
        cancelled = true;
        signalEvent(EvseEvent::SessionCancelled);
        return true;
    }
    return false;
}

bool Charger::switchThreePhasesWhileCharging(bool n) {
    r_bsp->call_switch_three_phases_while_charging(n);
    return false; // FIXME: implement real return value when protobuf has sync calls
}

void Charger::setup(bool three_phases, bool has_ventilation, const std::string& country_code, bool rcd_enabled,
                    const std::string& _charge_mode, bool _ac_hlc_enabled, bool _ac_hlc_use_5percent,
                    bool _ac_enforce_hlc, bool _ac_with_soc_timeout) {
    std::lock_guard<std::recursive_mutex> lock(configMutex);
    // set up board support package
    r_bsp->call_setup(three_phases, has_ventilation, country_code, rcd_enabled);
    // cache our config variables
    charge_mode = _charge_mode;
    ac_hlc_enabled_current_session = ac_hlc_enabled = _ac_hlc_enabled;
    ac_hlc_use_5percent = _ac_hlc_use_5percent;
    ac_enforce_hlc = _ac_enforce_hlc;
    ac_with_soc_timeout = _ac_with_soc_timeout;
    ac_with_soc_timer = 3600000;
}

Charger::EvseState Charger::getCurrentState() {
    std::lock_guard<std::recursive_mutex> lock(configMutex);
    return currentState;
}

bool Charger::Authorized_PnC() {
    std::lock_guard<std::recursive_mutex> lock(configMutex);
    return (authorized && authorized_pnc);
}

bool Charger::Authorized_EIM() {
    std::lock_guard<std::recursive_mutex> lock(configMutex);
    return (authorized && !authorized_pnc);
}

bool Charger::Authorized_PnC_ready_for_HLC() {
    bool auth = false, ready = false;
    {
        std::lock_guard<std::recursive_mutex> lock(configMutex);
        auth = (authorized && authorized_pnc);
    }
    {
        std::lock_guard<std::recursive_mutex> lock(stateMutex);
        ready = (currentState == EvseState::ChargingPausedEV) || (currentState == EvseState::ChargingPausedEVSE) ||
                (currentState == EvseState::Charging);
    }
    return (auth && ready);
}

bool Charger::Authorized_EIM_ready_for_HLC() {
    bool auth = false, ready = false;
    {
        std::lock_guard<std::recursive_mutex> lock(configMutex);
        auth = (authorized && !authorized_pnc);
    }
    {
        std::lock_guard<std::recursive_mutex> lock(stateMutex);
        ready = (currentState == EvseState::ChargingPausedEV) || (currentState == EvseState::ChargingPausedEVSE) ||
                (currentState == EvseState::Charging);
    }
    return (auth && ready);
}

void Charger::Authorize(bool a, const std::string& userid, bool pnc) {
    std::lock_guard<std::recursive_mutex> lock(configMutex);
    authorized = a;
    authorized_pnc = pnc;

    // FIXME: do sth useful with userid
#if 0
if (a) {
    // copy to local user id
} else {
    // clear local userid
}
#endif
}

bool Charger::AuthorizedEIM() {
    std::lock_guard<std::recursive_mutex> lock(configMutex);
    return (authorized && !authorized_pnc);
}

bool Charger::AuthorizedPnC() {
    std::lock_guard<std::recursive_mutex> lock(configMutex);
    return (authorized && authorized_pnc);
}

Charger::ErrorState Charger::getErrorState() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    return errorState;
}

// bool Charger::isPowerOn() { return control_pilot.isPowerOn(); }

bool Charger::disable() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    currentState = EvseState::Disabled;
    signalEvent(EvseEvent::Disabled);
    return true;
}

bool Charger::enable() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    if (currentState == EvseState::Disabled) {
        currentState = EvseState::Idle;
        signalEvent(EvseEvent::Enabled);
        return true;
    }
    return false;
}

bool Charger::set_faulted() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    currentState = EvseState::Faulted;
    signalEvent(EvseEvent::PermanentFault);
    return true;
}

bool Charger::restart() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);

    if (currentState == EvseState::Finished) {
        currentState = EvseState::Idle;
        return true;
    }
    return false;
}

std::string Charger::evseStateToString(EvseState s) {
    switch (s) {
    case EvseState::Disabled:
        return ("Disabled");
        break;
    case EvseState::Idle:
        return ("Idle");
        break;
    case EvseState::WaitingForAuthentication:
        return ("WaitAuth");
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
    case EvseState::Finished:
        return ("Finished");
        break;
    case EvseState::Error:
        return ("Error");
        break;
    case EvseState::Faulted:
        return ("Faulted");
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
    }
    return "Invalid";
}

std::string Charger::evseEventToString(EvseEvent e) {
    switch (e) {
    case EvseEvent::Enabled:
        return ("Enabled");
        break;
    case EvseEvent::Disabled:
        return ("Disabled");
        break;
    case EvseEvent::SessionStarted:
        return ("SessionStarted");
        break;
    case EvseEvent::AuthRequired:
        return ("AuthRequired");
        break;
    case EvseEvent::ChargingStarted:
        return ("ChargingStarted");
        break;
    case EvseEvent::ChargingPausedEV:
        return ("ChargingPausedEV");
        break;
    case EvseEvent::ChargingPausedEVSE:
        return ("ChargingPausedEVSE");
        break;
    case EvseEvent::ChargingResumed:
        return ("ChargingResumed");
        break;
    case EvseEvent::SessionFinished:
        return ("SessionFinished");
        break;
    case EvseEvent::SessionCancelled:
        return ("SessionCancelled");
        break;
    case EvseEvent::Error:
        return ("Error");
        break;
    case EvseEvent::PermanentFault:
        return ("PermanentFault");
        break;
    case EvseEvent::ReplugStarted:
        return ("ReplugStarted");
        break;
    case EvseEvent::ReplugFinished:
        return ("ReplugFinished");
        break;
    }
    return "Invalid";
}

std::string Charger::errorStateToString(ErrorState s) {
    switch (s) {
    case ErrorState::Error_E:
        return ("Car");
        break;
    case ErrorState::Error_OverCurrent:
        return ("OverCurrent");
        break;
    case ErrorState::Error_DF:
        return ("CarDiodeFault");
        break;
    case ErrorState::Error_Relais:
        return ("Relais");
        break;
    case ErrorState::Error_VentilationNotAvailable:
        return ("VentilationNotAvailable");
        break;
    case ErrorState::Error_RCD:
        return ("RCD");
        break;
    case ErrorState::Error_Internal:
        return ("Internal");
        break;
    case ErrorState::Error_SLAC:
        return ("SLAC");
        break;
    case ErrorState::Error_HLC:
        return ("HLC");
        break;
    }
    return "Invalid";
}

/*
FIXME
float Charger::getResidualCurrent() {
    return control_pilot.getResidualCurrent();
}*/

float Charger::getMaxCurrent() {
    std::lock_guard<std::recursive_mutex> lock(configMutex);
    return maxCurrent;
}

bool Charger::forceUnlock() {
    return r_bsp->call_force_unlock();
}

void Charger::setCurrentDrawnByVehicle(float l1, float l2, float l3) {
    std::lock_guard<std::recursive_mutex> lock(configMutex);
    currentDrawnByVehicle[0] = l1;
    currentDrawnByVehicle[1] = l2;
    currentDrawnByVehicle[2] = l3;
}

void Charger::checkSoftOverCurrent() {
    // Allow 10% tolerance
    float limit = getMaxCurrent() * 1.1;

    if (currentDrawnByVehicle[0] > limit || currentDrawnByVehicle[1] > limit || currentDrawnByVehicle[2] > limit) {
        if (!overCurrent) {
            overCurrent = true;
            // timestamp when over current happend first
            lastOverCurrentEvent = date::utc_clock::now();
        }
    } else {
        overCurrent = false;
    }
    auto now = date::utc_clock::now();
    auto timeSinceOverCurrentStarted =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - lastOverCurrentEvent).count();
    if (overCurrent && timeSinceOverCurrentStarted >= softOverCurrentTimeout) {
        currentState = EvseState::Error;
        errorState = ErrorState::Error_OverCurrent;
    }
}

// returns whether power is actually available from EnergyManager
// i.e. maxCurrent is in valid range
bool Charger::powerAvailable() {
    if (maxCurrentValidUntil < date::utc_clock::now()) {
        maxCurrent = 0.;
        signalMaxCurrent(maxCurrent);
    }
    return (getMaxCurrent() > 5.9);
}

void Charger::requestErrorSequence() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    if (currentState == EvseState::WaitingForAuthentication) {
        t_step_EF_returnState = EvseState::WaitingForAuthentication;
        currentState = EvseState::T_step_EF;
        if (ac_hlc_enabled_current_session && hlc_use_5percent_current_session){
            t_step_EF_returnPWM = PWM_5_PERCENT;
        } else {
            t_step_EF_returnPWM = 0.;
        }
    }
}

void Charger::setMatchingStarted(bool m) {
    std::lock_guard<std::recursive_mutex> lock(configMutex);
    matching_started = m;
}

bool Charger::getMatchingStarted() {
    std::lock_guard<std::recursive_mutex> lock(configMutex);
    return matching_started;
}

} // namespace module
