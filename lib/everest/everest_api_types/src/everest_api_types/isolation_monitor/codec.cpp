// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "isolation_monitor/codec.hpp"
#include "isolation_monitor/API.hpp"
#include "isolation_monitor/json_codec.hpp"
#include "nlohmann/json.hpp"
#include <string>

namespace everest::lib::API::V1_0::types::isolation_monitor {

std::string serialize(IsolationMeasurement const& val) noexcept {
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

std::ostream& operator<<(std::ostream& os, IsolationMeasurement const& val) {
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

template <> IsolationMeasurement deserialize(std::string const& s) {
    auto data = json::parse(s);
    IsolationMeasurement result = data;
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

} // namespace everest::lib::API::V1_0::types::isolation_monitor
