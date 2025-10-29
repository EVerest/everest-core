// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "everest/io/mqtt/dataset.hpp"
#include "protocol/cb_common.h"
#include "protocol/evse_bsp_cb_to_host.h"
#include <charge_bridge/everest_api/ovm_api.hpp>
#include <charge_bridge/utilities/logging.hpp>
#include <charge_bridge/utilities/string.hpp>
#include <chrono>
#include <cstring>
#include <everest_api_types/generic/codec.hpp>
#include <everest_api_types/over_voltage_monitor/API.hpp>
#include <everest_api_types/over_voltage_monitor/codec.hpp>
#include <everest_api_types/utilities/codec.hpp>

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

using namespace std::chrono_literals;
using namespace everest::lib::API::V1_0::types::generic;

namespace charge_bridge::evse_bsp {

ovm_api::ovm_api(evse_ovm_config const& config, std::string const& cb_identifier, evse_bsp_host_to_cb& host_status) :
    host_status(host_status), m_cb_identifier(cb_identifier) {

    last_everest_heartbeat = std::chrono::steady_clock::time_point::max();
}

void ovm_api::sync(bool cb_connected) {
    m_cb_connected = cb_connected;
    handle_everest_connection_state();
}

bool ovm_api::register_events([[maybe_unused]] everest::lib::io::event::fd_event_handler& handler) {
    // clang-format off
    return true;
    // clang-format on
}

bool ovm_api::unregister_events([[maybe_unused]] everest::lib::io::event::fd_event_handler& handler) {
    // clang-format off
    return true;
    // clang-format on
}

void ovm_api::set_cb_tx(tx_ftor const& handler) {
    m_tx = handler;
}

void ovm_api::tx(evse_bsp_host_to_cb const& msg) {
    if (m_tx) {
        m_tx(msg);
    }
}

void ovm_api::set_mqtt_tx(mqtt_ftor const& handler) {
    m_mqtt_tx = handler;
}

void ovm_api::set_cb_message(evse_bsp_cb_to_host const& msg) {
    const double voltage_V = msg.hv_mV * 0.001;
    send_voltage_measurement_V(voltage_V);

    if (msg.error_flags.dc_hv_ov not_eq m_cb_status.error_flags.dc_hv_ov) {
        handle_dc_hv_ov(msg.error_flags.dc_hv_ov not_eq 0, voltage_V);
    }
    if (msg.cp_state not_eq m_cb_status.cp_state) {
        handle_cp_state(static_cast<CpState>(msg.cp_state));
    }

    m_cb_status = msg;
}

void ovm_api::dispatch(std::string const& operation, std::string const& payload) {
    if (operation == "set_limits") {
        receive_set_limits(payload);
    } else if (operation == "start") {
        receive_start();
    } else if (operation == "stop") {
        receive_stop();
    } else if (operation == "reset_over_voltage_error") {
        receive_reset_over_voltage_error();
    } else if (operation == "heartbeat") {
        receive_heartbeat();
    } else {
        std::cerr << "ovm_api: RECEIVE invalid operation: " << operation << std::endl;
    }
}

void ovm_api::raise_comm_fault() {
    send_raise_error(API_OVM::ErrorEnum::CommunicationFault, "ChargeBridge not available", "",
                     API_OVM::ErrorSeverityEnum::High);
}

void ovm_api::clear_comm_fault() {
    send_clear_error(API_OVM::ErrorEnum::CommunicationFault, "ChargeBridge not available");
}

void ovm_api::handle_dc_hv_ov(bool high, double voltage_V) {
    if (high) {
        auto severity = (voltage_V > m_limits.emergency_limit_V) ? API_OVM::ErrorSeverityEnum::High
                                                                 : API_OVM::ErrorSeverityEnum::Medium;
        send_raise_error(API_OVM::ErrorEnum::MREC5OverVoltage, "", "", severity);
        std::cout << "OVM: status high" << std::endl;
    } else {
        send_clear_error(API_OVM::ErrorEnum::MREC5OverVoltage, "");
        std::cout << "OVM: status low" << std::endl;
    }
}

void ovm_api::handle_cp_state(CpState state) {
    if (state == CpState_A) {
        send_clear_error(API_OVM::ErrorEnum::MREC5OverVoltage, "");
    }
}

void ovm_api::receive_set_limits(std::string const& payload) {
    static auto const V_to_mV_factor = 1000;
    if (everest::lib::API::deserialize(payload, m_limits)) {
        host_status.ovm_limit_9ms_mV = static_cast<uint32_t>(m_limits.emergency_limit_V * V_to_mV_factor);
        host_status.ovm_limit_400ms_mV = static_cast<uint32_t>(m_limits.error_limit_V * V_to_mV_factor);
    } else {
        std::cerr << "ovm_api::receive_set_limits: payload invalid -> " << payload << std::endl;
    }
}

void ovm_api::receive_start() {
    host_status.ovm_enable = 1;
}

void ovm_api::receive_stop() {
    host_status.ovm_enable = 0;
}

void ovm_api::receive_reset_over_voltage_error() {
    std::cout << "reset_over_voltage_error()" << std::endl;
    // TODO Do something
}

void ovm_api::receive_heartbeat() {
    last_everest_heartbeat = std::chrono::steady_clock::now();
}

void ovm_api::send_voltage_measurement_V(double data) {
    send_mqtt("voltage_measurement_V", serialize(data));
}

void ovm_api::send_raise_error(API_OVM::ErrorEnum error, std::string const& subtype, std::string const& msg,
                               API_OVM::ErrorSeverityEnum severity) {
    API_OVM::Error error_msg;
    error_msg.type = error;
    error_msg.sub_type = subtype;
    error_msg.message = msg;
    error_msg.severity = severity;
    send_mqtt("raise_error", serialize(error_msg));
}

void ovm_api::send_clear_error(API_OVM::ErrorEnum error, std::string const& subtype) {
    API_OVM::Error error_msg;
    error_msg.type = error;
    error_msg.sub_type = subtype;
    send_mqtt("clear_error", serialize(error_msg));
}

void ovm_api::send_communication_check() {
    send_mqtt("communication_check", serialize(true));
}

void ovm_api::send_mqtt(std::string const& topic, std::string const& message) {
    if (m_mqtt_tx) {
        everest::lib::io::mqtt::Dataset payload;
        payload.topic = topic;
        payload.message = message;
        m_mqtt_tx(payload);
    }
}

bool ovm_api::check_everest_heartbeat() {
    if (last_everest_heartbeat == std::chrono::steady_clock::time_point::max()) {
        return false;
    }
    return std::chrono::steady_clock::now() - last_everest_heartbeat < 2s;
}

void ovm_api::handle_everest_connection_state() {
    send_communication_check();
    auto current = check_everest_heartbeat();
    auto handle_status = [this](bool status) {
        if (status) {
            utilities::print_error(m_cb_identifier, "OVM/EVEREST", 0) << "EVerest connected" << std::endl;
            // TODO is there anything to do???
        } else {
            utilities::print_error(m_cb_identifier, "OVM/EVEREST", 1) << "Waiting for EVerest...." << std::endl;
            // TODO is there anything to do???
            tx(host_status);
        }
    };

    if (m_bc_initial_comm_check) {
        handle_status(current);
        m_bc_initial_comm_check = false;
    } else if (m_everest_connected != current) {
        handle_status(not m_everest_connected);
    }
    m_everest_connected = current;
}

} // namespace charge_bridge::evse_bsp
