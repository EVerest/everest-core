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

void evse_board_supportImpl::init() {
    {
        std::scoped_lock lock(capsMutex);

        caps.min_current_A_import = 0;
        caps.max_current_A_import = 100;
        caps.min_phase_count_import = 1;
        caps.max_phase_count_import = 3;
        caps.supports_changing_phases_during_charging = false;
        caps.connector_type = types::evse_board_support::Connector_type::IEC62196Type2Cable;

        caps.min_current_A_export = 0;
        caps.max_current_A_export = 100;
        caps.min_phase_count_export = 1;
        caps.max_phase_count_export = 3;
    }

    mod->serial.signalKeepAliveLo.connect([this](KeepAliveLo l) {
        if (not keep_alive_printed) {
            EVLOG_info << "uMWC Controller Configuration:";
            EVLOG_info << "  Hardware revision: " << l.hw_revision;
            EVLOG_info << "  Firmware version: " << l.sw_version_string;
        }
        keep_alive_printed = true;
    });

    mod->serial.signalCPState.connect([this](CpState cp_state) {
        if (cp_state not_eq last_cp_state) {
            auto event_cp_state = cast_event_type(cp_state);
            EVLOG_info << "CP state changed: " << types::board_support_common::event_to_string(event_cp_state.event);
            publish_event(event_cp_state);
            last_cp_state = cp_state;
        }
    });
    mod->serial.signalRelaisState.connect([this](bool relais_state) {
        if (last_relais_state not_eq relais_state) {
            publish_event(cast_event_type(relais_state));
            last_relais_state = relais_state;
        }
    });
}

void evse_board_supportImpl::ready() {
    {
        // Publish caps once in the beginning
        std::scoped_lock lock(capsMutex);
        publish_capabilities(caps);
    }
}

void evse_board_supportImpl::handle_enable(bool& value) {
    mod->serial.enable(value);
}

void evse_board_supportImpl::handle_pwm_on(double& value) {
    mod->serial.setPWM(value * 100);
}

void evse_board_supportImpl::handle_pwm_off() {
    mod->serial.setPWM(10001.);
}

void evse_board_supportImpl::handle_pwm_F() {
    mod->serial.setPWM(0);
}

void evse_board_supportImpl::handle_allow_power_on(types::evse_board_support::PowerOnOff& value) {
    mod->serial.allowPowerOn(value.allow_power_on);
}

void evse_board_supportImpl::handle_ac_switch_three_phases_while_charging(bool& value) {
    // your code for cmd ac_switch_three_phases_while_charging goes here
}

void evse_board_supportImpl::handle_evse_replug(int& value) {
    mod->serial.replug();
}

types::board_support_common::ProximityPilot evse_board_supportImpl::handle_ac_read_pp_ampacity() {
    return {types::board_support_common::Ampacity::A_13};
}

void evse_board_supportImpl::handle_ac_set_overcurrent_limit_A(double& value) {
    // your code for cmd ac_set_overcurrent_limit_A goes here
}

void evse_board_supportImpl::handle_ce_on() {
    // your code for cmd ce_on goes here
}

void evse_board_supportImpl::handle_ce_off() {
    // your code for cmd ce_off goes here
}

} // namespace board_support
} // namespace module
