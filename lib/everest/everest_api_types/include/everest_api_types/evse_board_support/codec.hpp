// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include "API.hpp"
#include <optional>
#include <string>

namespace everest::lib::API::V1_0::types::evse_board_support {

std::string serialize(Event val) noexcept;
std::string serialize(BspEvent const& val) noexcept;
std::string serialize(ErrorEnum val) noexcept;
std::string serialize(Error const& val) noexcept;
std::string serialize(Connector_type val) noexcept;
std::string serialize(HardwareCapabilities const& val) noexcept;
std::string serialize(Reason val) noexcept;
std::string serialize(PowerOnOff const& val) noexcept;
std::string serialize(Ampacity val) noexcept;
std::string serialize(ProximityPilot const& val) noexcept;

std::ostream& operator<<(std::ostream& os, Event const& val);
std::ostream& operator<<(std::ostream& os, BspEvent const& val);
std::ostream& operator<<(std::ostream& os, Error const& val);
std::ostream& operator<<(std::ostream& os, ErrorEnum const& val);
std::ostream& operator<<(std::ostream& os, Connector_type const& val);
std::ostream& operator<<(std::ostream& os, HardwareCapabilities const& val);
std::ostream& operator<<(std::ostream& os, Reason const& val);
std::ostream& operator<<(std::ostream& os, PowerOnOff const& val);
std::ostream& operator<<(std::ostream& os, Ampacity const& val);
std::ostream& operator<<(std::ostream& os, ProximityPilot const& val);

template <class T> T deserialize(std::string const& val);
template <class T> std::optional<T> try_deserialize(std::string const& val) {
    try {
        return deserialize<T>(val);
    } catch (...) {
        return std::nullopt;
    }
}
template <class T> bool adl_deserialize(std::string const& json_data, T& obj) {
    auto opt = try_deserialize<T>(json_data);
    if (opt) {
        obj = opt.value();
        return true;
    }
    return false;
}

} // namespace everest::lib::API::V1_0::types::evse_board_support
