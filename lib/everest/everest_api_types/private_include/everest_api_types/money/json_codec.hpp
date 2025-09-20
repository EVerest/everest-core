// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <everest_api_types/money/API.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#include "generated/types/money.hpp"
#pragma GCC diagnostic pop

namespace everest::lib::API::V1_0::types::money {

using json = nlohmann::json;

void to_json(json& j, CurrencyCode const& k) noexcept;
void from_json(const json& j, CurrencyCode& k);

void to_json(json& j, Currency const& k) noexcept;
void from_json(const json& j, Currency& k);

void to_json(json& j, MoneyAmount const& k) noexcept;
void from_json(const json& j, MoneyAmount& k);

void to_json(json& j, Price const& k) noexcept;
void from_json(const json& j, Price& k);

} // namespace everest::lib::API::V1_0::types::money