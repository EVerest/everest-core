// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "nlohmann/json_fwd.hpp"
#include <everest_api_types/iso15118_charger/API.hpp>

namespace everest::lib::API::V1_0::types::iso15118_charger {

using json = nlohmann::json;

void to_json(json& j, CertificateActionEnum const& k) noexcept;
void from_json(json const& j, CertificateActionEnum& k);

void to_json(json& j, Status const& k) noexcept;
void from_json(json const& j, Status& k);

void to_json(json& j, RequestExiStreamSchema const& k) noexcept;
void from_json(json const& j, RequestExiStreamSchema& k);

void to_json(json& j, ResponseExiStreamStatus const& k) noexcept;
void from_json(json const& j, ResponseExiStreamStatus& k);

} // namespace everest::lib::API::V1_0::types::iso15118_charger
