// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "nlohmann/json_fwd.hpp"
#include <everest_api_types/ocpp/API.hpp>

namespace everest::lib::API::V1_0::types::ocpp {

using json = nlohmann::json;

#define create_json_interface(A)                                                                                       \
    void to_json(json& j, A const& k) noexcept;                                                                        \
    void from_json(const json& j, A& k)

create_json_interface(AttributeEnum);
create_json_interface(GetVariableStatusEnumType);
create_json_interface(SetVariableStatusEnumType);
create_json_interface(EventTriggerEnum);
create_json_interface(EventNotificationType);
create_json_interface(DataTransferStatus);
create_json_interface(RegistrationStatus);
create_json_interface(TransactionEvent);
create_json_interface(CustomData);
create_json_interface(DataTransferRequest);
create_json_interface(DataTransferResponse);
create_json_interface(EVSE);
create_json_interface(Component);
create_json_interface(Variable);
create_json_interface(ComponentVariable);
create_json_interface(GetVariableRequest);
create_json_interface(GetVariableResult);
create_json_interface(SetVariableRequest);
create_json_interface(SetVariableResult);
create_json_interface(GetVariableRequestList);
create_json_interface(GetVariableResultList);
create_json_interface(SetVariableRequestList);
create_json_interface(SetVariableResultList);
create_json_interface(SecurityEvent);
create_json_interface(StatusInfoType);
create_json_interface(BootNotificationResponse);
create_json_interface(OcppTransactionEvent);
create_json_interface(MonitorVariableRequestList);
create_json_interface(SetVariablesArgs);
create_json_interface(EventData);
create_json_interface(ChargingSchedules);
create_json_interface(ChargingSchedule);
create_json_interface(ChargingSchedulePeriod);
create_json_interface(OperationMode);
create_json_interface(V2XSignalWattPointCurve);
create_json_interface(V2XFreqWattPointType);

#undef create_json_interface

} // namespace everest::lib::API::V1_0::types::ocpp
