// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include <stdexcept>

#include <ocpp/v2/enums.hpp>

namespace ocpp::v2 {

namespace conversions {
/// \brief Converts the given std::string \p s to VariableMonitorType
/// \returns a VariableMonitorType from a string representation
VariableMonitorType string_to_variable_monitor_type(const std::string& s) {
    if (s == "HardWiredMonitor") {
        return VariableMonitorType::HardWiredMonitor;
    }
    if (s == "PreconfiguredMonitor") {
        return VariableMonitorType::PreconfiguredMonitor;
    }
    if (s == "CustomMonitor") {
        return VariableMonitorType::CustomMonitor;
    }

    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type VariableMonitorType");
}

EventNotificationEnum variable_monitor_type_to_event_notification_type(const VariableMonitorType& type) {
    switch (type) {
    case VariableMonitorType::HardWiredMonitor:
        return ocpp::v2::EventNotificationEnum::HardWiredMonitor;
    case VariableMonitorType::PreconfiguredMonitor:
        return ocpp::v2::EventNotificationEnum::PreconfiguredMonitor;
    case VariableMonitorType::CustomMonitor:
        return ocpp::v2::EventNotificationEnum::CustomMonitor;
    default:
        throw std::out_of_range("Provided VariableMonitorType could not be converted to EventNotificationType");
    }
}

} // namespace conversions

} // namespace ocpp::v2
