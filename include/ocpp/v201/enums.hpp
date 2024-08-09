// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#ifndef OCPP_V201_ENUMS_HPP
#define OCPP_V201_ENUMS_HPP

namespace ocpp {
namespace v201 {

enum class VariableMonitorType {
    HardWiredMonitor,
    PreconfiguredMonitor,
    CustomMonitor,
};

namespace MonitoringLevelSeverity {
constexpr int32_t Danger = 0;
constexpr int32_t HardwareFailure = 1;
constexpr int32_t SystemFailure = 2;
constexpr int32_t Critical = 3;
constexpr int32_t Error = 4;
constexpr int32_t Alert = 5;
constexpr int32_t Warning = 6;
constexpr int32_t Notice = 7;
constexpr int32_t Informational = 8;
constexpr int32_t Debug = 9;

constexpr int32_t MIN = Danger;
constexpr int32_t MAX = Debug;
} // namespace MonitoringLevelSeverity

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_ENUMS_HPP
