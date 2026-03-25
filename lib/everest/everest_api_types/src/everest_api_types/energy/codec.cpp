// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "energy/codec.hpp"
#include "energy/API.hpp"
#include "energy/json_codec.hpp"
#include "nlohmann/json.hpp"
#include "utilities/constants.hpp"

namespace everest::lib::API::V1_0::types::energy {

std::string serialize(NumberWithSource const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(IntegerWithSource const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(FrequencyWattPoint const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(SetpointType const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(PricePerkWh const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(LimitsReq const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(LimitsRes const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ScheduleReqEntry const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ScheduleResEntry const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ScheduleSetpointEntry const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ExternalLimits const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(EnforcedLimits const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
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
    return json::parse(val);
}

template <> IntegerWithSource deserialize(std::string const& val) {
    return json::parse(val);
}

template <> FrequencyWattPoint deserialize(std::string const& val) {
    return json::parse(val);
}

template <> SetpointType deserialize(std::string const& val) {
    return json::parse(val);
}

template <> PricePerkWh deserialize(std::string const& val) {
    return json::parse(val);
}

template <> LimitsReq deserialize(std::string const& val) {
    return json::parse(val);
}

template <> LimitsRes deserialize(std::string const& val) {
    return json::parse(val);
}

template <> ScheduleReqEntry deserialize(std::string const& val) {
    return json::parse(val);
}

template <> ScheduleResEntry deserialize(std::string const& val) {
    return json::parse(val);
}

template <> ScheduleSetpointEntry deserialize(std::string const& val) {
    return json::parse(val);
}

template <> ExternalLimits deserialize(std::string const& val) {
    return json::parse(val);
}

template <> EnforcedLimits deserialize(std::string const& val) {
    return json::parse(val);
}

} // namespace everest::lib::API::V1_0::types::energy
