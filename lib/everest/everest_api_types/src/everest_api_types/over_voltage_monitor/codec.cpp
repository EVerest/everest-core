// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "over_voltage_monitor/codec.hpp"
#include "nlohmann/json.hpp"
#include "over_voltage_monitor/API.hpp"
#include "over_voltage_monitor/json_codec.hpp"
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::over_voltage_monitor {

std::string serialize(ErrorEnum val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(Error const& val) noexcept {
    json result = val;
    return result.dump(4);
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

std::ostream& operator<<(std::ostream& os, const ErrorEnum& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, const Error& val) {
    os << serialize(val);
    return os;
}

} // namespace everest::lib::API::V1_0::types::over_voltage_monitor
