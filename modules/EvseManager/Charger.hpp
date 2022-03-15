// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
/*
 * Charger.h
 *
 *  Created on: 08.03.2021
 *  Author: cornelius
 *
 *  IEC 61851-1 compliant AC/DC high level charging logic
 *
 * This class provides:
 *  1) Hi level state machine that is controlled by a) events from board_support_ac interface
 *     and b) by external commands from higher levels
 *
 * The state machine runs in its own (big) thread. After plugin,
 * The charger waits in state WaitingForAuthentication forever. Send
 * Authenticate()
 * from hi level to start charging. After car is unplugged, it waits in
 * ChargingFinished forever (or in an error state if an error happens during
 * charging). Send restart() from hi level to allow a new charging session.
 */

#ifndef SRC_EVDRIVERS_CHARGER_H_
#define SRC_EVDRIVERS_CHARGER_H_

#include "ld-ev.hpp"
#include "utils/thread.hpp"
#include <generated/board_support_AC/Interface.hpp>
#include <chrono>
#include <mutex>
#include <queue>
#include <sigslot/signal.hpp>

namespace module {

enum class ControlPilotEvent
{
    CarPluggedIn,
    CarRequestedPower,
    PowerOn,
    PowerOff,
    CarRequestedStopPower,
    CarUnplugged,
    Error_E,
    Error_DF,
    Error_Relais,
    Error_RCD,
    Error_VentilationNotAvailable,
    Error_OverCurrent,
    RestartMatching,
    PermanentFault,
    Invalid
};

class Charger {
public:
    Charger(const std::unique_ptr<board_support_ACIntf>& r_bsp);
    ~Charger();

    // Public interface to configure Charger
    //
    // Call anytime also during charging, but call setters in this block at
    // least initially once.
    //

    // external input to charger: update max_current and new validUntil
    bool setMaxCurrent(float ampere, std::chrono::time_point<std::chrono::system_clock> validUntil);
    // update only max_current but keep the current validUntil
    bool setMaxCurrent(float ampere);
    float getMaxCurrent();
    sigslot::signal<float> signalMaxCurrent;

    void setup(bool three_phases, bool has_ventilation, const std::string& country_code, bool rcd_enabled,
               const std::string& charge_mode, bool ac_hlc_enabled, bool ac_hlc_use_5percent, bool ac_enforce_hlc);

    bool enable();
    bool disable();
    // switch to next charging session after Finished
    bool restart();

    // Public interface during charging
    //
    // Call anytime, but mostly used during charging session
    //

    // call when in state WaitingForAuthentication
    void Authorize(bool a, const std::string& userid, bool pnc);
    bool AuthorizedEIM();
    bool AuthorizedPnC();

    // trigger replug sequence while charging to switch number of phases
    bool switchThreePhasesWhileCharging(bool n);

    bool pauseCharging();
    bool pauseChargingWaitForPower();
    bool resumeCharging();
    bool resumeChargingPowerAvailable();
    bool cancelCharging();
    bool getPausedByEVSE();

    void setCurrentDrawnByVehicle(float l1, float l2, float l3);

    bool forceUnlock();

    // float getResidualCurrent();
    // bool isPowerOn();

    // Public states for Hi Level

    enum class EvseEvent
    {
        Enabled,
        Disabled,
        SessionStarted,
        AuthRequired,
        ChargingStarted,
        ChargingPausedEV,
        ChargingPausedEVSE,
        ChargingResumed,
        SessionFinished,
        Error,
        PermanentFault
    };

    enum class ErrorState
    {
        Error_E,
        Error_DF,
        Error_Relais,
        Error_VentilationNotAvailable,
        Error_RCD,
        Error_OverCurrent,
        Error_Internal,
        Error_SLAC,
        Error_HLC
    };

    // Signal for EvseEvents
    sigslot::signal<EvseEvent> signalEvent;
    std::string evseEventToString(EvseEvent e);

    sigslot::signal<> signalAuthRequired;

    // Request more details about the error that happend
    ErrorState getErrorState();

    void processEvent(std::string event);

    void run();

    void requestErrorSequence();

    void setMatchingStarted(bool m);
    bool getMatchingStarted();

    // Note: Deprecated, do not use EvseState externally.
    // Kept for compatibility, will be removed from public interface
    // in the future.
    // Use new EvseEvent interface instead.

    enum class EvseState
    {
        Disabled,
        Idle,
        WaitingForAuthentication,
        Charging,
        ChargingPausedEV,
        ChargingPausedEVSE,
        Finished,
        Error,
        Faulted,
        T_step_EF,
        T_step_X1,
    };

    std::string evseStateToString(EvseState s);
    std::string errorStateToString(ErrorState s);

    EvseState getCurrentState();
    sigslot::signal<EvseState> signalState;
    sigslot::signal<ErrorState> signalError;
    // /Deprecated

private:
    // main Charger thread
    Everest::Thread mainThreadHandle;

    const std::unique_ptr<board_support_ACIntf>& r_bsp;

    void mainThread();

    float maxCurrent;
    std::chrono::time_point<std::chrono::system_clock> maxCurrentValidUntil;

    bool powerAvailable();

    // This mutex locks all config type members
    std::recursive_mutex configMutex;

    // This mutex locks all state type members
    std::recursive_mutex stateMutex;

    EvseState currentState;
    EvseState lastState;
    ErrorState errorState;
    std::chrono::system_clock::time_point currentStateStarted;

    float ampereToDutyCycle(float ampere);

    void checkSoftOverCurrent();
    float currentDrawnByVehicle[3];
    bool overCurrent;
    std::chrono::system_clock::time_point lastOverCurrentEvent;
    const int softOverCurrentTimeout = 7000;

    // 4 seconds according to table 3 of ISO15118-3
    const int t_step_EF = 4000;
    EvseState t_step_EF_returnState;
    float t_step_EF_returnPWM;

    // 3 seconds according to IEC61851-1
    const int t_step_X1 = 3000;
    EvseState t_step_X1_returnState;
    float t_step_X1_returnPWM;

    const float PWM_5_PERCENT = 0.05;

    bool matching_started;

    ControlPilotEvent string_to_control_pilot_event(std::string event);

    void processCPEventsIndependent(ControlPilotEvent cp_event);
    void processCPEventsState(ControlPilotEvent cp_event);
    void runStateMachine();

    bool authorized;
    // set to true if auth is from PnC, otherwise to false (EIM)
    bool authorized_pnc;
    bool cancelled;

    // AC or DC
    std::string charge_mode;
    // Config option
    bool ac_hlc_enabled;
    // HLC enabled in current AC session. This can change during the session if e.g. HLC fails.
    bool ac_hlc_enabled_current_session;
    // Config option
    bool ac_hlc_use_5percent;
    // HLC uses 5 percent signalling. Used both for AC and DC modes.
    bool hlc_use_5percent_current_session;
    // non standard compliant option to enforce HLC in AC mode
    bool ac_enforce_hlc;

    std::chrono::system_clock::time_point lastPwmUpdate;

    float update_pwm_last_dc;
    void update_pwm_now(float dc);
    void update_pwm_max_every_5seconds(float dc);

    bool paused_by_user;
};

#define CHARGER_ABSOLUTE_MAX_CURRENT double(80.0F)

} // namespace module

#endif // SRC_EVDRIVERS_CHARGER_H_
