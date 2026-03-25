// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "generic/codec.hpp"
#include "generic/API.hpp"
#include "generic/json_codec.hpp"
#include "nlohmann/json.hpp"
#include "utilities/constants.hpp"
#include <stdexcept>
#include <string>

using json = nlohmann::json;

namespace everest::lib::API::V1_0::types::generic {

std::string serialize(bool val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(int val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(size_t val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(double val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(float val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(std::string const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(RequestReply const& val) {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ErrorEnum val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(Error const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::ostream& operator<<(std::ostream& os, RequestReply const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, const ErrorEnum& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, const Error& val) {
    os << serialize(val);
    return os;
}

template <> bool deserialize(std::string const& val) {
    return json::parse(val);
}

template <> int deserialize(std::string const& val) {
    return json::parse(val);
}

template <> size_t deserialize(std::string const& val) {
    return json::parse(val);
}

template <> double deserialize(std::string const& val) {
    return json::parse(val);
}

template <> float deserialize(std::string const& val) {
    return json::parse(val);
}

template <> std::string deserialize(std::string const& val) {
    return json::parse(val);
}

template <> RequestReply deserialize(std::string const& val) {
    return json::parse(val);
}

template <> ErrorEnum deserialize(std::string const& val) {
    return json::parse(val);
}

template <> Error deserialize(std::string const& val) {
    return json::parse(val);
}

} // namespace everest::lib::API::V1_0::types::generic
