// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#pragma once

#include "nlohmann/json_fwd.hpp"
#include <everest_api_types/charger_information/API.hpp>

namespace everest::lib::API::V1_0::types::charger_information {

using json = nlohmann::json;

void to_json(json& j, ChargerInformation const& k) noexcept;
void from_json(const json& j, ChargerInformation& k);

} // namespace everest::lib::API::V1_0::types::charger_information
