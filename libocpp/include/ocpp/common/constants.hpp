// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#pragma once

#include <chrono>
#include <cstdint>
#include <string>

namespace ocpp {

// Time
constexpr std::int32_t DAYS_PER_WEEK = 7;
constexpr std::int32_t HOURS_PER_DAY = 24;
constexpr std::int32_t SECONDS_PER_HOUR = 3600;
constexpr std::int32_t SECONDS_PER_DAY = 86400;

constexpr float DEFAULT_LIMIT_AMPS = 48.0;
constexpr float DEFAULT_LIMIT_WATTS = 33120.0;
constexpr std::int32_t DEFAULT_AND_MAX_NUMBER_PHASES = 3;
constexpr float LOW_VOLTAGE = 230;

constexpr float NO_LIMIT_SPECIFIED = -1.0;
constexpr float NO_SETPOINT_SPECIFIED = std::numeric_limits<float>::max();
constexpr float NO_DISCHARGE_LIMIT_SPECIFIED = -std::numeric_limits<float>::max();
constexpr std::int32_t NO_START_PERIOD = -1;
constexpr std::int32_t EVSEID_NOT_SET = -1;

constexpr std::chrono::seconds DEFAULT_WAIT_FOR_FUTURE_TIMEOUT = std::chrono::seconds(60);

const std::string VARIABLE_ATTRIBUTE_VALUE_SOURCE_INTERNAL = "internal";
const std::string VARIABLE_ATTRIBUTE_VALUE_SOURCE_CSMS = "csms";

} // namespace ocpp
