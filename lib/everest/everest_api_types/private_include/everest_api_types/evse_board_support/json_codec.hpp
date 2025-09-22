// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "nlohmann/json_fwd.hpp"
#include <everest_api_types/evse_board_support/API.hpp>

namespace everest::lib::API::V1_0::types::evse_board_support {

using json = nlohmann::json;

void to_json(json& j, Event const& k) noexcept;
void from_json(const json& j, Event& k);

void to_json(json& j, BspEvent const& k) noexcept;
void from_json(const json& j, BspEvent& k);

void to_json(json& j, const ErrorEnum& k) noexcept;
void from_json(const json& j, ErrorEnum& k);

void to_json(json& j, const Error& k) noexcept;
void from_json(const json& j, Error& k);

void to_json(json& j, Connector_type const& k) noexcept;
void from_json(const json& j, Connector_type& k);

void to_json(json& j, HardwareCapabilities const& k) noexcept;
void from_json(const json& j, HardwareCapabilities& k);

void to_json(json& j, Reason const& k) noexcept;
void from_json(const json& j, Reason& k);

void to_json(json& j, PowerOnOff const& k) noexcept;
void from_json(const json& j, PowerOnOff& k);

void to_json(json& j, Ampacity const& k) noexcept;
void from_json(json const& j, Ampacity& k);

void to_json(json& j, ProximityPilot const& k) noexcept;
void from_json(json const& j, ProximityPilot& k);

} // namespace everest::lib::API::V1_0::types::evse_board_support
