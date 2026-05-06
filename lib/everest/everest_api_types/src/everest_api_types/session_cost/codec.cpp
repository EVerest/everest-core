// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "session_cost/codec.hpp"
#include "nlohmann/json.hpp"
#include "session_cost/API.hpp"
#include "session_cost/json_codec.hpp"
#include "utilities/constants.hpp"
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::session_cost {

std::string serialize(DefaultPrice val) noexcept {
    json result = val;
    return result.dump(json_indent);
}
std::string serialize(TariffMessage val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}
std::string serialize(IdlePrice val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}
std::string serialize(CostCategory val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}
std::string serialize(ChargingPriceComponent val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}
std::string serialize(NextPeriodPrice val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}
std::string serialize(SessionCostChunk val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}
std::string serialize(SessionStatus val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}
std::string serialize(SessionCost val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::ostream& operator<<(std::ostream& os, DefaultPrice const& val) {
    os << serialize(val);
    return os;
}
std::ostream& operator<<(std::ostream& os, TariffMessage const& val) {
    os << serialize(val);
    return os;
}
std::ostream& operator<<(std::ostream& os, IdlePrice const& val) {
    os << serialize(val);
    return os;
}
std::ostream& operator<<(std::ostream& os, CostCategory const& val) {
    os << serialize(val);
    return os;
}
std::ostream& operator<<(std::ostream& os, ChargingPriceComponent const& val) {
    os << serialize(val);
    return os;
}
std::ostream& operator<<(std::ostream& os, NextPeriodPrice const& val) {
    os << serialize(val);
    return os;
}
std::ostream& operator<<(std::ostream& os, SessionCostChunk const& val) {
    os << serialize(val);
    return os;
}
std::ostream& operator<<(std::ostream& os, SessionStatus const& val) {
    os << serialize(val);
    return os;
}
std::ostream& operator<<(std::ostream& os, SessionCost const& val) {
    os << serialize(val);
    return os;
}

template <> DefaultPrice deserialize(std::string const& val) {
    auto data = json::parse(val);
    DefaultPrice obj = data;
    return obj;
}

template <> TariffMessage deserialize(std::string const& val) {
    return json::parse(val);
}

template <> IdlePrice deserialize(std::string const& val) {
    return json::parse(val);
}
template <> CostCategory deserialize(std::string const& val) {
    return json::parse(val);
}
template <> ChargingPriceComponent deserialize(std::string const& val) {
    return json::parse(val);
}

template <> NextPeriodPrice deserialize(std::string const& val) {
    return json::parse(val);
}

template <> SessionCostChunk deserialize(std::string const& val) {
    return json::parse(val);
}
template <> SessionStatus deserialize(std::string const& val) {
    return json::parse(val);
}
template <> SessionCost deserialize(std::string const& val) {
    return json::parse(val);
}

} // namespace everest::lib::API::V1_0::types::session_cost
