// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "power_supply_DC/codec.hpp"
#include "nlohmann/json.hpp"
#include "power_supply_DC/API.hpp"
#include "power_supply_DC/json_codec.hpp"
#include "utilities/constants.hpp"
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::power_supply_DC {

std::string serialize(Capabilities const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(Mode val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ChargingPhase val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ModeRequest val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(VoltageCurrent const& val) noexcept {
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

template <> Capabilities deserialize(std::string const& s) {
    auto data = json::parse(s);
    Capabilities result = data;
    return result;
}

template <> Mode deserialize(std::string const& s) {
    auto data = json::parse(s);
    Mode result = data;
    return result;
}

template <> ChargingPhase deserialize(std::string const& s) {
    auto data = json::parse(s);
    ChargingPhase result = data;
    return result;
}

template <> ModeRequest deserialize(std::string const& s) {
    auto data = json::parse(s);
    ModeRequest result = data;
    return result;
}

template <> VoltageCurrent deserialize(std::string const& s) {
    auto data = json::parse(s);
    VoltageCurrent result = data;
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

std::ostream& operator<<(std::ostream& os, const Capabilities& k) {
    os << serialize(k);
    return os;
}

std::ostream& operator<<(std::ostream& os, const Mode& k) {
    os << serialize(k);
    return os;
}

std::ostream& operator<<(std::ostream& os, const ChargingPhase& k) {
    os << serialize(k);
    return os;
}

std::ostream& operator<<(std::ostream& os, const ModeRequest& k) {
    os << serialize(k);
    return os;
}

std::ostream& operator<<(std::ostream& os, const VoltageCurrent& k) {
    os << serialize(k);
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

} // namespace everest::lib::API::V1_0::types::power_supply_DC
