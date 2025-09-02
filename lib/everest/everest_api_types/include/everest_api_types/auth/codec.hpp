// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "API.hpp"
#include <optional>
#include <string>

namespace everest::lib::API::V1_0::types::auth {

std::string serialize(AuthorizationStatus val) noexcept;
std::string serialize(CertificateStatus val) noexcept;
std::string serialize(TokenValidationStatus val) noexcept;
std::string serialize(SelectionAlgorithm val) noexcept;
std::string serialize(AuthorizationType val) noexcept;
std::string serialize(IdTokenType val) noexcept;
std::string serialize(WithdrawAuthorizationResult val) noexcept;
std::string serialize(CustomIdToken const& val) noexcept;
std::string serialize(IdToken const& val) noexcept;
std::string serialize(ProvidedIdToken const& val) noexcept;
std::string serialize(TokenValidationStatusMessage const& val) noexcept;
std::string serialize(ValidationResult const& val) noexcept;
std::string serialize(ValidationResultUpdate const& val) noexcept;
std::string serialize(WithdrawAuthorizationRequest const& val) noexcept;

std::ostream& operator<<(std::ostream& os, AuthorizationStatus const& val);
std::ostream& operator<<(std::ostream& os, CertificateStatus const& val);
std::ostream& operator<<(std::ostream& os, TokenValidationStatus const& val);
std::ostream& operator<<(std::ostream& os, SelectionAlgorithm const& val);
std::ostream& operator<<(std::ostream& os, AuthorizationType const& val);
std::ostream& operator<<(std::ostream& os, IdTokenType const& val);
std::ostream& operator<<(std::ostream& os, WithdrawAuthorizationResult const& val);
std::ostream& operator<<(std::ostream& os, CustomIdToken const& val);
std::ostream& operator<<(std::ostream& os, IdToken const& val);
std::ostream& operator<<(std::ostream& os, ProvidedIdToken const& val);
std::ostream& operator<<(std::ostream& os, TokenValidationStatusMessage const& val);
std::ostream& operator<<(std::ostream& os, ValidationResult const& val);
std::ostream& operator<<(std::ostream& os, ValidationResultUpdate const& val);
std::ostream& operator<<(std::ostream& os, WithdrawAuthorizationRequest const& val);

template <class T> T deserialize(std::string const& val);
template <class T> std::optional<T> try_deserialize(std::string const& val) {
    try {
        return deserialize<T>(val);
    } catch (...) {
        return std::nullopt;
    }
}
template <class T> bool adl_deserialize(std::string const& json_data, T& obj) {
    auto opt = try_deserialize<T>(json_data);
    if (opt) {
        obj = opt.value();
        return true;
    }
    return false;
}

} // namespace everest::lib::API::V1_0::types::auth
