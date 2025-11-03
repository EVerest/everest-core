// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <everest_api_types/over_voltage_monitor/API.hpp>
#include <optional>
#include <string>

namespace everest::lib::API::V1_0::types::over_voltage_monitor {

std::string serialize(ErrorEnum val) noexcept;
std::string serialize(ErrorSeverityEnum val) noexcept;
std::string serialize(Error const& val) noexcept;
std::string serialize(OverVoltageLimits const& val) noexcept;

std::ostream& operator<<(std::ostream& os, const Error& val);
std::ostream& operator<<(std::ostream& os, const ErrorEnum& val);
std::ostream& operator<<(std::ostream& os, const ErrorSeverityEnum& val);
std::ostream& operator<<(std::ostream& os, const OverVoltageLimits& val);

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

} // namespace everest::lib::API::V1_0::types::over_voltage_monitor
