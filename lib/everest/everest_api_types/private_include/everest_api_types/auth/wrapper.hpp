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

AuthorizationStatus_Internal toInternalApi(AuthorizationStatus_External const& val);
AuthorizationStatus_External toExternalApi(AuthorizationStatus_Internal const& val);

using CertificateStatus_Internal = ::types::authorization::CertificateStatus;
using CertificateStatus_External = CertificateStatus;

CertificateStatus_Internal toInternalApi(CertificateStatus_External const& val);
CertificateStatus_External toExternalApi(CertificateStatus_Internal const& val);

using TokenValidationStatus_Internal = ::types::authorization::TokenValidationStatus;
using TokenValidationStatus_External = TokenValidationStatus;

TokenValidationStatus_Internal toInternalApi(TokenValidationStatus_External const& val);
TokenValidationStatus_External toExternalApi(TokenValidationStatus_Internal const& val);

using SelectionAlgorithm_Internal = ::types::authorization::SelectionAlgorithm;
using SelectionAlgorithm_External = SelectionAlgorithm;

SelectionAlgorithm_Internal toInternalApi(SelectionAlgorithm_External const& val);
SelectionAlgorithm_External toExternalApi(SelectionAlgorithm_Internal const& val);

using AuthorizationType_Internal = ::types::authorization::AuthorizationType;
using AuthorizationType_External = AuthorizationType;

AuthorizationType_Internal toInternalApi(AuthorizationType_External const& val);
AuthorizationType_External toExternalApi(AuthorizationType_Internal const& val);

using IdTokenType_Internal = ::types::authorization::IdTokenType;
using IdTokenType_External = IdTokenType;

IdTokenType_Internal toInternalApi(IdTokenType_External const& val);
IdTokenType_External toExternalApi(IdTokenType_Internal const& val);

using WithdrawAuthorizationResult_Internal = ::types::authorization::WithdrawAuthorizationResult;
using WithdrawAuthorizationResult_External = WithdrawAuthorizationResult;

WithdrawAuthorizationResult_Internal toInternalApi(WithdrawAuthorizationResult_External const& val);
WithdrawAuthorizationResult_External toExternalApi(WithdrawAuthorizationResult_Internal const& val);

using CustomIdToken_Internal = ::types::authorization::CustomIdToken;
using CustomIdToken_External = CustomIdToken;

CustomIdToken_Internal toInternalApi(CustomIdToken_External const& val);
CustomIdToken_External toExternalApi(CustomIdToken_Internal const& val);

using IdToken_Internal = ::types::authorization::IdToken;
using IdToken_External = IdToken;

IdToken_Internal toInternalApi(IdToken_External const& val);
IdToken_External toExternalApi(IdToken_Internal const& val);

using ProvidedIdToken_Internal = ::types::authorization::ProvidedIdToken;
using ProvidedIdToken_External = ProvidedIdToken;

ProvidedIdToken_Internal toInternalApi(ProvidedIdToken_External const& val);
ProvidedIdToken_External toExternalApi(ProvidedIdToken_Internal const& val);

using TokenValidationStatusMessage_Internal = ::types::authorization::TokenValidationStatusMessage;
using TokenValidationStatusMessage_External = TokenValidationStatusMessage;

TokenValidationStatusMessage_Internal toInternalApi(TokenValidationStatusMessage_External const& val);
TokenValidationStatusMessage_External toExternalApi(TokenValidationStatusMessage_Internal const& val);

using ValidationResult_Internal = ::types::authorization::ValidationResult;
using ValidationResult_External = ValidationResult;

ValidationResult_Internal toInternalApi(ValidationResult_External const& val);
ValidationResult_External toExternalApi(ValidationResult_Internal const& val);

using ValidationResultUpdate_Internal = ::types::authorization::ValidationResultUpdate;
using ValidationResultUpdate_External = ValidationResultUpdate;

ValidationResultUpdate_Internal toInternalApi(ValidationResultUpdate_External const& val);
ValidationResultUpdate_External toExternalApi(ValidationResultUpdate_Internal const& val);

using WithdrawAuthorizationRequest_Internal = ::types::authorization::WithdrawAuthorizationRequest;
using WithdrawAuthorizationRequest_External = WithdrawAuthorizationRequest;

WithdrawAuthorizationRequest_Internal toInternalApi(WithdrawAuthorizationRequest_External const& val);
WithdrawAuthorizationRequest_External toExternalApi(WithdrawAuthorizationRequest_Internal const& val);

} // namespace everest::lib::API::V1_0::types::auth
