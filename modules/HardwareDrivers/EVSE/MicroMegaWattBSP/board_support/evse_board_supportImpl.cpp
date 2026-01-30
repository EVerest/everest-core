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
        caps.supports_cp_state_E = false;
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

            if (cp_state == CpState::CpState_STATE_A) {
                if (error_state_monitor->is_error_active("evse_board_support/MREC8EmergencyStop", "")) {
                    clear_error("evse_board_support/MREC8EmergencyStop");
                }
            }
        }
    });

    mod->serial.signalRelaisState.connect([this](bool relais_state) {
        if (last_relais_state not_eq relais_state) {
            publish_event(cast_event_type(relais_state));
            last_relais_state = relais_state;
        }
    });

    mod->mqtt.subscribe(fmt::format("everest_external/nodered/{}/cmd/emergency_stop", mod->config.connector_id),
                        [this](const std::string& data) {
                            types::evse_manager::StopTransactionRequest request;
                            request.reason = types::evse_manager::StopTransactionReason::EmergencyStop;
                            mod->p_board_support->publish_request_stop_transaction(request);
                            Everest::error::Error error_object = error_factory->create_error(
                                "evse_board_support/MREC8EmergencyStop", "", "Emergency stop button pushed by user",
                                Everest::error::Severity::High);
                            raise_error(error_object);
                        });

    mod->mqtt.subscribe(fmt::format("everest_external/nodered/{}/cmd/evse_utility_int", mod->config.connector_id),
                        [this](const std::string& data) {
                            types::evse_manager::StopTransactionRequest request;
                            request.reason = types::evse_manager::StopTransactionReason::PowerLoss;
                            mod->p_board_support->publish_request_stop_transaction(request);
                        });

    mod->mqtt.subscribe(fmt::format("everest_external/nodered/{}/cmd/stop_transaction", mod->config.connector_id),
                        [this](const std::string& data) {
                            types::evse_manager::StopTransactionRequest request;
                            request.reason = types::evse_manager::StopTransactionReason::Local;
                            mod->p_board_support->publish_request_stop_transaction(request);
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

void evse_board_supportImpl::handle_cp_state_X1() {
    mod->serial.setPWM(10001.);
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

void evse_board_supportImpl::handle_ac_switch_three_phases_while_charging(bool& value) {
    // your code for cmd ac_switch_three_phases_while_charging goes here
}

void evse_board_supportImpl::handle_evse_replug(int& value) {
    mod->serial.replug();
}

void evse_board_supportImpl::handle_ac_set_overcurrent_limit_A(double& value) {
    // your code for cmd ac_set_overcurrent_limit_A goes here
}

} // namespace board_support
} // namespace module
