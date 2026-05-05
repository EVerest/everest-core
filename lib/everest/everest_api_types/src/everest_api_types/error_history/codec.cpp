// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "error_history/codec.hpp"
#include "error_history/API.hpp"
#include "error_history/json_codec.hpp"
#include "nlohmann/json.hpp"
#include "utilities/constants.hpp"
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::error_history {

std::string serialize(State val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(SeverityFilter val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(Severity val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ImplementationIdentifier const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(TimeperiodFilter const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(FilterArguments const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ErrorObject const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ErrorList const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
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
    return json::parse(val);
}

template <> SeverityFilter deserialize<>(std::string const& val) {
    return json::parse(val);
}

template <> Severity deserialize<>(std::string const& val) {
    return json::parse(val);
}

template <> ImplementationIdentifier deserialize<>(std::string const& val) {
    return json::parse(val);
}

template <> TimeperiodFilter deserialize<>(std::string const& val) {
    return json::parse(val);
}

template <> FilterArguments deserialize<>(std::string const& val) {
    return json::parse(val);
}

template <> ErrorObject deserialize<>(const std::string& val) {
    return json::parse(val);
}

template <> ErrorList deserialize<>(const std::string& val) {
    return json::parse(val);
}

} // namespace everest::lib::API::V1_0::types::error_history
