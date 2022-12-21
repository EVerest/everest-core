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

#include "SessionLog.hpp"
#include "ld-ev.hpp"
#include "utils/thread.hpp"
#include <chrono>
#include <date/date.h>
#include <date/tz.h>
#include <generated/interfaces/board_support_AC/Interface.hpp>
#include <generated/types/evse_manager.hpp>
#include <mutex>
#include <queue>
#include <sigslot/signal.hpp>

namespace module {

enum class ControlPilotEvent {
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
    EnterBCD,
    LeaveBCD,
    PermanentFault,
    EvseReplugStarted,
    EvseReplugFinished,
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
    bool setMaxCurrent(float ampere, std::chrono::time_point<date::utc_clock> validUntil);
    // update only max_current but keep the current validUntil
    bool setMaxCurrent(float ampere);
    float getMaxCurrent();
    sigslot::signal<float> signalMaxCurrent;

    enum class ChargeMode {
        AC,
        DC
    };

    void setup(bool three_phases, bool has_ventilation, const std::string& country_code, bool rcd_enabled,
               const ChargeMode charge_mode, bool ac_hlc_enabled, bool ac_hlc_use_5percent, bool ac_enforce_hlc,
               bool ac_with_soc_timeout);

    bool enable();
    bool disable();
    void set_faulted();
    // switch to next charging session after Finished
    bool restart();

    // Public interface during charging
    //
    // Call anytime, but mostly used during charging session
    //

    // call when in state WaitingForAuthentication
    void Authorize(bool a, const std::string& userid, bool pnc);
    bool DeAuthorize();
    std::string getAuthTag();

    bool Authorized_PnC();
    bool Authorized_EIM();

    // this indicates the charger is done with all of its t_step_XX routines and HLC can now also start charging
    bool Authorized_EIM_ready_for_HLC();
    bool Authorized_PnC_ready_for_HLC();

    // trigger replug sequence while charging to switch number of phases
    bool switchThreePhasesWhileCharging(bool n);

    bool pauseCharging();
    bool pauseChargingWaitForPower();
    bool resumeCharging();
    bool resumeChargingPowerAvailable();
    bool getPausedByEVSE();

    bool cancelTransaction(const types::evse_manager::StopTransactionRequest& request); // cancel transaction ahead of time when car is still plugged
    std::string getStopTransactionIdTag();
    types::evse_manager::StopTransactionReason getTransactionFinishedReason(); // get reason for last finished event
    types::evse_manager::StartSessionReason getSessionStartedReason(); // get reason for last session start event

    // execute a virtual replug sequence. Does NOT generate a Car plugged in event etc,
    // since the session is not restarted. It can be used to e.g. restart the ISO session
    // and switch between AC and DC mode within a session.
    bool evseReplug();

    void setCurrentDrawnByVehicle(float l1, float l2, float l3);

    bool forceUnlock();

    // Signal for EvseEvents
    sigslot::signal<types::evse_manager::SessionEventEnum> signalEvent;

    sigslot::signal<> signalACWithSoCTimeout;

    sigslot::signal<> signal_DC_supply_off;

    // Request more details about the error that happend
    types::evse_manager::Error getErrorState();

    void processEvent(types::board_support::Event event);

    void run();

    void requestErrorSequence();

    void setMatchingStarted(bool m);
    bool getMatchingStarted();

    void notifyCurrentDemandStarted();

    // Note: Deprecated, do not use EvseState externally.
    // Kept for compatibility, will be removed from public interface
    // in the future.
    // Use new EvseEvent interface instead.

    enum class EvseState {
        Disabled,
        Idle,
        WaitingForAuthentication,
        PrepareCharging,
        Charging,
        ChargingPausedEV,
        ChargingPausedEVSE,
        StoppingCharging,
        Finished,
        Error,
        Faulted,
        T_step_EF,
        T_step_X1,
        Replug
    };

    std::string evseStateToString(EvseState s);

    EvseState getCurrentState();
    sigslot::signal<EvseState> signalState;
    sigslot::signal<types::evse_manager::Error> signalError;
    // /Deprecated

private:
    // main Charger thread
    Everest::Thread mainThreadHandle;

    const std::unique_ptr<board_support_ACIntf>& r_bsp;

    void mainThread();

    float maxCurrent;
    std::chrono::time_point<date::utc_clock> maxCurrentValidUntil;

    bool powerAvailable();

    bool AuthorizedEIM();
    bool AuthorizedPnC();

    void startSession(bool authfirst);
    void stopSession();
    bool sessionActive();

    void startTransaction();
    void stopTransaction();
    bool transactionActive();
    bool transaction_active;
    bool session_active;
    std::string stop_transaction_id_tag;
    types::evse_manager::StopTransactionReason last_stop_transaction_reason;
    types::evse_manager::StartSessionReason last_start_session_reason;

    // This mutex locks all config type members
    std::recursive_mutex configMutex;

    // This mutex locks all state type members
    std::recursive_mutex stateMutex;

    EvseState currentState;
    EvseState lastState;
    types::evse_manager::Error errorState{types::evse_manager::Error::Internal};
    std::chrono::system_clock::time_point currentStateStarted;

    float ampereToDutyCycle(float ampere);

    void checkSoftOverCurrent();
    float currentDrawnByVehicle[3];
    bool overCurrent;
    std::chrono::time_point<date::utc_clock> lastOverCurrentEvent;
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

    const int t_replug_ms = 4000;

    bool matching_started;

    ControlPilotEvent string_to_control_pilot_event(const types::board_support::Event& event);

    void processCPEventsIndependent(ControlPilotEvent cp_event);
    void processCPEventsState(ControlPilotEvent cp_event);
    void runStateMachine();

    bool authorized;
    // set to true if auth is from PnC, otherwise to false (EIM)
    bool authorized_pnc;
    std::string auth_tag;

    // AC or DC
    ChargeMode charge_mode{0};
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
    // non standard compliant option: time out after a while and switch back to DC to get SoC update
    bool ac_with_soc_timeout;
    int ac_with_soc_timer;

    std::chrono::time_point<date::utc_clock> lastPwmUpdate;

    float update_pwm_last_dc;
    void update_pwm_now(float dc);
    void update_pwm_max_every_5seconds(float dc);
    void pwm_off();
    void pwm_F();

    bool paused_by_user;
};

#define CHARGER_ABSOLUTE_MAX_CURRENT double(80.0F)

} // namespace module

#endif // SRC_EVDRIVERS_CHARGER_H_
