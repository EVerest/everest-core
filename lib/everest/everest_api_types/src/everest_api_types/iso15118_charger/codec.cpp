// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "iso15118_charger/codec.hpp"
#include "iso15118_charger/API.hpp"
#include "iso15118_charger/json_codec.hpp"
#include "nlohmann/json.hpp"
#include "utilities/constants.hpp"
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::iso15118_charger {

std::string serialize(CertificateActionEnum val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(EnergyTransferMode val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(Status val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(HashAlgorithm val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(RequestExiStreamSchema const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ResponseExiStreamStatus const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(CertificateHashDataInfo const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(EnergyTransferModeList const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::ostream& operator<<(std::ostream& os, CertificateActionEnum const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, EnergyTransferMode const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, Status const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, HashAlgorithm const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, RequestExiStreamSchema const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ResponseExiStreamStatus const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, CertificateHashDataInfo const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, EnergyTransferModeList const& val) {
    os << serialize(val);
    return os;
}

template <> CertificateActionEnum deserialize(std::string const& val) {
    return json::parse(val);
}

template <> EnergyTransferMode deserialize(std::string const& val) {
    return json::parse(val);
}

template <> Status deserialize(std::string const& val) {
    return json::parse(val);
}

template <> HashAlgorithm deserialize(std::string const& val) {
    return json::parse(val);
}

template <> RequestExiStreamSchema deserialize(std::string const& val) {
    return json::parse(val);
}

template <> ResponseExiStreamStatus deserialize(std::string const& val) {
    return json::parse(val);
}

template <> CertificateHashDataInfo deserialize(std::string const& val) {
    return json::parse(val);
}

template <> EnergyTransferModeList deserialize(std::string const& val) {
    return json::parse(val);
}

} // namespace everest::lib::API::V1_0::types::iso15118_charger
