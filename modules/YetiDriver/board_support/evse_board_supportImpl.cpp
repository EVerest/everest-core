// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "evse_board_supportImpl.hpp"

namespace module {
namespace board_support {
/*
    types::board_support_common::Event cast_event_type(const Event& e) {
    switch (e.type) {
    case Event_InterfaceEvent_CAR_PLUGGED_IN:
        return types::board_support_common::Event::CarPluggedIn;
    case Event_InterfaceEvent_CAR_REQUESTED_POWER:
        return types::board_support_common::Event::CarRequestedPower;
    case Event_InterfaceEvent_POWER_ON:
        return types::board_support_common::Event::PowerOn;
    case Event_InterfaceEvent_POWER_OFF:
        return types::board_support_common::Event::PowerOff;
    case Event_InterfaceEvent_CAR_REQUESTED_STOP_POWER:
        return types::board_support_common::Event::CarRequestedStopPower;
    case Event_InterfaceEvent_CAR_UNPLUGGED:
        return types::board_support_common::Event::CarUnplugged;
    case Event_InterfaceEvent_ERROR_E:
        return types::board_support_common::Event::ErrorE;
    case Event_InterfaceEvent_ERROR_DF:
        return types::board_support_common::Event::ErrorDF;
    case Event_InterfaceEvent_ERROR_RELAIS:
        return types::board_support_common::Event::ErrorRelais;
    case Event_InterfaceEvent_ERROR_RCD:
        return types::board_support_common::Event::ErrorRCD;
    case Event_InterfaceEvent_ERROR_VENTILATION_NOT_AVAILABLE:
        return types::board_support_common::Event::ErrorVentilationNotAvailable;
    case Event_InterfaceEvent_ERROR_OVER_CURRENT:
        return types::board_support_common::Event::ErrorOverCurrent;
    case Event_InterfaceEvent_ENTER_BCD:
        return types::board_support_common::Event::EFtoBCD;
    case Event_InterfaceEvent_LEAVE_BCD:
        return types::board_support_common::Event::BCDtoEF;
    case Event_InterfaceEvent_PERMANENT_FAULT:
        return types::board_support_common::Event::PermanentFault;
    case Event_InterfaceEvent_EVSE_REPLUG_STARTED:
        return types::board_support_common::Event::EvseReplugStarted;
    case Event_InterfaceEvent_EVSE_REPLUG_FINISHED:
        return types::board_support_common::Event::EvseReplugFinished;
    }

    EVLOG_AND_THROW(Everest::EverestConfigError("Received an unknown interface event from Yeti"));

}*/

void evse_board_supportImpl::init() {
    {
        std::lock_guard<std::mutex> lock(capsMutex);

        caps.min_current_A_import = mod->config.caps_min_current_A;
        caps.max_current_A_import = 6;
        caps.min_phase_count_import = 1;
        caps.max_phase_count_import = 3;
        caps.supports_changing_phases_during_charging = false;

        caps.min_current_A_export = mod->config.caps_min_current_A;
        caps.max_current_A_export = 6;
        caps.min_phase_count_export = 1;
        caps.max_phase_count_export = 3;
        caps.supports_changing_phases_during_charging = false;
    }

    mod->serial.signalEvent.connect([this](Event e) { /*publish_event(cast_event_type(e));*/ });

    // FIXME
    // Everything used here should be moved out of debug update in protobuf
    mod->serial.signalDebugUpdate.connect([this](DebugUpdate d) {
        publish_ac_nr_of_phases_available((d.use_three_phases ? 3 : 1));

        types::evse_board_support::Telemetry telemetry;
        telemetry.evse_temperature_C = d.cpu_temperature;
        telemetry.fan_rpm = 0.;
        telemetry.supply_voltage_12V = d.supply_voltage_12V;
        telemetry.supply_voltage_minus_12V = d.supply_voltage_N12V;
        // FIXME        telemetry.rcd_current = d.rcd_current;
        telemetry.relais_on = d.relais_on;

        publish_telemetry(telemetry);
    });

    mod->serial.signalKeepAliveLo.connect([this](KeepAliveLo l) {
        std::lock_guard<std::mutex> lock(capsMutex);

        caps.min_current_A_import =
            (mod->config.caps_min_current_A >= 0 ? mod->config.caps_min_current_A : l.hwcap_min_current);
        caps.max_current_A_import = l.hwcap_max_current;
        caps.min_phase_count_import = l.hwcap_min_phase_count;
        caps.max_phase_count_import = l.hwcap_max_phase_count;

        caps.min_current_A_export =
            (mod->config.caps_min_current_A >= 0 ? mod->config.caps_min_current_A : l.hwcap_min_current);
        caps.max_current_A_export = l.hwcap_max_current;
        caps.min_phase_count_export = l.hwcap_min_phase_count;
        caps.max_phase_count_export = l.hwcap_max_phase_count;

        caps.supports_changing_phases_during_charging = l.supports_changing_phases_during_charging;
        caps_received = true;
    });
}

void evse_board_supportImpl::ready() {
    // Wait for caps to be received at least once
    int i;
    for (i = 0; i < 50; i++) {
        {
            std::lock_guard<std::mutex> lock(capsMutex);
            if (caps_received)
                break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    if (i == 49) {
        EVLOG_AND_THROW(
            Everest::EverestTimeoutError("Did not receive hardware capabilities from Yeti hardware, exiting."));
    }
}

void evse_board_supportImpl::handle_setup(bool& three_phases, bool& has_ventilation, std::string& country_code) {
    mod->serial.setCountryCode(country_code.c_str());
    mod->serial.setHasVentilation(has_ventilation);
    mod->serial.setThreePhases(three_phases);
    // mod->serial.enableRCD(rcd_enabled);
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
    mod->serial.switchThreePhasesWhileCharging(value);
}

void evse_board_supportImpl::handle_evse_replug(int& value) {
    mod->serial.replug(value);
}

types::board_support_common::ProximityPilot evse_board_supportImpl::handle_ac_read_pp_ampacity() {
    // FIXME: read PP ampacity from yeti, report back maximum current the hardware can handle for now
    std::lock_guard<std::mutex> lock(capsMutex);
    return {types::board_support_common::Ampacity::A_32};
}

void evse_board_supportImpl::handle_ac_set_overcurrent_limit_A(double& value) {
    // your code for cmd ac_set_overcurrent_limit_A goes here
}

} // namespace board_support
} // namespace module
