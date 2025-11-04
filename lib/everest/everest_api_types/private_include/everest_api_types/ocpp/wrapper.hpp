// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <everest_api_types/ocpp/API.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#include "generated/types/ocpp.hpp"
#pragma GCC diagnostic pop

namespace everest::lib::API::V1_0::types::ocpp {

using AttributeEnum_Internal = ::types::ocpp::AttributeEnum;
using AttributeEnum_External = AttributeEnum;

AttributeEnum_Internal to_internal_api(AttributeEnum_External const& val);
AttributeEnum_External to_external_api(AttributeEnum_Internal const& val);

using GetVariableStatusEnumType_Internal = ::types::ocpp::GetVariableStatusEnumType;
using GetVariableStatusEnumType_External = GetVariableStatusEnumType;

GetVariableStatusEnumType_Internal to_internal_api(GetVariableStatusEnumType_External const& val);
GetVariableStatusEnumType_External to_external_api(GetVariableStatusEnumType_Internal const& val);

using SetVariableStatusEnumType_Internal = ::types::ocpp::SetVariableStatusEnumType;
using SetVariableStatusEnumType_External = SetVariableStatusEnumType;

SetVariableStatusEnumType_Internal to_internal_api(SetVariableStatusEnumType_External const& val);
SetVariableStatusEnumType_External to_external_api(SetVariableStatusEnumType_Internal const& val);

using EventTriggerEnum_Internal = ::types::ocpp::EventTriggerEnum;
using EventTriggerEnum_External = EventTriggerEnum;

EventTriggerEnum_Internal to_internal_api(EventTriggerEnum_External const& val);
EventTriggerEnum_External to_external_api(EventTriggerEnum_Internal const& val);

using EventNotificationType_Internal = ::types::ocpp::EventNotificationType;
using EventNotificationType_External = EventNotificationType;

EventNotificationType_Internal to_internal_api(EventNotificationType_External const& val);
EventNotificationType_External to_external_api(EventNotificationType_Internal const& val);

using DataTransferStatus_Internal = ::types::ocpp::DataTransferStatus;
using DataTransferStatus_External = DataTransferStatus;

DataTransferStatus_Internal to_internal_api(DataTransferStatus_External const& val);
DataTransferStatus_External to_external_api(DataTransferStatus_Internal const& val);

using RegistrationStatus_Internal = ::types::ocpp::RegistrationStatus;
using RegistrationStatus_External = RegistrationStatus;

RegistrationStatus_Internal to_internal_api(RegistrationStatus_External const& val);
RegistrationStatus_External to_external_api(RegistrationStatus_Internal const& val);

using TransactionEvent_Internal = ::types::ocpp::TransactionEvent;
using TransactionEvent_External = TransactionEvent;

TransactionEvent_Internal to_internal_api(TransactionEvent_External const& val);
TransactionEvent_External to_external_api(TransactionEvent_Internal const& val);

using CustomData_Internal = ::types::ocpp::CustomData;
using CustomData_External = CustomData;

CustomData_Internal to_internal_api(CustomData_External const& val);
CustomData_External to_external_api(CustomData_Internal const& val);

using DataTransferRequest_Internal = ::types::ocpp::DataTransferRequest;
using DataTransferRequest_External = DataTransferRequest;

DataTransferRequest_Internal to_internal_api(DataTransferRequest_External const& val);
DataTransferRequest_External to_external_api(DataTransferRequest_Internal const& val);

using DataTransferResponse_Internal = ::types::ocpp::DataTransferResponse;
using DataTransferResponse_External = DataTransferResponse;

DataTransferResponse_Internal to_internal_api(DataTransferResponse_External const& val);
DataTransferResponse_External to_external_api(DataTransferResponse_Internal const& val);

using EVSE_Internal = ::types::ocpp::EVSE;
using EVSE_External = EVSE;

EVSE_Internal to_internal_api(EVSE_External const& val);
EVSE_External to_external_api(EVSE_Internal const& val);

using Component_Internal = ::types::ocpp::Component;
using Component_External = Component;

Component_Internal to_internal_api(Component_External const& val);
Component_External to_external_api(Component_Internal const& val);

using Variable_Internal = ::types::ocpp::Variable;
using Variable_External = Variable;

Variable_Internal to_internal_api(Variable_External const& val);
Variable_External to_external_api(Variable_Internal const& val);

using ComponentVariable_Internal = ::types::ocpp::ComponentVariable;
using ComponentVariable_External = ComponentVariable;

ComponentVariable_Internal to_internal_api(ComponentVariable_External const& val);
ComponentVariable_External to_external_api(ComponentVariable_Internal const& val);

using GetVariableRequest_Internal = ::types::ocpp::GetVariableRequest;
using GetVariableRequest_External = GetVariableRequest;

GetVariableRequest_Internal to_internal_api(GetVariableRequest_External const& val);
GetVariableRequest_External to_external_api(GetVariableRequest_Internal const& val);

using GetVariableResult_Internal = ::types::ocpp::GetVariableResult;
using GetVariableResult_External = GetVariableResult;

GetVariableResult_Internal to_internal_api(GetVariableResult_External const& val);
GetVariableResult_External to_external_api(GetVariableResult_Internal const& val);

using SetVariableRequest_Internal = ::types::ocpp::SetVariableRequest;
using SetVariableRequest_External = SetVariableRequest;

SetVariableRequest_Internal to_internal_api(SetVariableRequest_External const& val);
SetVariableRequest_External to_external_api(SetVariableRequest_Internal const& val);

using SetVariableResult_Internal = ::types::ocpp::SetVariableResult;
using SetVariableResult_External = SetVariableResult;

SetVariableResult_Internal to_internal_api(SetVariableResult_External const& val);
SetVariableResult_External to_external_api(SetVariableResult_Internal const& val);

// MonitorVariableRequest is a ComponentVariable
using MonitorVariableRequest_Internal = ::types::ocpp::ComponentVariable;
using MonitorVariableRequest_External = ComponentVariable;

using GetVariableRequestList_Internal = std::vector<::types::ocpp::GetVariableRequest>;
using GetVariableRequestList_External = GetVariableRequestList;

GetVariableRequestList_Internal to_internal_api(GetVariableRequestList_External const& val);
GetVariableRequestList_External to_external_api(GetVariableRequestList_Internal const& val);

using GetVariableResultList_Internal = std::vector<::types::ocpp::GetVariableResult>;
using GetVariableResultList_External = GetVariableResultList;

GetVariableResultList_Internal to_internal_api(GetVariableResultList_External const& val);
GetVariableResultList_External to_external_api(GetVariableResultList_Internal const& val);

using SetVariableRequestList_Internal = std::vector<::types::ocpp::SetVariableRequest>;
using SetVariableRequestList_External = SetVariableRequestList;

SetVariableRequestList_Internal to_internal_api(SetVariableRequestList_External const& val);
SetVariableRequestList_External to_external_api(SetVariableRequestList_Internal const& val);

using SetVariableResultList_Internal = std::vector<::types::ocpp::SetVariableResult>;
using SetVariableResultList_External = SetVariableResultList;

SetVariableResultList_Internal to_internal_api(SetVariableResultList_External const& val);
SetVariableResultList_External to_external_api(SetVariableResultList_Internal const& val);

using MonitorVariableRequestList_Internal = std::vector<::types::ocpp::ComponentVariable>;
using MonitorVariableRequestList_External = MonitorVariableRequestList;

MonitorVariableRequestList_Internal to_internal_api(MonitorVariableRequestList_External const& val);
MonitorVariableRequestList_External to_external_api(MonitorVariableRequestList_Internal const& val);

using SecurityEvent_Internal = ::types::ocpp::SecurityEvent;
using SecurityEvent_External = SecurityEvent;

SecurityEvent_Internal to_internal_api(SecurityEvent_External const& val);
SecurityEvent_External to_external_api(SecurityEvent_Internal const& val);

using StatusInfoType_Internal = ::types::ocpp::StatusInfoType;
using StatusInfoType_External = StatusInfoType;

StatusInfoType_Internal to_internal_api(StatusInfoType_External const& val);
StatusInfoType_External to_external_api(StatusInfoType_Internal const& val);

using BootNotificationResponse_Internal = ::types::ocpp::BootNotificationResponse;
using BootNotificationResponse_External = BootNotificationResponse;

BootNotificationResponse_Internal to_internal_api(BootNotificationResponse_External const& val);
BootNotificationResponse_External to_external_api(BootNotificationResponse_Internal const& val);

using OcppTransactionEvent_Internal = ::types::ocpp::OcppTransactionEvent;
using OcppTransactionEvent_External = OcppTransactionEvent;

OcppTransactionEvent_Internal to_internal_api(OcppTransactionEvent_External const& val);
OcppTransactionEvent_External to_external_api(OcppTransactionEvent_Internal const& val);

using EventData_Internal = ::types::ocpp::EventData;
using EventData_External = EventData;

EventData_Internal to_internal_api(EventData_External const& val);
EventData_External to_external_api(EventData_Internal const& val);

} // namespace everest::lib::API::V1_0::types::ocpp
