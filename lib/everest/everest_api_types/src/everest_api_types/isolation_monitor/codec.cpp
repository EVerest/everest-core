// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "isolation_monitor/codec.hpp"
#include "isolation_monitor/API.hpp"
#include "isolation_monitor/json_codec.hpp"
#include "nlohmann/json.hpp"
#include "utilities/constants.hpp"
#include <string>

namespace everest::lib::API::V1_0::types::isolation_monitor {

std::string serialize(IsolationMeasurement const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ErrorEnum val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(Error const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
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

template <> IsolationMeasurement deserialize(std::string const& val) {
    return json::parse(val);
}

template <> ErrorEnum deserialize(std::string const& val) {
    return json::parse(val);
}

template <> Error deserialize(std::string const& val) {
    return json::parse(val);
}

} // namespace everest::lib::API::V1_0::types::isolation_monitor
