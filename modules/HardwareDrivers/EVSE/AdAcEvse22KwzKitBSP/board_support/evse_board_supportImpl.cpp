// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
// Portions (c) 2025 Analog Devices Inc.
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

        caps.min_current_A_import = 6;
        caps.max_current_A_import = 16;
        caps.min_phase_count_import = 1;
        caps.max_phase_count_import = 3;
        caps.supports_changing_phases_during_charging = false;
        caps.supports_cp_state_E = false;
        caps.connector_type = types::evse_board_support::Connector_type::IEC62196Type2Cable;

        caps.min_current_A_export = 6;
        caps.max_current_A_export = 16;
        caps.min_phase_count_export = 1;
        caps.max_phase_count_export = 3;
    }

    mod->serial.signalCPState.connect([this](CpState cp_state) {
        if (cp_state not_eq last_cp_state) {
            auto event_cp_state = cast_event_type(cp_state);
            EVLOG_info << "CP state changed: "
                       << types::board_support_common::event_to_string(cast_event_type(last_cp_state).event) << " -> "
                       << types::board_support_common::event_to_string(event_cp_state.event);
            if (enabled) {
                publish_event(event_cp_state);
            }

            if (cp_state == CpState_STATE_A) {
                mod->clear_errors_on_unplug();
            }
            last_cp_state = cp_state;
        }
    });
    mod->serial.signalRelaisState.connect([this](bool relais_state) {
        if (last_relais_state not_eq relais_state) {
            publish_event(cast_event_type(relais_state));
            last_relais_state = relais_state;
        }
    });

    mod->serial.signalPPState.connect([this](PpState pp_state) {
        last_pp = cast_pp_type(pp_state);
        publish_ac_pp_ampacity(last_pp);
    });

    mod->serial.signalKeepAliveLo.connect([this](KeepAliveLo l) {
        std::lock_guard<std::mutex> lock(capsMutex);

        caps.min_current_A_import =
            (mod->config.caps_min_current_A >= 0 ? mod->config.caps_min_current_A : l.hwcap_min_current);
        caps.max_current_A_import =
            (mod->config.caps_max_current_A >= 0 ? mod->config.caps_max_current_A : l.hwcap_max_current);
        caps.min_phase_count_import = l.hwcap_min_phase_count;
        caps.max_phase_count_import = l.hwcap_max_phase_count;

        caps.min_current_A_export =
            (mod->config.caps_min_current_A >= 0 ? mod->config.caps_min_current_A : l.hwcap_min_current);
        caps.max_current_A_export =
            (mod->config.caps_max_current_A >= 0 ? mod->config.caps_max_current_A : l.hwcap_max_current);
        caps.min_phase_count_export = l.hwcap_min_phase_count;
        caps.max_phase_count_export = l.hwcap_max_phase_count;

        caps.supports_changing_phases_during_charging = l.supports_changing_phases_during_charging;
        if (not caps_received) {
            EVLOG_info << "AD-ACEVSE22KWZ-KIT Configuration:";
            EVLOG_info << "  Hardware revision: " << l.hw_revision;
            EVLOG_info << "  Firmware version: " << l.sw_version_string;
            EVLOG_info << "  Current Limit: " << l.hwcap_max_current;
        }
        caps_received = true;
    });
}

void evse_board_supportImpl::ready() {
    wait_for_caps();
    {
        // Publish caps once in the beginning
        std::lock_guard<std::mutex> lock(capsMutex);
        publish_capabilities(caps);
    }
}

void evse_board_supportImpl::wait_for_caps() {
    // Wait for caps to be received at least once
    int i;
    for (i = 0; i < 50; i++) {
        if (caps_received)
            break;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    if (i == 50) {
        EVLOG_error << "Did not receive hardware capabilities from AD-ACEVSE22KWZ-KIT hardware, using defaults.";
    }
}

void evse_board_supportImpl::handle_pwm_on(double& value) {
    mod->serial.setPWM(value * 100);
}

void evse_board_supportImpl::handle_cp_state_X1() {
    mod->serial.setPWM(10001);
}

void evse_board_supportImpl::handle_cp_state_F() {
    mod->serial.setPWM(0);
}

void evse_board_supportImpl::handle_cp_state_E() {
    EVLOG_warning << "Command cp_state_E is not supported. Ignoring command.";
}

void evse_board_supportImpl::handle_allow_power_on(types::evse_board_support::PowerOnOff& value) {
    mod->serial.allowPowerOn(value.allow_power_on);
}

void evse_board_supportImpl::handle_ac_set_overcurrent_limit_A(double& value) {
    // your code for cmd ac_set_overcurrent_limit_A goes here
}

void evse_board_supportImpl::handle_ac_switch_three_phases_while_charging(bool& value) {
    EVLOG_warning << "AdAdEvse22KwzKitBSP doesn't support ac_switch_three_phases_while_charging, ignoring command.";
}

void evse_board_supportImpl::handle_evse_replug(int& value) {
    EVLOG_warning << "AdAdEvse22KwzKitBSP doesn't support evse_replug, ignoring command.";
}

void evse_board_supportImpl::handle_enable(bool& value) {
    enabled = true;
    // Publish CP state once on enable
    publish_event(cast_event_type(last_cp_state));
}

} // namespace board_support
} // namespace module
