// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "nlohmann/json_fwd.hpp"
#include <everest_api_types/error_history/API.hpp>

namespace everest::lib::API::V1_0::types::error_history {

using json = nlohmann::json;

void to_json(json& j, State const& k) noexcept;
void from_json(const json& j, State& k);

void to_json(json& j, SeverityFilter const& k) noexcept;
void from_json(const json& j, SeverityFilter& k);

void to_json(json& j, Severity const& k) noexcept;
void from_json(const json& j, Severity& k);

void to_json(json& j, ImplementationIdentifier const& k) noexcept;
void from_json(const json& j, ImplementationIdentifier& k);

void to_json(json& j, TimeperiodFilter const& k) noexcept;
void from_json(const json& j, TimeperiodFilter& k);

void to_json(json& j, FilterArguments const& k) noexcept;
void from_json(const json& j, FilterArguments& k);

void to_json(json& j, ErrorObject const& k) noexcept;
void from_json(const json& j, ErrorObject& k);

void to_json(json& j, ErrorList const& k) noexcept;
void from_json(const json& j, ErrorList& k);

} // namespace everest::lib::API::V1_0::types::error_history
