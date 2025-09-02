// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <everest_api_types/power_supply_DC/API.hpp>
#include <optional>
#include <string>

namespace everest::lib::API::V1_0::types::power_supply_DC {

std::string serialize(Capabilities const& val) noexcept;
std::string serialize(Mode val) noexcept;
std::string serialize(ModeRequest val) noexcept;
std::string serialize(ChargingPhase val) noexcept;
std::string serialize(VoltageCurrent const& val) noexcept;
std::string serialize(ErrorEnum val) noexcept;
std::string serialize(Error const& val) noexcept;

std::ostream& operator<<(std::ostream& os, const Capabilities& val);
std::ostream& operator<<(std::ostream& os, const Mode& val);
std::ostream& operator<<(std::ostream& os, const ChargingPhase& val);
std::ostream& operator<<(std::ostream& os, const ModeRequest& val);
std::ostream& operator<<(std::ostream& os, const VoltageCurrent& val);
std::ostream& operator<<(std::ostream& os, const Error& val);
std::ostream& operator<<(std::ostream& os, const ErrorEnum& val);

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

} // namespace everest::lib::API::V1_0::types::power_supply_DC
