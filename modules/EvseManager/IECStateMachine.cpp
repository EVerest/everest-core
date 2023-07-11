// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
/*
 * Charger.cpp
 *
 *  Created on: 08.03.2021
 *      Author: cornelius
 *
 */

#include "IECStateMachine.hpp"

#include <math.h>
#include <string.h>

namespace module {

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
    case types::board_support_common::Event::ErrorDF:
        return CPEvent::ErrorDF;
    case types::board_support_common::Event::ErrorRelais:
        return CPEvent::ErrorRelais;
    case types::board_support_common::Event::ErrorVentilationNotAvailable:
        return CPEvent::ErrorVentilationNotAvailable;
    case types::board_support_common::Event::ErrorOverCurrent:
        return CPEvent::ErrorOverCurrent;
    case types::board_support_common::Event::ErrorOverVoltage:
        return CPEvent::ErrorOverVoltage;
    case types::board_support_common::Event::ErrorUnderVoltage:
        return CPEvent::ErrorUnderVoltage;
    case types::board_support_common::Event::ErrorMotorLock:
        return CPEvent::ErrorMotorLock;
    case types::board_support_common::Event::ErrorOverTemperature:
        return CPEvent::ErrorOverTemperature;
    case types::board_support_common::Event::ErrorBrownOut:
        return CPEvent::ErrorBrownOut;
    case types::board_support_common::Event::ErrorCablePP:
        return CPEvent::ErrorCablePP;
    case types::board_support_common::Event::ErrorEnergyManagement:
        return CPEvent::ErrorEnergyManagement;
    case types::board_support_common::Event::ErrorNeutralPEN:
        return CPEvent::ErrorNeutralPEN;
    case types::board_support_common::Event::ErrorCpDriver:
        return CPEvent::ErrorCpDriver;
    case types::board_support_common::Event::PermanentFault:
        return CPEvent::PermanentFault;
    case types::board_support_common::Event::EvseReplugStarted:
        return CPEvent::EvseReplugStarted;
    case types::board_support_common::Event::EvseReplugFinished:
        return CPEvent::EvseReplugFinished;
    case types::board_support_common::Event::EmergencyStopButtonPressed:
        return CPEvent::EmergencyStopButtonPressed;

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
    case CPEvent::ErrorE:
        return "ErrorE";
    case CPEvent::ErrorDF:
        return "ErrorDF";
    case CPEvent::ErrorRelais:
        return "ErrorRelais";
    case CPEvent::ErrorVentilationNotAvailable:
        return "ErrorVentilationNotAvailable";
    case CPEvent::ErrorOverCurrent:
        return "ErrorOverCurrent";
    case CPEvent::ErrorOverVoltage:
        return "ErrorOverVoltage";
    case CPEvent::ErrorUnderVoltage:
        return "ErrorUnderVoltage";
    case CPEvent::ErrorMotorLock:
        return "ErrorMotorLock";
    case CPEvent::ErrorOverTemperature:
        return "ErrorOverTemperature";
    case CPEvent::ErrorBrownOut:
        return "ErrorBrownOut";
    case CPEvent::ErrorCablePP:
        return "ErrorCablePP";
    case CPEvent::ErrorEnergyManagement:
        return "ErrorEnergyManagement";
    case CPEvent::ErrorNeutralPEN:
        return "ErrorNeutralPEN";
    case CPEvent::ErrorCpDriver:
        return "ErrorCpDriver";
    case CPEvent::EFtoBCD:
        return "EFtoBCD";
    case CPEvent::BCDtoEF:
        return "BCDtoEF";
    case CPEvent::PermanentFault:
        return "PermanentFault";
    case CPEvent::EvseReplugStarted:
        return "EvseReplugStarted";
    case CPEvent::EvseReplugFinished:
        return "EvseReplugFinished";
    }

    throw std::out_of_range("No known string conversion for provided enum of type CPEvent");
}

IECStateMachine::IECStateMachine(const std::unique_ptr<evse_board_supportIntf>& r_bsp) : r_bsp(r_bsp) {
    // feed the state machine whenever the timer expires
    timeout_state_c1.signal_reached.connect(&IECStateMachine::feed_state_machine, this);

    // Subscribe to bsp driver to receive BspEvents from the hardware
    r_bsp->subscribe_event([this](const types::board_support_common::BspEvent event) {
        // feed into state machine
        process_bsp_event(event);
    });
}

void IECStateMachine::process_bsp_event(types::board_support_common::BspEvent bsp_event) {

    auto event_or_error = from_bsp_event(bsp_event.event);

    // If it was a CP change: feed state machine, else forward error
    if (std::holds_alternative<RawCPState>(event_or_error)) {
        {
            std::lock_guard l(state_mutex);
            cp_state = std::get<RawCPState>(event_or_error);
        }
        feed_state_machine();
    } else {
        signal_event(std::get<CPEvent>(event_or_error));
    }
}

void IECStateMachine::feed_state_machine() {
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
    std::lock_guard lock(state_mutex);

    switch (cp_state) {

    case RawCPState::Disabled:
        if (last_cp_state != RawCPState::Disabled) {
            pwm_running = false;
            r_bsp->call_pwm_off();
            ev_simplified_mode = false;
            timeout_state_c1.stop();
            call_allow_power_on_bsp(false);
        }
        break;

    case RawCPState::A:
        if (last_cp_state != RawCPState::A) {
            pwm_running = false;
            r_bsp->call_pwm_off();
            ev_simplified_mode = false;
            call_allow_power_on_bsp(false);
            timeout_state_c1.stop();
        }

        // Table A.6: Sequence 2.1 Unplug at state Bx (or any other
        // state) Table A.6: Sequence 2.2 Unplug at state Cx, Dx
        if (last_cp_state != RawCPState::A && last_cp_state != RawCPState::Disabled) {
            events.push(CPEvent::CarRequestedStopPower);
            events.push(CPEvent::CarUnplugged);
        }
        break;

    case RawCPState::B:
        // Table A.6: Sequence 7 EV stops charging
        // Table A.6: Sequence 8.2 EV supply equipment
        // responds to EV opens S2 (w/o PWM)

        if (last_cp_state != RawCPState::A && last_cp_state != RawCPState::B) {

            events.push(CPEvent::CarRequestedStopPower);
            // Need to switch off according to Table A.6 Sequence 8.1
            // within 100ms
            call_allow_power_on_bsp(false);
            timeout_state_c1.stop();
        }

        // Table A.6: Sequence 1.1 Plug-in
        if (last_cp_state == RawCPState::A || last_cp_state == RawCPState::Disabled) {
            events.push(CPEvent::CarPluggedIn);
            ev_simplified_mode = false;
        }

        if (last_cp_state == RawCPState::E || last_cp_state == RawCPState::F) {
            // Triggers SLAC start
            events.push(CPEvent::EFtoBCD);
        }
        break;

    case RawCPState::D:
        // If state D is not supported, signal an error event here and switch off.
        if (not has_ventilation) {
            call_allow_power_on_bsp(false);
            events.push(CPEvent::ErrorVentilationNotAvailable);
            timeout_state_c1.stop();
            break;
        }
        // no break, intended fall through: If we support state D it is handled the same way as state C
    case RawCPState::C:
        // Table A.6: Sequence 1.2 Plug-in
        if (last_cp_state == RawCPState::A) {
            events.push(CPEvent::CarPluggedIn);
            EVLOG_info << "Detected simplified mode.";
            ev_simplified_mode = true;
        }
        if (last_cp_state == RawCPState::B) {
            events.push(CPEvent::CarRequestedPower);
        }

        if (!pwm_running && last_pwm_running) { // X2->C1
                                                // Table A.6 Sequence 10.2: EV does not stop drawing power
                                                // even if PWM stops. Stop within 6 seconds (E.g. Kona1!)
            timeout_state_c1.start(std::chrono::seconds(6));
        }

        if (timeout_state_c1.reached()) {
            EVLOG_warning << "Timeout of 6 seconds reached, EV did not go back to state B after PWM was switch off. "
                             "Power off under load.";
            // We are still in state C, but the 6 seconds timeout has been reached. Now force power off under load.
            call_allow_power_on_bsp(false);
        }

        if (pwm_running) { // C2
            // 1) When we come from state B: switch on if we are allowed to
            // 2) When we are in C2 for a while now and finally get a delayed power_on_allowed: also switch on
            if (power_on_allowed && (!last_power_on_allowed || last_cp_state == RawCPState::B)) {
                // Table A.6: Sequence 4 EV ready to charge.
                // Must enable power within 3 seconds.
                call_allow_power_on_bsp(true);
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
        if (last_cp_state != RawCPState::E) {
            events.push(CPEvent::ErrorE);
            timeout_state_c1.stop();
            call_allow_power_on_bsp(false);
        }
        break;

    case RawCPState::F:
        timeout_state_c1.stop();
        call_allow_power_on_bsp(false);
        break;
    }

    last_cp_state = cp_state;
    last_pwm_running = pwm_running;
    last_power_on_allowed = power_on_allowed;

    return events;
}

// High level state machine sets PWM duty cycle
void IECStateMachine::set_pwm(double value) {
    {
        std::scoped_lock lock(state_mutex);
        if (value > 0 && value < 1) {
            pwm_running = true;
        } else {
            pwm_running = false;
        }
    }

    r_bsp->call_pwm_on(value * 100);
    feed_state_machine();
}

// High level state machine sets state X1
void IECStateMachine::set_pwm_off() {
    {
        std::scoped_lock lock(state_mutex);
        pwm_running = false;
    }
    r_bsp->call_pwm_off();
    feed_state_machine();
}

// High level state machine sets state F
void IECStateMachine::set_pwm_F() {
    {
        std::scoped_lock lock(state_mutex);
        pwm_running = false;
    }
    r_bsp->call_pwm_F();
    feed_state_machine();
}

// The higher level state machine in Charger.cpp calls this to indicate it allows contactors to be switched on
void IECStateMachine::allow_power_on(bool value, types::evse_board_support::Reason reason) {
    {
        std::scoped_lock lock(state_mutex);
        // Only set the flags here in case of power on.
        power_on_allowed = value;
        power_on_reason = reason;
        // In case of power off, we can directly forward this to the BSP driver here
        if (not power_on_allowed) {
            call_allow_power_on_bsp(false);
        }
    }
    // The actual power on will be handled in the state machine to verify it is in the correct CP state etc.
    feed_state_machine();
}

// Private member function used to actually call the BSP driver's allow_power_on
// No need to lock mutex as this will be called from state machine or locked context only
void IECStateMachine::call_allow_power_on_bsp(bool value) {
    if (not value) {
        power_on_allowed = false;
        power_on_reason = types::evse_board_support::Reason::PowerOff;
    }
    r_bsp->call_allow_power_on({value, power_on_reason});
}

// High level state machine requests reading PP ampacity value.
// We forware this request to the BSP driver. The high level state machine will never call this if it is not used (e.g.
// in DC or AC tethered charging)
double IECStateMachine::read_pp_ampacity() {
    auto a = r_bsp->call_ac_read_pp_ampacity();
    switch (a.ampacity) {
    case types::board_support_common::Ampacity::A_13:
        return 13.;
    case types::board_support_common::Ampacity::A_20:
        return 20.;
    case types::board_support_common::Ampacity::A_32:
        return 32.;
    case types::board_support_common::Ampacity::A_63:
        return 63.;
    default:
        return 0.;
    }
}

// Forward special replug request. Only for testing setups.
void IECStateMachine::evse_replug(int ms) {
    r_bsp->call_evse_replug(ms);
}

// Forward special request to switch the number of phases during charging. BSP will need to implement a special sequence
// to not destroy cars.
void IECStateMachine::switch_three_phases_while_charging(bool n) {
    r_bsp->call_ac_switch_three_phases_while_charging(n);
}

// Forwards config parameters from EvseManager module config to BSP
void IECStateMachine::setup(bool three_phases, bool has_ventilation, std::string country_code) {
    r_bsp->call_setup(three_phases, has_ventilation, country_code);
    this->has_ventilation = has_ventilation;
}

// Force an unlock now. This can be sent from OCPP. As locking/unlocking is handled in the BSP the BSP MCU will need to
// decide how and when to fulfill this request in a safe manner.
bool IECStateMachine::force_unlock() {
    return r_bsp->call_force_unlock();
}

// enable/disable the charging port and CP signal
void IECStateMachine::enable(bool en) {
    r_bsp->call_enable(en);
}

// Forward the over current detection limit to the BSP. Many BSP MCUs monitor the charge current and trigger a fault in
// case of over current. This sets the target charging current value to be used in OC detection. It cannot be derived
// from the PWM duty cycle, use this value instead.
void IECStateMachine::set_overcurrent_limit(double amps) {
    if (amps != last_amps) {
        r_bsp->call_ac_set_overcurrent_limit_A(amps);
        last_amps = amps;
    }
}

} // namespace module
