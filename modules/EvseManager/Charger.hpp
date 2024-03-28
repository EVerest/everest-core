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
#include "EventQueue.hpp"
#include "IECStateMachine.hpp"
#include "scoped_lock_timeout.hpp"

namespace module {

const std::string IEC62196Type2Cable = "IEC62196Type2Cable";
const std::string IEC62196Type2Socket = "IEC62196Type2Socket";

class Charger {
public:
    Charger(const std::unique_ptr<IECStateMachine>& bsp, const std::unique_ptr<ErrorHandling>& error_handling,
            const types::evse_board_support::Connector_type& connector_type);
    ~Charger();

    enum class ChargeMode {
        AC,
        DC
    };

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

    // Public interface to configure Charger
    //
    // Call anytime also during charging, but call setters in this block at
    // least initially once.
    //

    // external input to charger: update max_current and new validUntil
    bool set_max_current(float ampere, std::chrono::time_point<date::utc_clock> validUntil);
    float get_max_current();
    sigslot::signal<float> signal_max_current;

    void setup(bool three_phases, bool has_ventilation, const std::string& country_code, const ChargeMode charge_mode,
               bool ac_hlc_enabled, bool ac_hlc_use_5percent, bool ac_enforce_hlc, bool ac_with_soc_timeout,
               float soft_over_current_tolerance_percent, float soft_over_current_measurement_noise_A);

    bool enable_disable(int connector_id, const types::evse_manager::EnableDisableSource& source);

    void set_faulted();
    void set_hlc_error();

    // Public interface during charging
    //
    // Call anytime, but mostly used during charging session
    //

    // call when in state WaitingForAuthentication
    void authorize(bool a, const types::authorization::ProvidedIdToken& token);
    bool deauthorize();

    bool get_authorized_pnc();
    bool get_authorized_eim();

    // this indicates the charger is done with all of its t_step_XX routines and HLC can now also start charging
    bool get_authorized_eim_ready_for_hlc();
    bool get_authorized_pnc_ready_for_hlc();

    // trigger replug sequence while charging to switch number of phases
    bool switch_three_phases_while_charging(bool n);

    bool pause_charging();
    bool pause_charging_wait_for_power();
    bool resume_charging();
    bool resume_charging_power_available();

    bool cancel_transaction(const types::evse_manager::StopTransactionRequest&
                                request); // cancel transaction ahead of time when car is still plugged

    // execute a virtual replug sequence. Does NOT generate a Car plugged in event etc,
    // since the session is not restarted. It can be used to e.g. restart the ISO session
    // and switch between AC and DC mode within a session.
    bool evse_replug();

    void set_current_drawn_by_vehicle(float l1, float l2, float l3);

    // Signal for EvseEvents
    sigslot::signal<types::evse_manager::SessionEventEnum> signal_simple_event;
    sigslot::signal<types::evse_manager::StartSessionReason> signal_session_started_event;
    sigslot::signal<types::authorization::ProvidedIdToken> signal_transaction_started_event;
    sigslot::signal<types::evse_manager::StopTransactionReason, std::optional<types::authorization::ProvidedIdToken>>
        signal_transaction_finished_event;

    sigslot::signal<> signal_ac_with_soc_timeout;

    sigslot::signal<> signal_dc_supply_off;
    sigslot::signal<> signal_slac_reset;
    sigslot::signal<> signal_slac_start;

    sigslot::signal<> signal_hlc_stop_charging;

    void process_event(CPEvent event);

    void run();

    void request_error_sequence();

    void set_matching_started(bool m);

    void notify_currentdemand_started();

    std::string evse_state_to_string(EvseState s);

    EvseState get_current_state();
    sigslot::signal<EvseState> signal_state;

    void inform_new_evse_max_hlc_limits(const types::iso15118_charger::DC_EVSEMaximumLimits& l);
    types::iso15118_charger::DC_EVSEMaximumLimits get_evse_max_hlc_limits();

    void dlink_pause();
    void dlink_error();
    void dlink_terminate();

    void set_hlc_charging_active();
    void set_hlc_allow_close_contactor(bool on);

    bool errors_prevent_charging();

    types::evse_manager::EnableDisableSource get_last_enable_disable_source();

private:
    bool errors_prevent_charging_internal();
    float get_max_current_internal();
    bool deauthorize_internal();
    bool pause_charging_wait_for_power_internal();

    void bcb_toggle_reset();
    void bcb_toggle_detect_start_pulse();
    void bcb_toggle_detect_stop_pulse();
    bool bcb_toggle_detected();

    void clear_errors_on_unplug();

    void update_pwm_now(float dc);
    void update_pwm_now_if_changed(float dc);
    void update_pwm_max_every_5seconds(float dc);
    void pwm_off();
    void pwm_F();

    void process_cp_events_independent(CPEvent cp_event);
    void process_cp_events_state(CPEvent cp_event);
    void run_state_machine();

    void main_thread();

    void graceful_stop_charging();

    float ampere_to_duty_cycle(float ampere);

    void check_soft_over_current();

    bool power_available();

    void start_session(bool authfirst);
    void stop_session();

    void start_transaction();
    void stop_transaction();

    // This mutex locks all variables related to the state machine
    Everest::timed_mutex_traceable state_machine_mutex;

    // used by different threads, complete main loop must be locked for write access
    struct SharedContext {
        // As per IEC61851-1 A.5.3
        bool legacy_wakeup_done{false};
        bool hlc_allow_close_contactor{false};
        bool iec_allow_close_contactor{false};
        bool hlc_charging_active{false};
        HlcTerminatePause hlc_charging_terminate_pause;
        types::iso15118_charger::DC_EVSEMaximumLimits current_evse_max_limits;
        bool pwm_running{false};
        std::optional<types::authorization::ProvidedIdToken>
            stop_transaction_id_token; // only set in case transaction was stopped locally
        types::authorization::ProvidedIdToken id_token;
        bool authorized;
        // set to true if auth is from PnC, otherwise to false (EIM)
        bool authorized_pnc;
        bool matching_started;
        float max_current;
        std::chrono::time_point<date::utc_clock> max_current_valid_until;
        float max_current_cable{0.};
        bool transaction_active;
        bool session_active;
        bool connector_enabled;
        EvseState current_state;
        types::evse_manager::StopTransactionReason last_stop_transaction_reason;
        types::evse_manager::StartSessionReason last_start_session_reason;
        float current_drawn_by_vehicle[3];
        bool error_prevent_charging_flag{false};
        int ac_with_soc_timer;
        // non standard compliant option: time out after a while and switch back to DC to get SoC update
        bool ac_with_soc_timeout;
        bool contactor_welded{false};
    } shared_context;

    struct ConfigContext {
        // non standard compliant option to enforce HLC in AC mode
        bool ac_enforce_hlc;
        // Config option to use 5 percent PWM in HLC AC mode
        bool ac_hlc_use_5percent;
        // Config option to enable HLC in AC mode
        bool ac_hlc_enabled;
        // AC or DC
        ChargeMode charge_mode{0};
    } config_context;

    // Used by different threads, but requires no complete state machine locking
    std::atomic_bool contactors_closed{false};
    std::atomic<float> soft_over_current_tolerance_percent{10.};
    std::atomic<float> soft_over_current_measurement_noise_A{0.5};
    // HLC uses 5 percent signalling. Used both for AC and DC modes.
    std::atomic_bool hlc_use_5percent_current_session;
    // HLC enabled in current AC session. This can change during the session if e.g. HLC fails.
    std::atomic_bool ac_hlc_enabled_current_session;

    // This struct is only used from main loop thread
    struct InternalContext {
        bool hlc_bcb_sequence_started{false};
        int hlc_ev_pause_bcb_count{0};
        std::chrono::time_point<std::chrono::steady_clock> hlc_ev_pause_start_of_bcb;
        std::chrono::time_point<std::chrono::steady_clock> hlc_ev_pause_start_of_bcb_sequence;
        float update_pwm_last_dc;
        std::chrono::time_point<std::chrono::steady_clock> last_pwm_update;

        EvseState t_step_EF_return_state;
        float t_step_EF_return_pwm;

        EvseState t_step_X1_return_state;
        float t_step_X1_return_pwm;
        std::chrono::time_point<std::chrono::steady_clock> last_over_current_event;
        bool over_current{false};

        bool last_error_prevent_charging_flag{false};
        std::chrono::system_clock::time_point current_state_started;
        EvseState last_state_detect_state_change;
        EvseState last_state;
    } internal_context;

    // main Charger thread
    Everest::Thread main_thread_handle;

    const std::unique_ptr<IECStateMachine>& bsp;
    const std::unique_ptr<ErrorHandling>& error_handling;
    const types::evse_board_support::Connector_type& connector_type;

    // ErrorHandling events
    enum class ErrorHandlingEvents : std::uint8_t {
        prevent_charging,
        prevent_charging_welded,
        all_errors_cleared
    };

    EventQueue<ErrorHandlingEvents> error_handling_event_queue;

    // constants
    static constexpr float CHARGER_ABSOLUTE_MAX_CURRENT{80.};
    constexpr static int LEGACY_WAKEUP_TIMEOUT{30000};
    // valid Length of BCB toggles
    static constexpr auto TP_EV_VALD_STATE_DURATION_MIN =
        std::chrono::milliseconds(200 - 50); // We give 50 msecs tolerance to the norm values (table 3 ISO15118-3)
    static constexpr auto TP_EV_VALD_STATE_DURATION_MAX =
        std::chrono::milliseconds(400 + 50); // We give 50 msecs tolerance to the norm values (table 3 ISO15118-3)
    // Maximum duration of a BCB toggle sequence of 1-3 BCB toggles
    static constexpr auto TT_EVSE_VALD_TOGGLE =
        std::chrono::milliseconds(3500 + 200); // We give 200 msecs tolerance to the norm values (table 3 ISO15118-3)
    static constexpr auto SLEEP_BEFORE_ENABLING_PWM_HLC_MODE = std::chrono::seconds(1);
    static constexpr auto MAINLOOP_UPDATE_RATE = std::chrono::milliseconds(100);
    static constexpr float PWM_5_PERCENT = 0.05;
    static constexpr int T_REPLUG_MS = 4000;
    // 3 seconds according to IEC61851-1
    static constexpr int T_STEP_X1 = 3000;
    // 4 seconds according to table 3 of ISO15118-3
    static constexpr int T_STEP_EF = 4000;
    static constexpr int SOFT_OVER_CURRENT_TIMEOUT = 7000;

    types::evse_manager::EnableDisableSource active_enable_disable_source{
        types::evse_manager::Enable_source::Unspecified, types::evse_manager::Enable_state::Unassigned, 10000};
    std::vector<types::evse_manager::EnableDisableSource> enable_disable_source_table;
    bool parse_enable_disable_source_table();
};

} // namespace module

#endif // SRC_EVDRIVERS_CHARGER_H_
