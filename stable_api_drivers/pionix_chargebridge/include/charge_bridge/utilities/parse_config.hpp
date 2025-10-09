// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <charge_bridge/charge_bridge.hpp>
#include <everest_api_types/evse_board_support/API.hpp>

namespace charge_bridge::utilities {

bool parse_config(std::string const& config_file, charge_bridge_config& config);
std::vector<charge_bridge_config> parse_config_multi(std::string const& config_file);
} // namespace charge_bridge::utilities
