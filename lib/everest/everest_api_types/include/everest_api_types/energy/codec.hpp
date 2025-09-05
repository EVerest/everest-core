// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "API.hpp"
#include <optional>
#include <string>

namespace everest::lib::API::V1_0::types::energy {

std::string serialize(NumberWithSource const& val) noexcept;
std::string serialize(IntegerWithSource const& val) noexcept;
std::string serialize(FrequencyWattPoint const& val) noexcept;
std::string serialize(SetpointType const& val) noexcept;
std::string serialize(PricePerkWh const& val) noexcept;
std::string serialize(LimitsReq const& val) noexcept;
std::string serialize(LimitsRes const& val) noexcept;
std::string serialize(ScheduleReqEntry const& val) noexcept;
std::string serialize(ScheduleResEntry const& val) noexcept;
std::string serialize(ScheduleSetpointEntry const& val) noexcept;
std::string serialize(ExternalLimits const& val) noexcept;
std::string serialize(EnforcedLimits const& val) noexcept;

std::ostream& operator<<(std::ostream& os, NumberWithSource const& val);
std::ostream& operator<<(std::ostream& os, IntegerWithSource const& val);
std::ostream& operator<<(std::ostream& os, FrequencyWattPoint const& val);
std::ostream& operator<<(std::ostream& os, SetpointType const& val);
std::ostream& operator<<(std::ostream& os, PricePerkWh const& val);
std::ostream& operator<<(std::ostream& os, LimitsReq const& val);
std::ostream& operator<<(std::ostream& os, LimitsRes const& val);
std::ostream& operator<<(std::ostream& os, ScheduleReqEntry const& val);
std::ostream& operator<<(std::ostream& os, ScheduleResEntry const& val);
std::ostream& operator<<(std::ostream& os, ScheduleSetpointEntry const& val);
std::ostream& operator<<(std::ostream& os, ExternalLimits const& val);
std::ostream& operator<<(std::ostream& os, EnforcedLimits const& val);

template <class T> T deserialize(std::string const& val);
template <class T> std::optional<T> try_deserialize(std::string const& val) {
    try {
        return deserialize<T>(val);
    } catch (...) {
        return std::nullopt;
    }
}
template <class T> bool adl_deserialize(std::string const& json_data, T& obj) {
    auto opt = try_deserialize<T>(json_data);
    if (opt) {
        obj = opt.value();
        return true;
    }
    return false;
}

} // namespace everest::lib::API::V1_0::types::energy
