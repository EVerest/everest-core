// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "display_message/json_codec.hpp"
#include "auth/API.hpp"
#include "auth/codec.hpp"
#include "auth/json_codec.hpp"
#include "iso15118_charger/json_codec.hpp"
#include "nlohmann/json.hpp"

namespace everest::lib::API::V1_0::types::auth {

void to_json(json& j, AuthorizationStatus const& k) noexcept {
    switch (k) {
    case AuthorizationStatus::Accepted:
        j = "Accepted";
        return;
    case AuthorizationStatus::Blocked:
        j = "Blocked";
        return;
    case AuthorizationStatus::ConcurrentTx:
        j = "ConcurrentTx";
        return;
    case AuthorizationStatus::Expired:
        j = "Expired";
        return;
    case AuthorizationStatus::Invalid:
        j = "Invalid";
        return;
    case AuthorizationStatus::NoCredit:
        j = "NoCredit";
        return;
    case AuthorizationStatus::NotAllowedTypeEVSE:
        j = "NotAllowedTypeEVSE";
        return;
    case AuthorizationStatus::NotAtThisLocation:
        j = "NotAtThisLocation";
        return;
    case AuthorizationStatus::NotAtThisTime:
        j = "NotAtThisTime";
        return;
    case AuthorizationStatus::Unknown:
        j = "Unknown";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::auth::AuthorizationStatus";
}

void from_json(const json& j, AuthorizationStatus& k) {
    std::string s = j;
    if (s == "Accepted") {
        k = AuthorizationStatus::Accepted;
        return;
    }
    if (s == "Blocked") {
        k = AuthorizationStatus::Blocked;
        return;
    }
    if (s == "ConcurrentTx") {
        k = AuthorizationStatus::ConcurrentTx;
        return;
    }
    if (s == "Expired") {
        k = AuthorizationStatus::Expired;
        return;
    }
    if (s == "Invalid") {
        k = AuthorizationStatus::Invalid;
        return;
    }
    if (s == "NoCredit") {
        k = AuthorizationStatus::NoCredit;
        return;
    }
    if (s == "NotAllowedTypeEVSE") {
        k = AuthorizationStatus::NotAllowedTypeEVSE;
        return;
    }
    if (s == "NotAtThisLocation") {
        k = AuthorizationStatus::NotAtThisLocation;
        return;
    }
    if (s == "NotAtThisTime") {
        k = AuthorizationStatus::NotAtThisTime;
        return;
    }
    if (s == "Unknown") {
        k = AuthorizationStatus::Unknown;
        return;
    }

    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::auth::AuthorizationStatus");
}

void to_json(json& j, CertificateStatus const& k) noexcept {
    switch (k) {
    case CertificateStatus::Accepted:
        j = "Accepted";
        return;
    case CertificateStatus::SignatureError:
        j = "SignatureError";
        return;
    case CertificateStatus::CertificateExpired:
        j = "CertificateExpired";
        return;
    case CertificateStatus::CertificateRevoked:
        j = "CertificateRevoked";
        return;
    case CertificateStatus::NoCertificateAvailable:
        j = "NoCertificateAvailable";
        return;
    case CertificateStatus::CertChainError:
        j = "CertChainError";
        return;
    case CertificateStatus::ContractCancelled:
        j = "ContractCancelled";
        return;
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::auth::CertificateStatus";
}

void from_json(const json& j, CertificateStatus& k) {
    std::string s = j;
    if (s == "Accepted") {
        k = CertificateStatus::Accepted;
        return;
    }
    if (s == "SignatureError") {
        k = CertificateStatus::SignatureError;
        return;
    }
    if (s == "CertificateExpired") {
        k = CertificateStatus::CertificateExpired;
        return;
    }
    if (s == "CertificateRevoked") {
        k = CertificateStatus::CertificateRevoked;
        return;
    }
    if (s == "NoCertificateAvailable") {
        k = CertificateStatus::NoCertificateAvailable;
        return;
    }
    if (s == "CertChainError") {
        k = CertificateStatus::CertChainError;
        return;
    }
    if (s == "ContractCancelled") {
        k = CertificateStatus::ContractCancelled;
        return;
    }

    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::auth::CertificateStatus");
}

void to_json(json& j, TokenValidationStatus const& k) noexcept {
    switch (k) {
    case TokenValidationStatus::Processing:
        j = "Processing";
        return;
    case TokenValidationStatus::Accepted:
        j = "Accepted";
        return;
    case TokenValidationStatus::Rejected:
        j = "Rejected";
        return;
    case TokenValidationStatus::TimedOut:
        j = "TimedOut";
        return;
    case TokenValidationStatus::Withdrawn:
        j = "Withdrawn";
        return;
    case TokenValidationStatus::UsedToStart:
        j = "UsedToStart";
        return;
    case TokenValidationStatus::UsedToStop:
        j = "UsedToStop";
        return;
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::auth::TokenValidationStatus";
}

void from_json(const json& j, TokenValidationStatus& k) {
    std::string s = j;
    if (s == "Processing") {
        k = TokenValidationStatus::Processing;
        return;
    }
    if (s == "Accepted") {
        k = TokenValidationStatus::Accepted;
        return;
    }
    if (s == "Rejected") {
        k = TokenValidationStatus::Rejected;
        return;
    }
    if (s == "TimedOut") {
        k = TokenValidationStatus::TimedOut;
        return;
    }
    if (s == "Withdrawn") {
        k = TokenValidationStatus::Withdrawn;
        return;
    }
    if (s == "UsedToStart") {
        k = TokenValidationStatus::UsedToStart;
        return;
    }
    if (s == "UsedToStop") {
        k = TokenValidationStatus::UsedToStop;
        return;
    }
    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::auth::TokenValidationStatus");
}

void to_json(json& j, SelectionAlgorithm const& k) noexcept {
    switch (k) {
    case SelectionAlgorithm::UserInput:
        j = "UserInput";
        return;
    case SelectionAlgorithm::PlugEvents:
        j = "PlugEvents";
        return;
    case SelectionAlgorithm::FindFirst:
        j = "FindFirst";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::auth::SelectedAlgorithm";
}

void from_json(const json& j, SelectionAlgorithm& k) {
    std::string s = j;
    if (s == "UserInput") {
        k = SelectionAlgorithm::UserInput;
        return;
    }
    if (s == "PlugEvents") {
        k = SelectionAlgorithm::PlugEvents;
        return;
    }
    if (s == "FindFirst") {
        k = SelectionAlgorithm::FindFirst;
        return;
    }

    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::auth::SelectionAlgorithm");
}

void to_json(json& j, AuthorizationType const& k) noexcept {
    switch (k) {
    case AuthorizationType::OCPP:
        j = "OCPP";
        return;
    case AuthorizationType::RFID:
        j = "RFID";
        return;
    case AuthorizationType::Autocharge:
        j = "Autocharge";
        return;
    case AuthorizationType::PlugAndCharge:
        j = "PlugAndCharge";
        return;
    case AuthorizationType::BankCard:
        j = "BankCard";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::auth::AuthorizationType";
}

void from_json(const json& j, AuthorizationType& k) {
    std::string s = j;
    if (s == "OCPP") {
        k = AuthorizationType::OCPP;
        return;
    }
    if (s == "RFID") {
        k = AuthorizationType::RFID;
        return;
    }
    if (s == "Autocharge") {
        k = AuthorizationType::Autocharge;
        return;
    }
    if (s == "PlugAndCharge") {
        k = AuthorizationType::PlugAndCharge;
        return;
    }
    if (s == "BankCard") {
        k = AuthorizationType::BankCard;
        return;
    }

    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::auth::AuthorizationType");
}

void to_json(json& j, IdTokenType const& k) noexcept {
    switch (k) {
    case IdTokenType::Central:
        j = "Central";
        return;
    case IdTokenType::eMAID:
        j = "eMAID";
        return;
    case IdTokenType::MacAddress:
        j = "MacAddress";
        return;
    case IdTokenType::ISO14443:
        j = "ISO14443";
        return;
    case IdTokenType::ISO15693:
        j = "ISO15693";
        return;
    case IdTokenType::KeyCode:
        j = "KeyCode";
        return;
    case IdTokenType::Local:
        j = "Local";
        return;
    case IdTokenType::NoAuthorization:
        j = "NoAuthorization";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::auth::IdTokenType";
}

void from_json(const json& j, IdTokenType& k) {
    std::string s = j;
    if (s == "Central") {
        k = IdTokenType::Central;
        return;
    }
    if (s == "eMAID") {
        k = IdTokenType::eMAID;
        return;
    }
    if (s == "MacAddress") {
        k = IdTokenType::MacAddress;
        return;
    }
    if (s == "ISO14443") {
        k = IdTokenType::ISO14443;
        return;
    }
    if (s == "ISO15693") {
        k = IdTokenType::ISO15693;
        return;
    }
    if (s == "KeyCode") {
        k = IdTokenType::KeyCode;
        return;
    }
    if (s == "Local") {
        k = IdTokenType::Local;
        return;
    }
    if (s == "NoAuthorization") {
        k = IdTokenType::NoAuthorization;
        return;
    }

    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::auth::IdTokenType");
}

void to_json(json& j, WithdrawAuthorizationResult const& k) noexcept {
    switch (k) {
    case WithdrawAuthorizationResult::Accepted:
        j = "Accepted";
        return;
    case WithdrawAuthorizationResult::AuthorizationNotFound:
        j = "AuthorizationNotFound";
        return;
    case WithdrawAuthorizationResult::EvseNotFound:
        j = "EvseNotFound";
        return;
    case WithdrawAuthorizationResult::Rejected:
        j = "Rejected";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::auth::WithdrawAuthorizationResult";
}

void from_json(const json& j, WithdrawAuthorizationResult& k) {
    std::string s = j;
    if (s == "Accepted") {
        k = WithdrawAuthorizationResult::Accepted;
        return;
    }
    if (s == "AuthorizationNotFound") {
        k = WithdrawAuthorizationResult::AuthorizationNotFound;
        return;
    }
    if (s == "EvseNotFound") {
        k = WithdrawAuthorizationResult::EvseNotFound;
        return;
    }
    if (s == "Rejected") {
        k = WithdrawAuthorizationResult::Rejected;
        return;
    }

    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::auth::WithdrawAuthorizationResult");
}

void to_json(json& j, CustomIdToken const& k) noexcept {
    j = json{
        {"value", k.value},
        {"type", k.type},
    };
}

void from_json(const json& j, CustomIdToken& k) {
    k.value = j.at("value");
    k.type = j.at("type");
}

void to_json(json& j, IdToken const& k) noexcept {
    j = json{
        {"value", k.value},
        {"type", k.type},
    };
    if (k.additional_info) {
        j["additional_info"] = json::array();
        for (auto val : k.additional_info.value()) {
            j["additional_info"].push_back(val);
        }

        //        j["additional_info"] = k.additional_info.value();
    }
}

void from_json(const json& j, IdToken& k) {
    k.value = j.at("value");
    k.type = j.at("type");

    if (j.contains("additional_info")) {
        json arr = j.at("additional_info");
        std::vector<CustomIdToken> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.additional_info.emplace(vec);
    }
}

void to_json(json& j, ProvidedIdToken const& k) noexcept {
    j = json{
        {"id_token", k.id_token},
        {"authorization_type", k.authorization_type},
    };

    if (k.request_id) {
        j["request_id"] = k.request_id.value();
    }
    if (k.parent_id_token) {
        j["parent_id_token"] = k.parent_id_token.value();
    }
    if (k.connectors) {
        j["connectors"] = json::array();
        for (auto val : k.connectors.value()) {
            j["connectors"].push_back(val);
        }
    }
    if (k.prevalidated) {
        j["prevalidated"] = k.prevalidated.value();
    }
    if (k.certificate) {
        j["certificate"] = k.certificate.value();
    }
    if (k.iso15118CertificateHashData) {
        j["iso15118CertificateHashData"] = json::array();
        for (auto val : k.iso15118CertificateHashData.value()) {
            j["iso15118CertificateHashData"].push_back(val);
        }
    }
}

void from_json(const json& j, ProvidedIdToken& k) {
    k.id_token = j.at("id_token");
    k.authorization_type = j.at("authorization_type");

    if (j.contains("request_id")) {
        k.request_id.emplace(j.at("request_id"));
    }
    if (j.contains("parent_id_token")) {
        k.parent_id_token.emplace(j.at("parent_id_token"));
    }
    if (j.contains("connectors")) {
        json arr = j.at("connectors");
        std::vector<int32_t> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.connectors.emplace(vec);
    }
    if (j.contains("prevalidated")) {
        k.prevalidated.emplace(j.at("prevalidated"));
    }
    if (j.contains("certificate")) {
        k.certificate.emplace(j.at("certificate"));
    }
    if (j.contains("iso15118CertificateHashData")) {
        json arr = j.at("iso15118CertificateHashData");
        std::vector<iso15118_charger::CertificateHashDataInfo> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.iso15118CertificateHashData.emplace(vec);
    }
}

void to_json(json& j, TokenValidationStatusMessage const& k) noexcept {
    j = json{
        {"token", k.token},
        {"status", k.status},
    };
    if (k.messages) {
        j["messages"] = json::array();
        for (auto val : k.messages.value()) {
            j["messages"].push_back(val);
        }
    }
}

void from_json(const json& j, TokenValidationStatusMessage& k) {
    k.token = j.at("token");
    k.status = j.at("status");
    if (j.contains("messages")) {
        json arr = j.at("messages");
        std::vector<types::display_message::MessageContent> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.messages.emplace(vec);
    }
}

void to_json(json& j, ValidationResult const& k) noexcept {
    j = json{
        {"authorization_status", k.authorization_status},
    };

    if (k.certificate_status) {
        j["certificate_status"] = k.certificate_status.value();
    }
    if (k.expiry_time) {
        j["expiry_time"] = k.expiry_time.value();
    }
    if (k.parent_id_token) {
        j["parent_id_token"] = k.parent_id_token.value();
    }
    if (k.evse_ids) {
        j["evse_ids"] = json::array();
        for (auto val : k.evse_ids.value()) {
            j["evse_ids"].push_back(val);
        }
    }
    if (k.reservation_id) {
        j["reservation_id"] = k.reservation_id.value();
    }
}

void from_json(const json& j, ValidationResult& k) {
    k.authorization_status = j.at("authorization_status");

    if (j.contains("certificate_status")) {
        k.certificate_status.emplace(j.at("certificate_status"));
    }
    if (j.contains("expiry_time")) {
        k.expiry_time.emplace(j.at("expiry_time"));
    }
    if (j.contains("parent_id_token")) {
        k.parent_id_token.emplace(j.at("parent_id_token"));
    }
    if (j.contains("evse_ids")) {
        json arr = j.at("evse_ids");
        std::vector<int32_t> vec;
        for (auto val : arr) {
            vec.push_back(val);
        }
        k.evse_ids.emplace(vec);
    }
    if (j.contains("reservation_id")) {
        k.reservation_id.emplace(j.at("reservation_id"));
    }
}

void to_json(json& j, ValidationResultUpdate const& k) noexcept {
    j = json{
        {"validation_result", k.validation_result},
        {"connector_id", k.connector_id},
    };
}

void from_json(const json& j, ValidationResultUpdate& k) {
    k.validation_result = j.at("validation_result");
    k.connector_id = j.at("connector_id");
}

void to_json(json& j, WithdrawAuthorizationRequest const& k) noexcept {
    j = json({});
    if (k.evse_id) {
        j["evse_id"] = k.evse_id.value();
    }
    if (k.id_token) {
        j["id_token"] = k.id_token.value();
    }
}

void from_json(json const& j, WithdrawAuthorizationRequest& k) {
    if (j.contains("evse_id")) {
        k.evse_id.emplace(j.at("evse_id"));
    }
    if (j.contains("id_token")) {
        k.id_token.emplace(j.at("id_token"));
    }
}

} // namespace everest::lib::API::V1_0::types::auth
