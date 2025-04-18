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
 *  1) Hi level state machine that is controlled by a) events from evse_board_support interface
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
#include <generated/interfaces/powermeter/Interface.hpp>
#include <generated/types/authorization.hpp>
#include <generated/types/evse_manager.hpp>
#include <generated/types/units_signed.hpp>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <sigslot/signal.hpp>
#include <string>
#include <vector>

#include "ErrorHandling.hpp"
#include "EventQueue.hpp"
#include "IECStateMachine.hpp"
#include "PersistentStore.hpp"
#include "scoped_lock_timeout.hpp"
#include "utils.hpp"

namespace module {

const std::string IEC62196Type2Cable = "IEC62196Type2Cable";
const std::string IEC62196Type2Socket = "IEC62196Type2Socket";

class Charger {
public:
    Charger(const std::unique_ptr<IECStateMachine>& bsp, const std::unique_ptr<ErrorHandling>& error_handling,
            const std::vector<std::unique_ptr<powermeterIntf>>& r_powermeter_billing,
            const std::unique_ptr<PersistentStore>& store,
            const types::evse_board_support::Connector_type& connector_type, const std::string& evse_id);
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
        SwitchPhases,
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

    void setup(bool has_ventilation, const ChargeMode charge_mode, bool ac_hlc_enabled, bool ac_hlc_use_5percent,
               bool ac_enforce_hlc, bool ac_with_soc_timeout, float soft_over_current_tolerance_percent,
               float soft_over_current_measurement_noise_A, const int switch_3ph1ph_delay_s,
               const std::string switch_3ph1ph_cp_state, const int soft_over_current_timeout_ms,
               const int _state_F_after_fault_ms, const bool fail_on_powermeter_errors, const bool raise_mrec9,
               const int sleep_before_enabling_pwm_hlc_mode_ms);

    void enable_disable_initial_state_publish();
    bool enable_disable(int connector_id, const types::evse_manager::EnableDisableSource& source);

    void set_faulted();
    void set_hlc_error();

    // Public interface during charging
    //
    // Call anytime, but mostly used during charging session
    //

    // Returns active session_uuid. Returns empty string if not session is active
    std::string get_session_id() const;

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
    sigslot::signal<std::string> signal_session_resumed_event;
    sigslot::signal<types::evse_manager::StartSessionReason, std::optional<types::authorization::ProvidedIdToken>>
        signal_session_started_event;
    sigslot::signal<types::authorization::ProvidedIdToken> signal_transaction_started_event;
    sigslot::signal<types::evse_manager::StopTransactionReason, std::optional<types::authorization::ProvidedIdToken>>
        signal_transaction_finished_event;

    sigslot::signal<> signal_ac_with_soc_timeout;

    sigslot::signal<> signal_dc_supply_off;
    sigslot::signal<> signal_slac_reset;
    sigslot::signal<> signal_slac_start;

    sigslot::signal<> signal_hlc_stop_charging;
    sigslot::signal<types::iso15118::EvseError> signal_hlc_error;

    void process_event(CPEvent event);

    void run();

    void request_error_sequence();

    void set_matching_started(bool m);

    void notify_currentdemand_started();

    std::string evse_state_to_string(EvseState s);

    EvseState get_current_state();
    sigslot::signal<EvseState> signal_state;

    void inform_new_evse_max_hlc_limits(const types::iso15118::DcEvseMaximumLimits& l);
    types::iso15118::DcEvseMaximumLimits get_evse_max_hlc_limits();

    void dlink_pause();
    void dlink_error();
    void dlink_terminate();

    void set_hlc_charging_active();
    void set_hlc_allow_close_contactor(bool on);

    bool stop_charging_on_fatal_error();
    bool entered_fatal_error_state();
    int time_in_fatal_error_state_ms();

    /// @brief Returns the OCMF start data.
    ///
    /// The data is generated when starting the transaction. The call resets the
    /// internal variable and is thus not idempotent.
    std::optional<types::units_signed::SignedMeterValue> get_start_signed_meter_value();

    /// @brief Returns the OCMF stop data.
    ///
    /// The data is generated when stopping the transaction. The call resets the
    /// internal variable and is thus not idempotent.
    std::optional<types::units_signed::SignedMeterValue> get_stop_signed_meter_value();

    types::evse_manager::EnableDisableSource get_last_enable_disable_source();

    utils::Stopwatch& get_stopwatch() {
        return stopwatch;
    }

    void set_connector_type(types::evse_board_support::Connector_type t) {
        connector_type = t;
    }

    void cleanup_transactions_on_startup();

private:
    utils::Stopwatch stopwatch;

    std::optional<types::units_signed::SignedMeterValue>
    take_signed_meter_data(std::optional<types::units_signed::SignedMeterValue>& data);

    bool stop_charging_on_fatal_error_internal();
    float get_max_current_internal();
    float get_max_current_signalled_to_ev_internal();
    bool deauthorize_internal();
    bool pause_charging_wait_for_power_internal();

    void bcb_toggle_reset();
    void bcb_toggle_detect_start_pulse();
    void bcb_toggle_detect_stop_pulse();
    bool bcb_toggle_detected();

    void clear_errors_on_unplug();

    void update_pwm_now(float dc);
    void update_pwm_now_if_changed(float dc);
    void update_pwm_now_if_changed_ampere(float dc);
    void update_pwm_max_every_5seconds_ampere(float dc);
    void pwm_off();
    void pwm_F();

    void process_cp_events_independent(CPEvent cp_event);
    void process_cp_events_state(CPEvent cp_event);
    void run_state_machine();

    void main_thread();
    void error_thread();

    void graceful_stop_charging();

    float ampere_to_duty_cycle(float ampere);

    void check_soft_over_current();

    bool power_available();

    void start_session(bool authfirst);
    void stop_session();

    bool start_transaction();
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
        types::iso15118::DcEvseMaximumLimits current_evse_max_limits;
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
        std::string session_uuid;
        bool connector_enabled;
        EvseState current_state;
        types::evse_manager::StopTransactionReason last_stop_transaction_reason;
        types::evse_manager::StartSessionReason last_start_session_reason;
        float current_drawn_by_vehicle[3];
        bool error_prevent_charging_flag{false};
        bool last_error_prevent_charging_flag{false};
        int ac_with_soc_timer;
        // non standard compliant option: time out after a while and switch back to DC to get SoC update
        bool ac_with_soc_timeout;
        bool contactor_welded{false};
        bool switch_3ph1ph_threephase{false};
        bool switch_3ph1ph_threephase_ongoing{false};

        std::optional<types::units_signed::SignedMeterValue> stop_signed_meter_value;
        std::optional<types::units_signed::SignedMeterValue> start_signed_meter_value;
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
        // Delay when switching from 1ph to 3ph or 3ph to 1ph
        int switch_3ph1ph_delay_s{10};
        // Use state F if true, otherwise use X1
        bool switch_3ph1ph_cp_state_F{false};
        // Tolerate soft over current for given time
        int soft_over_current_timeout_ms{7000};
        // Switch to F for configured ms after a fatal error
        int state_F_after_fault_ms{300};
        // Fail on powermeter errors
        bool fail_on_powermeter_errors;
        // Raise MREC9 authorization timeout error
        bool raise_mrec9;
        // sleep before enabling pwm in hlc mode
        int sleep_before_enabling_pwm_hlc_mode_ms{1000};
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
        float t_step_EF_return_ampere;

        EvseState switching_phases_return_state;

        EvseState t_step_X1_return_state;
        float t_step_X1_return_pwm;
        std::chrono::time_point<std::chrono::steady_clock> last_over_current_event;
        bool over_current{false};

        bool last_error_prevent_charging_flag{false};
        std::chrono::system_clock::time_point current_state_started;
        EvseState last_state_detect_state_change;
        EvseState last_state;

        bool pp_warning_printed{false};
        bool no_energy_warning_printed{false};
        float pwm_set_last_ampere{0};
        bool t_step_ef_x1_pause{false};
        bool pwm_F_active{false};

        std::chrono::time_point<std::chrono::steady_clock> fatal_error_became_active;
        bool fatal_error_timer_running{false};
    } internal_context;

    // main Charger thread
    Everest::Thread main_thread_handle;
    Everest::Thread error_thread_handle;

    const std::unique_ptr<IECStateMachine>& bsp;
    const std::unique_ptr<ErrorHandling>& error_handling;
    const std::vector<std::unique_ptr<powermeterIntf>>& r_powermeter_billing;
    const std::unique_ptr<PersistentStore>& store;
    std::atomic<types::evse_board_support::Connector_type> connector_type{
        types::evse_board_support::Connector_type::IEC62196Type2Cable};
    const std::string evse_id;

    // ErrorHandling events
    enum class ErrorHandlingEvents : std::uint8_t {
        PreventCharging,
        AllErrorsPreventingChargingCleared,
        AllErrorCleared
    };

    EventQueue<ErrorHandlingEvents> error_handling_event_queue;

    // constants
    static constexpr float CHARGER_ABSOLUTE_MAX_CURRENT{1000.};
    constexpr static int LEGACY_WAKEUP_TIMEOUT{30000};
    constexpr static int PREPARING_TIMEOUT_PAUSED_BY_EV{10000};
    // valid Length of BCB toggles
    static constexpr auto TP_EV_VALD_STATE_DURATION_MIN =
        std::chrono::milliseconds(200 - 50); // We give 50 msecs tolerance to the norm values (table 3 ISO15118-3)
    static constexpr auto TP_EV_VALD_STATE_DURATION_MAX =
        std::chrono::milliseconds(400 + 50); // We give 50 msecs tolerance to the norm values (table 3 ISO15118-3)
    // Maximum duration of a BCB toggle sequence of 1-3 BCB toggles
    static constexpr auto TT_EVSE_VALD_TOGGLE =
        std::chrono::milliseconds(3500 + 200); // We give 200 msecs tolerance to the norm values (table 3 ISO15118-3)
    static constexpr auto MAINLOOP_UPDATE_RATE = std::chrono::milliseconds(100);
    static constexpr float PWM_5_PERCENT = 0.05;
    static constexpr int T_REPLUG_MS = 4000;
    // 3 seconds according to IEC61851-1
    static constexpr int T_STEP_X1 = 3000;
    // 4 seconds according to table 3 of ISO15118-3
    static constexpr int T_STEP_EF = 4000;
    static constexpr int IEC_PWM_MAX_UPDATE_INTERVAL = 5000;
    // EV READY certification requires a small pause of 500-1000 ms in X1 after a t_step_EF sequence before going to X2-
    // This is not required by IEC61851-1, but it is allowed by the IEC. It helps some older EVs to start charging
    // after the wake-up sequence.
    static constexpr int STAY_IN_X1_AFTER_TSTEP_EF_MS = 750;

    types::evse_manager::EnableDisableSource active_enable_disable_source{
        types::evse_manager::Enable_source::Unspecified, types::evse_manager::Enable_state::Unassigned, 10000};
    std::vector<types::evse_manager::EnableDisableSource> enable_disable_source_table;
    bool parse_enable_disable_source_table();
    void enable_disable_source_table_update(const types::evse_manager::EnableDisableSource& source);

protected:
    // provide access for unit tests
    constexpr auto& get_shared_context() {
        return shared_context;
    }
    constexpr const auto& get_enable_disable_source_table() const {
        return enable_disable_source_table;
    }
};

} // namespace module

namespace types::evse_manager {
constexpr bool operator==(const EnableDisableSource& lhs, const EnableDisableSource& rhs) {
    return lhs.enable_source == rhs.enable_source && lhs.enable_state == rhs.enable_state &&
           lhs.enable_priority == rhs.enable_priority;
}
constexpr bool operator!=(const EnableDisableSource& lhs, const EnableDisableSource& rhs) {
    return !operator==(lhs, rhs);
}
} // namespace types::evse_manager

#endif // SRC_EVDRIVERS_CHARGER_H_
