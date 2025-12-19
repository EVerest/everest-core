// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "nlohmann/json_fwd.hpp"
#include <everest_api_types/ev_board_support/API.hpp>

namespace everest::lib::API::V1_0::types::ev_board_support {

using json = nlohmann::json;

void to_json(json& j, EvCpState const& k) noexcept;
void from_json(const json& j, EvCpState& k);

void to_json(json& j, BspMeasurement const& k) noexcept;
void from_json(const json& j, BspMeasurement& k);

} // namespace everest::lib::API::V1_0::types::ev_board_support
