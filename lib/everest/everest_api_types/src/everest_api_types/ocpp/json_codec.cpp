// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "ocpp/json_codec.hpp"
#include "nlohmann/json.hpp"
#include "ocpp/API.hpp"
#include "ocpp/codec.hpp"

namespace everest::lib::API::V1_0::types::ocpp {

using json = nlohmann::json;

// ----------------------------------------------------------------------------
// Macros to simplify conversions

#define set_json_optional(A)                                                                                           \
    if (k.A) {                                                                                                         \
        j[#A] = k.A.value();                                                                                           \
    }

#define set_obj_optional(A)                                                                                            \
    if (j.contains(#A)) {                                                                                              \
        k.A.emplace(j.at(#A));                                                                                         \
    }

#define set_obj(A) k.A = j.at(#A)

#define set_json_enum(A, B)                                                                                            \
    case A::B:                                                                                                         \
        j = #B;                                                                                                        \
        return

#define set_string_enum(A, B)                                                                                          \
    if (s == #B) {                                                                                                     \
        k = A::B;                                                                                                      \
        return;                                                                                                        \
    }

#define set_json_list(A)                                                                                               \
    j[#A] = json::array();                                                                                             \
    for (auto val : k.A) {                                                                                             \
        j[#A].push_back(val);                                                                                          \
    }

#define set_json_optional_list(A)                                                                                      \
    if (k.A) {                                                                                                         \
        j[#A] = json::array();                                                                                         \
        for (auto val : k.A.value()) {                                                                                 \
            j[#A].push_back(val);                                                                                      \
        }                                                                                                              \
    }

#define set_list(A)                                                                                                    \
    k.A.clear();                                                                                                       \
    for (auto val : j.at(#A)) {                                                                                        \
        k.A.push_back(val);                                                                                            \
    }

#define set_optional_list(A)                                                                                           \
    if (j.contains(#A)) {                                                                                              \
        k.A = decltype(k.A)::value_type{};                                                                             \
        for (auto val : j.at(#A)) {                                                                                    \
            k.A.value().push_back(val);                                                                                \
        }                                                                                                              \
    }

// ----------------------------------------------------------------------------
// conversions using the above macros

void to_json(json& j, AttributeEnum const& k) noexcept {
    switch (k) {
        set_json_enum(AttributeEnum, Actual);
        set_json_enum(AttributeEnum, Target);
        set_json_enum(AttributeEnum, MinSet);
        set_json_enum(AttributeEnum, MaxSet);
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::ocpp::AttributeEnum";
}

void from_json(const json& j, AttributeEnum& k) {
    std::string s = j;
    set_string_enum(AttributeEnum, Actual);
    set_string_enum(AttributeEnum, Target);
    set_string_enum(AttributeEnum, MinSet);
    set_string_enum(AttributeEnum, MaxSet);
    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::ocpp::AttributeEnum");
}

void to_json(json& j, GetVariableStatusEnumType const& k) noexcept {
    switch (k) {
        set_json_enum(GetVariableStatusEnumType, Accepted);
        set_json_enum(GetVariableStatusEnumType, Rejected);
        set_json_enum(GetVariableStatusEnumType, UnknownComponent);
        set_json_enum(GetVariableStatusEnumType, UnknownVariable);
        set_json_enum(GetVariableStatusEnumType, NotSupportedAttributeType);
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::ocpp::GetVariableStatueEnum";
}

void from_json(const json& j, GetVariableStatusEnumType& k) {
    std::string s = j;
    set_string_enum(GetVariableStatusEnumType, Accepted);
    set_string_enum(GetVariableStatusEnumType, Rejected);
    set_string_enum(GetVariableStatusEnumType, UnknownComponent);
    set_string_enum(GetVariableStatusEnumType, UnknownVariable);
    set_string_enum(GetVariableStatusEnumType, NotSupportedAttributeType);
    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::ocpp::GetVariableStatusEnum");
}

void to_json(json& j, SetVariableStatusEnumType const& k) noexcept {
    switch (k) {
        set_json_enum(SetVariableStatusEnumType, Accepted);
        set_json_enum(SetVariableStatusEnumType, Rejected);
        set_json_enum(SetVariableStatusEnumType, UnknownComponent);
        set_json_enum(SetVariableStatusEnumType, UnknownVariable);
        set_json_enum(SetVariableStatusEnumType, NotSupportedAttributeType);
        set_json_enum(SetVariableStatusEnumType, RebootRequired);
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::ocpp::SetVariableStatueEnum";
}

void from_json(const json& j, SetVariableStatusEnumType& k) {
    std::string s = j;
    set_string_enum(SetVariableStatusEnumType, Accepted);
    set_string_enum(SetVariableStatusEnumType, Rejected);
    set_string_enum(SetVariableStatusEnumType, UnknownComponent);
    set_string_enum(SetVariableStatusEnumType, UnknownVariable);
    set_string_enum(SetVariableStatusEnumType, NotSupportedAttributeType);
    set_string_enum(SetVariableStatusEnumType, RebootRequired);
    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::ocpp::SetVariableStatusEnum");
}

void to_json(json& j, DataTransferStatus const& k) noexcept {
    switch (k) {
        set_json_enum(DataTransferStatus, Accepted);
        set_json_enum(DataTransferStatus, Rejected);
        set_json_enum(DataTransferStatus, UnknownMessageId);
        set_json_enum(DataTransferStatus, UnknownVendorId);
        set_json_enum(DataTransferStatus, Offline);
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::ocpp::DataTransferStatus";
}

void from_json(const json& j, DataTransferStatus& k) {
    std::string s = j;
    set_string_enum(DataTransferStatus, Accepted);
    set_string_enum(DataTransferStatus, Rejected);
    set_string_enum(DataTransferStatus, UnknownMessageId);
    set_string_enum(DataTransferStatus, UnknownVendorId);
    set_string_enum(DataTransferStatus, Offline);
    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::ocpp::DataTransferStatus");
}

void to_json(json& j, RegistrationStatus const& k) noexcept {
    switch (k) {
        set_json_enum(RegistrationStatus, Accepted);
        set_json_enum(RegistrationStatus, Pending);
        set_json_enum(RegistrationStatus, Rejected);
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::ocpp::RegistrationStatus";
}

void from_json(const json& j, RegistrationStatus& k) {
    std::string s = j;
    set_string_enum(RegistrationStatus, Accepted);
    set_string_enum(RegistrationStatus, Pending);
    set_string_enum(RegistrationStatus, Rejected);
    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::ocpp::RegistrationStatus");
}

void to_json(json& j, TransactionEvent const& k) noexcept {
    switch (k) {
        set_json_enum(TransactionEvent, Started);
        set_json_enum(TransactionEvent, Updated);
        set_json_enum(TransactionEvent, Ended);
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::ocpp::TransactionEvent";
}

void from_json(const json& j, TransactionEvent& k) {
    std::string s = j;
    set_string_enum(TransactionEvent, Started);
    set_string_enum(TransactionEvent, Updated);
    set_string_enum(TransactionEvent, Ended);
    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::ocpp::TransactionEvent");
}

void to_json(json& j, CustomData const& k) noexcept {
    j = json{
        {"vendor_id", k.vendor_id},
        {"data", k.data},
    };
}

void from_json(const json& j, CustomData& k) {
    set_obj(vendor_id);
    set_obj(data);
}

void to_json(json& j, DataTransferRequest const& k) noexcept {
    j = json{
        {"vendor_id", k.vendor_id},
    };
    set_json_optional(message_id);
    set_json_optional(data);
    set_json_optional(custom_data);
}

void from_json(const json& j, DataTransferRequest& k) {
    set_obj(vendor_id);
    set_obj_optional(message_id);
    set_obj_optional(data);
    set_obj_optional(custom_data);
}

void to_json(json& j, DataTransferResponse const& k) noexcept {
    j = json{
        {"status", k.status},
    };
    set_json_optional(data);
    set_json_optional(custom_data);
}

void from_json(const json& j, DataTransferResponse& k) {
    set_obj(status);
    set_obj_optional(data);
    set_obj_optional(custom_data);
}

void to_json(json& j, EVSE const& k) noexcept {
    j = json{
        {"id", k.id},
    };
    set_json_optional(connector_id);
}

void from_json(const json& j, EVSE& k) {
    set_obj(id);
    set_obj_optional(connector_id);
}

void to_json(json& j, Component const& k) noexcept {
    j = json{
        {"name", k.name},
    };
    set_json_optional(instance);
    set_json_optional(evse);
}

void from_json(const json& j, Component& k) {
    set_obj(name);
    set_obj_optional(instance);
    set_obj_optional(evse);
}

void to_json(json& j, Variable const& k) noexcept {
    j = json{
        {"name", k.name},
    };
    set_json_optional(instance);
}

void from_json(const json& j, Variable& k) {
    set_obj(name);
    set_obj_optional(instance);
}

void to_json(json& j, ComponentVariable const& k) noexcept {
    j = json{
        {"component", k.component},
        {"variable", k.variable},
    };
}

void from_json(const json& j, ComponentVariable& k) {
    set_obj(component);
    set_obj(variable);
}

void to_json(json& j, GetVariableRequest const& k) noexcept {
    j = json{
        {"component_variable", k.component_variable},
    };
    set_json_optional(attribute_type);
}

void from_json(const json& j, GetVariableRequest& k) {
    set_obj(component_variable);
    set_obj_optional(attribute_type);
}

void to_json(json& j, GetVariableResult const& k) noexcept {
    j = json{
        {"status", k.status},
        {"component_variable", k.component_variable},
    };
    set_json_optional(attribute_type);
    set_json_optional(value);
}

void from_json(const json& j, GetVariableResult& k) {
    set_obj(status);
    set_obj(component_variable);
    set_obj_optional(attribute_type);
    set_obj_optional(value);
}

void to_json(json& j, SetVariableRequest const& k) noexcept {
    j = json{
        {"component_variable", k.component_variable},
        {"value", k.value},
    };
    set_json_optional(attribute_type);
}

void from_json(const json& j, SetVariableRequest& k) {
    set_obj(component_variable);
    set_obj(value);
    set_obj_optional(attribute_type);
}

void to_json(json& j, SetVariableResult const& k) noexcept {
    j = json{
        {"status", k.status},
        {"component_variable", k.component_variable},
    };
    set_json_optional(attribute_type);
}

void from_json(const json& j, SetVariableResult& k) {
    set_obj(status);
    set_obj(component_variable);
    set_obj_optional(attribute_type);
}

void to_json(json& j, GetVariableRequestList const& k) noexcept {
    set_json_list(items);
}

void from_json(const json& j, GetVariableRequestList& k) {
    set_list(items);
}

void to_json(json& j, GetVariableResultList const& k) noexcept {
    set_json_list(items);
}

void from_json(const json& j, GetVariableResultList& k) {
    set_list(items);
}

void to_json(json& j, SetVariableRequestList const& k) noexcept {
    set_json_list(items);
}

void from_json(const json& j, SetVariableRequestList& k) {
    set_list(items);
}

void to_json(json& j, SetVariableResultList const& k) noexcept {
    set_json_list(items);
}

void from_json(const json& j, SetVariableResultList& k) {
    set_list(items);
}

void to_json(json& j, SetVariablesArgs const& k) noexcept {
    j = json{
        {"variables", k.variables},
        {"source", k.source},
    };
}

void from_json(const json& j, SetVariablesArgs& k) {
    set_obj(variables);
    set_obj(source);
}

void to_json(json& j, SecurityEvent const& k) noexcept {
    j = json{
        {"type", k.type},
    };
    set_json_optional(info);
    set_json_optional(critical);
    set_json_optional(timestamp);
}

void from_json(const json& j, SecurityEvent& k) {
    set_obj(type);
    set_obj_optional(info);
    set_obj_optional(critical);
    set_obj_optional(timestamp);
}

void to_json(json& j, StatusInfoType const& k) noexcept {
    j = json{
        {"reason_code", k.reason_code},
    };
    set_json_optional(additional_info);
}

void from_json(const json& j, StatusInfoType& k) {
    set_obj(reason_code);
    set_obj_optional(additional_info);
}

void to_json(json& j, BootNotificationResponse const& k) noexcept {
    j = json{
        {"status", k.status},
        {"current_time", k.current_time},
        {"interval", k.interval},
    };
    set_json_optional(status_info);
}

void from_json(const json& j, BootNotificationResponse& k) {
    set_obj(status);
    set_obj(current_time);
    set_obj(interval);
    set_obj_optional(status_info);
}

void to_json(json& j, OcppTransactionEvent const& k) noexcept {
    j = json{
        {"transaction_event", k.transaction_event},
        {"session_id", k.session_id},
    };
    set_json_optional(evse);
    set_json_optional(transaction_id);
}

void from_json(const json& j, OcppTransactionEvent& k) {
    set_obj(transaction_event);
    set_obj(session_id);
    set_obj_optional(evse);
    set_obj_optional(transaction_id);
}

void to_json(json& j, MonitorVariableRequestList const& k) noexcept {
    set_json_list(items);
}

void from_json(const json& j, MonitorVariableRequestList& k) {
    set_list(items);
}

void to_json(json& j, EventTriggerEnum const& k) noexcept {
    switch (k) {
        set_json_enum(EventTriggerEnum, Alerting);
        set_json_enum(EventTriggerEnum, Delta);
        set_json_enum(EventTriggerEnum, Periodic);
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::ocpp::EventTriggerEnum";
}

void from_json(const json& j, EventTriggerEnum& k) {
    std::string s = j;
    set_string_enum(EventTriggerEnum, Alerting);
    set_string_enum(EventTriggerEnum, Delta);
    set_string_enum(EventTriggerEnum, Periodic);
    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::ocpp::EventTriggerEnum");
}

void to_json(json& j, EventNotificationType const& k) noexcept {
    switch (k) {
        set_json_enum(EventNotificationType, HardWiredNotification);
        set_json_enum(EventNotificationType, HardWiredMonitor);
        set_json_enum(EventNotificationType, PreconfiguredMonitor);
        set_json_enum(EventNotificationType, CustomMonitor);
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::ocpp::EventNotificationType";
}

void from_json(const json& j, EventNotificationType& k) {
    std::string s = j;
    set_string_enum(EventNotificationType, HardWiredNotification);
    set_string_enum(EventNotificationType, HardWiredMonitor);
    set_string_enum(EventNotificationType, PreconfiguredMonitor);
    set_string_enum(EventNotificationType, CustomMonitor);
    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::ocpp::EventNotificationType");
}

void to_json(json& j, EventData const& k) noexcept {
    j = json{
        {"component_variable", k.component_variable},
        {"event_id", k.event_id},
        {"timestamp", k.timestamp},
        {"trigger", k.trigger},
        {"actual_value", k.actual_value},
        {"event_notification_type", k.event_notification_type},
    };
    set_json_optional(cause);
    set_json_optional(tech_code);
    set_json_optional(tech_info);
    set_json_optional(cleared);
    set_json_optional(transaction_id);
    set_json_optional(variable_monitoring_id);
}

void from_json(const json& j, EventData& k) {
    set_obj(component_variable);
    set_obj(event_id);
    set_obj(timestamp);
    set_obj(trigger);
    set_obj(actual_value);
    set_obj(event_notification_type);
    set_obj_optional(cause);
    set_obj_optional(tech_code);
    set_obj_optional(tech_info);
    set_obj_optional(cleared);
    set_obj_optional(transaction_id);
    set_obj_optional(variable_monitoring_id);
}

void to_json(json& j, ChargingSchedules const& k) noexcept {
    set_json_list(schedules);
}

void from_json(const json& j, ChargingSchedules& k) {
    set_list(schedules);
}

void to_json(json& j, ChargingSchedule const& k) noexcept {
    j = json{
        {"evse", k.evse},
        {"charging_rate_unit", k.charging_rate_unit},
    };
    set_json_list(charging_schedule_period);
    set_json_optional(duration);
    set_json_optional(start_schedule);
    set_json_optional(min_charging_rate);
}

void from_json(const json& j, ChargingSchedule& k) {
    set_obj(evse);
    set_obj(charging_rate_unit);
    set_list(charging_schedule_period);
    set_obj_optional(duration);
    set_obj_optional(start_schedule);
    set_obj_optional(min_charging_rate);
}

void to_json(json& j, ChargingSchedulePeriod const& k) noexcept {
    j = json{
        {"start_period", k.start_period},
        {"limit", k.limit},
    };
    set_json_optional(limit_L2);
    set_json_optional(limit_L3);
    set_json_optional(number_phases);
    set_json_optional(stack_level);
    set_json_optional(phase_to_use);
    set_json_optional(discharge_limit);
    set_json_optional(discharge_limit_L2);
    set_json_optional(discharge_limit_L3);
    set_json_optional(setpoint);
    set_json_optional(setpoint_L2);
    set_json_optional(setpoint_L3);
    set_json_optional(setpoint_reactive);
    set_json_optional(setpoint_reactive_L2);
    set_json_optional(setpoint_reactive_L3);
    set_json_optional(preconditioning_request);
    set_json_optional(evse_sleep);
    set_json_optional(v2x_baseline);
    set_json_optional(operation_mode);
    set_json_optional_list(v2x_freq_watt_curve);
    set_json_optional_list(v2x_signal_watt_curve);
}

void from_json(const json& j, ChargingSchedulePeriod& k) {
    set_obj(start_period);
    set_obj(limit);
    set_obj_optional(limit_L2);
    set_obj_optional(limit_L3);
    set_obj_optional(number_phases);
    set_obj_optional(stack_level);
    set_obj_optional(phase_to_use);
    set_obj_optional(discharge_limit);
    set_obj_optional(discharge_limit_L2);
    set_obj_optional(discharge_limit_L3);
    set_obj_optional(setpoint);
    set_obj_optional(setpoint_L2);
    set_obj_optional(setpoint_L3);
    set_obj_optional(setpoint_reactive);
    set_obj_optional(setpoint_reactive_L2);
    set_obj_optional(setpoint_reactive_L3);
    set_obj_optional(preconditioning_request);
    set_obj_optional(evse_sleep);
    set_obj_optional(v2x_baseline);
    set_obj_optional(operation_mode);
    set_optional_list(v2x_freq_watt_curve);
    set_optional_list(v2x_signal_watt_curve);
}

void to_json(json& j, OperationMode const& k) noexcept {
    switch (k) {
        set_json_enum(OperationMode, Idle);
        set_json_enum(OperationMode, ChargingOnly);
        set_json_enum(OperationMode, CentralSetpoint);
        set_json_enum(OperationMode, ExternalSetpoint);
        set_json_enum(OperationMode, ExternalLimits);
        set_json_enum(OperationMode, CentralFrequency);
        set_json_enum(OperationMode, LocalFrequency);
        set_json_enum(OperationMode, LocalLoadBalancing);
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::ocpp::OperationMode";
}

void from_json(const json& j, OperationMode& k) {
    std::string s = j;
    set_string_enum(OperationMode, Idle);
    set_string_enum(OperationMode, ChargingOnly);
    set_string_enum(OperationMode, CentralSetpoint);
    set_string_enum(OperationMode, ExternalSetpoint);
    set_string_enum(OperationMode, ExternalLimits);
    set_string_enum(OperationMode, CentralFrequency);
    set_string_enum(OperationMode, LocalFrequency);
    set_string_enum(OperationMode, LocalLoadBalancing);
    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::ocpp::OperationMode");
}

void to_json(json& j, V2XSignalWattPointCurve const& k) noexcept {
    j = json{
        {"signal", k.signal},
        {"power", k.power},
    };
}

void from_json(const json& j, V2XSignalWattPointCurve& k) {
    set_obj(signal);
    set_obj(power);
}

void to_json(json& j, V2XFreqWattPointType const& k) noexcept {
    j = json{
        {"frequency", k.frequency},
        {"power", k.power},
    };
}

void from_json(const json& j, V2XFreqWattPointType& k) {
    set_obj(frequency);
    set_obj(power);
}

} // namespace everest::lib::API::V1_0::types::ocpp
