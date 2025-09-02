// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "energy/codec.hpp"
#include "energy/API.hpp"
#include "energy/json_codec.hpp"
#include "nlohmann/json.hpp"

namespace everest::lib::API::V1_0::types::energy {

std::string serialize(NumberWithSource const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(IntegerWithSource const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(FrequencyWattPoint const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(SetpointType const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(PricePerkWh const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(LimitsReq const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(LimitsRes const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(ScheduleReqEntry const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(ScheduleResEntry const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(ScheduleSetpointEntry const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(ExternalLimits const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(EnforcedLimits const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::ostream& operator<<(std::ostream& os, NumberWithSource const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, IntegerWithSource const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, FrequencyWattPoint const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, SetpointType const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, PricePerkWh const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, LimitsReq const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, LimitsRes const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ScheduleReqEntry const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ScheduleResEntry const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ScheduleSetpointEntry const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ExternalLimits const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, EnforcedLimits const& val) {
    os << serialize(val);
    return os;
}

template <> NumberWithSource deserialize(std::string const& val) {
    auto data = json::parse(val);
    NumberWithSource obj = data;
    return obj;
}

template <> IntegerWithSource deserialize(std::string const& val) {
    auto data = json::parse(val);
    IntegerWithSource obj = data;
    return obj;
}

template <> FrequencyWattPoint deserialize(std::string const& val) {
    auto data = json::parse(val);
    FrequencyWattPoint obj = data;
    return obj;
}

template <> SetpointType deserialize(std::string const& val) {
    auto data = json::parse(val);
    SetpointType obj = data;
    return obj;
}

template <> PricePerkWh deserialize(std::string const& val) {
    auto data = json::parse(val);
    PricePerkWh obj = data;
    return obj;
}

template <> LimitsReq deserialize(std::string const& val) {
    auto data = json::parse(val);
    LimitsReq obj = data;
    return obj;
}

template <> LimitsRes deserialize(std::string const& val) {
    auto data = json::parse(val);
    LimitsRes obj = data;
    return obj;
}

template <> ScheduleReqEntry deserialize(std::string const& val) {
    auto data = json::parse(val);
    ScheduleReqEntry obj = data;
    return obj;
}

template <> ScheduleResEntry deserialize(std::string const& val) {
    auto data = json::parse(val);
    ScheduleResEntry obj = data;
    return obj;
}

template <> ScheduleSetpointEntry deserialize(std::string const& val) {
    auto data = json::parse(val);
    ScheduleSetpointEntry obj = data;
    return obj;
}

template <> ExternalLimits deserialize(std::string const& val) {
    auto data = json::parse(val);
    ExternalLimits obj = data;
    return obj;
}

template <> EnforcedLimits deserialize(std::string const& val) {
    auto data = json::parse(val);
    EnforcedLimits obj = data;
    return obj;
}

} // namespace everest::lib::API::V1_0::types::energy
