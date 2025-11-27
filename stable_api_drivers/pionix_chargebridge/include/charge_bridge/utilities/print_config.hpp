// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <protocol/cb_config.h>
#include <string>

namespace charge_bridge::utilities {

std::string to_string(CbCanBaudrate value);
std::string to_string(CbUartBaudrate value);
std::string to_string(CbUartParity value);
std::string to_string(CbUartStopbits value);
std::string to_string(CbUartConfig const& value);

} // namespace charge_bridge::utilities
