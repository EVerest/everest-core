// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "ocpp/json_codec.hpp"
#include "nlohmann/json.hpp"
#include "ocpp/API.hpp"
#include "ocpp/codec.hpp"

namespace everest::lib::API::V1_0::types::ocpp {

using json = nlohmann::json;

void to_json(json& j, AttributeEnum const& k) noexcept {
    switch (k) {
    case AttributeEnum::Actual:
        j = "Actual";
        return;
    case AttributeEnum::Target:
        j = "Target";
        return;
    case AttributeEnum::MinSet:
        j = "MinSet";
        return;
    case AttributeEnum::MaxSet:
        j = "MaxSet";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::ocpp::AttributeEnum";
}

void from_json(const json& j, AttributeEnum& k) {
    std::string s = j;
    if (s == "Actual") {
        k = AttributeEnum::Actual;
        return;
    }
    if (s == "Target") {
        k = AttributeEnum::Target;
        return;
    }
    if (s == "MinSet") {
        k = AttributeEnum::MinSet;
        return;
    }
    if (s == "MaxSet") {
        k = AttributeEnum::MaxSet;
        return;
    }

    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::ocpp::AttributeEnum");
}

void to_json(json& j, GetVariableStatusEnumType const& k) noexcept {
    switch (k) {
    case GetVariableStatusEnumType::Accepted:
        j = "Accepted";
        return;
    case GetVariableStatusEnumType::Rejected:
        j = "Rejected";
        return;
    case GetVariableStatusEnumType::UnknownComponent:
        j = "UnknownComponent";
        return;
    case GetVariableStatusEnumType::UnknownVariable:
        j = "UnknownVariable";
        return;
    case GetVariableStatusEnumType::NotSupportedAttributeType:
        j = "NotSupportedAttributeType";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::ocpp::GetVariableStatueEnum";
}

void from_json(const json& j, GetVariableStatusEnumType& k) {
    std::string s = j;
    if (s == "Accepted") {
        k = GetVariableStatusEnumType::Accepted;
        return;
    }
    if (s == "Rejected") {
        k = GetVariableStatusEnumType::Rejected;
        return;
    }
    if (s == "UnknownComponent") {
        k = GetVariableStatusEnumType::UnknownComponent;
        return;
    }
    if (s == "UnknownVariable") {
        k = GetVariableStatusEnumType::UnknownVariable;
        return;
    }
    if (s == "NotSupportedAttributeType") {
        k = GetVariableStatusEnumType::NotSupportedAttributeType;
        return;
    }

    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::ocpp::GetVariableStatusEnum");
}

void to_json(json& j, SetVariableStatusEnumType const& k) noexcept {
    switch (k) {
    case SetVariableStatusEnumType::Accepted:
        j = "Accepted";
        return;
    case SetVariableStatusEnumType::Rejected:
        j = "Rejected";
        return;
    case SetVariableStatusEnumType::UnknownComponent:
        j = "UnknownComponent";
        return;
    case SetVariableStatusEnumType::UnknownVariable:
        j = "UnknownVariable";
        return;
    case SetVariableStatusEnumType::NotSupportedAttributeType:
        j = "NotSupportedAttributeType";
        return;
    case SetVariableStatusEnumType::RebootRequired:
        j = "RebootRequired";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::ocpp::SetVariableStatueEnum";
}

void from_json(const json& j, SetVariableStatusEnumType& k) {
    std::string s = j;
    if (s == "Accepted") {
        k = SetVariableStatusEnumType::Accepted;
        return;
    }
    if (s == "Rejected") {
        k = SetVariableStatusEnumType::Rejected;
        return;
    }
    if (s == "UnknownComponent") {
        k = SetVariableStatusEnumType::UnknownComponent;
        return;
    }
    if (s == "UnknownVariable") {
        k = SetVariableStatusEnumType::UnknownVariable;
        return;
    }
    if (s == "NotSupportedAttributeType") {
        k = SetVariableStatusEnumType::NotSupportedAttributeType;
        return;
    }
    if (s == "RebootRequired") {
        k = SetVariableStatusEnumType::RebootRequired;
        return;
    }

    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::ocpp::SetVariableStatusEnum");
}

void to_json(json& j, DataTransferStatus const& k) noexcept {
    switch (k) {
    case DataTransferStatus::Accepted:
        j = "Accepted";
        return;
    case DataTransferStatus::Rejected:
        j = "Rejected";
        return;
    case DataTransferStatus::UnknownMessageId:
        j = "UnknownMessageId";
        return;
    case DataTransferStatus::UnknownVendorId:
        j = "UnknownVendorId";
        return;
    case DataTransferStatus::Offline:
        j = "Offline";
        return;

        j = "INVALID_VALUE__everest::lib::API::V1_0::types::ocpp::DataTransferStatus";
    }
}

void from_json(const json& j, DataTransferStatus& k) {
    std::string s = j;
    if (s == "Accepted") {
        k = DataTransferStatus::Accepted;
        return;
    }
    if (s == "Rejected") {
        k = DataTransferStatus::Rejected;
        return;
    }
    if (s == "UnknownMessageId") {
        k = DataTransferStatus::UnknownMessageId;
        return;
    }
    if (s == "UnknownVendorId") {
        k = DataTransferStatus::UnknownVendorId;
        return;
    }
    if (s == "Offline") {
        k = DataTransferStatus::Offline;
        return;
    }

    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::ocpp::DataTransferStatus");
}

void to_json(json& j, RegistrationStatus const& k) noexcept {
    switch (k) {
    case RegistrationStatus::Accepted:
        j = "Accepted";
        return;
    case RegistrationStatus::Pending:
        j = "Pending";
        return;
    case RegistrationStatus::Rejected:
        j = "Rejected";
        return;
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::ocpp::RegistrationStatus";
}

void from_json(const json& j, RegistrationStatus& k) {
    std::string s = j;
    if (s == "Accepted") {
        k = RegistrationStatus::Accepted;
        return;
    }
    if (s == "Pending") {
        k = RegistrationStatus::Pending;
        return;
    }
    if (s == "Rejected") {
        k = RegistrationStatus::Rejected;
        return;
    }

    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::ocpp::RegistrationStatus");
}

void to_json(json& j, TransactionEvent const& k) noexcept {
    switch (k) {
    case TransactionEvent::Started:
        j = "Started";
        return;
    case TransactionEvent::Updated:
        j = "Updated";
        return;
    case TransactionEvent::Ended:
        j = "Ended";
        return;
    }
    j = "INVALID_VALUE__everest::lib::API::V1_0::types::ocpp::TransactionEvent";
}

void from_json(const json& j, TransactionEvent& k) {
    std::string s = j;
    if (s == "Started") {
        k = TransactionEvent::Started;
        return;
    }
    if (s == "Updated") {
        k = TransactionEvent::Updated;
        return;
    }
    if (s == "Ended") {
        k = TransactionEvent::Ended;
        return;
    }

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
    k.vendor_id = j.at("vendor_id");
    k.data = j.at("data");
}

void to_json(json& j, DataTransferRequest const& k) noexcept {
    j = json{
        {"vendor_id", k.vendor_id},
    };
    if (k.message_id) {
        j["message_id"] = k.message_id.value();
    }
    if (k.data) {
        j["data"] = k.data.value();
    }
    if (k.custom_data) {
        j["custom_data"] = k.custom_data.value();
    }
}

void from_json(const json& j, DataTransferRequest& k) {
    k.vendor_id = j.at("vendor_id");

    if (j.contains("message_id")) {
        k.message_id.emplace(j.at("message_id"));
    }
    if (j.contains("data")) {
        k.data.emplace(j.at("data"));
    }
    if (j.contains("custom_data")) {
        k.custom_data.emplace(j.at("custom_data"));
    }
}

void to_json(json& j, DataTransferResponse const& k) noexcept {
    j = json{
        {"status", k.status},
    };
    if (k.data) {
        j["data"] = k.data.value();
    }
    if (k.custom_data) {
        j["custom_data"] = k.custom_data.value();
    }
}

void from_json(const json& j, DataTransferResponse& k) {
    k.status = j.at("status");
    if (j.contains("data")) {
        k.data.emplace(j.at("data"));
    }
    if (j.contains("custom_data")) {
        k.custom_data.emplace(j.at("custom_data"));
    }
}

void to_json(json& j, EVSE const& k) noexcept {
    j = json{
        {"id", k.id},
    };
    if (k.connector_id) {
        j["connector_id"] = k.connector_id.value();
    }
}

void from_json(const json& j, EVSE& k) {
    k.id = j.at("id");

    if (j.contains("connector_id")) {
        k.connector_id.emplace(j.at("connector_id"));
    }
}

void to_json(json& j, Component const& k) noexcept {
    j = json{
        {"name", k.name},
    };
    if (k.instance) {
        j["instance"] = k.instance.value();
    }
    if (k.evse) {
        j["evse"] = k.evse.value();
    }
}

void from_json(const json& j, Component& k) {
    k.name = j.at("name");
    if (j.contains("instance")) {
        k.instance.emplace(j.at("instance"));
    }
    if (j.contains("evse")) {
        k.evse.emplace(j.at("evse"));
    }
}

void to_json(json& j, Variable const& k) noexcept {
    j = json{
        {"name", k.name},
    };
    if (k.instance) {
        j["instance"] = k.instance.value();
    }
}

void from_json(const json& j, Variable& k) {
    k.name = j.at("name");
    if (j.contains("instance")) {
        k.instance.emplace(j.at("instance"));
    }
}

void to_json(json& j, ComponentVariable const& k) noexcept {
    j = json{
        {"component", k.component},
        {"variable", k.variable},
    };
}

void from_json(const json& j, ComponentVariable& k) {
    k.component = j.at("component");
    k.variable = j.at("variable");
}

void to_json(json& j, GetVariableRequest const& k) noexcept {
    j = json{
        {"component_variable", k.component_variable},
    };
    if (k.attribute_type) {
        j["attribute_type"] = k.attribute_type.value();
    }
}

void from_json(const json& j, GetVariableRequest& k) {
    k.component_variable = j.at("component_variable");
    if (j.contains("attribute_type")) {
        k.attribute_type.emplace(j.at("attribute_type"));
    }
}

void to_json(json& j, GetVariableResult const& k) noexcept {
    j = json{
        {"status", k.status},
        {"component_variable", k.component_variable},
    };
    if (k.attribute_type) {
        j["attribute_type"] = k.attribute_type.value();
    }
    if (k.value) {
        j["value"] = k.value.value();
    }
}

void from_json(const json& j, GetVariableResult& k) {
    k.status = j.at("status");
    k.component_variable = j.at("component_variable");

    if (j.contains("attribute_type")) {
        k.attribute_type.emplace(j.at("attribute_type"));
    }
    if (j.contains("value")) {
        k.value.emplace(j.at("value"));
    }
}

void to_json(json& j, SetVariableRequest const& k) noexcept {
    j = json{
        {"component_variable", k.component_variable},
        {"value", k.value},
    };
    if (k.attribute_type) {
        j["attribute_type"] = k.attribute_type.value();
    }
}

void from_json(const json& j, SetVariableRequest& k) {
    k.component_variable = j.at("component_variable");
    k.value = j.at("value");

    if (j.contains("attribute_type")) {
        k.attribute_type.emplace(j.at("attribute_type"));
    }
}

void to_json(json& j, SetVariableResult const& k) noexcept {
    j = json{
        {"status", k.status},
        {"component_variable", k.component_variable},
    };
    if (k.attribute_type) {
        j["attribute_type"] = k.attribute_type.value();
    }
}

void from_json(const json& j, SetVariableResult& k) {
    k.status = j.at("status");
    k.component_variable = j.at("component_variable");

    if (j.contains("attribute_type")) {
        k.attribute_type.emplace(j.at("attribute_type"));
    }
}

void to_json(json& j, GetVariableRequestList const& k) noexcept {
    j["items"] = json::array();
    for (auto val : k.items) {
        j["items"].push_back(val);
    }
}

void from_json(const json& j, GetVariableRequestList& k) {
    k.items.clear();
    for (auto val : j.at("items")) {
        k.items.push_back(val);
    }
}

void to_json(json& j, GetVariableResultList const& k) noexcept {
    j["items"] = json::array();
    for (auto val : k.items) {
        j["items"].push_back(val);
    }
}

void from_json(const json& j, GetVariableResultList& k) {
    k.items.clear();
    for (auto val : j.at("items")) {
        k.items.push_back(val);
    }
}

void to_json(json& j, SetVariableRequestList const& k) noexcept {
    j["items"] = json::array();
    for (auto val : k.items) {
        j["items"].push_back(val);
    }
}

void from_json(const json& j, SetVariableRequestList& k) {
    k.items.clear();
    for (auto val : j.at("items")) {
        k.items.push_back(val);
    }
}

void to_json(json& j, SetVariableResultList const& k) noexcept {
    j["items"] = json::array();
    for (auto val : k.items) {
        j["items"].push_back(val);
    }
}

void from_json(const json& j, SetVariableResultList& k) {
    k.items.clear();
    for (auto val : j.at("items")) {
        k.items.push_back(val);
    }
}

void to_json(json& j, SetVariablesArgs const& k) noexcept {
    j = json{
        {"variables", k.variables},
        {"source", k.source},
    };
}

void from_json(const json& j, SetVariablesArgs& k) {
    k.variables = j["variables"];
    k.source = j["source"];
}

void to_json(json& j, SecurityEvent const& k) noexcept {
    j = json{
        {"type", k.type},
    };
    if (k.info) {
        j["info"] = k.info.value();
    }
    if (k.critical) {
        j["critical"] = k.critical.value();
    }
    if (k.timestamp) {
        j["timestamp"] = k.timestamp.value();
    }
}

void from_json(const json& j, SecurityEvent& k) {
    k.type = j.at("type");
    if (j.contains("info")) {
        k.info.emplace(j.at("info"));
    }
    if (j.contains("critical")) {
        k.critical.emplace(j.at("critical"));
    }
    if (j.contains("timestamp")) {
        k.timestamp.emplace(j.at("timestamp"));
    }
}

void to_json(json& j, StatusInfoType const& k) noexcept {
    j = json{
        {"reason_code", k.reason_code},
    };
    if (k.additional_info) {
        j["additional_info"] = k.additional_info.value();
    }
}

void from_json(const json& j, StatusInfoType& k) {
    k.reason_code = j.at("reason_code");
    if (j.contains("additional_info")) {
        k.additional_info.emplace(j.at("additional_info"));
    }
}

void to_json(json& j, BootNotificationResponse const& k) noexcept {
    j = json{
        {"status", k.status},
        {"current_time", k.current_time},
        {"interval", k.interval},
    };
    if (k.status_info) {
        j["status_info"] = k.status_info.value();
    }
}

void from_json(const json& j, BootNotificationResponse& k) {
    k.status = j.at("status");
    k.current_time = j.at("current_time");
    k.interval = j.at("interval");

    if (j.contains("status_info")) {
        k.status_info.emplace(j.at("status_info"));
    }
}

void to_json(json& j, OcppTransactionEvent const& k) noexcept {
    j = json{
        {"transaction_event", k.transaction_event},
        {"session_id", k.session_id},
    };
    if (k.evse) {
        j["evse"] = k.evse.value();
    }
    if (k.transaction_id) {
        j["transaction_id"] = k.transaction_id.value();
    }
}

void from_json(const json& j, OcppTransactionEvent& k) {
    k.transaction_event = j.at("transaction_event");
    k.session_id = j.at("session_id");

    if (j.contains("evse")) {
        k.evse.emplace(j.at("evse"));
    }
    if (j.contains("transaction_id")) {
        k.transaction_id.emplace(j.at("transaction_id"));
    }
}

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
        k.A.value().clear();                                                                                           \
        for (auto val : j.at(#A)) {                                                                                    \
            k.A.value().push_back(val);                                                                                \
        }                                                                                                              \
    }

// ----------------------------------------------------------------------------
// conversions using the above macros

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
