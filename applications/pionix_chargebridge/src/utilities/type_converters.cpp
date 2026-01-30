// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "protocol/cb_config.h"
#include <charge_bridge/utilities/type_converters.hpp>
#include <cmath>
#include <everest_api_types/evse_board_support/API.hpp>
#include <everest_api_types/evse_board_support/codec.hpp>
#include <everest_api_types/utilities/codec.hpp>
// clang-format off
#include <optional>
#include <ryml_std.hpp>
#include <ryml.hpp>
// clang-format on
#include <functional>
#include <iostream>

namespace {

template <class T> void decode(c4::yml::ConstNodeRef const& node, T& rhs) {
    using namespace charge_bridge::utilities;
    try {
        node >> rhs;
    } catch (...) {
        std::string value;
        std::string key;
        if (node.has_key()) {
            key = std::string(node.key().str, node.key().len);
        }
        if (node.has_val()) {
            value = std::string(node.val().str, node.val().len);
        }
        throw charge_bridge::utilities::yml_node_error(node, key + "::" + value);
    }
}

template <class T, class TmpT>
T decode_t(c4::yml::ConstNodeRef parent, std::string const& node_id, std::function<T(TmpT)> const& transform,
           std::optional<T> const& def_value = std::nullopt) {
    auto index = ryml::to_csubstr(node_id);
    auto local_node = parent.find_child(index);
    if (local_node.invalid()) {
        if (def_value.has_value()) {
            return def_value.value();
        }

        throw charge_bridge::utilities::yml_node_error(parent, "Cannot find nested node: " + node_id);
    }
    TmpT tmp;
    decode(local_node, tmp);
    return transform(tmp);
}

template <class Ftor, class T>
auto decode_t(c4::yml::ConstNodeRef parent, std::string const& node_id, Ftor transform, std::optional<T> const& t) {
    std::function g = transform;
    return decode_t(parent, node_id, g, t);
}

template <class Ftor> auto decode_t(c4::yml::ConstNodeRef parent, std::string const& node_id, Ftor transform) {
    std::function g = transform;
    return decode_t(parent, node_id, g);
}

template <class T>
T decode(c4::yml::ConstNodeRef parent, std::string const& node_id, std::optional<T> const& def_value = std::nullopt) {
    auto identity = [](T v) { return v; };
    return decode_t(parent, node_id, identity, def_value);
}

} // namespace

namespace charge_bridge::utilities {

yml_node_error::yml_node_error(c4::yml::ConstNodeRef node) : m_node(node) {
}

yml_node_error::yml_node_error(c4::yml::ConstNodeRef node, std::string const& msg) : m_node(node), m_msg(msg) {
}

bool decode_CbGpioMode(c4::yml::ConstNodeRef const& node, CbGpioMode& rhs) {
    if (node.invalid()) {
        return false;
    }
    std::string value;
    decode(node, value);

    if (value == "Input") {
        rhs = CbGpioMode::CBG_Input;
        return true;
    } else if (value == "Output") {
        rhs = CbGpioMode::CBG_Output;
        return true;
    } else if (value == "Pwm_Input") {
        rhs = CbGpioMode::CBG_Pwm_Input;
        return true;
    } else if (value == "Pwm_Output") {
        rhs = CbGpioMode::CBG_Pwm_Output;
        return true;
    } else if (value == "RS485_2_DE") {
        rhs = CbGpioMode::CBG_RS485_2_DE;
        return true;
    } else if (value == "Rcd_Selftest_Output") {
        rhs = CbGpioMode::CBG_Rcd_Selftest_Output;
        return true;
    } else if (value == "Rcd_Error_Input") {
        rhs = CbGpioMode::CBG_Rcd_Error_Input;
        return true;
    } else if (value == "Rcd_PWM_Input") {
        rhs = CbGpioMode::CBG_Rcd_PWM_Input;
        return true;
    } else if (value == "MotorLock_1") {
        rhs = CbGpioMode::CBG_MotorLock_1;
        return true;
    } else if (value == "MotorLock_2") {
        rhs = CbGpioMode::CBG_MotorLock_2;
        return true;
    }
    throw yml_node_error(node);

    return false;
}

bool decode_CbGpioPulls(c4::yml::ConstNodeRef const& node, CbGpioPulls& rhs) {
    if (node.invalid()) {
        return false;
    }
    std::string value;
    decode(node, value);

    if (value == "NoPull") {
        rhs = CBGP_NoPull;
        return true;
    }
    if (value == "PullUp") {
        rhs = CBGP_PullUp;
        return true;
    }
    if (value == "PullDown") {
        rhs = CBGP_PullDown;
        return true;
    }
    throw yml_node_error(node);

    return false;
}

bool decode_CbUartBaudrate(c4::yml::ConstNodeRef const& node, CbUartBaudrate& rhs) {
    if (node.invalid()) {
        return false;
    }
    std::string value;
    decode(node, value);

    if (value == "9600") {
        rhs = CBUBR_9600;
        return true;
    }
    if (value == "19200") {
        rhs = CBUBR_19200;
        return true;
    }
    if (value == "38400") {
        rhs = CBUBR_38400;
        return true;
    }
    if (value == "57600") {
        rhs = CBUBR_57600;
        return true;
    }
    if (value == "115200") {
        rhs = CBUBR_115200;
        return true;
    }
    if (value == "230400") {
        rhs = CBUBR_230400;
        return true;
    }
    if (value == "250000") {
        rhs = CBUBR_250000;
        return true;
    }
    if (value == "460800") {
        rhs = CBUBR_460800;
        return true;
    }
    if (value == "500000") {
        rhs = CBUBR_500000;
        return true;
    }
    if (value == "1000000") {
        rhs = CBUBR_1000000;
        return true;
    }
    if (value == "2000000") {
        rhs = CBUBR_2000000;
        return true;
    }
    if (value == "3000000") {
        rhs = CBUBR_3000000;
        return true;
    }
    if (value == "4000000") {
        rhs = CBUBR_4000000;
        return true;
    }
    if (value == "6000000") {
        rhs = CBUBR_6000000;
        return true;
    }
    if (value == "8000000") {
        rhs = CBUBR_8000000;
        return true;
    }
    if (value == "10000000") {
        rhs = CBUBR_10000000;
        return true;
    }
    throw yml_node_error(node);

    return false;
}

bool decode_CbUartStopbits(c4::yml::ConstNodeRef const& node, CbUartStopbits& rhs) {
    if (node.invalid()) {
        return false;
    }
    std::string value;
    decode(node, value);

    if (value == "OneStopBit") {
        rhs = CBUS_OneStopBit;
        return true;
    }
    if (value == "TwoStopBits") {
        rhs = CBUS_TwoStopBits;
        return true;
    }
    throw yml_node_error(node);

    return false;
}

bool decode_CbUartParity(c4::yml::ConstNodeRef const& node, CbUartParity& rhs) {
    if (node.invalid()) {
        return false;
    }
    std::string value;
    decode(node, value);

    if (value == "None") {
        rhs = CBUP_None;
        return true;
    }
    if (value == "Odd") {
        rhs = CBUP_Odd;
        return true;
    }
    if (value == "Even") {
        rhs = CBUP_Even;
        return true;
    }
    throw yml_node_error(node);

    return false;
}

bool decode_CbCanBaudrate(c4::yml::ConstNodeRef const& node, CbCanBaudrate& rhs) {
    if (node.invalid()) {
        return false;
    }

    std::string value;
    decode(node, value);

    if (value == "125000") {
        rhs = CBCBR_125000;
        return true;
    }
    if (value == "250000") {
        rhs = CBCBR_250000;
        return true;
    }
    if (value == "500000") {
        rhs = CBCBR_500000;
        return true;
    }
    if (value == "1000000") {
        rhs = CBCBR_1000000;
        return true;
    }

    throw yml_node_error(node);
    return false;
}

bool decode_CbRelayMode(c4::yml::ConstNodeRef const& node, CbRelayMode& rhs) {
    if (node.invalid()) {
        return false;
    }

    std::string value;
    decode(node, value);

    if (value == "PowerRelay") {
        rhs = CBR_PowerRelay;
        return true;
    }
    if (value == "UserRelay") {
        rhs = CBR_UserRelay;
        return true;
    }
    throw yml_node_error(node);

    return false;
}

bool decode_CbSafetyMode(c4::yml::ConstNodeRef const& node, CbSafetyMode& rhs) {
    if (node.invalid()) {
        return false;
    }
    std::string value;
    decode(node, value);

    if (value == "disabled") {
        rhs = CBSM_disabled;
        return true;
    }
    if (value == "US") {
        rhs = CBSM_US;
        return true;
    }
    if (value == "EU") {
        rhs = CBSM_EU;
        return true;
    }

    throw yml_node_error(node);
    return false;
}

bool decode_RelayConfig(c4::yml::ConstNodeRef const& node, RelayConfig& rhs) {
    using ryml::ConstNodeRef;

    if (node.invalid()) {
        return false;
    }

    rhs.relay_mode = decode<decltype(rhs.relay_mode)>(node, "relay_mode");
    rhs.feedback_enabled = decode_t(node, "feedback_enabled", [](bool tmp) -> uint8_t { return tmp ? 1 : 0; });
    rhs.feedback_delay_ms = decode<decltype(rhs.feedback_delay_ms)>(node, "feedback_delay_ms");

    rhs.feedback_inverted = decode_t(node, "feedback_inverted", [](bool tmp) -> uint8_t { return tmp ? 1 : 0; });
    rhs.pwm_dc = decode_t(node, "pwm_dc", [](uint8_t tmp) { return std::min<uint8_t>(tmp, 100); });
    rhs.pwm_delay_ms = decode<decltype(rhs.pwm_delay_ms)>(node, "pwm_delay_ms");
    rhs.switchoff_delay_ms = decode<decltype(rhs.switchoff_delay_ms)>(node, "switchoff_delay_ms");

    return true;
}

bool decode_SafetyConfig(c4::yml::ConstNodeRef const& node, SafetyConfig& rhs) {
    using ryml::ConstNodeRef;

    if (node.invalid()) {
        return false;
    }

    rhs.pp_mode = decode<decltype(rhs.pp_mode)>(node, "pp_mode");
    rhs.cp_avg_ms = decode<decltype(rhs.cp_avg_ms)>(node, "cp_avg_ms", 10);
    rhs.temperature_limit_pt1000_C =
        decode<decltype(rhs.temperature_limit_pt1000_C)>(node, "temperature_limit_pt1000_C", 0);
    rhs.inverted_emergency_input = decode<decltype(rhs.inverted_emergency_input)>(node, "inverted_emergency_input", 0);

    rhs.relays[0] = decode<RelayConfig>(node, "relay_1");
    rhs.relays[1] = decode<RelayConfig>(node, "relay_2");
    rhs.relays[2] = decode<RelayConfig>(node, "relay_3");

    return true;
}

bool decode_CbGpioConfig(c4::yml::ConstNodeRef const& node, CbGpioConfig& rhs) {
    using ryml::ConstNodeRef;

    if (node.invalid()) {
        return false;
    }
    rhs.mode = decode<decltype(rhs.mode)>(node, "mode");
    rhs.pulls = decode<decltype(rhs.pulls)>(node, "pulls");
    rhs.strap_option_mdns_naming = decode_t(
        node, "mdns", [](bool tmp) -> uint8_t { return tmp ? 1 : 0; }, std::make_optional<uint8_t>(0));
    rhs.mode_config = decode<decltype(rhs.mode_config)>(node, "config", 0);

    return true;
}

bool decode_CbUartConfig(c4::yml::ConstNodeRef const& node, CbUartConfig& rhs) {
    using ryml::ConstNodeRef;

    if (node.invalid()) {
        return false;
    }

    rhs.baudrate = decode<decltype(rhs.baudrate)>(node, "baudrate");
    rhs.stopbits = decode<decltype(rhs.stopbits)>(node, "stopbits");
    rhs.parity = decode<decltype(rhs.parity)>(node, "parity");

    return true;
}

bool decode_CbCanConfig(c4::yml::ConstNodeRef const& node, CbCanConfig& rhs) {
    using ryml::ConstNodeRef;

    if (node.invalid()) {
        return false;
    }
    rhs.baudrate = decode<decltype(rhs.baudrate)>(node, "baudrate");

    return true;
}

bool decode_CbNetworkConfig(c4::yml::ConstNodeRef const& node, CbNetworkConfig& rhs) {
    using ryml::ConstNodeRef;

    if (node.invalid()) {
        return false;
    }

    ConstNodeRef local_node = node;
    local_node = node.find_child("mdns_name");

    if (not local_node.invalid()) {

        auto limit = sizeof(rhs.mdns_name);
        std::string name;
        decode(local_node, name);

        if (name.size() >= limit) {
            return false;
        }

        if (name.size() >= limit) {
            return false;
        }
        std::memset(rhs.mdns_name, 0, limit);
        std::memcpy(rhs.mdns_name, name.c_str(), std::min(name.size(), limit));
        return true;
    }
    throw yml_node_error(local_node);
    return false;
}

namespace EXT_API = everest::lib::API;
namespace EXT_API_BSP = EXT_API::V1_0::types::evse_board_support;

bool decode_Connector_type(c4::yml::ConstNodeRef const& node, EXT_API_BSP::Connector_type& rhs) {
    using ryml::ConstNodeRef;

    if (node.invalid()) {
        return false;
    }

    c4::csubstr value_view = node.val();
    size_t total_size = 2 + value_view.size();
    std::string quoted_value;
    quoted_value.reserve(total_size);
    quoted_value += "\"";
    quoted_value.append(value_view.data(), value_view.size());
    quoted_value += "\"";
    return EXT_API::deserialize(quoted_value, rhs);

    return true;
}

bool decode_HardwareCapabilities(c4::yml::ConstNodeRef const& node, EXT_API_BSP::HardwareCapabilities& rhs) {
    using ryml::ConstNodeRef;

    if (node.invalid()) {
        return false;
    }

    rhs.max_current_A_import = decode<decltype(rhs.max_current_A_import)>(node, "max_current_A_import");
    rhs.min_current_A_import = decode<decltype(rhs.min_current_A_import)>(node, "min_current_A_import");
    rhs.max_phase_count_import = decode<decltype(rhs.max_phase_count_import)>(node, "max_phase_count_import");
    rhs.min_phase_count_import = decode<decltype(rhs.min_phase_count_import)>(node, "min_phase_count_import");
    rhs.max_current_A_export = decode<decltype(rhs.max_current_A_export)>(node, "max_current_A_export");
    rhs.min_current_A_export = decode<decltype(rhs.min_current_A_export)>(node, "min_current_A_export");
    rhs.max_phase_count_export = decode<decltype(rhs.max_phase_count_export)>(node, "max_phase_count_export");
    rhs.min_phase_count_export = decode<decltype(rhs.min_phase_count_export)>(node, "min_phase_count_export");
    rhs.supports_changing_phases_during_charging = decode<decltype(rhs.supports_changing_phases_during_charging)>(
        node, "supports_changing_phases_during_charging");
    rhs.supports_cp_state_E = decode<decltype(rhs.supports_cp_state_E)>(node, "supports_cp_state_E");
    rhs.connector_type = decode<decltype(rhs.connector_type)>(node, "connector_type");
    auto tmp = decode<float>(node, "max_plug_temperature_C", NAN);
    rhs.max_plug_temperature_C = std::isnan(tmp) ? std::nullopt : std::make_optional<float>(tmp);

    return true;
}

c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, CbGpioMode& rhs) {
    if (decode_CbGpioMode(node, rhs)) {
        return node;
    }
    throw std::runtime_error("CbGpioMode");
}

c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, CbGpioPulls& rhs) {
    if (decode_CbGpioPulls(node, rhs)) {
        return node;
    }
    throw std::runtime_error("CbGpioPulls");
}

c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, CbUartBaudrate& rhs) {
    if (decode_CbUartBaudrate(node, rhs)) {
        return node;
    }
    throw std::runtime_error("CbUartBaudrate");
}

c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, CbUartStopbits& rhs) {
    if (decode_CbUartStopbits(node, rhs)) {
        return node;
    }
    throw std::runtime_error("CbUartStopbits");
}

c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, CbUartParity& rhs) {
    if (decode_CbUartParity(node, rhs)) {
        return node;
    }
    throw std::runtime_error("CbUartParity");
}

c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, CbCanBaudrate& rhs) {
    if (decode_CbCanBaudrate(node, rhs)) {
        return node;
    }
    throw std::runtime_error("CbCanBaudrate");
}

c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, CbRelayMode& rhs) {
    if (decode_CbRelayMode(node, rhs)) {
        return node;
    }
    throw std::runtime_error("CbRelayMode");
}

c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, CbSafetyMode& rhs) {
    if (decode_CbSafetyMode(node, rhs)) {
        return node;
    }
    throw std::runtime_error("CbSafetyMode");
}

c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, RelayConfig& rhs) {
    if (decode_RelayConfig(node, rhs)) {
        return node;
    }
    throw std::runtime_error("RelayConfig");
}

c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, SafetyConfig& rhs) {
    if (decode_SafetyConfig(node, rhs)) {
        return node;
    }
    throw std::runtime_error("SafetyConfig");
}

c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, CbGpioConfig& rhs) {
    if (decode_CbGpioConfig(node, rhs)) {
        return node;
    }
    throw std::runtime_error("CbGpioConfig");
}

c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, CbUartConfig& rhs) {
    if (decode_CbUartConfig(node, rhs)) {
        return node;
    }
    throw std::runtime_error("CbUartConfig");
}

c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, CbCanConfig& rhs) {
    if (decode_CbCanConfig(node, rhs)) {
        return node;
    }
    throw std::runtime_error("CbCanConfig");
}

c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, CbNetworkConfig& rhs) {
    if (decode_CbNetworkConfig(node, rhs)) {
        return node;
    }
    throw std::runtime_error("CbNetworkConfig");
}

c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, EXT_API_BSP::Connector_type& rhs) {
    if (decode_Connector_type(node, rhs)) {
        return node;
    }
    throw std::runtime_error("type");
}

c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, EXT_API_BSP::HardwareCapabilities& rhs) {
    if (decode_HardwareCapabilities(node, rhs)) {
        return node;
    }
    throw std::runtime_error("HardwareCapabilities");
}

} // namespace charge_bridge::utilities
