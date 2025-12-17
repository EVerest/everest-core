// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once
#include <everest_api_types/evse_board_support/API.hpp>
#include <protocol/cb_config.h>
#include <protocol/cb_management.h>
#include <ryml.hpp>
#include <string>

namespace charge_bridge::utilities {

class yml_node_error {
public:
    yml_node_error(c4::yml::ConstNodeRef node);
    yml_node_error(c4::yml::ConstNodeRef node, std::string const& msg);

    c4::yml::ConstNodeRef m_node;
    std::string m_msg;
};

namespace EXT_API = everest::lib::API;
namespace EXT_API_BSP = EXT_API::V1_0::types::evse_board_support;

bool decode_CbGpioMode(c4::yml::ConstNodeRef const& node, CbGpioMode& rhs);
bool decode_CbGpioPulls(c4::yml::ConstNodeRef const& node, CbGpioPulls& rhs);
bool decode_CbUartBaudrate(c4::yml::ConstNodeRef const& node, CbUartBaudrate& rhs);
bool decode_CbUartStopbits(c4::yml::ConstNodeRef const& node, CbUartStopbits& rhs);
bool decode_CbUartParity(c4::yml::ConstNodeRef const& node, CbUartParity& rhs);
bool decode_CbCanBaudrate(c4::yml::ConstNodeRef const& node, CbCanBaudrate& rhs);
bool decode_CbRelayMode(c4::yml::ConstNodeRef const& node, CbRelayMode& rhs);
bool decode_CbSafetyMode(c4::yml::ConstNodeRef const& node, CbSafetyMode& rhs);
bool decode_RelayConfig(c4::yml::ConstNodeRef const& node, RelayConfig& rhs);
bool decode_SafetyConfig(c4::yml::ConstNodeRef const& node, SafetyConfig& rhs);
bool decode_CbGpioConfig(c4::yml::ConstNodeRef const& node, CbGpioConfig& rhs);
bool decode_CbUartConfig(c4::yml::ConstNodeRef const& node, CbUartConfig& rhs);
bool decode_CbCanConfig(c4::yml::ConstNodeRef const& node, CbCanConfig& rhs);
bool decode_CbNetworkConfig(c4::yml::ConstNodeRef const& node, CbNetworkConfig& rhs);
bool decode_Connector_type(c4::yml::ConstNodeRef const& node, EXT_API_BSP::Connector_type& rhs);
bool decode_HardwareCapabilities(c4::yml::ConstNodeRef const& node, EXT_API_BSP::HardwareCapabilities& rhs);

c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, CbGpioMode& rhs);
c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, CbGpioPulls& rhs);
c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, CbUartBaudrate& rhs);
c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, CbUartStopbits& rhs);
c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, CbUartParity& rhs);
c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, CbCanBaudrate& rhs);
c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, CbRelayMode& rhs);
c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, CbSafetyMode& rhs);
c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, RelayConfig& rhs);
c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, SafetyConfig& rhs);
c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, CbGpioConfig& rhs);
c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, CbUartConfig& rhs);
c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, CbCanConfig& rhs);
c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, CbNetworkConfig& rhs);
c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, EXT_API_BSP::Connector_type& rhs);
c4::yml::ConstNodeRef const& operator>>(c4::yml::ConstNodeRef const& node, EXT_API_BSP::HardwareCapabilities& rhs);

} // namespace charge_bridge::utilities
