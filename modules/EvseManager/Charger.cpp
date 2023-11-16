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

#include <math.h>
#include <string.h>

#include <fmt/core.h>

namespace module {

Charger::Charger(const std::unique_ptr<board_support_ACIntf>& r_bsp, const std::string& connector_type) :
    r_bsp(r_bsp), connector_type(connector_type) {

    connectorEnabled = true;

    maxCurrent = 6.0;
    maxCurrentCable = r_bsp->call_read_pp_ampacity();
    authorized = false;
    r_bsp->call_enable(false);

    update_pwm_last_dc = 0.;

    currentState = EvseState::Disabled;
    last_state = EvseState::Disabled;

    currentDrawnByVehicle[0] = 0.;
    currentDrawnByVehicle[1] = 0.;
    currentDrawnByVehicle[2] = 0.;

    t_step_EF_returnState = EvseState::Faulted;
    t_step_X1_returnState = EvseState::Faulted;

    matching_started = false;

    transaction_active = false;
    session_active = false;

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

        std::this_thread::sleep_for(MAINLOOP_UPDATE_RATE);

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

    // run over state machine loop until currentState does not change anymore
    do {
        bool new_in_state = last_state_detect_state_change != currentState;
        if (new_in_state) {
            session_log.evse(false,
                             fmt::format("Charger state: {}->{}", evseStateToString(last_state_detect_state_change),
                                         evseStateToString(currentState)));
        }

        last_state = last_state_detect_state_change;
        last_state_detect_state_change = currentState;

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

        auto timeInCurrentState =
            std::chrono::duration_cast<std::chrono::milliseconds>(now - currentStateStarted).count();

        switch (currentState) {
        case EvseState::Disabled:
            if (new_in_state) {
                signalEvent(types::evse_manager::SessionEventEnum::Disabled);
                pwm_F();
            }
            break;

        case EvseState::Replug:
            if (new_in_state) {
                signalEvent(types::evse_manager::SessionEventEnum::ReplugStarted);
                // start timer in case we need to
                if (ac_with_soc_timeout)
                    ac_with_soc_timer = 120000;
            }
            // simply wait here until BSP informs us that replugging was finished
            break;

        case EvseState::Idle:
            // make sure we signal availability to potential new cars
            if (new_in_state) {
                bcb_toggle_reset();
                iec_allow_close_contactor = false;
                hlc_charging_active = false;
                hlc_allow_close_contactor = false;
                maxCurrentCable = 0;
                hlc_charging_terminate_pause = HlcTerminatePause::Unknown;
                pwm_off();
                DeAuthorize();
                transaction_active = false;
            }
            break;

        case EvseState::WaitingForAuthentication:

            // Explicitly do not allow to be powered on. This is important
            // to make sure control_pilot does not switch on relais even if
            // we start PWM here
            if (new_in_state) {
                r_bsp->call_allow_power_on(false);

                if (last_state == EvseState::Replug) {
                    signalEvent(types::evse_manager::SessionEventEnum::ReplugFinished);
                } else {
                    // First user interaction was plug in of car? Start session here.
                    if (!sessionActive())
                        startSession(false);
                    // External signal on MQTT
                    signalEvent(types::evse_manager::SessionEventEnum::AuthRequired);
                }
                hlc_use_5percent_current_session = false;

                // switch on HLC if configured. May be switched off later on after retries for this session only.
                if (charge_mode == ChargeMode::AC) {
                    ac_hlc_enabled_current_session = ac_hlc_enabled;
                    if (ac_hlc_enabled_current_session) {
                        hlc_use_5percent_current_session = ac_hlc_use_5percent;
                    }
                } else if (charge_mode == ChargeMode::DC) {
                    hlc_use_5percent_current_session = true;
                } else {
                    // unsupported charging mode, give up here.
                    currentState = EvseState::Error;
                    errorState = types::evse_manager::ErrorEnum::Internal;
                }

                if (hlc_use_5percent_current_session) {
                    // FIXME: wait for SLAC to be ready. Teslas are really fast with sending the first slac packet after
                    // enabling PWM.
                    std::this_thread::sleep_for(SLEEP_BEFORE_ENABLING_PWM_HLC_MODE);
                    update_pwm_now(PWM_5_PERCENT);
                }
            }

            // Read PP value in case of AC socket
            if (connector_type == IEC62196Type2Socket && maxCurrentCable == 0) {
                maxCurrentCable = r_bsp->call_read_pp_ampacity();
                // retry if the value is not yet available. Some BSPs may take some time to measure the PP.
                if (maxCurrentCable == 0) {
                    break;
                }
            }

            // Wait for Energy Manager to supply some power, otherwise wait here.
            // If we have zero power, some cars will not like the ChargingParameter message.
            if (charge_mode == ChargeMode::DC &&
                !(currentEvseMaxLimits.EVSEMaximumCurrentLimit > 0 && currentEvseMaxLimits.EVSEMaximumPowerLimit > 0)) {
                break;
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

            if (AuthorizedEIM()) {
                session_log.evse(false, "EIM Authorization received");

                // If we are restarting, the transaction may already be active
                if (!transactionActive()) {
                    startTransaction();
                }

                const EvseState targetState(EvseState::PrepareCharging);

                // EIM done and matching process not started -> we need to go through t_step_EF and fall back to nominal
                // PWM. This is a complete waste of 4 precious seconds.
                if (charge_mode == ChargeMode::AC) {
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
                                    fmt::format(
                                        "AC mode, HLC enabled, matching not started yet. Go through t_step_EF and "
                                        "disable 5 percent if it was enabled before: {}",
                                        hlc_use_5percent_current_session));

                                // Figure 3 of ISO15118-3: 5 percent start, PnC and EIM
                                // Figure 4 of ISO15118-3: X1 start, PnC and EIM
                                t_step_EF_returnState = targetState;
                                t_step_EF_returnPWM = 0.;
                                // fall back to nominal PWM after the t_step_EF break. Note that
                                // ac_hlc_enabled_current_session remains untouched as HLC can still start later in
                                // nominal PWM mode
                                hlc_use_5percent_current_session = false;
                                currentState = EvseState::T_step_EF;
                            } else {
                                // SLAC matching was started already when EIM arrived
                                if (hlc_use_5percent_current_session) {
                                    // Figure 5 of ISO15118-3: 5 percent start, PnC and EIM, matching already started
                                    // when EIM was done
                                    session_log.evse(
                                        false, "AC mode, HLC enabled(5percent), matching already started. Go through "
                                               "t_step_X1 and disable 5 percent.");
                                    t_step_X1_returnState = targetState;
                                    t_step_X1_returnPWM = 0.;
                                    hlc_use_5percent_current_session = false;
                                    currentState = EvseState::T_step_X1;
                                } else {
                                    // Figure 6 of ISO15118-3: X1 start, PnC and EIM, matching already started when EIM
                                    // was done. We can go directly to PrepareCharging, as we do not need to switch from
                                    // 5 percent to nominal first
                                    session_log.evse(
                                        false,
                                        "AC mode, HLC enabled(X1), matching already started. We are in X1 so we can "
                                        "go directly to nominal PWM.");
                                    currentState = targetState;
                                }
                            }
                        }

                    } else {
                        // HLC is disabled for this session.
                        // simply proceed to PrepareCharging, as we are fully authorized to start charging whenever car
                        // wants to.
                        session_log.evse(false, "AC mode, HLC disabled. We are in X1 so we can "
                                                "go directly to nominal PWM.");
                        currentState = targetState;
                    }
                } else if (charge_mode == ChargeMode::DC) {
                    // Figure 8 of ISO15118-3: DC with EIM before or after plugin or PnC
                    // simple here as we always stay within 5 percent mode anyway.
                    session_log.evse(false,
                                     "DC mode. We are in 5percent mode so we can continue without further action.");
                    currentState = targetState;
                } else {
                    // unsupported charging mode, give up here.
                    EVLOG_error << "Unsupported charging mode.";
                    currentState = EvseState::Error;
                    errorState = types::evse_manager::ErrorEnum::Internal;
                }
            } else if (AuthorizedPnC()) {

                startTransaction();

                const EvseState targetState(EvseState::PrepareCharging);

                // We got authorization by Plug and Charge
                session_log.evse(false, "PnC Authorization received");
                if (charge_mode == ChargeMode::AC) {
                    // Figures 3,4,5,6 of ISO15118-3: Independent on how we started we can continue with 5 percent
                    // signalling once we got PnC authorization without going through t_step_EF or t_step_X1.

                    session_log.evse(
                        false, "AC mode, HLC enabled, PnC auth received. We will continue with 5percent independent on "
                               "how we started.");
                    hlc_use_5percent_current_session = true;
                    currentState = targetState;

                } else if (charge_mode == ChargeMode::DC) {
                    // Figure 8 of ISO15118-3: DC with EIM before or after plugin or PnC
                    // simple here as we always stay within 5 percent mode anyway.
                    session_log.evse(false,
                                     "DC mode. We are in 5percent mode so we can continue without further action.");
                    currentState = targetState;
                } else {
                    // unsupported charging mode, give up here.
                    EVLOG_error << "Unsupported charging mode.";
                    currentState = EvseState::Error;
                    errorState = types::evse_manager::ErrorEnum::Internal;
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

        case EvseState::PrepareCharging:

            if (new_in_state) {
                signalEvent(types::evse_manager::SessionEventEnum::PrepareCharging);
                bcb_toggle_reset();

                if (hlc_use_5percent_current_session) {
                    update_pwm_now_if_changed(PWM_5_PERCENT);
                }
            }

            if (charge_mode == ChargeMode::DC) {
                if (hlc_allow_close_contactor && iec_allow_close_contactor) {
                    r_bsp->call_allow_power_on(true);
                }
            }

            // make sure we are enabling PWM
            if (!hlc_use_5percent_current_session) {
                update_pwm_now_if_changed(ampereToDutyCycle(getMaxCurrent()));
            }

            if (charge_mode == ChargeMode::AC) {
                // In AC mode BASIC, iec_allow is sufficient.  The same is true for HLC mode when nominal PWM is used as
                // the car can do BASIC and HLC charging any time. In AC HLC with 5 percent mode, we need to wait for
                // both iec_allow and hlc_allow.
                if ((iec_allow_close_contactor && !hlc_use_5percent_current_session) ||
                    (iec_allow_close_contactor && hlc_allow_close_contactor && hlc_use_5percent_current_session)) {

                    signalEvent(types::evse_manager::SessionEventEnum::ChargingStarted);

                    if (powerAvailable()) {
                        currentState = EvseState::Charging;
                    } else {
                        currentState = EvseState::Charging;
                        pauseChargingWaitForPower();
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
            if (new_in_state) {
                hlc_charging_terminate_pause = HlcTerminatePause::Unknown;
            }

            if (charge_mode == ChargeMode::DC) {
                // FIXME: handle DC pause/resume here
                // FIXME: handle DC no power available from Energy management
            } else {
                checkSoftOverCurrent();

                if (!powerAvailable()) {
                    pauseChargingWaitForPower();
                    break;
                }

                if (new_in_state) {
                    if (last_state != EvseState::PrepareCharging) {
                        signalEvent(types::evse_manager::SessionEventEnum::ChargingResumed);
                    }

                    r_bsp->call_allow_power_on(true);
                    // make sure we are enabling PWM
                    if (hlc_use_5percent_current_session)
                        update_pwm_now_if_changed(PWM_5_PERCENT);
                    else
                        update_pwm_now_if_changed(ampereToDutyCycle(getMaxCurrent()));
                } else {
                    // update PWM if it has changed and 5 seconds have passed since last update
                    if (!hlc_use_5percent_current_session)
                        update_pwm_max_every_5seconds(ampereToDutyCycle(getMaxCurrent()));
                }

                // If we are in charging for some time now, but relais did not close something is wrong.
                // Try a T_step_EF to restart charging once (legacy wakeup according to IEC61851-1 A.5.3)
                if (!hlc_charging_active && !legacy_wakeup_done && timeInCurrentState > legacy_wakeup_timeout &&
                    !contactors_closed) {
                    session_log.evse(
                        false, "Relais not closed after timeout, trying legacy wakeup according to IEC61851-1 A.5.3");
                    legacy_wakeup_done = true;
                    t_step_EF_returnState = EvseState::Charging;
                    t_step_EF_returnPWM = ampereToDutyCycle(getMaxCurrent());
                    currentState = EvseState::T_step_EF;
                }
            }
            break;

        case EvseState::ChargingPausedEV:

            if (charge_mode == ChargeMode::AC) {
                checkSoftOverCurrent();
            }

            // A pause issued by the EV needs to be handled differently for the different charging modes

            // 1) BASIC AC charging: Nominal PWM needs be running, so the EV can actually resume charging when it wants
            // to

            // 2) HLC charging: [V2G3-M07-19] requires the EV to switch to state B, so we will end up here in this state
            //    [V2G3-M07-20] forces us to switch off PWM.
            //    This is also true for nominal PWM AC HLC charging, so an EV that does HLC AC and pauses can only
            //    resume in HLC mode and not in BASIC charging.

            if (hlc_charging_active) {
                // This is for HLC charging (both AC and DC)
                if (new_in_state) {
                    bcb_toggle_reset();
                    r_bsp->call_allow_power_on(false);
                    if (charge_mode == ChargeMode::DC) {
                        signal_DC_supply_off();
                    }
                    signalEvent(types::evse_manager::SessionEventEnum::ChargingPausedEV);
                }

                if (bcb_toggle_detected()) {
                    currentState = EvseState::PrepareCharging;
                }

                // We come here by a state C->B transition but the ISO message may not have arrived yet,
                // so we wait here until we know wether it is Terminate or Pause. Until we leave PWM on (should not be
                // shut down before SessionStop.req)

                if (hlc_charging_terminate_pause == HlcTerminatePause::Terminate) {
                    // EV wants to terminate session
                    currentState = EvseState::StoppingCharging;
                    if (pwm_running)
                        pwm_off();
                } else if (hlc_charging_terminate_pause == HlcTerminatePause::Pause) {
                    // EV wants an actual pause
                    if (pwm_running)
                        pwm_off();
                }

            } else {
                // This is for BASIC charging only

                // Normally power should be available, since we request a minimum power also during EV pause.
                // In case the energy manager gives us no energy, we effectivly switch to a pause by EVSE here.
                if (!powerAvailable()) {
                    pauseChargingWaitForPower();
                    break;
                }

                if (new_in_state) {
                    signalEvent(types::evse_manager::SessionEventEnum::ChargingPausedEV);
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
                signalEvent(types::evse_manager::SessionEventEnum::ChargingPausedEVSE);
                if (hlc_charging_active) {
                    // currentState = EvseState::StoppingCharging;
                    last_stop_transaction_reason = types::evse_manager::StopTransactionReason::Local;
                    // tell HLC stack to stop the session
                    signal_hlc_stop_charging();
                    pwm_off();
                } else {
                    pwm_off();
                }
            }
            break;

        case EvseState::WaitingForEnergy:
            if (new_in_state) {
                signalEvent(types::evse_manager::SessionEventEnum::WaitingForEnergy);
                if (!hlc_use_5percent_current_session)
                    pwm_off();
            }
            break;

        case EvseState::StoppingCharging:
            if (new_in_state) {
                bcb_toggle_reset();
                if (transactionActive() || sessionActive()) {
                    signalEvent(types::evse_manager::SessionEventEnum::StoppingCharging);
                }

                if (hlc_charging_active) {
                    if (charge_mode == ChargeMode::DC) {
                        // DC supply off - actually this is after relais switched off
                        // this is a backup switch off, normally it should be switched off earlier by ISO protocol.
                        signal_DC_supply_off();
                    }
                    // Car is maybe not unplugged yet, so for HLC(AC/DC) wait in this state. We will go to Finished once
                    // car is unplugged.
                } else {
                    // For AC BASIC charging, we reached StoppingCharging because an unplug happend.
                    pwm_off();
                    currentState = EvseState::Finished;
                }
            }

            // Allow session restart after SessionStop.terminate (full restart including new SLAC).
            // Only allow that if the transaction is still running. If it was cancelled externally with
            // cancelTransaction(), we do not allow restart. If OCPP cancels a transaction it assumes it cannot be
            // restarted. In all other cases, e.g. the EV stopping the transaction it may resume with a BCB toggle.
            if (hlc_charging_active && bcb_toggle_detected()) {
                if (transaction_active) {
                    currentState = EvseState::PrepareCharging;
                    // wake up SLAC as well
                    signal_SLAC_start();
                } else {
                    session_log.car(false, "Car requested restarting with BCB toggle. Ignored, since we were cancelled "
                                           "externally before.");
                }
            }
            break;

        case EvseState::Finished:

            if (new_in_state) {
                // Transaction may already be stopped when it was cancelled earlier.
                // In that case, do not sent a second transactionFinished event.
                if (transactionActive())
                    stopTransaction();
                // We may come here from an error state, so a session was maybe not active.
                if (sessionActive()) {
                    stopSession();
                }

                if (charge_mode == ChargeMode::DC) {
                    signal_DC_supply_off();
                }
            }

            restart();
            break;

        case EvseState::Error:
            if (new_in_state) {
                signalEvent(types::evse_manager::SessionEventEnum::Error);
                pwm_off();
            }
            // Do not set F here, as F cannot detect unplugging of car!
            break;

        case EvseState::Faulted:
            if (new_in_state) {
                signalEvent(types::evse_manager::SessionEventEnum::PermanentFault);
                pwm_F();
            }
            break;
        }
    } while (last_state_detect_state_change != currentState);
}

void Charger::processEvent(types::board_support::Event cp_event) {

    switch (cp_event) {
    case ControlPilotEvent::CarPluggedIn:
    case ControlPilotEvent::CarRequestedPower:
    case ControlPilotEvent::CarRequestedStopPower:
    case ControlPilotEvent::CarUnplugged:
    case ControlPilotEvent::ErrorDF:
    case ControlPilotEvent::ErrorE:
    case ControlPilotEvent::BCDtoEF:
    case ControlPilotEvent::EFtoBCD:
        session_log.car(false, fmt::format("Event {}", types::board_support::event_to_string(cp_event)));
        break;
    case ControlPilotEvent::ErrorOverCurrent:
    case ControlPilotEvent::ErrorRCD:
    case ControlPilotEvent::ErrorRelais:
    case ControlPilotEvent::ErrorVentilationNotAvailable:
    case ControlPilotEvent::PermanentFault:
    case ControlPilotEvent::PowerOff:
    case ControlPilotEvent::PowerOn:
    case ControlPilotEvent::EvseReplugStarted:
    case ControlPilotEvent::EvseReplugFinished:

    default:
        session_log.evse(false, fmt::format("Event {}", types::board_support::event_to_string(cp_event)));
        break;
    }

    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    if (cp_event == ControlPilotEvent::PowerOn) {
        contactors_closed = true;
    } else if (cp_event == ControlPilotEvent::PowerOff) {
        contactors_closed = false;
    }

    runStateMachine();

    // Process all event actions that are independent of the current state
    processCPEventsIndependent(cp_event);

    runStateMachine();

    // Process all events that depend on the current state
    processCPEventsState(cp_event);

    runStateMachine();
}

void Charger::processCPEventsState(ControlPilotEvent cp_event) {
    switch (currentState) {

    case EvseState::Idle:
        if (cp_event == ControlPilotEvent::CarPluggedIn) {
            currentState = EvseState::WaitingForAuthentication;
        }
        break;

    case EvseState::WaitingForAuthentication:
        if (cp_event == ControlPilotEvent::CarRequestedPower) {
            session_log.car(false, "Ignoring CarRequestedPower in WaitingForAuth state. Probably a BCB toggle that "
                                   "does not make sense here.");
        } else if (cp_event == ControlPilotEvent::CarRequestedStopPower) {
            session_log.car(false, "Ignoring CarRequestedStopPower in WaitingForAuth state. Probably a BCB toggle that "
                                   "does not make sense here.");
        }
        break;

    case EvseState::PrepareCharging:
        if (cp_event == ControlPilotEvent::CarRequestedPower) {
            iec_allow_close_contactor = true;
        } else if (cp_event == ControlPilotEvent::CarRequestedStopPower) {
            iec_allow_close_contactor = false;
            // currentState = EvseState::StoppingCharging;
        }
        break;

    case EvseState::Charging:
        if (cp_event == ControlPilotEvent::CarRequestedStopPower) {
            iec_allow_close_contactor = false;
            currentState = EvseState::ChargingPausedEV;
        }
        break;

    case EvseState::ChargingPausedEV:
        if (cp_event == ControlPilotEvent::CarRequestedPower) {
            iec_allow_close_contactor = true;
            // For BASIC charging we can simply switch back to Charging
            if (charge_mode == ChargeMode::AC && !hlc_charging_active) {
                currentState = EvseState::Charging;
            } else if (!pwm_running) {
                bcb_toggle_detect_start_pulse();
            }
        }

        if (cp_event == ControlPilotEvent::CarRequestedStopPower && !pwm_running && hlc_charging_active) {
            bcb_toggle_detect_stop_pulse();
        }
        break;

    case EvseState::StoppingCharging:
        // Allow session restart from EV after SessionStop.terminate with BCB toggle
        if (hlc_charging_active && !pwm_running) {
            if (cp_event == ControlPilotEvent::CarRequestedPower) {
                bcb_toggle_detect_start_pulse();
            } else if (cp_event == ControlPilotEvent::CarRequestedStopPower) {
                bcb_toggle_detect_stop_pulse();
            }
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
        if (currentState == EvseState::Error)
            signalEvent(types::evse_manager::SessionEventEnum::AllErrorsCleared);
        if (!hlc_charging_active) {
            currentState = EvseState::StoppingCharging;
        } else {
            currentState = EvseState::Finished;
        }
        break;
    case ControlPilotEvent::ErrorE:
        currentState = EvseState::Error;
        errorState = types::evse_manager::ErrorEnum::Car;
        break;
    case ControlPilotEvent::ErrorDF:
        currentState = EvseState::Error;
        errorState = types::evse_manager::ErrorEnum::CarDiodeFault;
        break;
    case ControlPilotEvent::ErrorRelais:
        currentState = EvseState::Error;
        errorState = types::evse_manager::ErrorEnum::Relais;
        break;
    case ControlPilotEvent::ErrorRCD:
        currentState = EvseState::Error;
        errorState = types::evse_manager::ErrorEnum::RCD;
        break;
    case ControlPilotEvent::ErrorVentilationNotAvailable:
        currentState = EvseState::Error;
        errorState = types::evse_manager::ErrorEnum::VentilationNotAvailable;
        break;
    case ControlPilotEvent::ErrorOverCurrent:
        currentState = EvseState::Error;
        errorState = types::evse_manager::ErrorEnum::OverCurrent;
        break;
    case ControlPilotEvent::ErrorRCD_DC:
        currentState = EvseState::Error;
        errorState = types::evse_manager::ErrorEnum::OverCurrent;
        break;
    case ControlPilotEvent::ErrorOverVoltage:
        currentState = EvseState::Error;
        errorState = types::evse_manager::ErrorEnum::OverVoltage;
        break;
    case ControlPilotEvent::ErrorUnderVoltage:
        currentState = EvseState::Error;
        errorState = types::evse_manager::ErrorEnum::UnderVoltage;
        break;
    case ControlPilotEvent::ErrorMotorLock:
        currentState = EvseState::Error;
        errorState = types::evse_manager::ErrorEnum::MotorLock;
        break;
    case ControlPilotEvent::ErrorOverTemperature:
        currentState = EvseState::Error;
        errorState = types::evse_manager::ErrorEnum::OverTemperature;
        break;
    case ControlPilotEvent::ErrorBrownOut:
        currentState = EvseState::Error;
        errorState = types::evse_manager::ErrorEnum::BrownOut;
        break;
    case ControlPilotEvent::ErrorCablePP:
        currentState = EvseState::Error;
        errorState = types::evse_manager::ErrorEnum::CablePP;
        break;
    case ControlPilotEvent::ErrorEnergyManagement:
        currentState = EvseState::Error;
        errorState = types::evse_manager::ErrorEnum::EnergyManagement;
        break;
    case ControlPilotEvent::ErrorNeutralPEN:
        currentState = EvseState::Error;
        errorState = types::evse_manager::ErrorEnum::NeutralPEN;
        break;
    case ControlPilotEvent::ErrorCpDriver:
        currentState = EvseState::Error;
        errorState = types::evse_manager::ErrorEnum::CpDriver;
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
    auto start = date::utc_clock::now();
    update_pwm_last_dc = dc;
    pwm_running = true;
    r_bsp->call_pwm_on(dc);

    session_log.evse(
        false,
        fmt::format("Set PWM On ({}%) took {} ms", dc * 100.,
                    (std::chrono::duration_cast<std::chrono::milliseconds>(date::utc_clock::now() - start)).count()));
    lastPwmUpdate = date::utc_clock::now();
}

void Charger::update_pwm_now_if_changed(float dc) {
    if (update_pwm_last_dc != dc) {
        update_pwm_now(dc);
    }
}

void Charger::pwm_off() {
    session_log.evse(false, "Set PWM Off");
    pwm_running = false;
    update_pwm_last_dc = 1.;
    r_bsp->call_pwm_off();
}

void Charger::pwm_F() {
    session_log.evse(false, "Set PWM F");
    pwm_running = false;
    update_pwm_last_dc = 0.;
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
        // is it still valid?
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
    if (c >= 0.0 && c <= CHARGER_ABSOLUTE_MAX_CURRENT) {
        std::lock_guard<std::recursive_mutex> lock(configMutex);
        // is it still valid?
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
        legacy_wakeup_done = false;
        currentState = EvseState::ChargingPausedEVSE;
        return true;
    }
    return false;
}

bool Charger::resumeCharging() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);

    if (hlc_charging_active && transaction_active && currentState == EvseState::ChargingPausedEVSE) {
        currentState = EvseState::PrepareCharging;
        // wake up SLAC as well
        signal_SLAC_start();
        return true;
    } else if (transactionActive() && currentState == EvseState::ChargingPausedEVSE) {
        currentState = EvseState::WaitingForEnergy;
        return true;
    }

    return false;
}

// pause charging since no power is available at the moment
bool Charger::pauseChargingWaitForPower() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    if (currentState == EvseState::Charging) {
        currentState = EvseState::WaitingForEnergy;
        return true;
    }
    return false;
}

// resume charging since power became available. Does not resume if user paused charging.
bool Charger::resumeChargingPowerAvailable() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);

    if (transactionActive() && currentState == EvseState::WaitingForEnergy && powerAvailable()) {
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

// Cancel transaction/charging from external EvseManager interface (e.g. via OCPP)
bool Charger::cancelTransaction(const types::evse_manager::StopTransactionRequest& request) {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    if (transactionActive()) {
        if (hlc_charging_active) {
            currentState = EvseState::StoppingCharging;
            signal_hlc_stop_charging();
        } else {
            currentState = EvseState::ChargingPausedEVSE;
        }

        transaction_active = false;
        last_stop_transaction_reason = request.reason;
        if (request.id_tag) {
            stop_transaction_id_tag = request.id_tag.value();
        }
        signalEvent(types::evse_manager::SessionEventEnum::ChargingFinished);
        signalEvent(types::evse_manager::SessionEventEnum::TransactionFinished);
        return true;
    }
    return false;
}

bool Charger::transactionActive() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    return transaction_active;
}

bool Charger::sessionActive() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    return session_active;
}

void Charger::startSession(bool authfirst) {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    session_active = true;
    authorized = false;
    if (authfirst)
        last_start_session_reason = types::evse_manager::StartSessionReason::Authorized;
    else
        last_start_session_reason = types::evse_manager::StartSessionReason::EVConnected;
    signalEvent(types::evse_manager::SessionEventEnum::SessionStarted);
}

void Charger::stopSession() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    session_active = false;
    authorized = false;
    signalEvent(types::evse_manager::SessionEventEnum::SessionFinished);
}

void Charger::startTransaction() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    stop_transaction_id_tag.clear();
    transaction_active = true;
    signalEvent(types::evse_manager::SessionEventEnum::TransactionStarted);
}

void Charger::stopTransaction() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    transaction_active = false;
    last_stop_transaction_reason = types::evse_manager::StopTransactionReason::EVDisconnected;
    signalEvent(types::evse_manager::SessionEventEnum::ChargingFinished);
    signalEvent(types::evse_manager::SessionEventEnum::TransactionFinished);
}

std::string Charger::getStopTransactionIdTag() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    return stop_transaction_id_tag;
}

types::evse_manager::StopTransactionReason Charger::getTransactionFinishedReason() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    return last_stop_transaction_reason;
}

types::evse_manager::StartSessionReason Charger::getSessionStartedReason() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    return last_start_session_reason;
}

bool Charger::switchThreePhasesWhileCharging(bool n) {
    r_bsp->call_switch_three_phases_while_charging(n);
    return false; // FIXME: implement real return value when protobuf has sync calls
}

void Charger::setup(bool three_phases, bool has_ventilation, const std::string& country_code, bool rcd_enabled,
                    const ChargeMode _charge_mode, bool _ac_hlc_enabled, bool _ac_hlc_use_5percent,
                    bool _ac_enforce_hlc, bool _ac_with_soc_timeout, float _soft_over_current_tolerance_percent,
                    float _soft_over_current_measurement_noise_A) {
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
    soft_over_current_tolerance_percent = _soft_over_current_tolerance_percent;
    soft_over_current_measurement_noise_A = _soft_over_current_measurement_noise_A;
    if (charge_mode == ChargeMode::AC && ac_hlc_enabled)
        EVLOG_info << "AC HLC mode enabled.";
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
                (currentState == EvseState::Charging) || (currentState == EvseState::WaitingForEnergy);
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
                (currentState == EvseState::Charging) || (currentState == EvseState::WaitingForEnergy);
    }
    return (auth && ready);
}

void Charger::Authorize(bool a, const types::authorization::ProvidedIdToken& token) {
    if (a) {
        // First user interaction was auth? Then start session already here and not at plug in
        if (!sessionActive())
            startSession(true);
        std::lock_guard<std::recursive_mutex> lock(configMutex);
        authorized = true;
        authorized_pnc = token.authorization_type == types::authorization::AuthorizationType::PlugAndCharge;
        id_token = token;
    } else {
        if (sessionActive()) {
            stopSession();
        }
        std::lock_guard<std::recursive_mutex> lock(configMutex);
        authorized = false;
    }
}

types::authorization::ProvidedIdToken Charger::getIdToken() {
    std::lock_guard<std::recursive_mutex> lock(configMutex);
    return id_token;
}

bool Charger::AuthorizedEIM() {
    std::lock_guard<std::recursive_mutex> lock(configMutex);
    return (authorized && !authorized_pnc);
}

bool Charger::AuthorizedPnC() {
    std::lock_guard<std::recursive_mutex> lock(configMutex);
    return (authorized && authorized_pnc);
}

bool Charger::DeAuthorize() {

    if (sessionActive()) {
        auto s = getCurrentState();

        if (s == EvseState::Disabled || s == EvseState::Idle || s == EvseState::WaitingForAuthentication ||
            s == EvseState::Error || s == EvseState::Faulted) {

            // We can safely remove auth as it is not in use right now
            std::lock_guard<std::recursive_mutex> lock(configMutex);
            if (!authorized) {
                signalEvent(types::evse_manager::SessionEventEnum::PluginTimeout);
                return false;
            }
            authorized = false;
            stopSession();
            return true;
        }
    }
    return false;
}

types::evse_manager::ErrorEnum Charger::getErrorState() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    return errorState;
}

bool Charger::disable(int connector_id) {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    if (connector_id != 0) {
        connectorEnabled = false;
    }
    currentState = EvseState::Disabled;
    signalEvent(types::evse_manager::SessionEventEnum::Disabled);
    return true;
}

bool Charger::enable(int connector_id) {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    if (connector_id != 0) {
        connectorEnabled = true;
    }

    signalEvent(types::evse_manager::SessionEventEnum::Enabled);
    if (currentState == EvseState::Disabled) {
        if (connectorEnabled) {
            currentState = EvseState::Idle;
        }
        return true;
    }
    return false;
}

void Charger::set_faulted() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    currentState = EvseState::Faulted;
    signalEvent(types::evse_manager::SessionEventEnum::PermanentFault);
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
    case EvseState::PrepareCharging:
        return ("PrepareCharging");
        break;
    case EvseState::StoppingCharging:
        return ("StoppingCharging");
        break;
    }
    return "Invalid";
}

float Charger::getMaxCurrent() {
    std::lock_guard<std::recursive_mutex> lock(configMutex);
    auto maxc = maxCurrent;

    if (connector_type == IEC62196Type2Socket && maxCurrentCable < maxc && currentState != EvseState::Idle) {
        maxc = maxCurrentCable;
    }

    return maxc;
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
    // Allow some tolerance
    float limit =
        (getMaxCurrent() + soft_over_current_measurement_noise_A) * (1. + soft_over_current_tolerance_percent / 100.);

    if (currentDrawnByVehicle[0] > limit || currentDrawnByVehicle[1] > limit || currentDrawnByVehicle[2] > limit) {
        if (!overCurrent) {
            overCurrent = true;
            // timestamp when over current happend first
            lastOverCurrentEvent = date::utc_clock::now();
            session_log.evse(false,
                             fmt::format("Soft overcurrent event (L1:{}, L2:{}, L3:{}, limit {}), starting timer.",
                                         currentDrawnByVehicle[0], currentDrawnByVehicle[1], currentDrawnByVehicle[2],
                                         limit));
        }
    } else {
        overCurrent = false;
    }
    auto now = date::utc_clock::now();
    auto timeSinceOverCurrentStarted =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - lastOverCurrentEvent).count();
    if (overCurrent && timeSinceOverCurrentStarted >= softOverCurrentTimeout) {
        session_log.evse(false, fmt::format("Soft overcurrent event (L1:{}, L2:{}, L3:{}, limit {}) triggered",
                                            currentDrawnByVehicle[0], currentDrawnByVehicle[1],
                                            currentDrawnByVehicle[2], limit));
        currentState = EvseState::Error;
        errorState = types::evse_manager::ErrorEnum::OverCurrent;
    }
}

// returns whether power is actually available from EnergyManager
// i.e. maxCurrent is in valid range
bool Charger::powerAvailable() {
    if (maxCurrentValidUntil < date::utc_clock::now()) {
        EVLOG_warning << "Power budget expired, falling back to 0.";
        maxCurrent = 0.;
        signalMaxCurrent(maxCurrent);
    }
    return (getMaxCurrent() > 5.9);
}

void Charger::requestErrorSequence() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    if (currentState == EvseState::WaitingForAuthentication || currentState == EvseState::PrepareCharging) {
        t_step_EF_returnState = currentState;
        currentState = EvseState::T_step_EF;
        signal_SLAC_reset();
        if (hlc_use_5percent_current_session) {
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

void Charger::notifyCurrentDemandStarted() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    if (currentState == EvseState::PrepareCharging) {
        signalEvent(types::evse_manager::SessionEventEnum::ChargingStarted);
        currentState = EvseState::Charging;
    }
}

void Charger::inform_new_evse_max_hlc_limits(
    const types::iso15118_charger::DC_EVSEMaximumLimits& _currentEvseMaxLimits) {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    currentEvseMaxLimits = _currentEvseMaxLimits;
}

types::iso15118_charger::DC_EVSEMaximumLimits Charger::get_evse_max_hlc_limits() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    return currentEvseMaxLimits;
}

// HLC stack signalled a pause request for the lower layers.
void Charger::dlink_pause() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    hlc_allow_close_contactor = false;
    pwm_off();
    hlc_charging_terminate_pause = HlcTerminatePause::Pause;
}

// HLC requested end of charging session, so we can stop the 5% PWM
void Charger::dlink_terminate() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    hlc_allow_close_contactor = false;
    pwm_off();
    hlc_charging_terminate_pause = HlcTerminatePause::Terminate;
}

void Charger::dlink_error() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);

    hlc_allow_close_contactor = false;

    // Is PWM on at the moment?
    if (!pwm_running) {
        // [V2G3-M07-04]: With receiving a D-LINK_ERROR.request from HLE in X1 state, the EVSE’s communication node
        // shall perform a state X1 to state E/F to state X1 or X2 transition.
    } else {
        // [V2G3-M07-05]: With receiving a D-LINK_ERROR.request in X2 state from HLE, the EVSE’s communication node
        // shall perform a state X2 to X1 to state E/F to state X1 or X2 transition.

        // Are we in 5% mode or not?
        if (hlc_use_5percent_current_session) {
            // [V2G3-M07-06] Within the control pilot state X1, the communication node shall leave the logical
            // network and change the matching state to "Unmatched". [V2G3-M07-07] With reaching the state
            // “Unmatched”, the EVSE shall switch to state E/F.

            // FIXME: We don't wait for SLAC to go to UNMATCHED in X1 for now but just do a normal 3 seconds
            // t_step_X1 instead. This should be more then sufficient for the SLAC module to reset.

            // Do t_step_X1 with a t_step_EF afterwards
            // [V2G3-M07-08] The state E/F shall be applied at least T_step_EF: This is already handled in the
            // t_step_EF state.
            t_step_X1_returnState = EvseState::T_step_EF;
            t_step_X1_returnPWM = 0.;
            currentState = EvseState::T_step_X1;

            // After returning from T_step_EF, go to Waiting for Auth (We are restarting the session)
            t_step_EF_returnState = EvseState::WaitingForAuthentication;
            // [V2G3-M07-09] After applying state E/F, the EVSE shall switch to contol pilot state X1 or X2 as soon
            // as the EVSE is ready control for pilot incoming duty matching cycle requests: This is already handled
            // in the Auth step.

            // [V2G3-M07-05] says we need to go through X1 at the end of the sequence
            t_step_EF_returnPWM = 0.;
        }
        // else {
        // [V2G3-M07-10] Gives us two options for nominal PWM mode and HLC in case of error: We choose [V2G3-M07-12]
        // (Don't interrupt basic AC charging just because an error in HLC happend)
        // So we don't do anything here, SLAC will be notified anyway to reset
        //}
    }
}

void Charger::set_hlc_charging_active() {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    hlc_charging_active = true;
}

void Charger::set_hlc_allow_close_contactor(bool on) {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    hlc_allow_close_contactor = on;
}

void Charger::set_hlc_error(types::evse_manager::ErrorEnum e) {
    std::lock_guard<std::recursive_mutex> lock(stateMutex);
    currentState = EvseState::Error;
    errorState = e;
}

// this resets the BCB sequence (which may contain 1-3 toggle pulses)
void Charger::bcb_toggle_reset() {
    hlc_ev_pause_bcb_count = 0;
    hlc_bcb_sequence_started = false;
}

// call this B->C transitions
void Charger::bcb_toggle_detect_start_pulse() {
    // For HLC charging, PWM already off: This is probably a BCB Toggle to wake us up from sleep mode.
    // Remember start of BCB toggle.
    hlc_ev_pause_start_of_bcb = std::chrono::steady_clock::now();
    if (hlc_ev_pause_bcb_count == 0) {
        // remember sequence start
        hlc_ev_pause_start_of_bcb_sequence = std::chrono::steady_clock::now();
        hlc_bcb_sequence_started = true;
    }
}

// call this on C->B transitions
void Charger::bcb_toggle_detect_stop_pulse() {
    // This is probably and end of BCB toggle, verify it was not too long or too short
    auto pulse_length = std::chrono::steady_clock::now() - hlc_ev_pause_start_of_bcb;

    if (pulse_length > TP_EV_VALD_STATE_DURATION_MIN && pulse_length < TP_EV_VALD_STATE_DURATION_MAX) {

        // enable PWM again. ISO stack should have been ready for the whole time.
        // FIXME where do we go from here? Auth?
        hlc_ev_pause_bcb_count++;

        session_log.car(false, fmt::format("BCB toggle ({} ms), #{} in sequence",
                                           std::chrono::duration_cast<std::chrono::milliseconds>(pulse_length).count(),
                                           hlc_ev_pause_bcb_count));

    } else {
        hlc_ev_pause_bcb_count = 0;
        EVLOG_warning << "BCB toggle with invalid duration detected: "
                      << std::chrono::duration_cast<std::chrono::milliseconds>(pulse_length).count();
    }
}

// Query if a BCB sequence (of 1-3 pulses) was detected and is finished. If that is true, PWM can be enabled again
// etc
bool Charger::bcb_toggle_detected() {
    auto sequence_length = std::chrono::steady_clock::now() - hlc_ev_pause_start_of_bcb_sequence;
    if (hlc_bcb_sequence_started && (sequence_length > TT_EVSE_VALD_TOGGLE || hlc_ev_pause_bcb_count >= 3)) {
        // no need to wait for further BCB toggles
        hlc_ev_pause_bcb_count = 0;
        return true;
    }
    return false;
}

} // namespace module
