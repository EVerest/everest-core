// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "money/codec.hpp"
#include "money/API.hpp"
#include "money/json_codec.hpp"
#include "nlohmann/json.hpp"
#include "utilities/constants.hpp"
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::money {

std::string serialize(CurrencyCode val) noexcept {
    json result = val;
    return result.dump(json_indent);
}
std::string serialize(Currency val) noexcept {
    json result = val;
    return result.dump(json_indent);
}
std::string serialize(MoneyAmount val) noexcept {
    json result = val;
    return result.dump(json_indent);
}
std::string serialize(Price val) noexcept {
    json result = val;
    return result.dump(json_indent);
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
    auto data = json::parse(val);
    CurrencyCode obj = data;
    return obj;
}

template <> Currency deserialize(std::string const& val) {
    auto data = json::parse(val);
    Currency obj = data;
    return obj;
}

template <> MoneyAmount deserialize(std::string const& val) {
    auto data = json::parse(val);
    MoneyAmount obj = data;
    return obj;
}
template <> Price deserialize(std::string const& val) {
    auto data = json::parse(val);
    Price obj = data;
    return obj;
}

} // namespace everest::lib::API::V1_0::types::money
