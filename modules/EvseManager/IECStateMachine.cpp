// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest

#include "IECStateMachine.hpp"
#include "everest/logging.hpp"

#include <cstdint>
#include <math.h>
#include <string.h>

namespace module {

// helper type for visitor
template <class... Ts> struct overloaded : Ts... {
    using Ts::operator()...;
};
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

enum class TimerControl : std::uint8_t {
    do_nothing,
    start,
    stop,
};

static std::variant<RawCPState, CPEvent> from_bsp_event(types::board_support_common::Event e) {
    switch (e) {
    case types::board_support_common::Event::A:
        return RawCPState::A;
    case types::board_support_common::Event::B:
        return RawCPState::B;
    case types::board_support_common::Event::C:
        return RawCPState::C;
    case types::board_support_common::Event::D:
        return RawCPState::D;
    case types::board_support_common::Event::E:
        return RawCPState::E;
    case types::board_support_common::Event::F:
        return RawCPState::F;
    case types::board_support_common::Event::PowerOn:
        return CPEvent::PowerOn;
    case types::board_support_common::Event::PowerOff:
        return CPEvent::PowerOff;
    case types::board_support_common::Event::EvseReplugStarted:
        return CPEvent::EvseReplugStarted;
    case types::board_support_common::Event::EvseReplugFinished:
        return CPEvent::EvseReplugFinished;
    default:
        return RawCPState::Disabled;
    }
}

/// \brief Converts the given Event \p e to human readable string
/// \returns a string representation of the Event
const std::string cpevent_to_string(CPEvent e) {
    switch (e) {
    case CPEvent::CarPluggedIn:
        return "CarPluggedIn";
    case CPEvent::CarRequestedPower:
        return "CarRequestedPower";
    case CPEvent::PowerOn:
        return "PowerOn";
    case CPEvent::PowerOff:
        return "PowerOff";
    case CPEvent::CarRequestedStopPower:
        return "CarRequestedStopPower";
    case CPEvent::CarUnplugged:
        return "CarUnplugged";
    case CPEvent::EFtoBCD:
        return "EFtoBCD";
    case CPEvent::BCDtoEF:
        return "BCDtoEF";
    case CPEvent::BCDtoE:
        return "BCDtoE";
    case CPEvent::EvseReplugStarted:
        return "EvseReplugStarted";
    case CPEvent::EvseReplugFinished:
        return "EvseReplugFinished";
    }
    throw std::out_of_range("No known string conversion for provided enum of type CPEvent");
}

IECStateMachine::IECStateMachine(const std::unique_ptr<evse_managerImplBase>& p_evse_,
                                 const std::unique_ptr<evse_board_supportIntf>& r_bsp_,
                                 bool lock_connector_in_state_b_) :
    p_evse(p_evse_), r_bsp(r_bsp_), lock_connector_in_state_b(lock_connector_in_state_b_) {
    // feed the state machine whenever the timer expires
    timeout_state_c1.signal_reached.connect(&IECStateMachine::feed_state_machine_no_thread, this);
    timeout_unlock_state_F.signal_reached.connect(&IECStateMachine::feed_state_machine_no_thread, this);

    // Subscribe to bsp driver to receive BspEvents from the hardware
    r_bsp->subscribe_event([this](const types::board_support_common::BspEvent event) {
        if (enabled) {
            // feed into state machine
            process_bsp_event(event);
        } else {
            EVLOG_info << "Ignoring BSP Event, BSP is not enabled yet.";
        }
    });
}

void IECStateMachine::process_bsp_event(const types::board_support_common::BspEvent bsp_event) {
    auto event = from_bsp_event(bsp_event.event);
    std::visit(overloaded{[this](RawCPState& raw_state) {
                              // If it is a raw CP state, run it through the state machine
                              {
                                  Everest::scoped_lock_timeout lock(state_machine_mutex,
                                                                    Everest::MutexDescription::IEC_process_bsp_event);
                                  cp_state = raw_state;
                              }
                              feed_state_machine();
                          },
                          // If it is another CP event, pass through
                          [this](CPEvent& event) {
                              // track relais state as confirmed by BSP
                              if (event == CPEvent::PowerOn) {
                                  relais_on = true;
                              } else if (event == CPEvent::PowerOff) {
                                  relais_on = false;
                              }
                              check_connector_lock();

                              signal_event(event);
                          }},
               event);
}

void IECStateMachine::feed_state_machine() {
    std::thread feed([this]() { feed_state_machine_no_thread(); });
    feed.detach();
}

void IECStateMachine::feed_state_machine_no_thread() {
    auto events = state_machine();

    // Process all events
    while (not events.empty()) {
        signal_event(events.front());
        events.pop();
    }
}

// Main IEC state machine. Needs to be called whenever:
// - CP state changes (both events from hardware as well as duty cycle changes)
// - Allow power on changes
// - The C1 6s timer expires
std::queue<CPEvent> IECStateMachine::state_machine() {

    std::queue<CPEvent> events;
    auto timer_state_C1 = TimerControl::do_nothing;
    auto timer_unlock_state_F = TimerControl::do_nothing;

    {
        // mutex protected section

        Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::IEC_state_machine);

        if (cp_state not_eq RawCPState::F and last_cp_state == RawCPState::F) {
            timer_unlock_state_F = TimerControl::stop;
        }

        switch (cp_state) {

        case RawCPState::Disabled:
            if (last_cp_state != RawCPState::Disabled) {
                pwm_running = false;
                r_bsp->call_pwm_off();
                ev_simplified_mode = false;
                timer_state_C1 = TimerControl::stop;
                notify_allow_power_on(false);
                connector_unlock();
            }
            break;

        case RawCPState::A:
            if (last_cp_state != RawCPState::A) {
                pwm_running = false;
                r_bsp->call_pwm_off();
                ev_simplified_mode = false;
                car_plugged_in = false;
                notify_allow_power_on(false);
                timer_state_C1 = TimerControl::stop;
                connector_unlock();
            }

            // Table A.6: Sequence 2.1 Unplug at state Bx (or any other
            // state) Table A.6: Sequence 2.2 Unplug at state Cx, Dx
            if (last_cp_state != RawCPState::A && last_cp_state != RawCPState::Disabled) {
                events.push(CPEvent::CarUnplugged);
            }
            break;

        case RawCPState::B:
            // Table A.6: Sequence 7 EV stops charging
            // Table A.6: Sequence 8.2 EV supply equipment
            // responds to EV opens S2 (w/o PWM)
            if (lock_connector_in_state_b) {
                connector_lock();
            } else {
                connector_unlock();
            }

            if (last_cp_state != RawCPState::A && last_cp_state != RawCPState::B) {

                events.push(CPEvent::CarRequestedStopPower);
                // Need to switch off according to Table A.6 Sequence 8.1
                // within 100ms
                notify_allow_power_on(false);
                timer_state_C1 = TimerControl::stop;
            }

            // Table A.6: Sequence 1.1 Plug-in
            if (last_cp_state == RawCPState::A || last_cp_state == RawCPState::Disabled ||
                (!car_plugged_in && last_cp_state == RawCPState::F)) {
                events.push(CPEvent::CarPluggedIn);
                car_plugged_in = true;
                ev_simplified_mode = false;
            }

            if (last_cp_state == RawCPState::E || last_cp_state == RawCPState::F) {
                // Triggers SLAC start
                events.push(CPEvent::EFtoBCD);
            }
            break;

        case RawCPState::D:
            connector_lock();
            // If state D is not supported switch off.
            if (not has_ventilation) {
                notify_allow_power_on(false);
                timer_state_C1 = TimerControl::stop;
                break;
            }
            // no break, intended fall through: If we support state D it is handled the same way as state C
            [[fallthrough]];

        case RawCPState::C:
            connector_lock();
            // Table A.6: Sequence 1.2 Plug-in
            if (last_cp_state == RawCPState::A || last_cp_state == RawCPState::Disabled ||
                (!car_plugged_in && last_cp_state == RawCPState::F)) {
                events.push(CPEvent::CarPluggedIn);
                car_plugged_in = true;
                EVLOG_info << "Detected simplified mode.";
                ev_simplified_mode = true;
            } else if (last_cp_state == RawCPState::B) {
                events.push(CPEvent::CarRequestedPower);
            }

            if (!pwm_running && last_pwm_running) { // X2->C1
                                                    // Table A.6 Sequence 10.2: EV does not stop drawing power
                                                    // even if PWM stops. Stop within 6 seconds (E.g. Kona1!)
                timer_state_C1 = TimerControl::start;
            }

            // PWM switches on while in state C
            if (pwm_running && !last_pwm_running) {
                // when resuming after a pause before the EV goes to state B, stop the timer.
                timer_state_C1 = TimerControl::stop;

                // If we resume charging and the EV never left state C during pause we allow non-compliant EVs to switch
                // on again.
                if (power_on_allowed) {
                    notify_allow_power_on(true);
                }
            }

            if (timeout_state_c1.reached()) {
                EVLOG_warning
                    << "Timeout of 6 seconds reached, EV did not go back to state B after PWM was switched off. "
                       "Powering off under load.";
                // We are still in state C, but the 6 seconds timeout has been reached. Now force power off under load.
                notify_allow_power_on(false);
            }

            if (pwm_running) { // C2
                // 1) When we come from state B: switch on if we are allowed to
                // 2) When we are in C2 for a while now and finally get a delayed power_on_allowed: also switch on

                if (power_on_allowed && (!last_power_on_allowed || last_cp_state == RawCPState::B)) {
                    // Table A.6: Sequence 4 EV ready to charge.
                    // Must enable power within 3 seconds.
                    notify_allow_power_on(true);
                }

                // Simulate Request power Event here for simplified mode
                // to ensure that this mode behaves similar for higher
                // layers. Note this does not work with 5% mode
                // correctly, but simplified mode does not support HLC
                // anyway.
                if (!last_pwm_running && ev_simplified_mode) {
                    events.push(CPEvent::CarRequestedPower);
                }
            }
            break;

        case RawCPState::E:
            connector_unlock();
            if (last_cp_state != RawCPState::E) {
                timer_state_C1 = TimerControl::stop;
                notify_allow_power_on(false);
                pwm_running = false;
                r_bsp->call_pwm_off();
                if (last_cp_state == RawCPState::B || last_cp_state == RawCPState::C ||
                    last_cp_state == RawCPState::D) {
                    events.push(CPEvent::BCDtoEF);
                    events.push(CPEvent::BCDtoE);
                }
            }
            break;

        case RawCPState::F:
            timer_state_C1 = TimerControl::stop;
            notify_allow_power_on(false);
            if (last_cp_state not_eq RawCPState::F) {
                timer_unlock_state_F = TimerControl::start;
                pwm_running = false;
            }
            if (last_cp_state == RawCPState::B || last_cp_state == RawCPState::C || last_cp_state == RawCPState::D) {
                events.push(CPEvent::BCDtoEF);
            }

            if (timeout_unlock_state_F.reached()) {
                connector_unlock();
            }
            break;
        }

        check_connector_lock();

        last_cp_state = cp_state;
        last_pwm_running = pwm_running;
        last_power_on_allowed = power_on_allowed;

        // end of mutex protected section
    }

    // stopping the timer could lead to a deadlock when called from the
    // mutex protected section
    switch (timer_state_C1) {
    case TimerControl::start:
        timeout_state_c1.start(power_off_under_load_in_c1_timeout);
        break;
    case TimerControl::stop:
        timeout_state_c1.stop();
        break;
    case TimerControl::do_nothing:
    default:
        break;
    }

    switch (timer_unlock_state_F) {
    case TimerControl::start:
        timeout_unlock_state_F.start(unlock_in_state_f_timeout);
        break;
    case TimerControl::stop:
        timeout_unlock_state_F.stop();
        break;
    case TimerControl::do_nothing:
    default:
        break;
    }

    return events;
}

// High level state machine sets PWM duty cycle
void IECStateMachine::set_pwm(double value) {
    {
        Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::IEC_set_pwm);
        if (value > 0 && value < 1) {
            pwm_running = true;
        } else {
            pwm_running = false;
        }
    }

    if (ev_simplified_mode_evse_limit and ev_simplified_mode and value > ev_simplified_mode_evse_limit_pwm) {
        EVLOG_warning
            << "Simplified mode: Limiting output PWM to 10A due to config option \"hack_simplified_mode_limit_10A\"";
        value = ev_simplified_mode_evse_limit_pwm;
    }

    r_bsp->call_pwm_on(value * 100);

    feed_state_machine();
}

// High level state machine sets state X1
void IECStateMachine::set_pwm_off() {
    {
        Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::IEC_set_pwm_off);
        pwm_running = false;
    }
    r_bsp->call_pwm_off();
    // Don't run the state machine in the callers context
    feed_state_machine();
}

// High level state machine sets state F
void IECStateMachine::set_pwm_F() {
    {
        Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::IEC_set_pwm_F);
        pwm_running = false;
    }
    r_bsp->call_pwm_F();
    // Don't run the state machine in the callers context
    feed_state_machine();
}

// The higher level state machine in Charger.cpp calls this to indicate it allows contactors to be switched on
void IECStateMachine::allow_power_on(bool value, types::evse_board_support::Reason reason) {
    {
        Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::IEC_allow_power_on);
        // Only set the flags here in case of power on.
        power_on_allowed = value;
        power_on_reason = reason;
        // In case of power off, we can directly forward this to the BSP driver here
        if (not power_on_allowed) {
            notify_allow_power_on(false);
        }
    }
    // The actual power on will be handled in the state machine to verify it is in the correct CP state etc.
    // Don't run the state machine in the callers context
    feed_state_machine();
}

// Private member function used to actually call the BSP driver's allow_power_on and to publish the event
// to the evse_manager interface.
// No need to lock mutex as this will be called from state machine or locked context only
void IECStateMachine::notify_allow_power_on(bool value) {
    if (not value) {
        power_on_allowed = false;
        power_on_reason = types::evse_board_support::Reason::PowerOff;
    }
    r_bsp->call_allow_power_on({value, power_on_reason});
    p_evse->publish_allow_power_on({value, power_on_reason});
}

// High level state machine requests reading PP ampacity value.
// We forward this request to the BSP driver. The high level state machine will never call this if it is not used
// (e.g. in DC or AC tethered charging)
double IECStateMachine::read_pp_ampacity() {
    auto a = r_bsp->call_ac_read_pp_ampacity();
    switch (a.ampacity) {
    case types::board_support_common::Ampacity::A_13:
        return 13.;
    case types::board_support_common::Ampacity::A_20:
        return 20.;
    case types::board_support_common::Ampacity::A_32:
        return 32.;
    case types::board_support_common::Ampacity::A_63_3ph_70_1ph:
        if (three_phases) {
            return 63.;
        } else {
            return 70.;
        }
    default:
        return 0.;
    }
}

// Forward special replug request. Only for testing setups.
void IECStateMachine::evse_replug(int ms) {
    r_bsp->call_evse_replug(ms);
}

// Forward special request to switch the number of phases during charging. BSP will need to implement a special
// sequence to not destroy cars.
void IECStateMachine::switch_three_phases_while_charging(bool n) {
    r_bsp->call_ac_switch_three_phases_while_charging(n);
}

// Forwards config parameters from EvseManager module config to BSP
void IECStateMachine::setup(bool has_ventilation) {
    this->has_ventilation = has_ventilation;
}

// enable/disable the charging port and CP signal
void IECStateMachine::enable(bool en) {
    enabled = en;
    r_bsp->call_enable(en);
}

// Forward the over current detection limit to the BSP. Many BSP MCUs monitor the charge current and trigger a fault
// in case of over current. This sets the target charging current value to be used in OC detection. It cannot be
// derived from the PWM duty cycle, use this value instead.
void IECStateMachine::set_overcurrent_limit(double amps) {
    if (amps != last_amps) {
        r_bsp->call_ac_set_overcurrent_limit_A(amps);
        last_amps = amps;
    }
}

void IECStateMachine::connector_lock() {
    should_be_locked = true;
}

void IECStateMachine::connector_unlock() {
    should_be_locked = false;
    force_unlocked = false;
}

void IECStateMachine::connector_force_unlock() {
    RawCPState cp;

    {
        Everest::scoped_lock_timeout lock(state_machine_mutex, Everest::MutexDescription::IEC_force_unlock);
        cp = cp_state;
    }

    if (not relais_on) {
        // Unconditionally try to unlock, as `is_locked` might not always reflect the physical state of the lock.
        // This can occur for example in case of a failed unlock due to a hardware issue.
        signal_unlock();
        is_locked = false;
    }

    if (cp == RawCPState::B or cp == RawCPState::C) {
        force_unlocked = true;
        check_connector_lock();
    }
}

void IECStateMachine::check_connector_lock() {
    bool should_be_locked_considering_relais_and_force = relais_on or (should_be_locked and not force_unlocked);

    if (not is_locked and should_be_locked_considering_relais_and_force) {
        signal_lock();
        is_locked = true;
    } else if (is_locked and not should_be_locked_considering_relais_and_force) {
        signal_unlock();
        is_locked = false;
    }
}

} // namespace module
