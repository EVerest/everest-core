// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once
#include <everest_api_types/display_message/API.hpp>
#include <everest_api_types/iso15118_charger/API.hpp>
#include <optional>
#include <string>
#include <vector>

namespace everest::lib::API::V1_0::types::auth {

enum class AuthorizationStatus {
    Accepted,
    Blocked,
    ConcurrentTx,
    Expired,
    Invalid,
    NoCredit,
    NotAllowedTypeEVSE,
    NotAtThisLocation,
    NotAtThisTime,
    Unknown,
};

enum class CertificateStatus {
    Accepted,
    SignatureError,
    CertificateExpired,
    CertificateRevoked,
    NoCertificateAvailable,
    CertChainError,
    ContractCancelled,
};

enum class TokenValidationStatus {
    Processing,
    Accepted,
    Rejected,
    TimedOut,
    UsedToStart,
    UsedToStop,
    Withdrawn,
};

enum class SelectionAlgorithm {
    UserInput,
    PlugEvents,
    FindFirst,
};

enum class AuthorizationType {
    OCPP,
    RFID,
    Autocharge,
    PlugAndCharge,
    BankCard,
};

enum class IdTokenType {
    Central,
    eMAID,
    MacAddress,
    ISO14443,
    ISO15693,
    KeyCode,
    Local,
    NoAuthorization,
};

enum class WithdrawAuthorizationResult {
    Accepted,
    AuthorizationNotFound,
    EvseNotFound,
    Rejected,
};

struct CustomIdToken {
    std::string value;
    std::string type;
};

struct IdToken {
    std::string value;
    IdTokenType type;
    std::optional<std::vector<CustomIdToken>> additional_info;
};

struct ProvidedIdToken {
    IdToken id_token;
    AuthorizationType authorization_type;
    std::optional<int32_t> request_id;
    std::optional<IdToken> parent_id_token;
    std::optional<std::vector<int32_t>> connectors;
    std::optional<bool> prevalidated;
    std::optional<std::string> certificate;
    std::optional<std::vector<iso15118_charger::CertificateHashDataInfo>> iso15118CertificateHashData;
};

struct TokenValidationStatusMessage {
    ProvidedIdToken token;
    TokenValidationStatus status;
    std::optional<std::vector<display_message::MessageContent>> messages;
};

struct ValidationResult {
    AuthorizationStatus authorization_status;
    std::optional<CertificateStatus> certificate_status;
    std::optional<std::string> expiry_time;
    std::optional<IdToken> parent_id_token;
    std::optional<std::vector<int32_t>> evse_ids;
    std::optional<int32_t> reservation_id;
};

struct ValidationResultUpdate {
    ValidationResult validation_result;
    int32_t connector_id;
};

struct WithdrawAuthorizationRequest {
    std::optional<int32_t> evse_id;
    std::optional<IdToken> id_token;
};

} // namespace everest::lib::API::V1_0::types::auth
