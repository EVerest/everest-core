// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "API.hpp"
#include <optional>
#include <string>

namespace everest::lib::API::V1_0::types::ocpp {

#define create_serialize_interface(A)                                                                                  \
    std::string serialize(A const& val) noexcept;                                                                      \
    std::ostream& operator<<(std::ostream& os, A const& val)

create_serialize_interface(AttributeEnum);
create_serialize_interface(GetVariableStatusEnumType);
create_serialize_interface(SetVariableStatusEnumType);
create_serialize_interface(EventTriggerEnum);
create_serialize_interface(EventNotificationType);
create_serialize_interface(DataTransferStatus);
create_serialize_interface(RegistrationStatus);
create_serialize_interface(TransactionEvent);
create_serialize_interface(CustomData);
create_serialize_interface(DataTransferRequest);
create_serialize_interface(DataTransferResponse);
create_serialize_interface(EVSE);
create_serialize_interface(Component);
create_serialize_interface(Variable);
create_serialize_interface(ComponentVariable);
create_serialize_interface(GetVariableRequest);
create_serialize_interface(GetVariableResult);
create_serialize_interface(SetVariableRequest);
create_serialize_interface(SetVariableResult);
create_serialize_interface(GetVariableRequestList);
create_serialize_interface(GetVariableResultList);
create_serialize_interface(SetVariableRequestList);
create_serialize_interface(SetVariableResultList);
create_serialize_interface(SecurityEvent);
create_serialize_interface(StatusInfoType);
create_serialize_interface(BootNotificationResponse);
create_serialize_interface(OcppTransactionEvent);
create_serialize_interface(MonitorVariableRequestList);
create_serialize_interface(SetVariablesArgs);
create_serialize_interface(EventData);
create_serialize_interface(ChargingSchedules);
create_serialize_interface(ChargingSchedule);
create_serialize_interface(ChargingSchedulePeriod);
create_serialize_interface(OperationMode);
create_serialize_interface(V2XSignalWattPointCurve);
create_serialize_interface(V2XFreqWattPointType);

#undef create_serialize_interface

template <class T> T deserialize(std::string const& val);
template <class T> std::optional<T> try_deserialize(std::string const& val) {
    try {
        return deserialize<T>(val);
    } catch (...) {
        return std::nullopt;
    }
}
template <class T> bool adl_deserialize(std::string const& json_data, T& obj) {
    auto opt = try_deserialize<T>(json_data);
    if (opt) {
        obj = opt.value();
        return true;
    }
    return false;
}

} // namespace everest::lib::API::V1_0::types::ocpp
