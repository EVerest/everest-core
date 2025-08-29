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

AttributeEnum_Internal toInternalApi(AttributeEnum_External const& val);
AttributeEnum_External toExternalApi(AttributeEnum_Internal const& val);

using GetVariableStatusEnumType_Internal = ::types::ocpp::GetVariableStatusEnumType;
using GetVariableStatusEnumType_External = GetVariableStatusEnumType;

GetVariableStatusEnumType_Internal toInternalApi(GetVariableStatusEnumType_External const& val);
GetVariableStatusEnumType_External toExternalApi(GetVariableStatusEnumType_Internal const& val);

using SetVariableStatusEnumType_Internal = ::types::ocpp::SetVariableStatusEnumType;
using SetVariableStatusEnumType_External = SetVariableStatusEnumType;

SetVariableStatusEnumType_Internal toInternalApi(SetVariableStatusEnumType_External const& val);
SetVariableStatusEnumType_External toExternalApi(SetVariableStatusEnumType_Internal const& val);

using DataTransferStatus_Internal = ::types::ocpp::DataTransferStatus;
using DataTransferStatus_External = DataTransferStatus;

DataTransferStatus_Internal toInternalApi(DataTransferStatus_External const& val);
DataTransferStatus_External toExternalApi(DataTransferStatus_Internal const& val);

using RegistrationStatus_Internal = ::types::ocpp::RegistrationStatus;
using RegistrationStatus_External = RegistrationStatus;

RegistrationStatus_Internal toInternalApi(RegistrationStatus_External const& val);
RegistrationStatus_External toExternalApi(RegistrationStatus_Internal const& val);

using TransactionEvent_Internal = ::types::ocpp::TransactionEvent;
using TransactionEvent_External = TransactionEvent;

TransactionEvent_Internal toInternalApi(TransactionEvent_External const& val);
TransactionEvent_External toExternalApi(TransactionEvent_Internal const& val);

using CustomData_Internal = ::types::ocpp::CustomData;
using CustomData_External = CustomData;

CustomData_Internal toInternalApi(CustomData_External const& val);
CustomData_External toExternalApi(CustomData_Internal const& val);

using DataTransferRequest_Internal = ::types::ocpp::DataTransferRequest;
using DataTransferRequest_External = DataTransferRequest;

DataTransferRequest_Internal toInternalApi(DataTransferRequest_External const& val);
DataTransferRequest_External toExternalApi(DataTransferRequest_Internal const& val);

using DataTransferResponse_Internal = ::types::ocpp::DataTransferResponse;
using DataTransferResponse_External = DataTransferResponse;

DataTransferResponse_Internal toInternalApi(DataTransferResponse_External const& val);
DataTransferResponse_External toExternalApi(DataTransferResponse_Internal const& val);

using EVSE_Internal = ::types::ocpp::EVSE;
using EVSE_External = EVSE;

EVSE_Internal toInternalApi(EVSE_External const& val);
EVSE_External toExternalApi(EVSE_Internal const& val);

using Component_Internal = ::types::ocpp::Component;
using Component_External = Component;

Component_Internal toInternalApi(Component_External const& val);
Component_External toExternalApi(Component_Internal const& val);

using Variable_Internal = ::types::ocpp::Variable;
using Variable_External = Variable;

Variable_Internal toInternalApi(Variable_External const& val);
Variable_External toExternalApi(Variable_Internal const& val);

using ComponentVariable_Internal = ::types::ocpp::ComponentVariable;
using ComponentVariable_External = ComponentVariable;

ComponentVariable_Internal toInternalApi(ComponentVariable_External const& val);
ComponentVariable_External toExternalApi(ComponentVariable_Internal const& val);

using GetVariableRequest_Internal = ::types::ocpp::GetVariableRequest;
using GetVariableRequest_External = GetVariableRequest;

GetVariableRequest_Internal toInternalApi(GetVariableRequest_External const& val);
GetVariableRequest_External toExternalApi(GetVariableRequest_Internal const& val);

using GetVariableResult_Internal = ::types::ocpp::GetVariableResult;
using GetVariableResult_External = GetVariableResult;

GetVariableResult_Internal toInternalApi(GetVariableResult_External const& val);
GetVariableResult_External toExternalApi(GetVariableResult_Internal const& val);

using SetVariableRequest_Internal = ::types::ocpp::SetVariableRequest;
using SetVariableRequest_External = SetVariableRequest;

SetVariableRequest_Internal toInternalApi(SetVariableRequest_External const& val);
SetVariableRequest_External toExternalApi(SetVariableRequest_Internal const& val);

using SetVariableResult_Internal = ::types::ocpp::SetVariableResult;
using SetVariableResult_External = SetVariableResult;

SetVariableResult_Internal toInternalApi(SetVariableResult_External const& val);
SetVariableResult_External toExternalApi(SetVariableResult_Internal const& val);

using GetVariableRequestList_Internal = std::vector<::types::ocpp::GetVariableRequest>;
using GetVariableRequestList_External = GetVariableRequestList;

GetVariableRequestList_Internal toInternalApi(GetVariableRequestList_External const& val);
GetVariableRequestList_External toExternalApi(GetVariableRequestList_Internal const& val);

using GetVariableResultList_Internal = std::vector<::types::ocpp::GetVariableResult>;
using GetVariableResultList_External = GetVariableResultList;

GetVariableResultList_Internal toInternalApi(GetVariableResultList_External const& val);
GetVariableResultList_External toExternalApi(GetVariableResultList_Internal const& val);

using SetVariableRequestList_Internal = std::vector<::types::ocpp::SetVariableRequest>;
using SetVariableRequestList_External = SetVariableRequestList;

SetVariableRequestList_Internal toInternalApi(SetVariableRequestList_External const& val);
SetVariableRequestList_External toExternalApi(SetVariableRequestList_Internal const& val);

using SetVariableResultList_Internal = std::vector<::types::ocpp::SetVariableResult>;
using SetVariableResultList_External = SetVariableResultList;

SetVariableResultList_Internal toInternalApi(SetVariableResultList_External const& val);
SetVariableResultList_External toExternalApi(SetVariableResultList_Internal const& val);

using SecurityEvent_Internal = ::types::ocpp::SecurityEvent;
using SecurityEvent_External = SecurityEvent;

SecurityEvent_Internal toInternalApi(SecurityEvent_External const& val);
SecurityEvent_External toExternalApi(SecurityEvent_Internal const& val);

using StatusInfoType_Internal = ::types::ocpp::StatusInfoType;
using StatusInfoType_External = StatusInfoType;

StatusInfoType_Internal toInternalApi(StatusInfoType_External const& val);
StatusInfoType_External toExternalApi(StatusInfoType_Internal const& val);

using BootNotificationResponse_Internal = ::types::ocpp::BootNotificationResponse;
using BootNotificationResponse_External = BootNotificationResponse;

BootNotificationResponse_Internal toInternalApi(BootNotificationResponse_External const& val);
BootNotificationResponse_External toExternalApi(BootNotificationResponse_Internal const& val);

using OcppTransactionEvent_Internal = ::types::ocpp::OcppTransactionEvent;
using OcppTransactionEvent_External = OcppTransactionEvent;

OcppTransactionEvent_Internal toInternalApi(OcppTransactionEvent_External const& val);
OcppTransactionEvent_External toExternalApi(OcppTransactionEvent_Internal const& val);

} // namespace everest::lib::API::V1_0::types::ocpp
