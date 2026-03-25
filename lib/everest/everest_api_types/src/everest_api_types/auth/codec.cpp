// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "auth/codec.hpp"
#include "auth/API.hpp"
#include "auth/json_codec.hpp"
#include "nlohmann/json.hpp"
#include "utilities/constants.hpp"
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::auth {

std::string serialize(AuthorizationStatus val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(CertificateStatus val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(TokenValidationStatus val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(SelectionAlgorithm val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(AuthorizationType val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(IdTokenType val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(CustomIdToken const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(IdToken const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(WithdrawAuthorizationResult val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ProvidedIdToken const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(TokenValidationStatusMessage const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ValidationResult const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ValidationResultUpdate const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(WithdrawAuthorizationRequest const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::ostream& operator<<(std::ostream& os, AuthorizationStatus const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, CertificateStatus const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, TokenValidationStatus const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, SelectionAlgorithm const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, AuthorizationType const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, IdTokenType const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, WithdrawAuthorizationResult const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, CustomIdToken const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, IdToken const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ProvidedIdToken const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, TokenValidationStatusMessage const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ValidationResult const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ValidationResultUpdate const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, WithdrawAuthorizationRequest const& val) {
    os << serialize(val);
    return os;
}

template <> AuthorizationStatus deserialize(std::string const& val) {
    return json::parse(val);
}

template <> CertificateStatus deserialize<>(std::string const& val) {
    return json::parse(val);
}

template <> TokenValidationStatus deserialize<>(std::string const& val) {
    return json::parse(val);
}

template <> SelectionAlgorithm deserialize<>(std::string const& val) {
    return json::parse(val);
}

template <> AuthorizationType deserialize<>(std::string const& val) {
    return json::parse(val);
}

template <> IdTokenType deserialize<>(std::string const& val) {
    return json::parse(val);
}

template <> WithdrawAuthorizationResult deserialize<>(std::string const& val) {
    return json::parse(val);
}

template <> CustomIdToken deserialize<>(const std::string& val) {
    return json::parse(val);
}

template <> IdToken deserialize<>(const std::string& val) {
    return json::parse(val);
}

template <> ProvidedIdToken deserialize<>(const std::string& val) {
    return json::parse(val);
}

template <> TokenValidationStatusMessage deserialize<>(const std::string& val) {
    return json::parse(val);
}

template <> ValidationResult deserialize<>(const std::string& val) {
    return json::parse(val);
}

template <> ValidationResultUpdate deserialize<>(const std::string& val) {
    return json::parse(val);
}

template <> WithdrawAuthorizationRequest deserialize<>(std::string const& val) {
    return json::parse(val);
}

} // namespace everest::lib::API::V1_0::types::auth
