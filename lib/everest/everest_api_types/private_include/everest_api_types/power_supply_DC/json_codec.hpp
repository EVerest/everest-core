// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "nlohmann/json_fwd.hpp"
#include <everest_api_types/power_supply_DC/API.hpp>

namespace everest::lib::API::V1_0::types::power_supply_DC {

using json = nlohmann::json;

void from_json(const json& j, Capabilities& k);
void to_json(json& j, const Capabilities& k) noexcept;

void from_json(const json& j, Mode& k);
void to_json(json& j, const Mode& k) noexcept;

void from_json(const json& j, ChargingPhase& k);
void to_json(json& j, const ChargingPhase& k) noexcept;

void from_json(const json& j, ModeRequest& k);
void to_json(json& j, const ModeRequest& k) noexcept;

void from_json(const json& j, VoltageCurrent& k);
void to_json(json& j, const VoltageCurrent& k) noexcept;

void from_json(const json& j, ErrorEnum& k);
void to_json(json& j, const ErrorEnum& k) noexcept;

void from_json(const json& j, Error& k);
void to_json(json& j, const Error& k) noexcept;

} // namespace everest::lib::API::V1_0::types::power_supply_DC
