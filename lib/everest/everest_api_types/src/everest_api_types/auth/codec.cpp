// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "auth/codec.hpp"
#include "auth/API.hpp"
#include "auth/json_codec.hpp"
#include "nlohmann/json.hpp"
#include "utilities/constants.hpp"
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::auth {

std::string serialize(AuthorizationStatus val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(CertificateStatus val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(TokenValidationStatus val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(SelectionAlgorithm val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(AuthorizationType val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(IdTokenType val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(CustomIdToken const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(IdToken const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(WithdrawAuthorizationResult val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ProvidedIdToken const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(TokenValidationStatusMessage const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ValidationResult const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ValidationResultUpdate const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(WithdrawAuthorizationRequest const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
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
    auto data = json::parse(val);
    AuthorizationStatus obj = data;
    return obj;
}

template <> CertificateStatus deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    CertificateStatus obj = data;
    return obj;
}

template <> TokenValidationStatus deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    TokenValidationStatus obj = data;
    return obj;
}

template <> SelectionAlgorithm deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    SelectionAlgorithm obj = data;
    return obj;
}

template <> AuthorizationType deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    AuthorizationType obj = data;
    return obj;
}

template <> IdTokenType deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    IdTokenType obj = data;
    return obj;
}

template <> WithdrawAuthorizationResult deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    WithdrawAuthorizationResult obj = data;
    return obj;
}

template <> CustomIdToken deserialize<>(const std::string& val) {
    auto data = json::parse(val);
    CustomIdToken obj = data;
    return obj;
}

template <> IdToken deserialize<>(const std::string& val) {
    auto data = json::parse(val);
    IdToken obj = data;
    return obj;
}

template <> ProvidedIdToken deserialize<>(const std::string& val) {
    auto data = json::parse(val);
    ProvidedIdToken obj = data;
    return obj;
}

template <> TokenValidationStatusMessage deserialize<>(const std::string& val) {
    auto data = json::parse(val);
    TokenValidationStatusMessage obj = data;
    return obj;
}

template <> ValidationResult deserialize<>(const std::string& val) {
    auto data = json::parse(val);
    ValidationResult obj = data;
    return obj;
}

template <> ValidationResultUpdate deserialize<>(const std::string& val) {
    auto data = json::parse(val);
    ValidationResultUpdate obj = data;
    return obj;
}

template <> WithdrawAuthorizationRequest deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    WithdrawAuthorizationRequest obj = data;
    return obj;
}

} // namespace everest::lib::API::V1_0::types::auth
