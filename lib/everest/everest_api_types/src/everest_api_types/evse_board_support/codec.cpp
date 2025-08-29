// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "evse_board_support/codec.hpp"
#include "evse_board_support/API.hpp"
#include "evse_board_support/json_codec.hpp"
#include "nlohmann/json.hpp"
#include <string>

namespace everest::lib::API::V1_0::types::evse_board_support {

std::string serialize(Event val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(BspEvent const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(ErrorEnum val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(Error const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(Connector_type val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(HardwareCapabilities const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(Reason val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(PowerOnOff const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(Ampacity val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(ProximityPilot const& val) noexcept {
    json result = val;
    return result.dump(4);
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

template <> Event deserialize(std::string const& s) {
    auto data = json::parse(s);
    Event result = data;
    return result;
}

template <> BspEvent deserialize(std::string const& s) {
    auto data = json::parse(s);
    BspEvent result = data;
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

template <> Connector_type deserialize(std::string const& s) {
    auto data = json::parse(s);
    Connector_type result = data;
    return result;
}

template <> HardwareCapabilities deserialize(std::string const& s) {
    auto data = json::parse(s);
    HardwareCapabilities result = data;
    return result;
}

template <> Reason deserialize(std::string const& s) {
    auto data = json::parse(s);
    Reason result = data;
    return result;
}

template <> PowerOnOff deserialize(std::string const& s) {
    auto data = json::parse(s);
    PowerOnOff result = data;
    return result;
}

template <> Ampacity deserialize(std::string const& s) {
    auto data = json::parse(s);
    Ampacity result = data;
    return result;
}

template <> ProximityPilot deserialize(std::string const& s) {
    auto data = json::parse(s);
    ProximityPilot result = data;
    return result;
}

} // namespace everest::lib::API::V1_0::types::evse_board_support
