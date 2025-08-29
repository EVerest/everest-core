// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "error_history/codec.hpp"
#include "error_history/API.hpp"
#include "error_history/json_codec.hpp"
#include "nlohmann/json.hpp"
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::error_history {

std::string serialize(State val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(SeverityFilter val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(Severity val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(ImplementationIdentifier const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(TimeperiodFilter const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(FilterArguments const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(ErrorObject const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(ErrorList const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::ostream& operator<<(std::ostream& os, State const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, SeverityFilter const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, Severity const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ImplementationIdentifier const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, TimeperiodFilter const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, FilterArguments const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ErrorObject const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ErrorList const& val) {
    os << serialize(val);
    return os;
}

template <> State deserialize(std::string const& val) {
    auto data = json::parse(val);
    State obj = data;
    return obj;
}

template <> SeverityFilter deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    SeverityFilter obj = data;
    return obj;
}

template <> Severity deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    Severity obj = data;
    return obj;
}

template <> ImplementationIdentifier deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    ImplementationIdentifier obj = data;
    return obj;
}

template <> TimeperiodFilter deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    TimeperiodFilter obj = data;
    return obj;
}

template <> FilterArguments deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    FilterArguments obj = data;
    return obj;
}

template <> ErrorObject deserialize<>(const std::string& val) {
    auto data = json::parse(val);
    ErrorObject obj = data;
    return obj;
}

template <> ErrorList deserialize<>(const std::string& val) {
    auto data = json::parse(val);
    ErrorList obj = data;
    return obj;
}

} // namespace everest::lib::API::V1_0::types::error_history
