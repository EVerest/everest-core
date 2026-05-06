// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "over_voltage_monitor/codec.hpp"
#include "nlohmann/json.hpp"
#include "over_voltage_monitor/API.hpp"
#include "over_voltage_monitor/json_codec.hpp"
#include "utilities/constants.hpp"
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::over_voltage_monitor {

std::string serialize(ErrorEnum val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ErrorSeverityEnum val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(Error const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(OverVoltageLimits const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

template <> ErrorEnum deserialize(std::string const& val) {
    return json::parse(val);
}

template <> ErrorSeverityEnum deserialize(std::string const& val) {
    return json::parse(val);
}

template <> Error deserialize(std::string const& val) {
    return json::parse(val);
}

template <> OverVoltageLimits deserialize(std::string const& val) {
    return json::parse(val);
}

std::ostream& operator<<(std::ostream& os, const ErrorEnum& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, const ErrorSeverityEnum& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, const Error& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, const OverVoltageLimits& val) {
    os << serialize(val);
    return os;
}

} // namespace everest::lib::API::V1_0::types::over_voltage_monitor
