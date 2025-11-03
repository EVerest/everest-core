// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include "API.hpp"
#include <optional>
#include <string>

namespace everest::lib::API::V1_0::types::iso15118_charger {

std::string serialize(CertificateActionEnum val) noexcept;
std::string serialize(EnergyTransferMode val) noexcept;
std::string serialize(Status val) noexcept;
std::string serialize(HashAlgorithm val) noexcept;
std::string serialize(RequestExiStreamSchema const& val) noexcept;
std::string serialize(ResponseExiStreamStatus const& val) noexcept;
std::string serialize(CertificateHashDataInfo const& val) noexcept;

std::ostream& operator<<(std::ostream& os, CertificateActionEnum const& val);
std::ostream& operator<<(std::ostream& os, EnergyTransferMode const& val);
std::ostream& operator<<(std::ostream& os, Status const& val);
std::ostream& operator<<(std::ostream& os, HashAlgorithm const& val);
std::ostream& operator<<(std::ostream& os, RequestExiStreamSchema const& val);
std::ostream& operator<<(std::ostream& os, ResponseExiStreamStatus const& val);
std::ostream& operator<<(std::ostream& os, CertificateHashDataInfo const& val);

template <class T> T deserialize(std::string const& val);
template <class T> std::optional<T> try_deserialize(std::string const& val) noexcept {
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

} // namespace everest::lib::API::V1_0::types::iso15118_charger
