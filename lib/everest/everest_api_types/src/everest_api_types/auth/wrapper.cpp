// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "auth/wrapper.hpp"
#include "auth/API.hpp"
#include "display_message/wrapper.hpp"
#include <vector>

namespace everest::lib::API::V1_0::types {

namespace {
using namespace auth;
using namespace display_message;
template <class SrcT, class ConvT>
auto srcToTarOpt(std::optional<SrcT> const& src, ConvT const& converter)
    -> std::optional<decltype(converter(src.value()))> {
    if (src) {
        return std::make_optional(converter(src.value()));
    }
    return std::nullopt;
}

template <class SrcT, class ConvT> auto srcToTarVec(std::vector<SrcT> const& src, ConvT const& converter) {
    using TarT = decltype(converter(src[0]));
    std::vector<TarT> result;
    for (SrcT const& elem : src) {
        result.push_back(converter(elem));
    }
    return result;
}

template <class SrcT>
auto optToInternal(std::optional<SrcT> const& src) -> std::optional<decltype(toInternalApi(src.value()))> {
    return srcToTarOpt(src, [](SrcT const& val) { return toInternalApi(val); });
}

template <class SrcT>
auto optToExternal(std::optional<SrcT> const& src) -> std::optional<decltype(toExternalApi(src.value()))> {
    return srcToTarOpt(src, [](SrcT const& val) { return toExternalApi(val); });
}

template <class SrcT> auto vecToExternal(std::vector<SrcT> const& src) {
    return srcToTarVec(src, [](SrcT const& val) { return toExternalApi(val); });
}

template <class SrcT> auto vecToInternal(std::vector<SrcT> const& src) {
    return srcToTarVec(src, [](SrcT const& val) { return toInternalApi(val); });
}
} // namespace

namespace auth {

AuthorizationStatus_Internal toInternalApi(AuthorizationStatus_External const& val) {
    using SrcT = AuthorizationStatus_External;
    using TarT = AuthorizationStatus_Internal;
    switch (val) {
    case SrcT::Accepted:
        return TarT::Accepted;
    case SrcT::Blocked:
        return TarT::Blocked;
    case SrcT::ConcurrentTx:
        return TarT::ConcurrentTx;
    case SrcT::Expired:
        return TarT::Expired;
    case SrcT::Invalid:
        return TarT::Invalid;
    case SrcT::NoCredit:
        return TarT::NoCredit;
    case SrcT::NotAllowedTypeEVSE:
        return TarT::NotAllowedTypeEVSE;
    case SrcT::NotAtThisLocation:
        return TarT::NotAtThisLocation;
    case SrcT::NotAtThisTime:
        return TarT::NotAtThisTime;
    case SrcT::Unknown:
        return TarT::Unknown;
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::auth::AuthorizationStatus_External");
}

AuthorizationStatus_External toExternalApi(AuthorizationStatus_Internal const& val) {
    using SrcT = AuthorizationStatus_Internal;
    using TarT = AuthorizationStatus_External;
    switch (val) {
    case SrcT::Accepted:
        return TarT::Accepted;
    case SrcT::Blocked:
        return TarT::Blocked;
    case SrcT::ConcurrentTx:
        return TarT::ConcurrentTx;
    case SrcT::Expired:
        return TarT::Expired;
    case SrcT::Invalid:
        return TarT::Invalid;
    case SrcT::NoCredit:
        return TarT::NoCredit;
    case SrcT::NotAllowedTypeEVSE:
        return TarT::NotAllowedTypeEVSE;
    case SrcT::NotAtThisLocation:
        return TarT::NotAtThisLocation;
    case SrcT::NotAtThisTime:
        return TarT::NotAtThisTime;
    case SrcT::Unknown:
        return TarT::Unknown;
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::auth::AuthorizationStatus_Internal");
}

CertificateStatus_Internal toInternalApi(CertificateStatus_External const& val) {
    using SrcT = CertificateStatus_External;
    using TarT = CertificateStatus_Internal;
    switch (val) {
    case SrcT::Accepted:
        return TarT::Accepted;
    case SrcT::SignatureError:
        return TarT::SignatureError;
    case SrcT::CertificateExpired:
        return TarT::CertificateExpired;
    case SrcT::CertificateRevoked:
        return TarT::CertificateRevoked;
    case SrcT::NoCertificateAvailable:
        return TarT::NoCertificateAvailable;
    case SrcT::CertChainError:
        return TarT::CertChainError;
    case SrcT::ContractCancelled:
        return TarT::ContractCancelled;
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::auth::CertifgicateStatus_External");
}
CertificateStatus_External toExternalApi(CertificateStatus_Internal const& val) {
    using SrcT = CertificateStatus_Internal;
    using TarT = CertificateStatus_External;
    switch (val) {
    case SrcT::Accepted:
        return TarT::Accepted;
    case SrcT::SignatureError:
        return TarT::SignatureError;
    case SrcT::CertificateExpired:
        return TarT::CertificateExpired;
    case SrcT::CertificateRevoked:
        return TarT::CertificateRevoked;
    case SrcT::NoCertificateAvailable:
        return TarT::NoCertificateAvailable;
    case SrcT::CertChainError:
        return TarT::CertChainError;
    case SrcT::ContractCancelled:
        return TarT::ContractCancelled;
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::auth::CertifgicateStatus_Internal");
}

TokenValidationStatus_Internal toInternalApi(TokenValidationStatus_External const& val) {
    using SrcT = TokenValidationStatus_External;
    using TarT = TokenValidationStatus_Internal;
    switch (val) {
    case SrcT::Processing:
        return TarT::Processing;
    case SrcT::Accepted:
        return TarT::Accepted;
    case SrcT::Rejected:
        return TarT::Rejected;
    case SrcT::TimedOut:
        return TarT::TimedOut;
    case SrcT::Withdrawn:
        return TarT::Withdrawn;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::auth::TokenValidationStatus_External");
}
TokenValidationStatus_External toExternalApi(TokenValidationStatus_Internal const& val) {
    using SrcT = TokenValidationStatus_Internal;
    using TarT = TokenValidationStatus_External;
    switch (val) {
    case SrcT::Processing:
        return TarT::Processing;
    case SrcT::Accepted:
        return TarT::Accepted;
    case SrcT::Rejected:
        return TarT::Rejected;
    case SrcT::TimedOut:
        return TarT::TimedOut;
    case SrcT::Withdrawn:
        return TarT::Withdrawn;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::auth::TokenValidationStatus_Internal");
}

SelectionAlgorithm_Internal toInternalApi(SelectionAlgorithm_External const& val) {
    using SrcT = SelectionAlgorithm_External;
    using TarT = SelectionAlgorithm_Internal;
    switch (val) {
    case SrcT::UserInput:
        return TarT::UserInput;
    case SrcT::PlugEvents:
        return TarT::PlugEvents;
    case SrcT::FindFirst:
        return TarT::FindFirst;
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::auth::SelectedAlgorithm_External");
}
SelectionAlgorithm_External toExternalApi(SelectionAlgorithm_Internal const& val) {
    using SrcT = SelectionAlgorithm_Internal;
    using TarT = SelectionAlgorithm_External;
    switch (val) {
    case SrcT::UserInput:
        return TarT::UserInput;
    case SrcT::PlugEvents:
        return TarT::PlugEvents;
    case SrcT::FindFirst:
        return TarT::FindFirst;
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::auth::SelectedAlgorithm_Internal");
}

AuthorizationType_Internal toInternalApi(AuthorizationType_External const& val) {
    using SrcT = AuthorizationType_External;
    using TarT = AuthorizationType_Internal;
    switch (val) {
    case SrcT::OCPP:
        return TarT::OCPP;
    case SrcT::RFID:
        return TarT::RFID;
    case SrcT::Autocharge:
        return TarT::Autocharge;
    case SrcT::PlugAndCharge:
        return TarT::PlugAndCharge;
    case SrcT::BankCard:
        return TarT::BankCard;
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::auth::AuthoriozationType_External");
}

AuthorizationType_External toExternalApi(AuthorizationType_Internal const& val) {
    using SrcT = AuthorizationType_Internal;
    using TarT = AuthorizationType_External;
    switch (val) {
    case SrcT::OCPP:
        return TarT::OCPP;
    case SrcT::RFID:
        return TarT::RFID;
    case SrcT::Autocharge:
        return TarT::Autocharge;
    case SrcT::PlugAndCharge:
        return TarT::PlugAndCharge;
    case SrcT::BankCard:
        return TarT::BankCard;
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::auth::AuthoriozationType_Internal");
}

IdTokenType_Internal toInternalApi(IdTokenType_External const& val) {
    using SrcT = IdTokenType_External;
    using TarT = IdTokenType_Internal;
    switch (val) {
    case SrcT::Central:
        return TarT::Central;
    case SrcT::eMAID:
        return TarT::eMAID;
    case SrcT::MacAddress:
        return TarT::MacAddress;
    case SrcT::ISO14443:
        return TarT::ISO14443;
    case SrcT::ISO15693:
        return TarT::ISO15693;
    case SrcT::KeyCode:
        return TarT::KeyCode;
    case SrcT::Local:
        return TarT::Local;
    case SrcT::NoAuthorization:
        return TarT::NoAuthorization;
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::auth::IdTokenType_External");
}

IdTokenType_External toExternalApi(IdTokenType_Internal const& val) {
    using SrcT = IdTokenType_Internal;
    using TarT = IdTokenType_External;
    switch (val) {
    case SrcT::Central:
        return TarT::Central;
    case SrcT::eMAID:
        return TarT::eMAID;
    case SrcT::MacAddress:
        return TarT::MacAddress;
    case SrcT::ISO14443:
        return TarT::ISO14443;
    case SrcT::ISO15693:
        return TarT::ISO15693;
    case SrcT::KeyCode:
        return TarT::KeyCode;
    case SrcT::Local:
        return TarT::Local;
    case SrcT::NoAuthorization:
        return TarT::NoAuthorization;
    }
    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::auth::IdTokenType_Internal");
}

WithdrawAuthorizationResult_Internal toInternalApi(WithdrawAuthorizationResult_External const& val) {
    using SrcT = WithdrawAuthorizationResult_External;
    using TarT = WithdrawAuthorizationResult_Internal;
    switch (val) {
    case SrcT::Accepted:
        return TarT::Accepted;
    case SrcT::AuthorizationNotFound:
        return TarT::AuthorizationNotFound;
    case SrcT::EvseNotFound:
        return TarT::EvseNotFound;
    case SrcT::Rejected:
        return TarT::Rejected;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::auth::WithdrawAuthorizationResult_External");
}
WithdrawAuthorizationResult_External toExternalApi(WithdrawAuthorizationResult_Internal const& val) {
    using SrcT = WithdrawAuthorizationResult_Internal;
    using TarT = WithdrawAuthorizationResult_External;
    switch (val) {
    case SrcT::Accepted:
        return TarT::Accepted;
    case SrcT::AuthorizationNotFound:
        return TarT::AuthorizationNotFound;
    case SrcT::EvseNotFound:
        return TarT::EvseNotFound;
    case SrcT::Rejected:
        return TarT::Rejected;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::auth::WithdrawAuthorizationResult_Internal");
}

////////

CustomIdToken_Internal toInternalApi(CustomIdToken_External const& val) {
    CustomIdToken_Internal result;
    result.value = val.value;
    result.type = val.type;
    return result;
}

CustomIdToken_External toExternalApi(CustomIdToken_Internal const& val) {
    CustomIdToken_External result;
    result.value = val.value;
    result.type = val.type;
    return result;
}

IdToken_Internal toInternalApi(IdToken_External const& val) {
    IdToken_Internal result;
    result.value = val.value;
    result.type = toInternalApi(val.type);
    if (val.additional_info) {
        std::vector<CustomIdToken_Internal> vec;
        for (auto const& item : val.additional_info.value()) {
            vec.push_back(toInternalApi(item));
        }
        result.additional_info.emplace(vec);
    }
    return result;
}

IdToken_External toExternalApi(IdToken_Internal const& val) {
    IdToken_External result;
    result.value = val.value;
    result.type = toExternalApi(val.type);
    if (val.additional_info) {
        std::vector<CustomIdToken_External> vec;
        for (auto const& item : val.additional_info.value()) {
            vec.push_back(toExternalApi(item));
        }
        result.additional_info.emplace(vec);
    }
    return result;
}

ProvidedIdToken_Internal toInternalApi(ProvidedIdToken_External const& val) {
    ProvidedIdToken_Internal result;
    result.id_token = toInternalApi(val.id_token);
    result.authorization_type = toInternalApi(val.authorization_type);
    result.request_id = val.request_id;
    result.parent_id_token = optToInternal(val.parent_id_token);
    result.connectors = val.connectors;
    result.prevalidated = val.prevalidated;
    result.certificate = val.certificate;
    return result;
}

ProvidedIdToken_External toExternalApi(ProvidedIdToken_Internal const& val) {
    ProvidedIdToken_External result;
    result.id_token = toExternalApi(val.id_token);
    result.authorization_type = toExternalApi(val.authorization_type);
    result.request_id = val.request_id;
    result.parent_id_token = optToExternal(val.parent_id_token);
    result.connectors = val.connectors;
    result.prevalidated = val.prevalidated;
    result.certificate = val.certificate;
    return result;
}

TokenValidationStatusMessage_Internal toInternalApi(TokenValidationStatusMessage_External const& val) {
    TokenValidationStatusMessage_Internal result;
    result.token = toInternalApi(val.token);
    result.status = toInternalApi(val.status);
    if (val.messages) {
        result.messages = vecToInternal(val.messages.value());
    }
    return result;
}

TokenValidationStatusMessage_External toExternalApi(TokenValidationStatusMessage_Internal const& val) {
    TokenValidationStatusMessage_External result;
    result.token = toExternalApi(val.token);
    result.status = toExternalApi(val.status);
    if (val.messages) {
        result.messages = vecToExternal(val.messages.value());
    }
    return result;
}

ValidationResult_Internal toInternalApi(ValidationResult_External const& val) {
    ValidationResult_Internal result;
    result.authorization_status = toInternalApi(val.authorization_status);
    result.certificate_status = optToInternal(val.certificate_status);
    //    Removed due to incompatible changes in everest-core. Never contained any useful information.
    //    result.reason = optToInternal(val.reason);
    result.expiry_time = val.expiry_time;
    result.parent_id_token = optToInternal(val.parent_id_token);
    result.evse_ids = val.evse_ids;
    result.reservation_id = val.reservation_id;
    return result;
}

ValidationResult_External toExternalApi(ValidationResult_Internal const& val) {
    ValidationResult_External result;
    result.authorization_status = toExternalApi(val.authorization_status);
    result.certificate_status = optToExternal(val.certificate_status);
    //    Removed due to incompatible changes in everest-core. Never contained any useful information.
    //    result.reason = optToExternal(val.reason);
    result.expiry_time = val.expiry_time;
    result.parent_id_token = optToExternal(val.parent_id_token);
    result.evse_ids = val.evse_ids;
    result.reservation_id = val.reservation_id;
    return result;
}

ValidationResultUpdate_Internal toInternalApi(ValidationResultUpdate_External const& val) {
    ValidationResultUpdate_Internal result;
    result.validation_result = toInternalApi(val.validation_result);
    result.connector_id = val.connector_id;
    return result;
}

ValidationResultUpdate_External toExternalApi(ValidationResultUpdate_Internal const& val) {
    ValidationResultUpdate_External result;
    result.validation_result = toExternalApi(val.validation_result);
    result.connector_id = val.connector_id;
    return result;
}

WithdrawAuthorizationRequest_Internal toInternalApi(WithdrawAuthorizationRequest_External const& val) {
    WithdrawAuthorizationRequest_Internal result;
    result.evse_id = val.evse_id;
    result.id_token = optToInternal(val.id_token);
    return result;
}

WithdrawAuthorizationRequest_External toExternalApi(WithdrawAuthorizationRequest_Internal const& val) {
    WithdrawAuthorizationRequest_External result;
    result.evse_id = val.evse_id;
    result.id_token = optToExternal(val.id_token);
    return result;
}

} // namespace auth
} // namespace everest::lib::API::V1_0::types
