// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once
#include <optional>
#include <string>
#include <vector>

namespace everest::lib::API::V1_0::types::ocpp {

enum class AttributeEnum {
    Actual,
    Target,
    MinSet,
    MaxSet,
};

enum class GetVariableStatusEnumType {
    Accepted,
    Rejected,
    UnknownComponent,
    UnknownVariable,
    NotSupportedAttributeType,
};

enum class SetVariableStatusEnumType {
    Accepted,
    Rejected,
    UnknownComponent,
    UnknownVariable,
    NotSupportedAttributeType,
    RebootRequired,
};

enum class DataTransferStatus {
    Accepted,
    Rejected,
    UnknownMessageId,
    UnknownVendorId,
    Offline,
};

enum class RegistrationStatus {
    Accepted,
    Pending,
    Rejected,
};

enum class TransactionEvent {
    Started,
    Updated,
    Ended,
};

enum class EventTriggerEnum {
    Alerting,
    Delta,
    Periodic,
};

enum class EventNotificationType {
    HardWiredNotification,
    HardWiredMonitor,
    PreconfiguredMonitor,
    CustomMonitor,
};

struct CustomData {
    std::string vendor_id;
    std::string data;
};

struct DataTransferRequest {
    std::string vendor_id;
    std::optional<std::string> message_id;
    std::optional<std::string> data;
    std::optional<CustomData> custom_data;
};

struct DataTransferResponse {
    DataTransferStatus status;
    std::optional<std::string> data;
    std::optional<CustomData> custom_data;
};

struct EVSE {
    int32_t id;
    std::optional<int32_t> connector_id;
};

struct Component {
    std::string name;
    std::optional<std::string> instance;
    std::optional<EVSE> evse;
};

struct Variable {
    std::string name;
    std::optional<std::string> instance;
};

struct ComponentVariable {
    Component component;
    Variable variable;
};

struct GetVariableRequest {
    ComponentVariable component_variable;
    std::optional<AttributeEnum> attribute_type;
};

struct GetVariableResult {
    GetVariableStatusEnumType status;
    ComponentVariable component_variable;
    std::optional<AttributeEnum> attribute_type;
    std::optional<std::string> value;
};

struct SetVariableRequest {
    ComponentVariable component_variable;
    std::string value;
    std::optional<AttributeEnum> attribute_type;
};

struct SetVariableResult {
    SetVariableStatusEnumType status;
    ComponentVariable component_variable;
    std::optional<AttributeEnum> attribute_type;
};

using MonitorVariableRequest = ComponentVariable;

struct GetVariableRequestList {
    std::vector<GetVariableRequest> items;
};

struct GetVariableResultList {
    std::vector<GetVariableResult> items;
};

struct SetVariableRequestList {
    std::vector<SetVariableRequest> items;
};

struct SetVariableResultList {
    std::vector<SetVariableResult> items;
};

struct SetVariablesArgs {
    SetVariableRequestList variables;
    std::string source;
};

struct MonitorVariableRequestList {
    std::vector<ComponentVariable> items;
};

struct SecurityEvent {
    std::string type;
    std::optional<std::string> info;
    std::optional<bool> critical;
    std::optional<std::string> timestamp;
};

struct StatusInfoType {
    std::string reason_code;
    std::optional<std::string> additional_info;
};

struct BootNotificationResponse {
    RegistrationStatus status;
    std::string current_time;
    int32_t interval;
    std::optional<StatusInfoType> status_info;
};

struct OcppTransactionEvent {
    TransactionEvent transaction_event;
    std::string session_id;
    std::optional<EVSE> evse;
    std::optional<std::string> transaction_id;
};

struct EventData {
    ComponentVariable component_variable;
    int32_t event_id;
    std::string timestamp;
    EventTriggerEnum trigger;
    std::string actual_value;
    EventNotificationType event_notification_type;
    std::optional<int32_t> cause;
    std::optional<std::string> tech_code;
    std::optional<std::string> tech_info;
    std::optional<bool> cleared;
    std::optional<std::string> transaction_id;
    std::optional<int32_t> variable_monitoring_id;
};

struct V2XFreqWattPointType {
    float frequency;
    float power;
};

struct V2XSignalWattPointCurve {
    int32_t signal;
    float power;
};

enum class OperationMode {
    Idle,
    ChargingOnly,
    CentralSetpoint,
    ExternalSetpoint,
    ExternalLimits,
    CentralFrequency,
    LocalFrequency,
    LocalLoadBalancing,
};

struct ChargingSchedulePeriod {
    int32_t start_period;
    float limit;
    std::optional<float> limit_L2;
    std::optional<float> limit_L3;
    std::optional<int32_t> number_phases;
    std::optional<int32_t> stack_level;
    std::optional<int32_t> phase_to_use;
    std::optional<float> discharge_limit;
    std::optional<float> discharge_limit_L2;
    std::optional<float> discharge_limit_L3;
    std::optional<float> setpoint;
    std::optional<float> setpoint_L2;
    std::optional<float> setpoint_L3;
    std::optional<float> setpoint_reactive;
    std::optional<float> setpoint_reactive_L2;
    std::optional<float> setpoint_reactive_L3;
    std::optional<bool> preconditioning_request;
    std::optional<bool> evse_sleep;
    std::optional<float> v2x_baseline;
    std::optional<OperationMode> operation_mode;
    std::optional<std::vector<V2XFreqWattPointType>> v2x_freq_watt_curve;
    std::optional<std::vector<V2XSignalWattPointCurve>> v2x_signal_watt_curve;
};

struct ChargingSchedule {
    int32_t evse;
    std::string charging_rate_unit;
    std::vector<ChargingSchedulePeriod> charging_schedule_period;
    std::optional<int32_t> duration;
    std::optional<std::string> start_schedule;
    std::optional<float> min_charging_rate;
};

struct ChargingSchedules {
    std::vector<ChargingSchedule> schedules;
};

} // namespace everest::lib::API::V1_0::types::ocpp
