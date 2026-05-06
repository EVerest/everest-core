// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "power_supply_DC/codec.hpp"
#include "nlohmann/json.hpp"
#include "power_supply_DC/API.hpp"
#include "power_supply_DC/json_codec.hpp"
#include "utilities/constants.hpp"
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::power_supply_DC {

std::string serialize(Capabilities const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(Mode val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ChargingPhase val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ModeRequest val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(VoltageCurrent const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ErrorEnum val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(Error const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

template <> Capabilities deserialize(std::string const& val) {
    return json::parse(val);
}

template <> Mode deserialize(std::string const& val) {
    return json::parse(val);
}

template <> ChargingPhase deserialize(std::string const& val) {
    return json::parse(val);
}

template <> ModeRequest deserialize(std::string const& val) {
    return json::parse(val);
}

template <> VoltageCurrent deserialize(std::string const& val) {
    return json::parse(val);
}

template <> ErrorEnum deserialize(std::string const& val) {
    return json::parse(val);
}

template <> Error deserialize(std::string const& val) {
    return json::parse(val);
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
