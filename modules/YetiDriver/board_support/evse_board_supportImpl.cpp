// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "evse_board_supportImpl.hpp"

namespace module {
namespace board_support {

static types::board_support_common::BspEvent cast_event_type(CpState cp_state) {
    types::board_support_common::BspEvent event;
    switch (cp_state) {
    case CpState_STATE_A:
        event.event = types::board_support_common::Event::A;
        break;
    case CpState_STATE_B:
        event.event = types::board_support_common::Event::B;
        break;
    case CpState_STATE_C:
        event.event = types::board_support_common::Event::C;
        break;
    case CpState_STATE_D:
        event.event = types::board_support_common::Event::D;
        break;
    case CpState_STATE_E:
        event.event = types::board_support_common::Event::E;
        break;
    case CpState_STATE_F:
        event.event = types::board_support_common::Event::F;
        break;
    }
    return event;
}

static types::board_support_common::BspEvent cast_event_type(bool relais_state) {
    types::board_support_common::BspEvent event;
    if (relais_state) {
        event.event = types::board_support_common::Event::PowerOn;
    } else {
        event.event = types::board_support_common::Event::PowerOff;
    }
    return event;
}

static types::board_support_common::ProximityPilot cast_pp_type(PpState pp_state) {
    types::board_support_common::ProximityPilot pp;
    switch (pp_state) {
    case PpState_STATE_13A:
        pp.ampacity = types::board_support_common::Ampacity::A_13;
        break;
    case PpState_STATE_20A:
        pp.ampacity = types::board_support_common::Ampacity::A_20;
        break;
    case PpState_STATE_32A:
        pp.ampacity = types::board_support_common::Ampacity::A_32;
        break;
    case PpState_STATE_70A:
        pp.ampacity = types::board_support_common::Ampacity::A_63_3ph_70_1ph;
        break;
    case PpState_STATE_FAULT:
        pp.ampacity = types::board_support_common::Ampacity::None;
        break;
    case PpState_STATE_NC:
        pp.ampacity = types::board_support_common::Ampacity::None;
        break;
    }
    return pp;
}

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

    mod->serial.signalCPState.connect([this](CpState cp_state) {
        publish_event(cast_event_type(cp_state));

        if (cp_state == CpState_STATE_A) {
            mod->clear_errors_on_unplug();
        }
    });
    mod->serial.signalRelaisState.connect([this](bool relais_state) { publish_event(cast_event_type(relais_state)); });
    mod->serial.signalPPState.connect([this](PpState pp_state) {
        std::lock_guard<std::mutex> lock(capsMutex);
        last_pp = cast_pp_type(pp_state);
        publish_ac_pp_ampacity(last_pp);
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

types::evse_board_support::HardwareCapabilities evse_board_supportImpl::handle_get_hw_capabilities() {
    std::lock_guard<std::mutex> lock(capsMutex);
    return caps;
}

void evse_board_supportImpl::handle_pwm_on(double& value) {
    mod->serial.setPWM(value * 100);
}

void evse_board_supportImpl::handle_pwm_off() {
    mod->serial.setPWM(10001);
}

void evse_board_supportImpl::handle_pwm_F() {
    mod->serial.setPWM(0);
}

void evse_board_supportImpl::handle_allow_power_on(types::evse_board_support::PowerOnOff& value) {
    mod->serial.allowPowerOn(value.allow_power_on);
}

types::board_support_common::ProximityPilot evse_board_supportImpl::handle_ac_read_pp_ampacity() {
    // FIXME: read PP ampacity from yeti, report back maximum current the hardware can handle for now
    std::lock_guard<std::mutex> lock(capsMutex);
    return last_pp;
}

void evse_board_supportImpl::handle_ac_set_overcurrent_limit_A(double& value) {
    // your code for cmd ac_set_overcurrent_limit_A goes here
}

void evse_board_supportImpl::handle_ac_switch_three_phases_while_charging(bool& value) {
}

void evse_board_supportImpl::handle_evse_replug(int& value) {
}

void evse_board_supportImpl::handle_enable(bool& value) {
    // Query CP state once and publish
}

void evse_board_supportImpl::handle_setup(bool& three_phases, bool& has_ventilation, std::string& country_code) {
}

} // namespace board_support
} // namespace module
