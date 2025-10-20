// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <everest_api_types/session_cost/API.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#include "generated/types/session_cost.hpp"
#pragma GCC diagnostic pop

namespace everest::lib::API::V1_0::types::session_cost {

using json = nlohmann::json;

void to_json(json& j, TariffMessage const& k) noexcept;
void from_json(const json& j, TariffMessage& k);

void to_json(json& j, IdlePrice const& k) noexcept;
void from_json(const json& j, IdlePrice& k);

void to_json(json& j, CostCategory const& k) noexcept;
void from_json(const json& j, CostCategory& k);

void to_json(json& j, ChargingPriceComponent const& k) noexcept;
void from_json(const json& j, ChargingPriceComponent& k);

void to_json(json& j, NextPeriodPrice const& k) noexcept;
void from_json(const json& j, NextPeriodPrice& k);

void to_json(json& j, SessionCostChunk const& k) noexcept;
void from_json(const json& j, SessionCostChunk& k);

void to_json(json& j, SessionStatus const& k) noexcept;
void from_json(const json& j, SessionStatus& k);

void to_json(json& j, SessionCost const& k) noexcept;
void from_json(const json& j, SessionCost& k);

} // namespace everest::lib::API::V1_0::types::session_cost
