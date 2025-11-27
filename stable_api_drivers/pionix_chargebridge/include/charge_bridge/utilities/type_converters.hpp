// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <everest_api_types/evse_board_support/API.hpp>
#include <everest_api_types/evse_board_support/codec.hpp>
#include <everest_api_types/utilities/codec.hpp>
#include <iostream>
#include <protocol/cb_config.h>
#include <protocol/cb_management.h>
#include <yaml-cpp/yaml.h>

namespace YAML {

template <> struct convert<CbGpioMode> {
    static bool decode(Node const& node, CbGpioMode& rhs) {
        auto value = node.as<std::string>("");
        if (value.empty()) {
            return false;
        }
        if (value == "Input") {
            rhs = CbGpioMode::CBG_Input;
            return true;
        }
        if (value == "Output") {
            rhs = CbGpioMode::CBG_Output;
            return true;
        }
        if (value == "Pwm_Input") {
            rhs = CbGpioMode::CBG_Pwm_Input;
            return true;
        }
        if (value == "Pwm_Output") {
            rhs = CbGpioMode::CBG_Pwm_Output;
            return true;
        }
        if (value == "RS485_2_DE") {
            rhs = CbGpioMode::CBG_RS485_2_DE;
            return true;
        }
        if (value == "Rcd_Selftest_Output") {
            rhs = CbGpioMode::CBG_Rcd_Selftest_Output;
            return true;
        }

        if (value == "Rcd_Error_Input") {
            rhs = CbGpioMode::CBG_Rcd_Error_Input;
            return true;
        }
        if (value == "Rcd_PWM_Input") {
            rhs = CbGpioMode::CBG_Rcd_PWM_Input;
            return true;
        }
        if (value == "MotorLock_1") {
            rhs = CbGpioMode::CBG_MotorLock_1;
            return true;
        }
        if (value == "MotorLock_2") {
            rhs = CbGpioMode::CBG_MotorLock_2;
            return true;
        }

        return false;
    }
};

template <> struct convert<CbGpioPulls> {
    static bool decode(Node const& node, CbGpioPulls& rhs) {
        auto value = node.as<std::string>("");
        if (value.empty()) {
            return false;
        }
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

        return false;
    }
};

template <> struct convert<CbUartBaudrate> {
    static bool decode(Node const& node, CbUartBaudrate& rhs) {
        auto value = node.as<std::string>("");
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
        return false;
    }
};

template <> struct convert<CbUartStopbits> {
    static bool decode(Node const& node, CbUartStopbits& rhs) {
        auto value = node.as<std::string>("");
        if (value == "OneStopBit") {
            rhs = CBUS_OneStopBit;
            return true;
        }
        if (value == "TwoStopBits") {
            rhs = CBUS_TwoStopBits;
            return true;
        }
        return false;
    }
};

template <> struct convert<CbUartParity> {
    static bool decode(Node const& node, CbUartParity& rhs) {
        auto value = node.as<std::string>("");
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
        return false;
    }
};

template <> struct convert<CbCanBaudrate> {
    static bool decode(Node const& node, CbCanBaudrate& rhs) {
        auto value = node.as<std::string>("");
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
        return false;
    };
};

template <> struct convert<CbRelayMode> {
    static bool decode(Node const& node, CbRelayMode& rhs) {
        auto value = node.as<std::string>("");
        if (value == "PowerRelay") {
            rhs = CBR_PowerRelay;
            return true;
        }
        if (value == "UserRelay") {
            rhs = CBR_UserRelay;
            return true;
        }
        return false;
    }
};

template <> struct convert<CbSafetyMode> {
    static bool decode(Node const& node, CbSafetyMode& rhs) {
        auto value = node.as<std::string>("");
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

        return false;
    }
};

template <> struct convert<RelayConfig> {
    static bool decode(Node const& node, RelayConfig& rhs) {
        if (node["relay_mode"]) {
            rhs.relay_mode = node["relay_mode"].as<CbRelayMode>();
        } else {
            return false;
        }
        if (node["feedback_enabled"]) {
            rhs.feedback_enabled = node["feedback_enabled"].as<bool>() ? 1 : 0;
        } else {
            return false;
        }
        if (node["feedback_delay_ms"]) {
            rhs.feedback_delay_ms = node["feedback_delay_ms"].as<uint16_t>();
        } else {
            return false;
        }
        if (node["feedback_inverted"]) {
            rhs.feedback_inverted = node["feedback_inverted"].as<bool>();
        } else {
            return false;
        }
        if (node["pwm_dc"]) {
            rhs.pwm_dc = std::min<uint8_t>(node["pwm_dc"].as<uint8_t>(), 100);
        } else {
            return false;
        }
        if (node["pwm_delay_ms"]) {
            rhs.pwm_delay_ms = node["pwm_delay_ms"].as<uint16_t>();
        } else {
            return false;
        }
        if (node["switchoff_delay_ms"]) {
            rhs.switchoff_delay_ms = node["switchoff_delay_ms"].as<uint16_t>();
        } else {
            return false;
        }
        return true;
    }
};

template <> struct convert<SafetyConfig> {
    static bool decode(Node const& node, SafetyConfig& rhs) {
        if (node["pp_mode"]) {
            rhs.pp_mode = node["pp_mode"].as<CbSafetyMode>();
        } else {
            return false;
        }

        if (node["cp_avg_ms"]) {
            rhs.cp_avg_ms = node["cp_avg_ms"].as<uint8_t>(10);
        } else {
            rhs.cp_avg_ms = 10;
        }
        if (node["temperature_limit_pt1000_C"]) {
            rhs.temperature_limit_pt1000_C = node["temperature_limit_pt1000_C"].as<uint8_t>(0);
        } else {
            rhs.temperature_limit_pt1000_C = 0;
        }

        if (node["inverted_emergency_input"]) {
            rhs.inverted_emergency_input = node["inverted_emergency_input"].as<uint8_t>(0);
        } else {
            rhs.inverted_emergency_input = 0;
        }

        if (node["relay_1"]) {
            rhs.relays[0] = node["relay_1"].as<RelayConfig>();
        } else {
            return false;
        }

        if (node["relay_2"]) {
            rhs.relays[1] = node["relay_2"].as<RelayConfig>();
        } else {
            return false;
        }

        if (node["relay_3"]) {
            rhs.relays[2] = node["relay_3"].as<RelayConfig>();
        } else {
            return false;
        }
        return true;
    }
};

template <> struct convert<CbGpioConfig> {
    static bool decode(Node const& node, CbGpioConfig& rhs) {
        if (node["mode"]) {
            rhs.mode = node["mode"].as<CbGpioMode>();
        } else {
            return false;
        }
        if (node["pulls"]) {
            rhs.pulls = node["pulls"].as<CbGpioPulls>();
        } else {
            return false;
        }
        if (node["mdns"]) {
            rhs.strap_option_mdns_naming = node["mdns"].as<uint8_t>(0);
        } else {
            rhs.strap_option_mdns_naming = 0;
        }
        if (node["config"]) {
            rhs.mode_config = node["config"].as<uint16_t>();
        } else {
            rhs.mode_config = 0;
        }
        return true;
    }
};

template <> struct convert<CbUartConfig> {
    static bool decode(Node const& node, CbUartConfig& rhs) {
        if (node["baudrate"]) {
            rhs.baudrate = node["baudrate"].as<CbUartBaudrate>();
        } else {
            return false;
        }
        if (node["stopbits"]) {
            rhs.stopbits = node["stopbits"].as<CbUartStopbits>();
        } else {
            return false;
        }
        if (node["parity"]) {
            rhs.parity = node["parity"].as<CbUartParity>();
        } else {
            return false;
        }
        return true;
    }
};

template <> struct convert<CbCanConfig> {
    static bool decode(Node const& node, CbCanConfig& rhs) {
        if (node["baudrate"]) {
            rhs.baudrate = node["baudrate"].as<CbCanBaudrate>();
        } else {
            return false;
        }
        return true;
    }
};

template <> struct convert<CbNetworkConfig> {
    static bool decode(Node const& node, CbNetworkConfig& rhs) {
        if (node["mdns_name"]) {
            auto limit = sizeof(rhs.mdns_name);
            auto name = node["mdns_name"].as<std::string>("");
            if (name.size() >= limit) {
                return false;
            }
            std::memset(rhs.mdns_name, 0, limit);
            std::memcpy(rhs.mdns_name, name.c_str(), std::min(name.size(), limit));
            return true;
        }
        return false;
    }
};

namespace EXT_API = everest::lib::API;
namespace EXT_API_BSP = EXT_API::V1_0::types::evse_board_support;

template <> struct convert<EXT_API_BSP::Connector_type> {
    static bool decode(Node const& node, EXT_API_BSP::Connector_type& rhs) {
        auto value = node.as<std::string>("");
        return EXT_API::deserialize("\"" + value + "\"", rhs);
    }
};

template <> struct convert<EXT_API_BSP::HardwareCapabilities> {
    static bool decode(Node const& node, EXT_API_BSP::HardwareCapabilities& rhs) {
        rhs.max_current_A_import = node["max_current_A_import"].as<float>();
        rhs.min_current_A_import = node["min_current_A_import"].as<float>();
        rhs.max_phase_count_import = node["max_phase_count_import"].as<int32_t>();
        rhs.min_phase_count_import = node["min_phase_count_import"].as<int32_t>();
        rhs.max_current_A_export = node["max_current_A_export"].as<float>();
        rhs.min_current_A_export = node["min_current_A_export"].as<float>();
        rhs.max_phase_count_export = node["max_phase_count_export"].as<int32_t>();
        rhs.min_phase_count_export = node["min_phase_count_export"].as<int32_t>();
        rhs.supports_changing_phases_during_charging = node["supports_changing_phases_during_charging"].as<bool>();
        rhs.connector_type = node["connector_type"].as<EXT_API_BSP::Connector_type>();
        if (node["max_plug_temperature_C"]) {
            rhs.max_plug_temperature_C = node["max_plug_temperature_C"].as<float>();
        } else {
            rhs.max_plug_temperature_C = std::nullopt;
        }
        return true;
    }
};
} // namespace YAML
