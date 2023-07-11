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
 * charging).
 */

#ifndef SRC_EVDRIVERS_CHARGER_H_
#define SRC_EVDRIVERS_CHARGER_H_

#include "SessionLog.hpp"
#include "ld-ev.hpp"
#include "utils/thread.hpp"
#include <chrono>
#include <date/date.h>
#include <date/tz.h>
#include <generated/interfaces/ISO15118_charger/Interface.hpp>
#include <generated/types/authorization.hpp>
#include <generated/types/evse_manager.hpp>
#include <mutex>
#include <queue>
#include <sigslot/signal.hpp>

#include "ErrorHandling.hpp"
#include "IECStateMachine.hpp"

namespace module {

const std::string IEC62196Type2Cable = "IEC62196Type2Cable";
const std::string IEC62196Type2Socket = "IEC62196Type2Socket";

class Charger {
public:
    Charger(const std::unique_ptr<IECStateMachine>& bsp, const std::unique_ptr<ErrorHandling>& error_handling,
            const types::evse_board_support::Connector_type& connector_type);
    ~Charger();

    // Public interface to configure Charger
    //
    // Call anytime also during charging, but call setters in this block at
    // least initially once.
    //

    // external input to charger: update max_current and new validUntil
    bool setMaxCurrent(float ampere, std::chrono::time_point<date::utc_clock> validUntil);

    float getMaxCurrent();
    sigslot::signal<float> signalMaxCurrent;

    enum class ChargeMode {
        AC,
        DC
    };

    void setup(bool three_phases, bool has_ventilation, const std::string& country_code, const ChargeMode charge_mode,
               bool ac_hlc_enabled, bool ac_hlc_use_5percent, bool ac_enforce_hlc, bool ac_with_soc_timeout,
               float soft_over_current_tolerance_percent, float soft_over_current_measurement_noise_A);

    bool enable(int connector_id);
    bool disable(int connector_id);
    void set_faulted();
    void set_hlc_error(types::evse_manager::ErrorEnum e);
    void set_rcd_error();
    // switch to next charging session after Finished
    bool restart();

    // Public interface during charging
    //
    // Call anytime, but mostly used during charging session
    //

    // call when in state WaitingForAuthentication
    void Authorize(bool a, const types::authorization::ProvidedIdToken& token);
    bool DeAuthorize();
    types::authorization::ProvidedIdToken getIdToken();

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

    bool cancelTransaction(const types::evse_manager::StopTransactionRequest&
                               request); // cancel transaction ahead of time when car is still plugged
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
    sigslot::signal<> signal_SLAC_reset;
    sigslot::signal<> signal_SLAC_start;

    sigslot::signal<> signal_hlc_stop_charging;

    void processEvent(CPEvent event);

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
        WaitingForEnergy,
        Charging,
        ChargingPausedEV,
        ChargingPausedEVSE,
        StoppingCharging,
        Finished,
        T_step_EF,
        T_step_X1,
        Replug
    };

    enum class HlcTerminatePause {
        Unknown,
        Terminate,
        Pause
    };

    std::string evseStateToString(EvseState s);

    EvseState getCurrentState();
    sigslot::signal<EvseState> signalState;
    // sigslot::signal<types::evse_manager::ErrorEnum> signalError;
    //  /Deprecated

    void inform_new_evse_max_hlc_limits(const types::iso15118_charger::DC_EVSEMaximumLimits& l);
    types::iso15118_charger::DC_EVSEMaximumLimits get_evse_max_hlc_limits();

    void dlink_pause();
    void dlink_error();
    void dlink_terminate();

    void set_hlc_charging_active();
    void set_hlc_allow_close_contactor(bool on);

    bool errors_prevent_charging();

private:
    void bcb_toggle_reset();
    void bcb_toggle_detect_start_pulse();
    void bcb_toggle_detect_stop_pulse();
    bool bcb_toggle_detected();

    // main Charger thread
    Everest::Thread mainThreadHandle;

    const std::unique_ptr<IECStateMachine>& bsp;
    const std::unique_ptr<ErrorHandling>& error_handling;
    const types::evse_board_support::Connector_type& connector_type;

    void mainThread();

    float maxCurrent;
    std::chrono::time_point<date::utc_clock> maxCurrentValidUntil;
    float maxCurrentCable{0.};

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
    EvseState last_state;
    EvseState last_state_detect_state_change;

    types::evse_manager::ErrorEnum errorState{types::evse_manager::ErrorEnum::Internal};
    std::chrono::system_clock::time_point currentStateStarted;

    bool connectorEnabled;

    bool error_prevent_charging_flag{false};
    bool last_error_prevent_charging_flag{false};
    void graceful_stop_charging();

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

    void processCPEventsIndependent(CPEvent cp_event);
    void processCPEventsState(CPEvent cp_event);
    void runStateMachine();

    bool authorized;
    // set to true if auth is from PnC, otherwise to false (EIM)
    bool authorized_pnc;

    types::authorization::ProvidedIdToken id_token;

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
    void update_pwm_now_if_changed(float dc);
    void update_pwm_max_every_5seconds(float dc);
    void pwm_off();
    void pwm_F();
    bool pwm_running{false};

    types::iso15118_charger::DC_EVSEMaximumLimits currentEvseMaxLimits;

    static constexpr auto SLEEP_BEFORE_ENABLING_PWM_HLC_MODE = std::chrono::seconds(1);
    static constexpr auto MAINLOOP_UPDATE_RATE = std::chrono::milliseconds(100);

    float soft_over_current_tolerance_percent{10.};
    float soft_over_current_measurement_noise_A{0.5};

    HlcTerminatePause hlc_charging_terminate_pause;
    bool hlc_charging_active{false};
    bool hlc_allow_close_contactor{false};
    bool iec_allow_close_contactor{false};

    // valid Length of BCB toggles
    static constexpr auto TP_EV_VALD_STATE_DURATION_MIN =
        std::chrono::milliseconds(200 - 50); // We give 50 msecs tolerance to the norm values (table 3 ISO15118-3)
    static constexpr auto TP_EV_VALD_STATE_DURATION_MAX =
        std::chrono::milliseconds(400 + 50); // We give 50 msecs tolerance to the norm values (table 3 ISO15118-3)

    // Maximum duration of a BCB toggle sequence of 1-3 BCB toggles
    static constexpr auto TT_EVSE_VALD_TOGGLE =
        std::chrono::milliseconds(3500 + 200); // We give 200 msecs tolerance to the norm values (table 3 ISO15118-3)

    std::chrono::time_point<std::chrono::steady_clock> hlc_ev_pause_start_of_bcb;
    std::chrono::time_point<std::chrono::steady_clock> hlc_ev_pause_start_of_bcb_sequence;
    int hlc_ev_pause_bcb_count{0};
    bool hlc_bcb_sequence_started{false};

    bool contactors_closed{false};

    // As per IEC61851-1 A.5.3
    bool legacy_wakeup_done{false};
    constexpr static int legacy_wakeup_timeout{30000};
};

#define CHARGER_ABSOLUTE_MAX_CURRENT double(80.0F)

} // namespace module

#endif // SRC_EVDRIVERS_CHARGER_H_
