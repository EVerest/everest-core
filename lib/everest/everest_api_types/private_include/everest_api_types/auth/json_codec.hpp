// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "nlohmann/json_fwd.hpp"
#include <everest_api_types/auth/API.hpp>

namespace everest::lib::API::V1_0::types::auth {

using json = nlohmann::json;

void to_json(json& j, AuthorizationStatus const& k) noexcept;
void from_json(const json& j, AuthorizationStatus& k);

void to_json(json& j, CertificateStatus const& k) noexcept;
void from_json(const json& j, CertificateStatus& k);

void to_json(json& j, TokenValidationStatus const& k) noexcept;
void from_json(const json& j, TokenValidationStatus& k);

void to_json(json& j, SelectionAlgorithm const& k) noexcept;
void from_json(const json& j, SelectionAlgorithm& k);

void to_json(json& j, AuthorizationType const& k) noexcept;
void from_json(const json& j, AuthorizationType& k);

void to_json(json& j, IdTokenType const& k) noexcept;
void from_json(const json& j, IdTokenType& k);

void to_json(json& j, WithdrawAuthorizationResult const& k) noexcept;
void from_json(const json& j, WithdrawAuthorizationResult& k);

void to_json(json& j, CustomIdToken const& k) noexcept;
void from_json(const json& j, CustomIdToken& k);

void to_json(json& j, IdToken const& k) noexcept;
void from_json(const json& j, IdToken& k);

void to_json(json& j, ProvidedIdToken const& k) noexcept;
void from_json(const json& j, ProvidedIdToken& k);

void to_json(json& j, TokenValidationStatusMessage const& k) noexcept;
void from_json(const json& j, TokenValidationStatusMessage& k);

void to_json(json& j, ValidationResult const& k) noexcept;
void from_json(const json& j, ValidationResult& k);

void to_json(json& j, ValidationResultUpdate const& k) noexcept;
void from_json(const json& j, ValidationResultUpdate& k);

void to_json(json& j, WithdrawAuthorizationRequest const& k) noexcept;
void from_json(const json& j, WithdrawAuthorizationRequest& k);

} // namespace everest::lib::API::V1_0::types::auth
