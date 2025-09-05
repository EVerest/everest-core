// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "iso15118_charger/codec.hpp"
#include "iso15118_charger/API.hpp"
#include "iso15118_charger/json_codec.hpp"
#include "nlohmann/json.hpp"
#include "utilities/constants.hpp"
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::iso15118_charger {

std::string serialize(CertificateActionEnum val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(Status val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(HashAlgorithm val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(RequestExiStreamSchema const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ResponseExiStreamStatus const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(CertificateHashDataInfo const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::ostream& operator<<(std::ostream& os, CertificateActionEnum const& val) {
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

template <> CertificateActionEnum deserialize(std::string const& s) {
    auto data = json::parse(s);
    CertificateActionEnum result = data;
    return result;
}

template <> Status deserialize(std::string const& s) {
    auto data = json::parse(s);
    Status result = data;
    return result;
}

template <> HashAlgorithm deserialize(std::string const& s) {
    auto data = json::parse(s);
    HashAlgorithm result = data;
    return result;
}

template <> RequestExiStreamSchema deserialize(std::string const& s) {
    auto data = json::parse(s);
    RequestExiStreamSchema result = data;
    return result;
}

template <> ResponseExiStreamStatus deserialize(std::string const& s) {
    auto data = json::parse(s);
    ResponseExiStreamStatus result = data;
    return result;
}

template <> CertificateHashDataInfo deserialize(std::string const& s) {
    auto data = json::parse(s);
    CertificateHashDataInfo result = data;
    return result;
}

} // namespace everest::lib::API::V1_0::types::iso15118_charger
