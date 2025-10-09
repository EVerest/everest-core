// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "everest/io/mqtt/dataset.hpp"
#include "protocol/evse_bsp_cb_to_host.h"
#include <charge_bridge/evse_bsp/evse_bsp.hpp>
#include <charge_bridge/utilities/logging.hpp>
#include <charge_bridge/utilities/string.hpp>
#include <chrono>
#include <cstring>
#include <everest_api_types/evse_board_support/API.hpp>
#include <everest_api_types/evse_board_support/codec.hpp>
#include <everest_api_types/evse_manager/codec.hpp>
#include <everest_api_types/generic/codec.hpp>
#include <everest_api_types/utilities/codec.hpp>

#include <iostream>
#include <sstream>
#include <string>

using namespace std::chrono_literals;
using namespace everest::lib::API::V1_0::types::generic;

namespace charge_bridge::evse_bsp {

evse_bsp::evse_bsp(evse_bsp_config const& config, std::string const& cb_identifier) :
    mqtt(config.mqtt_remote, config.mqtt_port, config.mqtt_ping_interval_ms),
    capabilities(config.capabilities),
    m_cb_identifier(cb_identifier) {

    api_topics.setTargetApiModuleID(config.module_id, "evse_board_support");
    receive_topic = api_topics.everest_to_extern("");
    send_topic = api_topics.extern_to_everest("");

    last_everest_heartbeat = std::chrono::steady_clock::time_point::max();
    last_cb_heartbeat = std::chrono::steady_clock::time_point::max();
    mqtt.subscribe(receive_topic + "#");
    mqtt.set_message_handler([this](auto& data, auto&) { dispatch(data); });
    mqtt.set_error_handler([this, config](int id, std::string const& msg) {
        m_mqtt_on_error = id not_eq 0;
        utilities::print_error(m_cb_identifier, "MQTT/EVSE", id) << msg << std::endl;
    });

    sync_timer.set_timeout(1s);
    capabilities_timer.set_timeout(10s);
    mqtt_timer.set_timeout(5s);

    host_status.connector_lock = 0;
    host_status.pwm_duty_cycle = 0;
    host_status.allow_power_on = 0;
    host_status.reset = 0;

    cb_status.reset_reason = 0;
    cb_status.cp_state = 0;
    cb_status.relay_state = 0;
    cb_status.error_flags = {0};
    cb_status.pp_state_type1 = 0;
    cb_status.pp_state_type2 = 0;
    cb_status.lock_state = 0;
    cb_status.stop_charging = 0;
    enabled = true;
}

bool evse_bsp::register_events(everest::lib::io::event::fd_event_handler& handler) {
    // clang-format off
    return
        handler.register_event_handler(&mqtt) &&
        handler.register_event_handler(&sync_timer, [this](auto&) {
            handle_everest_connection_state();
            handle_cb_connection_state();
        }) &&
        handler.register_event_handler(&capabilities_timer, [this](auto&) {
            send_capabilities();
        }) &&
        handler.register_event_handler(&mqtt_timer, [this](auto&) {
            if(m_mqtt_on_error){
                mqtt.reset();
            }
        });
    // clang-format on
}

bool evse_bsp::unregister_events(everest::lib::io::event::fd_event_handler& handler) {
    // clang-format off
    return
        handler.unregister_event_handler(&mqtt) &&
        handler.unregister_event_handler(&sync_timer) &&
        handler.unregister_event_handler(&capabilities_timer) &&
        handler.unregister_event_handler(&mqtt_timer);
    // clang-format on
}

void evse_bsp::set_cb_tx(tx_ftor const& handler) {
    m_tx = handler;
}

void evse_bsp::tx(evse_bsp_host_to_cb const& msg) {
    if (m_tx) {
        m_tx(msg);
    }
}

inline static bool operator==(const SafetyErrorFlags& a, const SafetyErrorFlags& b) {
    return a.raw == b.raw;
}
inline static bool operator!=(const SafetyErrorFlags& a, const SafetyErrorFlags& b) {
    return a.raw != b.raw;
}

void evse_bsp::set_cb_message(evse_bsp_cb_to_host const& msg) {
    last_cb_heartbeat = std::chrono::steady_clock::now();
    if (cb_status.reset_reason not_eq msg.reset_reason) {
    }
    if (cb_status.cp_state not_eq msg.cp_state) {
        handle_event_cp(msg.cp_state);
    }
    if (cb_status.relay_state != msg.relay_state) {
        handle_event_relay(msg.relay_state);
    }
    if (cb_status.error_flags not_eq msg.error_flags) {
        handle_error(msg.error_flags);
    }
    if (cb_status.pp_state_type1 not_eq msg.pp_state_type1) {
        handle_pp_type1(msg.pp_state_type1);
    }
    if (cb_status.pp_state_type2 not_eq msg.pp_state_type2) {
        handle_pp_type2(msg.pp_state_type2);
    }
    if (cb_status.lock_state not_eq msg.lock_state) {
        // TODO: No idea what to do here
    }
    if (cb_status.stop_charging not_eq msg.stop_charging) {
        handle_stop_button(msg.stop_charging);
    }

    cb_status = msg;
}

void evse_bsp::dispatch(everest::lib::io::mqtt::Dataset const& data) {
    auto& topic = data.topic;
    auto& payload = data.message;

    auto operation = utilities::string_after_pattern(topic, receive_topic);
    if (operation == "enable") {
        receive_enable(payload);
    } else if (operation == "pwm_on") {
        receive_pwm_on(payload);
    } else if (operation == "pwm_off") {
        receive_pwm_off(payload);
    } else if (operation == "pwm_F") {
        receive_pwm_F(payload);
    } else if (operation == "allow_power_on") {
        receive_allow_power_on(payload);
    } else if (operation == "ac_switch_three_phases_while_charging") {
        receive_ac_switch_three_phases_while_charging(payload);
    } else if (operation == "evse_replug") {
        receive_evse_replug(payload);
    } else if (operation == "ac_overcurrent_limit") {
        receive_ac_overcurrent_limit(payload);
    } else if (operation == "lock") {
        receive_lock();
    } else if (operation == "unlock") {
        receive_unlock();
    } else if (operation == "self_test") {
        receive_self_test(payload);
    } else if (operation == "ac_pp_ampacity") {
        receive_request_ac_pp_ampacity(payload);
    } else if (operation == "reset") {
        receive_request_reset(payload);
    } else if (operation == "heartbeat") {
        receive_heartbeat();
    } else {
        std::cerr << "evse_bsp: RECEIVE invalid topic: " << topic << std::endl;
    }
}

void evse_bsp::handle_event_cp(uint8_t cp) {
    using bc_event = API_BSP::Event;
    bc_event cp_event;
    bool cp_state_valid = true;
    switch (cp) {
    case CpState_A:
        cp_event = bc_event::A;
        send_clear_error(API_BSP::ErrorEnum::MREC14PilotFault, "", "");
        send_clear_error(API_BSP::ErrorEnum::DiodeFault, "", "");
        break;
    case CpState_B:
        cp_event = bc_event::B;
        break;
    case CpState_C:
        cp_event = bc_event::C;
        break;
    case CpState_D:
        cp_event = bc_event::D;
        break;
    case CpState_E:
        cp_event = bc_event::E;
        break;
    case CpState_F:
        cp_event = bc_event::F;
        break;
    case CpState_DF:
        cp_event = bc_event::E;
        send_raise_error(API_BSP::ErrorEnum::DiodeFault, "", "Diode Fault");
        break;
    case CpState::CpState_INVALID:
        cp_event = bc_event::E;
        send_raise_error(API_BSP::ErrorEnum::MREC14PilotFault, "", "Pilot Fault");
        break;
    default:
        cp_state_valid = false;
    }
    if (cp_state_valid and enabled) {
        send_event(cp_event);
    }
}

void evse_bsp::handle_event_relay(uint8_t relay) {
    using bc_event = API_BSP::Event;
    bc_event relaise_event;
    bool relaise_state_valid = true;
    switch (relay) {
    case RelaiseState::RelayState_Open:
        relaise_event = bc_event::PowerOff;
        break;
    case RelaiseState::RelayState_Closed:
        relaise_event = bc_event::PowerOn;
        break;
    default:
        relaise_state_valid = false;
    }
    if (relaise_state_valid) {
        send_event(relaise_event);
    }
}

void evse_bsp::handle_pp_type2(uint8_t data) {
    API_BSP::Ampacity bc_ampacity;
    bool bc_ampacity_valid = true;
    switch (data) {
    case PpState_Type2_STATE_NC:
        bc_ampacity = API_BSP::Ampacity::None;
        break;
    case PpState_Type2_STATE_13A:
        bc_ampacity = API_BSP::Ampacity::A_13;
        break;
    case PpState_Type2_STATE_20A:
        bc_ampacity = API_BSP::Ampacity::A_20;
        break;
    case PpState_Type2_STATE_32A:
        bc_ampacity = API_BSP::Ampacity::A_32;
        break;
    case PpState_Type2_STATE_70A:
        bc_ampacity = API_BSP::Ampacity::A_63_3ph_70_1ph;
        break;
    case PpState_Type2_STATE_FAULT:
        // Raise error check state
        bc_ampacity_valid = false;
        send_raise_error(API_BSP::ErrorEnum::MREC23ProximityFault, "", "Proximity Pilot Fault State");
        break;
    default:
        bc_ampacity_valid = false;
    }
    if (bc_ampacity_valid) {
        send_ac_pp_amapcity(bc_ampacity);
    }
}

void evse_bsp::handle_pp_type1(uint8_t data) {
    // EVerest does not really have support for type 1 PP.
    // We just send a stop charging if some one presses the button,
    // for everything else the PP state does not really matter.
    if (data == PpState_Type1_STATE_Connected_Button_Pressed) {
        auto reason = API_EVM::StopTransactionReason::EVDisconnected;
        send_request_stop_transaction(reason);
    }
}

// Error handling
// Define bit masks
enum class SafetyErrorMask : std::uint32_t {
    cp_not_state_c = (1 << 0),
    pwm_not_enabled = (1 << 1),
    pp_invalid = (1 << 2),
    plug_temperature_too_high = (1 << 3),
    internal_temperature_too_high = (1 << 4),
    emergency_input_latched = (1 << 5),
    relay_health_latched = (1 << 6),
    vdd_3v3_out_of_range = (1 << 7),
    vdd_core_out_of_range = (1 << 8),
    vdd_12V_out_of_range = (1 << 9),
    vdd_N12V_out_of_range = (1 << 10),
    vdd_refint_out_of_range = (1 << 11),
    external_allow_power_on = (1 << 12),
    config_mem_error = (1 << 13),
};

// Table that maps a mask to our API error + message
struct FlagSpec {
    SafetyErrorMask mask;
    API_BSP::ErrorEnum error;
    const char* subtype;
    const char* message;
};

static constexpr FlagSpec error_specs[] = {
    {SafetyErrorMask::pp_invalid, API_BSP::ErrorEnum::MREC23ProximityFault, "", "PP invalid"},
    {SafetyErrorMask::plug_temperature_too_high, API_BSP::ErrorEnum::MREC19CableOverTempStop, "",
     "Plug temperature too high"},
    {SafetyErrorMask::internal_temperature_too_high, API_BSP::ErrorEnum::VendorError, "INTTEMP",
     "ChargeBridge internal over temperature"},
    {SafetyErrorMask::emergency_input_latched, API_BSP::ErrorEnum::VendorError, "EMGINPUT", "Emergency input latched"},
    {SafetyErrorMask::relay_health_latched, API_BSP::ErrorEnum::VendorError, "RELAYS", "Relay welded error"},
    {SafetyErrorMask::vdd_3v3_out_of_range, API_BSP::ErrorEnum::VendorError, "3V3", "Supply voltage 3.3V out of range"},
    {SafetyErrorMask::vdd_core_out_of_range, API_BSP::ErrorEnum::VendorError, "VDDCORE",
     "Internal supply core voltage out of range"},
    {SafetyErrorMask::vdd_12V_out_of_range, API_BSP::ErrorEnum::VendorError, "VCC12",
     "Internal supply 12V voltage out of range"},
    {SafetyErrorMask::vdd_N12V_out_of_range, API_BSP::ErrorEnum::VendorError, "VCCN12",
     "Internal supply -12V voltage out of range"},
    {SafetyErrorMask::vdd_refint_out_of_range, API_BSP::ErrorEnum::VendorError, "VCCREF",
     "Internal supply VREF voltage out of range"},
    {SafetyErrorMask::config_mem_error, API_BSP::ErrorEnum::VendorError, "CONFIGMEM", "Internal config memory error"},
};

static constexpr FlagSpec print_warning_specs[] = {
    {SafetyErrorMask::cp_not_state_c, API_BSP::ErrorEnum::VendorWarning, "", "CP is not state C"},
    {SafetyErrorMask::pwm_not_enabled, API_BSP::ErrorEnum::VendorWarning, "", "PWM not enabled"},
    {SafetyErrorMask::external_allow_power_on, API_BSP::ErrorEnum::VendorWarning, "",
     "Allow power on from EVerest missing"},
};

// 4) Edge-driven handler
void evse_bsp::handle_error(const SafetyErrorFlags& data) {
    uint32_t prev = cb_status.error_flags.raw; // cached raw value from before
    uint32_t next = data.raw;                  // current raw value

    uint32_t became_active = next & ~prev;   // rising edges
    uint32_t became_inactive = prev & ~next; // falling edges

    for (const auto& s : error_specs) {
        if (became_active & static_cast<uint32_t>(s.mask)) {
            send_raise_error(s.error, s.subtype, s.message);
        }
        if (became_inactive & static_cast<uint32_t>(s.mask)) {
            send_clear_error(s.error, s.subtype, "");
        }
    }

    std::stringstream log;

    for (const auto& s : print_warning_specs) {
        if (next & static_cast<uint32_t>(s.mask)) {
            log << "[" << s.message << "] ";
        }
    }

    for (const auto& s : error_specs) {
        if (next & static_cast<uint32_t>(s.mask)) {
            log << "[" << s.message << "] ";
        }
    }

    if (everest_connected && cb_connected) {
        if (log.str().empty()) {
            utilities::print_error(m_cb_identifier, "EVSE/EVEREST", 0) << "Relays can be switched on." << std::endl;
        } else {
            utilities::print_error(m_cb_identifier, "EVSE/EVEREST", 0)
                << "Relays off due to:" << log.str() << std::endl;
        }
    }
}

void evse_bsp::handle_stop_button([[maybe_unused]] uint8_t data) {
    auto reason = API_EVM::StopTransactionReason::Local;
    send_request_stop_transaction(reason);
}

void evse_bsp::receive_enable(std::string const& payload) {
    if (everest::lib::API::deserialize(payload, enabled)) {
        handle_event_cp(cb_status.cp_state);
        handle_event_relay(cb_status.relay_state);
    } else {
        std::cerr << "evse_bsp::receive_enabled: payload invalid -> " << payload << std::endl;
    }
}

void evse_bsp::receive_pwm_on(std::string const& payload) {
    double pwm = 0;
    if (everest::lib::API::deserialize(payload, pwm)) {
        host_status.pwm_duty_cycle = pwm * 100;
        tx(host_status);
    } else {
        std::cerr << "evse_bsp::receive_pwm_on: payload invalid -> " << payload << std::endl;
    }
}

void evse_bsp::receive_pwm_off([[maybe_unused]] std::string const& payload) {
    host_status.pwm_duty_cycle = 10001;
    tx(host_status);
}

void evse_bsp::receive_pwm_F([[maybe_unused]] std::string const& payload) {
    host_status.pwm_duty_cycle = 0;
    tx(host_status);
}

void evse_bsp::receive_allow_power_on(std::string const& payload) {
    API_BSP::PowerOnOff obj;
    if (everest::lib::API::deserialize(payload, obj)) {
        host_status.allow_power_on = obj.allow_power_on;
        tx(host_status);
    } else {
        std::cerr << "evse_bsp::receive_allow_power_on: payload invalid -> " << payload << std::endl;
    }
}

void evse_bsp::receive_ac_switch_three_phases_while_charging(std::string const&) {
    std::cerr << "evse_bsp::receive_ac_switch_three_phases_while_charging: not implemented" << std::endl;
}

void evse_bsp::receive_evse_replug(std::string const&) {
    std::cerr << "evse_bsp::receive_evse_replug: not implemented" << std::endl;
}

void evse_bsp::receive_ac_overcurrent_limit(std::string const&) {
    std::cerr << "evse_bsp::receive_ac_overcurrent_limit: not implemented" << std::endl;
}

void evse_bsp::receive_lock() {
    host_status.connector_lock = 1;
    tx(host_status);
}

void evse_bsp::receive_unlock() {
    host_status.connector_lock = 0;
    tx(host_status);
}

void evse_bsp::receive_self_test([[maybe_unused]] std::string const& payload) {
    std::cerr << "evse_bsp::receive_self_test: not implemented" << std::endl;
}

void evse_bsp::receive_request_ac_pp_ampacity(std::string const& payload) {
    everest::lib::API::V1_0::types::generic::RequestReply reqrpl;
    if (everest::lib::API::deserialize(payload, reqrpl)) {
        send_reply_ac_pp_ampacity(reqrpl.replyTo);
    } else {
        std::cerr << "evse_bsp::receive_request_ac_pp_amapacity: payload invalid -> " << payload << std::endl;
    }
}

void evse_bsp::receive_request_reset(std::string const&) {
    std::cerr << "evse_bsp::receive_request_reset: not implemented" << std::endl;
}

void evse_bsp::receive_heartbeat() {
    last_everest_heartbeat = std::chrono::steady_clock::now();
}

void evse_bsp::send_event(API_BSP::Event data) {
    API_BSP::BspEvent event{data};
    send_mqtt("event", serialize(event));
}

void evse_bsp::send_ac_nr_of_phases(uint8_t data) {
    auto phases = static_cast<int>(data);
    if (phases > 0 && phases <= 3) {
        send_mqtt("ac_nr_of_phases", serialize(phases));
    }
}

void evse_bsp::send_capabilities() {
    send_mqtt("capabilities", serialize(capabilities));
}

void evse_bsp::send_ac_pp_amapcity(API_BSP::Ampacity data) {
    API_BSP::ProximityPilot msg{data};
    send_mqtt("ac_pp_ampacity", serialize(msg));
}

void evse_bsp::send_request_stop_transaction(API_EVM::StopTransactionReason data) {
    API_EVM::StopTransactionRequest request;
    request.reason = data;
    send_mqtt("request_stop_transaction", serialize(request));
}

void evse_bsp::send_rcd_current(uint8_t) {
    std::cerr << "evse_bsp::send_rcd_current: not implemented" << std::endl;
}

void evse_bsp::send_raise_error(API_BSP::ErrorEnum error, std::string const& subtype, std::string const& msg) {
    API_BSP::Error error_msg;
    error_msg.type = error;
    error_msg.sub_type = subtype;
    error_msg.message = msg;
    send_mqtt("raise_error", serialize(error_msg));
}

void evse_bsp::send_clear_error(API_BSP::ErrorEnum error, std::string const& subtype, std::string const& msg) {
    API_BSP::Error error_msg;
    error_msg.type = error;
    error_msg.sub_type = subtype;
    error_msg.message = msg;
    send_mqtt("clear_error", serialize(error_msg));
}

void evse_bsp::send_communication_check() {
    send_mqtt("communication_check", serialize(true));
}

void evse_bsp::send_reply_ac_pp_ampacity(std::string const& replyTo) {
    everest::lib::io::mqtt::Dataset payload;
    payload.topic = replyTo;
    payload.message = serialize(capabilities);
    mqtt.tx(payload);
}

void evse_bsp::send_reply_reset([[maybe_unused]] std::string const& replyTo) {
    std::cerr << "evse_bsp::send_reply_reset: not implemented" << std::endl;
}

void evse_bsp::send_mqtt(std::string const& topic, std::string const& message) {
    everest::lib::io::mqtt::Dataset payload;
    payload.topic = send_topic + topic;
    payload.message = message;
    mqtt.tx(payload);
}

bool evse_bsp::check_everest_heartbeat() {
    if (last_everest_heartbeat == std::chrono::steady_clock::time_point::max()) {
        return false;
    }
    return std::chrono::steady_clock::now() - last_everest_heartbeat < 2s;
}

void evse_bsp::handle_everest_connection_state() {
    send_communication_check();
    auto current = check_everest_heartbeat();
    auto handle_status = [this](bool status) {
        if (status) {
            utilities::print_error(m_cb_identifier, "EVSE/EVEREST", 0) << "EVerest connected" << std::endl;
            send_capabilities();
        } else {
            utilities::print_error(m_cb_identifier, "EVSE/EVEREST", 1) << "Waiting for EVerest...." << std::endl;
            host_status.allow_power_on = 0;
            host_status.pwm_duty_cycle = 65535;
            tx(host_status);
        }
    };

    if (m_bc_initial_comm_check) {
        handle_status(current);
        m_bc_initial_comm_check = false;
    } else if (everest_connected != current) {
        handle_status(not everest_connected);
    }
    everest_connected = current;
}

bool evse_bsp::check_cb_heartbeat() {
    if (last_cb_heartbeat == std::chrono::steady_clock::time_point::max()) {
        return false;
    }
    return std::chrono::steady_clock::now() - last_cb_heartbeat < 2s;
}

void evse_bsp::handle_cb_connection_state() {
    tx(host_status);
    auto current = check_cb_heartbeat();
    auto handle_status = [this](bool status) {
        if (status) {
            utilities::print_error(m_cb_identifier, "EVSE/CB", 0) << "ChargeBridge connected." << std::endl;
            send_clear_error(API_BSP::ErrorEnum::CommunicationFault, "ChargeBridge not available", "");

        } else {
            send_raise_error(API_BSP::ErrorEnum::CommunicationFault, "ChargeBridge not available", "");
            utilities::print_error(m_cb_identifier, "EVSE/CB", 1) << "Waiting for ChargeBridge...." << std::endl;
        }
    };
    if (m_cb_initial_comm_check) {
        handle_status(current);
        m_cb_initial_comm_check = false;
    }
    if (cb_connected != current) {
        handle_status(not cb_connected);
    }
    cb_connected = current;
}

} // namespace charge_bridge::evse_bsp
