// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest

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
    EvseReplugStarted,
    EvseReplugFinished,
    ErrorE,
    ErrorDF,
    ErrorVentilationNotAvailable,
    ErrorBrownOut,
    ErrorEnergyManagement,
    PermanentFault,
    MREC_2_GroundFailure,
    MREC_3_HighTemperature,
    MREC_4_OverCurrentFailure,
    MREC_5_OverVoltage,
    MREC_6_UnderVoltage,
    MREC_8_EmergencyStop,
    MREC_10_InvalidVehicleMode,
    MREC_14_PilotFault,
    MREC_15_PowerLoss,
    MREC_17_EVSEContactorFault,
    MREC_18_CableOverTempDerate,
    MREC_19_CableOverTempStop,
    MREC_20_PartialInsertion,
    MREC_23_ProximityFault,
    MREC_24_ConnectorVoltageHigh,
    MREC_25_BrokenLatch,
    MREC_26_CutCable,
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
    explicit IECStateMachine(const std::unique_ptr<evse_board_supportIntf>& r_bsp);

    // Call when new events from BSP requirement come in. Will signal internal events
    void process_bsp_event(types::board_support_common::BspEvent bsp_event);
    // Allow power on from Charger state machine
    void allow_power_on(bool value, types::evse_board_support::Reason reason);

    double read_pp_ampacity();
    void evse_replug(int ms);
    void switch_three_phases_while_charging(bool n);
    void setup(bool three_phases, bool has_ventilation, std::string country_code);
    bool force_unlock();
    void set_overcurrent_limit(double amps);

    void set_pwm(double value);
    void set_pwm_off();
    void set_pwm_F();

    void enable(bool en);

    // Signal for internal events type
    sigslot::signal<CPEvent> signal_event;

private:
    const std::unique_ptr<evse_board_supportIntf>& r_bsp;

    bool pwm_running{false};
    bool last_pwm_running{false};

    bool ev_simplified_mode{false};
    bool has_ventilation{false};
    bool power_on_allowed{false};
    bool last_power_on_allowed{false};
    std::atomic<double> last_amps{-1};

    RawCPState cp_state{RawCPState::Disabled}, last_cp_state{RawCPState::Disabled};
    AsyncTimeout timeout_state_c1;

    std::mutex state_mutex;
    void feed_state_machine();
    std::queue<CPEvent> state_machine();

    types::evse_board_support::Reason power_on_reason{types::evse_board_support::Reason::PowerOff};
    void call_allow_power_on_bsp(bool value);

    std::atomic_bool three_phases{true};
};

} // namespace module

#endif // SRC_BSP_STATE_MACHINE_H_
