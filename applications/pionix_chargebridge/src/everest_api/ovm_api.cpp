// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

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
using namespace everest::lib::API;

namespace charge_bridge::evse_bsp {

ovm_api::ovm_api([[maybe_unused]] evse_ovm_config const& config, std::string const& cb_identifier,
                 evse_bsp_host_to_cb& host_status) :
    host_status(host_status), m_cb_identifier(cb_identifier) {

    last_everest_heartbeat = std::chrono::steady_clock::time_point();
}

void ovm_api::sync(bool cb_connected) {
    m_cb_connected = cb_connected;
    handle_everest_connection_state();
}

bool ovm_api::register_events([[maybe_unused]] everest::lib::io::event::fd_event_handler& handler) {
    return true;
}

bool ovm_api::unregister_events([[maybe_unused]] everest::lib::io::event::fd_event_handler& handler) {
    return true;
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

    if (msg.error_flags.flags.dc_hv_ov_emergency not_eq m_cb_status.error_flags.flags.dc_hv_ov_emergency) {
        handle_dc_hv_ov_emergency(msg.error_flags.flags.dc_hv_ov_emergency not_eq 0);
    }
    if (msg.error_flags.flags.dc_hv_ov_error not_eq m_cb_status.error_flags.flags.dc_hv_ov_error) {
        handle_dc_hv_ov_error(msg.error_flags.flags.dc_hv_ov_error not_eq 0);
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
        receive_heartbeat(payload);
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

void ovm_api::handle_dc_hv_ov_emergency(bool high) {
    static const std::string subtype = "Emergency";
    if (high) {
        send_raise_error(API_OVM::ErrorEnum::MREC5OverVoltage, subtype, "", API_OVM::ErrorSeverityEnum::High);
    } else {
        send_clear_error(API_OVM::ErrorEnum::MREC5OverVoltage, subtype);
    }
}

void ovm_api::handle_dc_hv_ov_error(bool high) {
    static const std::string subtype = "Error";
    if (high) {
        send_raise_error(API_OVM::ErrorEnum::MREC5OverVoltage, subtype, "", API_OVM::ErrorSeverityEnum::Medium);
    } else {
        send_clear_error(API_OVM::ErrorEnum::MREC5OverVoltage, subtype);
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
        host_status.ovm_limit_emergency_mV = static_cast<std::uint32_t>(m_limits.emergency_limit_V * V_to_mV_factor);
        host_status.ovm_limit_error_mV = static_cast<std::uint32_t>(m_limits.error_limit_V * V_to_mV_factor);
        tx(host_status);
    } else {
        std::cerr << "ovm_api::receive_set_limits: payload invalid -> " << payload << std::endl;
    }
}

void ovm_api::receive_start() {
    host_status.ovm_enable = 1;
    host_status.ovm_reset_errors = 0;
    tx(host_status);
}

void ovm_api::receive_stop() {
    host_status.ovm_enable = 0;
    tx(host_status);
}

void ovm_api::receive_reset_over_voltage_error() {
    host_status.ovm_reset_errors = 1;
    tx(host_status);
}

void ovm_api::receive_heartbeat(std::string const& pl) {
    last_everest_heartbeat = std::chrono::steady_clock::now();
    std::size_t id = 0;
    if (deserialize(pl, id)) {
        auto delta = id - m_last_hb_id;
        if (delta > 1) {
            utilities::print_error(m_cb_identifier, "OVM/EVEREST", -1)
                << "EVerest heartbeat missmatch: " << m_last_hb_id << "<->" << id << std::endl;
        }
        m_last_hb_id = id;
    } else {
        utilities::print_error(m_cb_identifier, "EVSE/EVEREST", -1)
            << "EVerest invalid heartbeat message: " << pl << std::endl;
    }
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
        m_mqtt_tx(topic, message);
    }
}

bool ovm_api::check_everest_heartbeat() {
    return std::chrono::steady_clock::now() - last_everest_heartbeat < 2s;
}

void ovm_api::handle_everest_connection_state() {
    send_communication_check();
    auto current = check_everest_heartbeat();
    auto handle_status = [this](bool status) {
        if (status) {
            utilities::print_error(m_cb_identifier, "OVM/EVEREST", 0) << "EVerest connected" << std::endl;
        } else {
            utilities::print_error(m_cb_identifier, "OVM/EVEREST", 1) << "Waiting for EVerest...." << std::endl;
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
