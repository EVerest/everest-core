// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
// Manually added enums for OCPP, for the auto-generated ones see 'ocpp_enums.hpp'

#ifndef OCPP_V2_ENUMS_HPP
#define OCPP_V2_ENUMS_HPP

#include <ocpp/v2/ocpp_enums.hpp>

#include <cstdint>
#include <string>

namespace ocpp {
namespace v2 {

enum class VariableMonitorType {
    HardWiredMonitor,
    PreconfiguredMonitor,
    CustomMonitor,
};

namespace conversions {
/// \brief Converts the given std::string \p s to VariableMonitorType
/// \returns a VariableMonitorType from a string representation
VariableMonitorType string_to_variable_monitor_type(const std::string& s);

/// \brief Converts the given VariableMonitorType \p type to std::string
/// \returns a string representation of the VariableMonitorType
EventNotificationEnum variable_monitor_type_to_event_notification_type(const VariableMonitorType& type);
} // namespace conversions

namespace MonitoringLevelSeverity {
constexpr std::int32_t Danger = 0;
constexpr std::int32_t HardwareFailure = 1;
constexpr std::int32_t SystemFailure = 2;
constexpr std::int32_t Critical = 3;
constexpr std::int32_t Error = 4;
constexpr std::int32_t Alert = 5;
constexpr std::int32_t Warning = 6;
constexpr std::int32_t Notice = 7;
constexpr std::int32_t Informational = 8;
constexpr std::int32_t Debug = 9;

constexpr std::int32_t MIN = Danger;
constexpr std::int32_t MAX = Debug;
} // namespace MonitoringLevelSeverity

} // namespace v2
} // namespace ocpp

#endif // OCPP_V2_ENUMS_HPP
