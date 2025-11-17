// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

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
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(int val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(size_t val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(double val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(float val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(std::string const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(RequestReply const& val) {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ErrorEnum val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(Error const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
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

template <> bool deserialize(std::string const& s) {
    auto data = json::parse(s);
    bool result = data;
    return result;
}

template <> int deserialize(std::string const& s) {
    auto data = json::parse(s);
    int result = data;
    return result;
}

template <> size_t deserialize(std::string const& s) {
    auto data = json::parse(s);
    size_t result = data;
    return result;
}

template <> double deserialize(std::string const& s) {
    auto data = json::parse(s);
    double result = data;
    return result;
}

template <> float deserialize(std::string const& s) {
    auto data = json::parse(s);
    float result = data;
    return result;
}

template <> std::string deserialize(std::string const& s) {
    auto data = json::parse(s);
    std::string result = data;
    return result;
}

template <> RequestReply deserialize(std::string const& s) {
    auto data = json::parse(s);
    RequestReply result = data;
    return result;
}

template <> ErrorEnum deserialize(std::string const& s) {
    auto data = json::parse(s);
    ErrorEnum result = data;
    return result;
}

template <> Error deserialize(std::string const& s) {
    auto data = json::parse(s);
    Error result = data;
    return result;
}

} // namespace everest::lib::API::V1_0::types::generic
