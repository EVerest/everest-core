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

struct SecurityEvent {
    std::string type;
    std::optional<std::string> info;
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

} // namespace everest::lib::API::V1_0::types::ocpp
