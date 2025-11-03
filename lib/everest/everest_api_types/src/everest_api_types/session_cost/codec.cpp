// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "session_cost/codec.hpp"
#include "nlohmann/json.hpp"
#include "session_cost/API.hpp"
#include "session_cost/json_codec.hpp"
#include "utilities/constants.hpp"
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::session_cost {

std::string serialize(TariffMessage val) noexcept {
    json result = val;
    return result.dump(json_indent);
}
std::string serialize(IdlePrice val) noexcept {
    json result = val;
    return result.dump(json_indent);
}
std::string serialize(CostCategory val) noexcept {
    json result = val;
    return result.dump(json_indent);
}
std::string serialize(ChargingPriceComponent val) noexcept {
    json result = val;
    return result.dump(json_indent);
}
std::string serialize(NextPeriodPrice val) noexcept {
    json result = val;
    return result.dump(json_indent);
}
std::string serialize(SessionCostChunk val) noexcept {
    json result = val;
    return result.dump(json_indent);
}
std::string serialize(SessionStatus val) noexcept {
    json result = val;
    return result.dump(json_indent);
}
std::string serialize(SessionCost val) noexcept {
    json result = val;
    return result.dump(json_indent);
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

template <> TariffMessage deserialize(std::string const& val) {
    auto data = json::parse(val);
    TariffMessage obj = data;
    return obj;
}

template <> IdlePrice deserialize(std::string const& val) {
    auto data = json::parse(val);
    IdlePrice obj = data;
    return obj;
}
template <> CostCategory deserialize(std::string const& val) {
    auto data = json::parse(val);
    CostCategory obj = data;
    return obj;
}
template <> ChargingPriceComponent deserialize(std::string const& val) {
    auto data = json::parse(val);
    ChargingPriceComponent obj = data;
    return obj;
}

template <> NextPeriodPrice deserialize(std::string const& val) {
    auto data = json::parse(val);
    NextPeriodPrice obj = data;
    return obj;
}

template <> SessionCostChunk deserialize(std::string const& val) {
    auto data = json::parse(val);
    SessionCostChunk obj = data;
    return obj;
}
template <> SessionStatus deserialize(std::string const& val) {
    auto data = json::parse(val);
    SessionStatus obj = data;
    return obj;
}
template <> SessionCost deserialize(std::string const& val) {
    auto data = json::parse(val);
    SessionCost obj = data;
    return obj;
}

} // namespace everest::lib::API::V1_0::types::session_cost
