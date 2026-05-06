// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <everest_api_types/auth/API.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#include "generated/types/authorization.hpp"
#pragma GCC diagnostic pop

namespace everest::lib::API::V1_0::types::auth {

using AuthorizationStatus_Internal = ::types::authorization::AuthorizationStatus;
using AuthorizationStatus_External = AuthorizationStatus;

AuthorizationStatus_Internal to_internal_api(AuthorizationStatus_External const& val);
AuthorizationStatus_External to_external_api(AuthorizationStatus_Internal const& val);

using CertificateStatus_Internal = ::types::authorization::CertificateStatus;
using CertificateStatus_External = CertificateStatus;

CertificateStatus_Internal to_internal_api(CertificateStatus_External const& val);
CertificateStatus_External to_external_api(CertificateStatus_Internal const& val);

using TokenAction_Internal = ::types::authorization::TokenActionStatus;
using TokenAction_External = TokenAction;

TokenAction_Internal to_internal_api(TokenAction_External const& val);
TokenAction_External to_external_api(TokenAction_Internal const& val);

using SelectionAlgorithm_Internal = ::types::authorization::SelectionAlgorithm;
using SelectionAlgorithm_External = SelectionAlgorithm;

SelectionAlgorithm_Internal to_internal_api(SelectionAlgorithm_External const& val);
SelectionAlgorithm_External to_external_api(SelectionAlgorithm_Internal const& val);

using AuthorizationType_Internal = ::types::authorization::AuthorizationType;
using AuthorizationType_External = AuthorizationType;

AuthorizationType_Internal to_internal_api(AuthorizationType_External const& val);
AuthorizationType_External to_external_api(AuthorizationType_Internal const& val);

using IdTokenType_Internal = ::types::authorization::IdTokenType;
using IdTokenType_External = IdTokenType;

IdTokenType_Internal to_internal_api(IdTokenType_External const& val);
IdTokenType_External to_external_api(IdTokenType_Internal const& val);

using WithdrawAuthorizationResult_Internal = ::types::authorization::WithdrawAuthorizationResult;
using WithdrawAuthorizationResult_External = WithdrawAuthorizationResult;

WithdrawAuthorizationResult_Internal to_internal_api(WithdrawAuthorizationResult_External const& val);
WithdrawAuthorizationResult_External to_external_api(WithdrawAuthorizationResult_Internal const& val);

using CustomIdToken_Internal = ::types::authorization::CustomIdToken;
using CustomIdToken_External = CustomIdToken;

CustomIdToken_Internal to_internal_api(CustomIdToken_External const& val);
CustomIdToken_External to_external_api(CustomIdToken_Internal const& val);

using IdToken_Internal = ::types::authorization::IdToken;
using IdToken_External = IdToken;

IdToken_Internal to_internal_api(IdToken_External const& val);
IdToken_External to_external_api(IdToken_Internal const& val);

using ProvidedIdToken_Internal = ::types::authorization::ProvidedIdToken;
using ProvidedIdToken_External = ProvidedIdToken;

ProvidedIdToken_Internal to_internal_api(ProvidedIdToken_External const& val);
ProvidedIdToken_External to_external_api(ProvidedIdToken_Internal const& val);

using TokenActionMessage_Internal = ::types::authorization::TokenActionMessage;
using TokenActionMessage_External = TokenActionMessage;

TokenActionMessage_Internal to_internal_api(TokenActionMessage_External const& val);
TokenActionMessage_External to_external_api(TokenActionMessage_Internal const& val);

using TokenValidationMessage_Internal = ::types::authorization::TokenValidationMessage;
using TokenValidationMessage_External = TokenValidationMessage;

TokenValidationMessage_Internal to_internal_api(TokenValidationMessage_External const& val);
TokenValidationMessage_External to_external_api(TokenValidationMessage_Internal const& val);

using ValidationResult_Internal = ::types::authorization::ValidationResult;
using ValidationResult_External = ValidationResult;

ValidationResult_Internal to_internal_api(ValidationResult_External const& val);
ValidationResult_External to_external_api(ValidationResult_Internal const& val);

using ValidationResultUpdate_Internal = ::types::authorization::ValidationResultUpdate;
using ValidationResultUpdate_External = ValidationResultUpdate;

ValidationResultUpdate_Internal to_internal_api(ValidationResultUpdate_External const& val);
ValidationResultUpdate_External to_external_api(ValidationResultUpdate_Internal const& val);

using WithdrawAuthorizationRequest_Internal = ::types::authorization::WithdrawAuthorizationRequest;
using WithdrawAuthorizationRequest_External = WithdrawAuthorizationRequest;

WithdrawAuthorizationRequest_Internal to_internal_api(WithdrawAuthorizationRequest_External const& val);
WithdrawAuthorizationRequest_External to_external_api(WithdrawAuthorizationRequest_Internal const& val);

} // namespace everest::lib::API::V1_0::types::auth
