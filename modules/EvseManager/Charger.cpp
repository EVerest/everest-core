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
#include <chrono>

namespace module {
Charger::Charger(const std::unique_ptr<board_support_ACIntf>& r_bsp) : r_bsp(r_bsp) {
    maxCurrent = 16.0; // FIXME make configurable
    authorized = false;
    r_bsp->call_enable(false);

    update_pwm_last_dc = 0.;
    cancelled = false;

    currentState = EvseState::Disabled;
    lastState = EvseState::Disabled;

    currentDrawnByVehicle[0] = 0.;
    currentDrawnByVehicle[1] = 0.;
    currentDrawnByVehicle[2] = 0.;
}

Charger::~Charger() { r_bsp->call_pwm_F(); }


void Charger::mainThread() {

    int cnt = 0;

    // Enable CP output
    r_bsp->call_enable(true);

    // publish initial values
    signalMaxCurrent(getMaxCurrent());
    signalState(currentState);
    signalError(errorState);


    while (true) {

        if (mainThreadHandle.shouldExit()) break;

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        stateMutex.lock();

        // Run our own state machine update (i.e. run everything that needs
        // to be done on regular intervals independent from events)
        runStateMachine();

        stateMutex.unlock();
    }
}

void Charger::runStateMachine() {
    bool new_in_state = lastState!=currentState;
    lastState = currentState;
    if (new_in_state) {
        signalState(currentState);
        if (currentState==EvseState::Error) signalError(errorState);
    }

    switch (currentState) {
    case EvseState::Disabled:
        if (new_in_state) r_bsp->call_pwm_F();
        break;

    case EvseState::Idle:
        // make sure we signal availability to potential new cars
        if (new_in_state) {
            r_bsp->call_pwm_off();
            Authorize(false, "");
            cancelled = false;
        }
        // reset 5 percent flag for this session
        /*if (dc_mode)
            use_5percent = true;
        else if (ac_start_with_5percent)
            use_5percent = ac_start_with_5percent;
        else
            use_5percent = false;*/
        break;

    case EvseState::WaitingForAuthentication:

        // Explicitly do not allow to be powered on. This is important
        // to make sure control_pilot does not switch on relais even if
        // we start PWM here
        if (new_in_state) r_bsp->call_allow_power_on(false);

        // retry on Low level etc until SLAC matched.
        ISO_IEC_Coordination();

        // we get Auth (maybe before SLAC matching or during matching)
        if (getAuthorization()) {
            currentState = EvseState::ChargingPausedEV;
        }

        break;

    case EvseState::Charging:
        /*if (use_5percent)
            control_pilot.pwmOn(0.05);
        else*/

        checkSoftOverCurrent();
        
        if (new_in_state) {
            r_bsp->call_allow_power_on(true);
            // make sure we are enabling PWM
            update_pwm_now(ampereToDutyCycle(getMaxCurrent()));
        } else {
            // update PWM if it has changed and 5 seconds have passed since last update
            update_pwm_max_every_5seconds(ampereToDutyCycle(getMaxCurrent()));
        }
        break;

    case EvseState::ChargingPausedEV:
        /*if (use_5percent)
            control_pilot.pwmOn(0.05);
        else*/

        checkSoftOverCurrent();

        if (new_in_state) {
            r_bsp->call_allow_power_on(true);
            // make sure we are enabling PWM
            r_bsp->call_pwm_on(ampereToDutyCycle(getMaxCurrent()));
        } else {
            // update PWM if it has changed and 5 seconds have passed since last update
            update_pwm_max_every_5seconds(ampereToDutyCycle(getMaxCurrent()));
        }
        break;

    case EvseState::ChargingPausedEVSE:
        if (new_in_state) r_bsp->call_pwm_off();
        break;

    case EvseState::Finished:
        // We are temporarily unavailable to avoid that new cars plug in
        // during the cleanup phase. Re-Enable once we are in idle.
        if (new_in_state) r_bsp->call_pwm_F();
        currentState = EvseState::Idle;
        break;

    case EvseState::Error:
        if (new_in_state) r_bsp->call_pwm_off();
        // Do not set F here, as F cannot detect unplugging of car!
        break;

    case EvseState::Faulted:
        if (new_in_state) r_bsp->call_pwm_F();
        break;
    }
}

ControlPilotEvent Charger::string_to_control_pilot_event(std::string event) {
    if (event == "CarPluggedIn") {
        return ControlPilotEvent::CarPluggedIn;
    } else if (event == "CarRequestedPower") {
        return ControlPilotEvent::CarRequestedPower;
    } else if (event == "PowerOn") {
        return ControlPilotEvent::PowerOn;
    } else if (event == "PowerOff") {
        return ControlPilotEvent::PowerOff;
    } else if (event == "CarRequestedStopPower") {
        return ControlPilotEvent::CarRequestedStopPower;
    } else if (event == "CarUnplugged") {
        return ControlPilotEvent::CarUnplugged;
    } else if (event == "ErrorE") {
        return ControlPilotEvent::Error_E;
    } else if (event == "ErrorDF") {
        return ControlPilotEvent::Error_DF;
    } else if (event == "ErrorRelais") {
        return ControlPilotEvent::Error_Relais;
    } else if (event == "ErrorRCD") {
        return ControlPilotEvent::Error_RCD;
    } else if (event == "ErrorVentilationNotAvailable") {
        return ControlPilotEvent::Error_VentilationNotAvailable;
    } else if (event == "ErrorOverCurrent") {
        return ControlPilotEvent::Error_OverCurrent;
    } else if (event == "RestartMatching") {
        return ControlPilotEvent::RestartMatching;
    } else if (event == "PermanentFault") {
        return ControlPilotEvent::PermanentFault;
    }
    
    return ControlPilotEvent::Invalid; 
}

void Charger::processEvent(std::string event) {
    auto cp_event = string_to_control_pilot_event(event);

    std::lock_guard<std::mutex> lock(stateMutex);
    // Process all event actions that are independent of the current state
    processCPEventsIndependent(cp_event);

    // Process all events that depend on the current state
    processCPEventsState(cp_event);
}

void Charger::processCPEventsState(ControlPilotEvent cp_event) {
    switch (currentState) {

    case EvseState::Idle:
        if (cp_event == ControlPilotEvent::CarPluggedIn) {
            // FIXME: Enable SLAC sounding here, as sounding is forbidden before
            // CP state B
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
    case ControlPilotEvent::CarUnplugged:
        // FIXME: Stop SLAC sounding here
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
        auto now = std::chrono::system_clock::now();
        auto timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(now-lastPwmUpdate).count();
        if (timeSinceLastUpdate>=5000)
            update_pwm_now(dc);
    }
}

void Charger::update_pwm_now(float dc) {
    update_pwm_last_dc = dc; 
    r_bsp->call_pwm_on(dc);
    lastPwmUpdate = std::chrono::system_clock::now();
}

void Charger::ISO_IEC_Coordination() {
    /*
     *
     * ISO15118 / IEC61851 coordination
     * --------------------------------
     *
     * Hi->Lo settings:
     * ----------------
     * dc_mode: true (DC), false (AC)
     * ac_start_with_5percent: true (start with 5% in AC), false
     * (use only nominal PWM). Note that the internal flag
     * use_5percent can be disabled by Yeti e.g. on failover.
     *
     * ac_enforce_hlc_with_eim: true (non-std; don't shortcut to
     * nominal if auth arrives before matching), false: conform
     * to norm
     *
     * Hi provides the following information to Lo:
     * --------------------------------------------
     * slac state: disabled, unmatched, matching, matched
     *
     * Lo can trigger the following commands on Hi:
     * --------------------------------------------
     * slac_enable(true/false): enable/disable SLAC to start
     * matching sessions slac_dlink_terminate: if e.g. car
     * unplugs or error E is detected from car Yeti can
     * terminate the Link.
     *
     * Disabled: F
     * Idle: A1
     * WaitingForAuthAndOrMatching/Setup: B1,B2,E,F
     * Charging: C2,D2,E,F
     * PausedByEV: B1,E,F
     * PausedByEVSE: B1,C1,D1,E,F
     * Finished: A1
     * Faulted: F
     * Error: F, E
     *
     * 1) ABCDEF state (PWM output, relais an aus etc)
     * 2) Logical states / coordination (Disabled ... Charging
     * etc) 3) SLAC 4) V2G
     *
     * ABCDE <> coordiator <> GreenPHY <> V2G
     *
     *
     * terminate: fallback nominal BX1B?
     *
     * Hi can trigger the following commands on Lo:
     * --------------------------------------------
     * slac_dlink_error: ISO15118-3 Table 6 says "restart
     * matching by CP transition through E". They probably meant
     * a BFB sequence, see [V2G3-M07-04]. Initialize a BFB to
     * WaitingForAuth. IEC requires auth to be kept, so we keep
     * the authorized flag.
     *
     *                   if (use_5percent == true):
     *                   SLAC sends this command to LO _after_
     * it left the logical network [V2G3-M07-07]
     *
     *                   -> BFB to WaitingForAuth (as this
     * triggers retries etc). Note that on AC this leads to
     * immediate progression to BC as auth is still valid and
     * auth comes before matching started.
     *
     *                   if (use_5percent == false):
     *                   we choose to ignore it here as leaving
     * the network is handled in SLAC stack [V2G3-M07-12]
     *
     *                   Note: don't use dlink_error on AC with
     * 5% when e.g. tcp connection dies. This will lead to
     * endless retries and no fallback to nominal.
     *
     * (not needed here) slac_dlink_pause: pause is sent to both
     * ISO and IEC stack, so no communication is needed during
     * entering pause mode. Unclear how to exit pause mode (i.e.
     * wake-up sequencing)
     *
     *
     *
     * dc_mode == false && ac_use_5percent == false: wait for
     * auth and just continue. ignore matching/matched events.
     * This mode is typically BC, but matching can occur at any
     * time during the session (initiated by car).
     *
     * dc_mode == false && ac_use_5percent == true: wait for
     * auth, timeout on matched (3 times). if matching
     * timeouted, trigger BFB. BFB should end up in auth again!
     * if all retries exceeded, set ac_use_5percent = false &&
     * BFB to WaitingForAuth If auth is received &&
     * !ac_enforce_hlc_with_eim, check if matching was started
     * -> go through BFB sequence to restart in nominal PWM mode
     * BFB goes to B1 when finished. set ac_use_5percent to
     * false. matched was already done -> go through  B2 X1
     * B2(nominal) to restart in nominal mode BX1B goes to B1
     * when finished. set ac_use_5percent to false. neither ->
     * keep 5% on, continue to B1 If 3 retries on trying to
     * match, restart with B2 X1 B2(nominal)
     *
     * dc_mode: use_5percent always true. wait for auth, keep 5%
     * on, continue to B1. No retries for DC.
     *
     * todo: execute DLINK commands etc
     * */
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

bool Charger::setMaxCurrent(float c) {
    if (c > 5.9 && c <= 80) {
        std::lock_guard<std::mutex> lock(configMutex);
        // FIXME: limit to cable limit (PP reading) if that is smaller!
        maxCurrent = c;
        signalMaxCurrent(c);
        return true;
    }
    return false;
}


// pause if currently charging, else do nothing.
bool Charger::pauseCharging() {
    std::lock_guard<std::mutex> lock(stateMutex);
    if (currentState == EvseState::Charging) {
        currentState = EvseState::ChargingPausedEVSE;
        return true;
    }
    return false;
}

bool Charger::resumeCharging() {
    std::lock_guard<std::mutex> lock(stateMutex);
    if (!cancelled && currentState == EvseState::ChargingPausedEVSE) {
        currentState = EvseState::Charging;
        return true;
    }
    return false;
}

bool Charger::cancelCharging() {
    std::lock_guard<std::mutex> lock(stateMutex);
    if (currentState == EvseState::Charging 
        || currentState == EvseState::ChargingPausedEVSE 
        || currentState == EvseState::ChargingPausedEV) {
        currentState = EvseState::ChargingPausedEVSE;
        cancelled = true;
        return true;
    }
    return false;
}

bool Charger::switchThreePhasesWhileCharging(bool n) {
    r_bsp->call_switch_three_phases_while_charging(n);
    return false; // FIXME: implement real return value when protobuf has sync calls
}

void Charger::setup(bool three_phases, bool has_ventilation, std::string country_code, bool rcd_enabled) {
    std::lock_guard<std::mutex> lock(configMutex);
    r_bsp->call_setup(three_phases, has_ventilation, country_code, rcd_enabled);
}

Charger::EvseState Charger::getCurrentState() {
    std::lock_guard<std::mutex> lock(configMutex);
    return currentState;
}

void Charger::Authorize(bool a, const char *userid) {
    std::lock_guard<std::mutex> lock(configMutex);
    authorized = a;
    // FIXME: do sth useful with userid
#if 0
if (a) {
    // copy to local user id
} else {
    // clear local userid
}
#endif
}

bool Charger::getAuthorization() {
    std::lock_guard<std::mutex> lock(configMutex);
    //return authorized;
    return true; // FIXME: do not always return true here...
}

Charger::ErrorState Charger::getErrorState() {
    std::lock_guard<std::mutex> lock(stateMutex);
    return errorState;
}

//bool Charger::isPowerOn() { return control_pilot.isPowerOn(); }


bool Charger::disable() {
    std::lock_guard<std::mutex> lock(stateMutex);
    currentState = EvseState::Disabled;
    return true;
}

bool Charger::enable() {
    std::lock_guard<std::mutex> lock(stateMutex);
    if (currentState == EvseState::Disabled) {
        currentState = EvseState::Idle;
        return true;
    }
    return false;
}

bool Charger::restart() {
    std::lock_guard<std::mutex> lock(stateMutex);

    if (currentState == EvseState::Finished) {
        currentState = EvseState::Idle;
        return true;
    }
    return false;
}

std::string Charger::evseStateToString(EvseState s) {
    switch (s) {
    case EvseState::Disabled:
        return("Disabled");
        break;
    case EvseState::Idle:
        return("Idle");
        break;
    case EvseState::WaitingForAuthentication:
        return("WaitAuth");
        break;
    case EvseState::Charging:
        return("Charging");
        break;
    case EvseState::ChargingPausedEV:
        return("Car Paused");
        break;
    case EvseState::ChargingPausedEVSE:
        return("EVSE Paused");
        break;
    case EvseState::Finished:
        return("Finished");
        break;
    case EvseState::Error:
        return("Error");
        break;
    case EvseState::Faulted:
        return("Faulted");
        break;
    }
    return "Invalid";
}

std::string Charger::errorStateToString(ErrorState s) {
    switch (s) {
    case ErrorState::Error_E:
        return("Error E");
        break;
    case ErrorState::Error_F:
        return("Error F");
        break;
    case ErrorState::Error_DF:
        return("Error DF");
        break;
    case ErrorState::Error_Relais:
        return("Error Relais");
        break;
    case ErrorState::Error_VentilationNotAvailable:
        return("Error Vent n/a");
        break;
    case ErrorState::Error_RCD:
        return("Error RCD");
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
    std::lock_guard<std::mutex> lock(configMutex);
    return maxCurrent;
}

void Charger::setCurrentDrawnByVehicle(float l1, float l2, float l3) {
    std::lock_guard<std::mutex> lock(configMutex);
    currentDrawnByVehicle[0] = l1;
    currentDrawnByVehicle[1] = l2;
    currentDrawnByVehicle[2] = l3;
}

void Charger::checkSoftOverCurrent() {
    // Allow 10% tolerance
    float limit = getMaxCurrent() * 1.1;

    if (currentDrawnByVehicle[0] > limit ||
        currentDrawnByVehicle[1] > limit ||
        currentDrawnByVehicle[2] > limit) {
        if (!overCurrent) {
            overCurrent = true;
            // timestamp when over current happend first
            lastOverCurrentEvent = std::chrono::system_clock::now();
        }
    } else overCurrent = false;

    auto now = std::chrono::system_clock::now();
    auto timeSinceOverCurrentStarted = std::chrono::duration_cast<std::chrono::milliseconds>(now-lastOverCurrentEvent).count();
    if (overCurrent && timeSinceOverCurrentStarted>=softOverCurrentTimeout) {
        currentState = EvseState::Error;
        errorState = ErrorState::Error_OverCurrent;
    }
}

}
