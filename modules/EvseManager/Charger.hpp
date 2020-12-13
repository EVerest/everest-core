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

#include <queue>
#include <mutex>
#include "utils/thread.hpp"
#include "ld-ev.hpp"
#include <sigslot/signal.hpp>
#include <generated/board_support_AC/Interface.hpp>

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

    // external input to charger
    bool setMaxCurrent(float ampere);
    float getMaxCurrent();
    sigslot::signal<float> signalMaxCurrent;

    void setup(bool three_phases, bool has_ventilation, std::string country_code, bool rcd_enabled);

    bool enable();
    bool disable();
    // switch to next charging session after Finished
    bool restart();

    // Public interface during charging
    //
    // Call anytime, but mostly used during charging session
    //

    // call when in state WaitingForAuthentication
    void Authorize(bool a, const char *userid);
    bool getAuthorization();

    // trigger replug sequence while charging to switch number of phases
    bool switchThreePhasesWhileCharging(bool n);

    bool pauseCharging();
    bool resumeCharging();
    bool cancelCharging();
    bool getPausedByEVSE();

    void setCurrentDrawnByVehicle(float l1, float l2, float l3);

    //float getResidualCurrent();
    //bool isPowerOn();

    // Public states for Hi Level
    enum class EvseState {
        Disabled,
        Idle,
        WaitingForAuthentication,
        Charging,
        ChargingPausedEV,
        ChargingPausedEVSE,
        Finished,
        Error,
        Faulted
    };

    enum class ErrorState {
        Error_E,
        Error_F,
        Error_DF,
        Error_Relais,
        Error_VentilationNotAvailable,
        Error_RCD,
        Error_OverCurrent
    };

    // Logical states for Hi Level
    EvseState getCurrentState();
    sigslot::signal<EvseState> signalState;

    ErrorState getErrorState();
    sigslot::signal<ErrorState> signalError;

    void processEvent(std::string event);

    void run();

    std::string evseStateToString(EvseState s);
    std::string errorStateToString(ErrorState s);

private:
    // main Charger thread
    Everest::Thread mainThreadHandle;

    const std::unique_ptr<board_support_ACIntf>& r_bsp;

    void mainThread();

    float maxCurrent;

    // This mutex locks all config type members
    std::mutex configMutex;

    // This mutex locks all state type members
    std::mutex stateMutex;

    EvseState currentState;
    EvseState lastState;
    ErrorState errorState;

    float ampereToDutyCycle(float ampere);

    void checkSoftOverCurrent();
    float currentDrawnByVehicle[3];
    bool overCurrent;
    std::chrono::system_clock::time_point lastOverCurrentEvent;
    const int softOverCurrentTimeout = 7000;

    ControlPilotEvent string_to_control_pilot_event(std::string event);

    void ISO_IEC_Coordination();

    void processCPEventsIndependent(ControlPilotEvent cp_event);
    void processCPEventsState(ControlPilotEvent cp_event);
    void runStateMachine();

    bool authorized;
    bool cancelled;

    std::chrono::system_clock::time_point lastPwmUpdate;
    
    float update_pwm_last_dc;
    void update_pwm_now(float dc);
    void update_pwm_max_every_5seconds(float dc);
};

}

#endif // SRC_EVDRIVERS_CHARGER_H_
