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

#define create_interface(A)                                                                                            \
    using A##_Internal = ::types::ocpp::A;                                                                             \
    using A##_External = A;                                                                                            \
    A##_Internal to_internal_api(A##_External const& val);                                                             \
    A##_External to_external_api(A##_Internal const& val)

#define create_interface2(A, B)                                                                                        \
    using A##_Internal = B;                                                                                            \
    using A##_External = A;                                                                                            \
    A##_Internal to_internal_api(A##_External const& val);                                                             \
    A##_External to_external_api(A##_Internal const& val)

create_interface(AttributeEnum);
create_interface(GetVariableStatusEnumType);
create_interface(SetVariableStatusEnumType);
create_interface(EventTriggerEnum);
create_interface(EventNotificationType);
create_interface(DataTransferStatus);
create_interface(RegistrationStatus);
create_interface(TransactionEvent);
create_interface(CustomData);
create_interface(DataTransferRequest);
create_interface(DataTransferResponse);
create_interface(EVSE);
create_interface(Component);
create_interface(Variable);
create_interface(ComponentVariable);
create_interface(GetVariableRequest);
create_interface(GetVariableResult);
create_interface(SetVariableRequest);
create_interface(SetVariableResult);
create_interface2(GetVariableRequestList, std::vector<::types::ocpp::GetVariableRequest>);
create_interface2(GetVariableResultList, std::vector<::types::ocpp::GetVariableResult>);
create_interface2(SetVariableRequestList, std::vector<::types::ocpp::SetVariableRequest>);
create_interface2(SetVariableResultList, std::vector<::types::ocpp::SetVariableResult>);
create_interface(SecurityEvent);
create_interface(StatusInfoType);
create_interface(BootNotificationResponse);
create_interface(OcppTransactionEvent);

// MonitorVariableRequest is a ComponentVariable
using MonitorVariableRequest_Internal = ::types::ocpp::ComponentVariable;
using MonitorVariableRequest_External = ComponentVariable;

create_interface2(MonitorVariableRequestList, std::vector<::types::ocpp::ComponentVariable>);
create_interface(EventData);
create_interface(ChargingSchedules);
create_interface(ChargingSchedule);
create_interface(ChargingSchedulePeriod);
create_interface2(OperationMode, ::types::ocpp::Operation_mode);
create_interface(V2XSignalWattPointCurve);
create_interface(V2XFreqWattPointType);

#undef create_interface
#undef create_interface2

} // namespace everest::lib::API::V1_0::types::ocpp
