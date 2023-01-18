// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "board_support_ACImpl.hpp"

namespace module {
namespace board_support {

types::board_support::Event cast_event_type(const Event& e) {
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

    EVLOG_AND_THROW(Everest::EverestConfigError("Received an unknown interface event from Yeti"));
}

void board_support_ACImpl::init() {
    {
        std::lock_guard<std::mutex> lock(capsMutex);

        caps.min_current_A = 6;
        caps.max_current_A = 6;
        caps.min_phase_count = 1;
        caps.max_phase_count = 3;
        caps.supports_changing_phases_during_charging = false;
    }

    mod->serial.signalEvent.connect([this](Event e) { publish_event(cast_event_type(e)); });

    // FIXME
    // Everything used here should be moved out of debug update in protobuf
    mod->serial.signalDebugUpdate.connect([this](DebugUpdate d) {
        publish_nr_of_phases_available((d.use_three_phases ? 3 : 1));

        types::board_support::Telemetry telemetry;
        telemetry.temperature = d.cpu_temperature;
        telemetry.fan_rpm = 0.;
        telemetry.supply_voltage_12V = d.supply_voltage_12V;
        telemetry.supply_voltage_minus_12V = d.supply_voltage_N12V;
        telemetry.rcd_current = d.rcd_current;
        telemetry.relais_on = d.relais_on;

        publish_telemetry(telemetry);
    });

    mod->serial.signalKeepAliveLo.connect([this](KeepAliveLo l) {
        std::lock_guard<std::mutex> lock(capsMutex);

        caps.min_current_A = l.hwcap_min_current;
        caps.max_current_A = l.hwcap_max_current;
        caps.min_phase_count = l.hwcap_min_phase_count;
        caps.max_phase_count = l.hwcap_max_phase_count;
        caps.supports_changing_phases_during_charging = l.supports_changing_phases_during_charging;
    });
} // namespace board_support

void board_support_ACImpl::ready() {
}

void board_support_ACImpl::handle_setup(bool& three_phases, bool& has_ventilation, std::string& country_code,
                                        bool& rcd_enabled) {
    mod->serial.setCountryCode(country_code.c_str());
    mod->serial.setHasVentilation(has_ventilation);
    mod->serial.setThreePhases(three_phases);
    mod->serial.enableRCD(rcd_enabled);
};

void board_support_ACImpl::handle_enable(bool& value) {
    if (value)
        mod->serial.enable();
    else
        mod->serial.disable();
};

void board_support_ACImpl::handle_pwm_on(double& value) {
    mod->serial.setPWM(1, value);
};

void board_support_ACImpl::handle_pwm_off() {
    mod->serial.setPWM(0, 0.);
};

void board_support_ACImpl::handle_pwm_F() {
    mod->serial.setPWM(2, 0.);
};

void board_support_ACImpl::handle_allow_power_on(bool& value) {
    mod->serial.allowPowerOn(value);
};

bool board_support_ACImpl::handle_force_unlock() {
    return mod->serial.forceUnlock();
};

void board_support_ACImpl::handle_switch_three_phases_while_charging(bool& value) {
    mod->serial.switchThreePhasesWhileCharging(value);
};

void board_support_ACImpl::handle_evse_replug(int& value) {
    mod->serial.replug(value);
};

types::board_support::HardwareCapabilities board_support_ACImpl::handle_get_hw_capabilities() {
    std::lock_guard<std::mutex> lock(capsMutex);
    return caps;
};

} // namespace board_support
} // namespace module
