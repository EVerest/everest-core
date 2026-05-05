// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "evse_board_support/codec.hpp"
#include "evse_board_support/API.hpp"
#include "evse_board_support/json_codec.hpp"
#include "nlohmann/json.hpp"
#include "utilities/constants.hpp"
#include <string>

namespace everest::lib::API::V1_0::types::evse_board_support {

std::string serialize(Event val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(BspEvent const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ErrorEnum val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(Error const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(Connector_type val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(HardwareCapabilities const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(Reason val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(PowerOnOff const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(Ampacity val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ProximityPilot const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::ostream& operator<<(std::ostream& os, Event const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, const BspEvent& val) {
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

std::ostream& operator<<(std::ostream& os, const Connector_type& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, const HardwareCapabilities& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, const Reason& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, const PowerOnOff& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, const Ampacity& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, const ProximityPilot& val) {
    os << serialize(val);
    return os;
}

template <> Event deserialize(std::string const& val) {
    return json::parse(val);
}

template <> BspEvent deserialize(std::string const& val) {
    return json::parse(val);
}

template <> ErrorEnum deserialize(std::string const& val) {
    return json::parse(val);
}

template <> Error deserialize(std::string const& val) {
    return json::parse(val);
}

template <> Connector_type deserialize(std::string const& val) {
    return json::parse(val);
}

template <> HardwareCapabilities deserialize(std::string const& val) {
    return json::parse(val);
}

template <> Reason deserialize(std::string const& val) {
    return json::parse(val);
}

template <> PowerOnOff deserialize(std::string const& val) {
    return json::parse(val);
}

template <> Ampacity deserialize(std::string const& val) {
    return json::parse(val);
}

template <> ProximityPilot deserialize(std::string const& val) {
    return json::parse(val);
}

} // namespace everest::lib::API::V1_0::types::evse_board_support
