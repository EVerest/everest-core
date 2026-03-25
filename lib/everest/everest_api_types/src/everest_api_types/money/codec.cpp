// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "money/codec.hpp"
#include "money/API.hpp"
#include "money/json_codec.hpp"
#include "nlohmann/json.hpp"
#include "utilities/constants.hpp"
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::money {

std::string serialize(CurrencyCode val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}
std::string serialize(Currency val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}
std::string serialize(MoneyAmount val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}
std::string serialize(Price val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}
std::ostream& operator<<(std::ostream& os, CurrencyCode const& val) {
    os << serialize(val);
    return os;
}
std::ostream& operator<<(std::ostream& os, Currency const& val) {
    os << serialize(val);
    return os;
}
std::ostream& operator<<(std::ostream& os, MoneyAmount const& val) {
    os << serialize(val);
    return os;
}
std::ostream& operator<<(std::ostream& os, Price const& val) {
    os << serialize(val);
    return os;
}

template <> CurrencyCode deserialize(std::string const& val) {
    return json::parse(val);
}

template <> Currency deserialize(std::string const& val) {
    return json::parse(val);
}

template <> MoneyAmount deserialize(std::string const& val) {
    return json::parse(val);
}
template <> Price deserialize(std::string const& val) {
    return json::parse(val);
}

} // namespace everest::lib::API::V1_0::types::money
