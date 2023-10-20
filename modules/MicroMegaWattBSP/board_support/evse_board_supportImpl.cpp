// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "evse_board_supportImpl.hpp"

namespace module {
namespace board_support {

/*


static types::board_support::Event cast_event_type(const Event& e) {
switch (e.type) {
case Event_InterfaceEvent_CAR_PLUGGED_IN:
    return types::board_support::Event::CarPluggedIn;
case Event_InterfaceEvent_CAR_REQUESTED_POWER:
    return types::board_support::Event::CarRequestedPower;
case Event_InterfaceEvent_POWER_ON:
    return types::board_support::Event::PowerOn;
case Event_InterfaceEvent_POWER_OFF:
    return types::board_support::Event::PowerOff;
case Event_InterfaceEvent_CAR_REQUESTED_STOP_POWER:
    return types::board_support::Event::CarRequestedStopPower;
case Event_InterfaceEvent_CAR_UNPLUGGED:
    return types::board_support::Event::CarUnplugged;
case Event_InterfaceEvent_ERROR_E:
    return types::board_support::Event::ErrorE;
case Event_InterfaceEvent_ERROR_DF:
    return types::board_support::Event::ErrorDF;
case Event_InterfaceEvent_ERROR_RELAIS:
    return types::board_support::Event::ErrorRelais;
case Event_InterfaceEvent_ERROR_RCD:
    return types::board_support::Event::ErrorRCD;
case Event_InterfaceEvent_ERROR_VENTILATION_NOT_AVAILABLE:
    return types::board_support::Event::ErrorVentilationNotAvailable;
case Event_InterfaceEvent_ERROR_OVER_CURRENT:
    return types::board_support::Event::ErrorOverCurrent;
case Event_InterfaceEvent_ENTER_BCD:
    return types::board_support::Event::EFtoBCD;
case Event_InterfaceEvent_LEAVE_BCD:
    return types::board_support::Event::BCDtoEF;
case Event_InterfaceEvent_PERMANENT_FAULT:
    return types::board_support::Event::PermanentFault;
case Event_InterfaceEvent_EVSE_REPLUG_STARTED:
    return types::board_support::Event::EvseReplugStarted;
case Event_InterfaceEvent_EVSE_REPLUG_FINISHED:
    return types::board_support::Event::EvseReplugFinished;
}

EVLOG_error << "Received an unknown interface event from uMWC: " << (int)e.type;
return types::board_support::Event::ErrorVentilationNotAvailable;
}
*/

void evse_board_supportImpl::init() {
    {
        std::lock_guard<std::mutex> lock(capsMutex);

        caps.min_current_A_import = 0;
        caps.max_current_A_import = 6;
        caps.min_phase_count_import = 1;
        caps.max_phase_count_import = 3;
        caps.supports_changing_phases_during_charging = false;

        caps.min_current_A_export = 0;
        caps.max_current_A_export = 6;
        caps.min_phase_count_export = 1;
        caps.max_phase_count_export = 3;
    }

    /*   mod->serial.signalEvent.connect([this](Event e) {
           EVLOG_info << "CP EVENT: " << types::board_support::event_to_string(cast_event_type(e));
           publish_event(cast_event_type(e));
       });*/
}

void evse_board_supportImpl::ready() {
}

void evse_board_supportImpl::handle_setup(bool& three_phases, bool& has_ventilation, std::string& country_code) {
    // your code for cmd setup goes here
}

types::evse_board_support::HardwareCapabilities evse_board_supportImpl::handle_get_hw_capabilities() {
    std::lock_guard<std::mutex> lock(capsMutex);
    return caps;
}

void evse_board_supportImpl::handle_enable(bool& value) {
    if (value)
        mod->serial.enable();
    else
        mod->serial.disable();
}

void evse_board_supportImpl::handle_pwm_on(double& value) {
    mod->serial.setPWM(1, value);
}

void evse_board_supportImpl::handle_pwm_off() {
    mod->serial.setPWM(0, 0.);
}

void evse_board_supportImpl::handle_pwm_F() {
    mod->serial.setPWM(2, 0.);
}

void evse_board_supportImpl::handle_allow_power_on(types::evse_board_support::PowerOnOff& value) {
    mod->serial.allowPowerOn(value.allow_power_on);
}

void evse_board_supportImpl::handle_ac_switch_three_phases_while_charging(bool& value) {
    // your code for cmd ac_switch_three_phases_while_charging goes here
}

void evse_board_supportImpl::handle_evse_replug(int& value) {
    mod->serial.replug(value);
}

types::board_support_common::ProximityPilot evse_board_supportImpl::handle_ac_read_pp_ampacity() {
    return {types::board_support_common::Ampacity::A_13};
}

void evse_board_supportImpl::handle_ac_set_overcurrent_limit_A(double& value) {
    // your code for cmd ac_set_overcurrent_limit_A goes here
}

} // namespace board_support
} // namespace module
