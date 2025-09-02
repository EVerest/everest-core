// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "nlohmann/json_fwd.hpp"
#include <everest_api_types/energy/API.hpp>

namespace everest::lib::API::V1_0::types::energy {

using json = nlohmann::json;

void to_json(json& j, NumberWithSource const& k) noexcept;
void from_json(const json& j, NumberWithSource& k);

void to_json(json& j, IntegerWithSource const& k) noexcept;
void from_json(const json& j, IntegerWithSource& k);

void to_json(json& j, FrequencyWattPoint const& k) noexcept;
void from_json(const json& j, FrequencyWattPoint& k);

void to_json(json& j, SetpointType const& k) noexcept;
void from_json(const json& j, SetpointType& k);

void to_json(json& j, PricePerkWh const& k) noexcept;
void from_json(const json& j, PricePerkWh& k);

void to_json(json& j, LimitsReq const& k) noexcept;
void from_json(const json& j, LimitsReq& k);

void to_json(json& j, LimitsRes const& k) noexcept;
void from_json(const json& j, LimitsRes& k);

void to_json(json& j, ScheduleReqEntry const& k) noexcept;
void from_json(const json& j, ScheduleReqEntry& k);

void to_json(json& j, ScheduleResEntry const& k) noexcept;
void from_json(const json& j, ScheduleResEntry& k);

void to_json(json& j, ScheduleSetpointEntry const& k) noexcept;
void from_json(const json& j, ScheduleSetpointEntry& k);

void to_json(json& j, ExternalLimits const& k) noexcept;
void from_json(const json& j, ExternalLimits& k);

void to_json(json& j, EnforcedLimits const& k) noexcept;
void from_json(const json& j, EnforcedLimits& k);

} // namespace everest::lib::API::V1_0::types::energy
