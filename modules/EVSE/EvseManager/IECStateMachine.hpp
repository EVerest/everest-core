// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest

/*
 The IECStateMachine class provides an adapter between the board support package driver (in a seperate module) and the
 high level state machine in Charger.cpp.

 Typically the CP signal generation/reading and control of contactors/RCD etc is handled by a dedicated MCU. This MCU
 and/or the HW is responsible for the basic electrical safety of the system (such as safely shut down in case of RCD
 trigger or Linux crashing). The BSP driver is just a simple HW abstraction layer that translates the commands for
 setting PWM duty cycle/allow contactors on as well as the CP signal readings/error conditions into the everest world.
 It should not need to implement any logic or understanding of the IEC61851-1 or any higher protocol.

 This IECStateMachine is the low level state machine translating the IEC61851-1 CP states ABCDEF into more useful
 abstract events such as "CarPluggedIn/CarRequestedPower" etc. These events drive the high level state machine in
 Charger.cpp which handles the actual charging session and coordinates IEC/ISO/SLAC.

*/

#ifndef SRC_BSP_STATE_MACHINE_H_
#define SRC_BSP_STATE_MACHINE_H_

#include "ld-ev.hpp"

#include <chrono>
#include <mutex>
#include <queue>

#include <generated/interfaces/evse_board_support/Interface.hpp>
#include <sigslot/signal.hpp>

#include "Timeout.hpp"
#include "utils/thread.hpp"

#include "scoped_lock_timeout.hpp"

namespace module {

// Abstract events that drive the higher level state machine in Charger.cpp
enum class CPEvent {
    CarPluggedIn,
    CarRequestedPower,
    PowerOn,
    PowerOff,
    CarRequestedStopPower,
    CarUnplugged,
    EFtoBCD,
    BCDtoEF,
    BCDtoE,
    EvseReplugStarted,
    EvseReplugFinished,
};

// Just a helper for log printing
const std::string cpevent_to_string(CPEvent e);

// Raw (valid) CP states for the IECStateMachine
enum class RawCPState {
    Disabled,
    A,
    B,
    C,
    D,
    E,
    F
};

class IECStateMachine {
public:
    // We need the r_bsp reference to be able to talk to the bsp driver module
    IECStateMachine(const std::unique_ptr<evse_board_supportIntf>& r_bsp_, bool lock_connector_in_state_b_);
    // Call when new events from BSP requirement come in. Will signal internal events
    void process_bsp_event(const types::board_support_common::BspEvent bsp_event);
    // Allow power on from Charger state machine
    void allow_power_on(bool value, types::evse_board_support::Reason reason);

    void set_pp_ampacity(types::board_support_common::ProximityPilot pp);
    double read_pp_ampacity();
    void evse_replug(int ms);
    void switch_three_phases_while_charging(bool n);
    void setup(bool has_ventilation);

    void set_overcurrent_limit(double amps);

    void set_pwm(double value);
    void set_cp_state_X1();
    void set_cp_state_F();

    void set_three_phases(bool t) {
        three_phases = t;
    }

    void enable(bool en);

    void connector_force_unlock();

    void set_ev_simplified_mode_evse_limit(bool l) {
        ev_simplified_mode_evse_limit = l;
    }

    // Signal for internal events type
    sigslot::signal<CPEvent> signal_event;
    sigslot::signal<> signal_lock;
    sigslot::signal<> signal_unlock;

private:
    void connector_lock();
    void connector_unlock();
    void check_connector_lock();
    const std::unique_ptr<evse_board_supportIntf>& r_bsp;
    bool lock_connector_in_state_b{true};

    bool pwm_running{false};
    bool last_pwm_running{false};

    static constexpr float ev_simplified_mode_evse_limit_pwm{10 / 0.6 / 100.}; // Fixed 10A limit
    // If set to true, EVSE will limit to 10A in case of simplified charging
    bool ev_simplified_mode_evse_limit{false};
    bool ev_simplified_mode{false};
    bool has_ventilation{false};
    bool power_on_allowed{false};
    bool last_power_on_allowed{false};
    std::atomic<double> pp_ampacity{0.0};
    std::atomic<double> last_amps{-1};
    std::atomic_bool three_phases{true};

    bool car_plugged_in{false};

    RawCPState last_cp_state{RawCPState::Disabled};
    AsyncTimeout timeout_state_c1;
    AsyncTimeout timeout_unlock_state_F;

    Everest::timed_mutex_traceable state_machine_mutex;
    void feed_state_machine(RawCPState cp_state);
    std::queue<CPEvent> state_machine(RawCPState cp_state);

    types::evse_board_support::Reason power_on_reason{types::evse_board_support::Reason::PowerOff};
    void call_allow_power_on_bsp(bool value);

    std::atomic_bool is_locked{false};
    std::atomic_bool should_be_locked{false};
    std::atomic_bool force_unlocked{false};

    std::atomic_bool enabled{false};
    std::atomic_bool relais_on{false};

    static constexpr std::chrono::seconds power_off_under_load_in_c1_timeout{6};
    static constexpr std::chrono::seconds unlock_in_state_f_timeout{5};
};

} // namespace module

#endif // SRC_BSP_STATE_MACHINE_H_
