#include "board_support_ACImpl.hpp"

namespace module {
namespace board_support {

std::string event_to_string(Event e) {
    switch (e.type) {
    case Event_InterfaceEvent_CAR_PLUGGED_IN:
        return "CarPluggedIn";
    case Event_InterfaceEvent_CAR_REQUESTED_POWER:
        return "CarRequestedPower";
    case Event_InterfaceEvent_POWER_ON:
        return "PowerOn";
    case Event_InterfaceEvent_POWER_OFF:
        return "PowerOff";
    case Event_InterfaceEvent_CAR_REQUESTED_STOP_POWER:
        return "CarRequestedStopPower";
    case Event_InterfaceEvent_CAR_UNPLUGGED:
        return "CarUnplugged";
    case Event_InterfaceEvent_ERROR_E:
        return "ErrorE";
    case Event_InterfaceEvent_ERROR_DF:
        return "ErrorDF";
    case Event_InterfaceEvent_ERROR_RELAIS:
        return "ErrorRelais";
    case Event_InterfaceEvent_ERROR_RCD:
        return "ErrorRCD";
    case Event_InterfaceEvent_ERROR_VENTILATION_NOT_AVAILABLE:
        return "ErrorVentilationNotAvailable";
    case Event_InterfaceEvent_ERROR_OVER_CURRENT:
        return "ErrorOverCurrent";
    case Event_InterfaceEvent_RESTART_MATCHING:
        return "RestartMatching";
    case Event_InterfaceEvent_PERMANENT_FAULT:
        return "PermanentFault";
    }
    return "";
}

void board_support_ACImpl::init() {
    mod->serial.signalEvent.connect([this](Event e) { publish_event(event_to_string(e)); });
}

void board_support_ACImpl::ready() {

}

void board_support_ACImpl::handle_setup(bool& three_phases, bool& has_ventilation, std::string& country_code, bool& rcd_enabled){
    mod->serial.setCountryCode(country_code.c_str());
    mod->serial.setHasVentilation(has_ventilation);
    mod->serial.setThreePhases(three_phases);
    mod->serial.enableRCD(rcd_enabled);
};

void board_support_ACImpl::handle_enable(bool& value){
    if (value) mod->serial.enable();
    else mod->serial.disable();
};

void board_support_ACImpl::handle_pwm_on(double& value){
    mod->serial.setPWM(1,value);
};

void board_support_ACImpl::handle_pwm_off(){
    mod->serial.setPWM(0,0.);
};

void board_support_ACImpl::handle_pwm_F(){
    mod->serial.setPWM(2,0.);
};

void board_support_ACImpl::handle_allow_power_on(bool& value){
    mod->serial.allowPowerOn(value);
};

void board_support_ACImpl::handle_force_unlock(){
    mod->serial.forceUnlock();
};

void board_support_ACImpl::handle_switch_three_phases_while_charging(bool& value){
    mod->serial.switchThreePhasesWhileCharging(value);
};

} // namespace board_support
} // namespace module
