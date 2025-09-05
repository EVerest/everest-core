// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "nlohmann/json_fwd.hpp"
#include <everest_api_types/over_voltage_monitor/API.hpp>

namespace everest::lib::API::V1_0::types::over_voltage_monitor {

using json = nlohmann::json;

void from_json(const json& j, ErrorEnum& k);
void to_json(json& j, const ErrorEnum& k) noexcept;

void from_json(const json& j, Error& k);
void to_json(json& j, const Error& k) noexcept;

} // namespace everest::lib::API::V1_0::types::over_voltage_monitor
