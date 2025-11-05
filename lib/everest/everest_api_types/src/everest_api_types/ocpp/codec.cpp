// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "ocpp/codec.hpp"
#include "nlohmann/json.hpp"
#include "ocpp/API.hpp"
#include "ocpp/json_codec.hpp"
#include "utilities/constants.hpp"
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::ocpp {

#define create_serialize_impl(A)                                                                                       \
    std::string serialize(A const& val) noexcept {                                                                     \
        json result = val;                                                                                             \
        return result.dump(json_indent);                                                                               \
    }                                                                                                                  \
    std::ostream& operator<<(std::ostream& os, A const& val) {                                                         \
        os << serialize(val);                                                                                          \
        return os;                                                                                                     \
    }                                                                                                                  \
    template <> A deserialize(std::string const& val) {                                                                \
        auto data = json::parse(val);                                                                                  \
        A obj = data;                                                                                                  \
        return obj;                                                                                                    \
    }

create_serialize_impl(AttributeEnum);
create_serialize_impl(GetVariableStatusEnumType);
create_serialize_impl(SetVariableStatusEnumType);
create_serialize_impl(EventTriggerEnum);
create_serialize_impl(EventNotificationType);
create_serialize_impl(DataTransferStatus);
create_serialize_impl(RegistrationStatus);
create_serialize_impl(TransactionEvent);
create_serialize_impl(CustomData);
create_serialize_impl(DataTransferRequest);
create_serialize_impl(DataTransferResponse);
create_serialize_impl(EVSE);
create_serialize_impl(Component);
create_serialize_impl(Variable);
create_serialize_impl(ComponentVariable);
create_serialize_impl(GetVariableRequest);
create_serialize_impl(GetVariableResult);
create_serialize_impl(SetVariableRequest);
create_serialize_impl(SetVariableResult);
create_serialize_impl(GetVariableRequestList);
create_serialize_impl(GetVariableResultList);
create_serialize_impl(SetVariableRequestList);
create_serialize_impl(SetVariableResultList);
create_serialize_impl(SecurityEvent);
create_serialize_impl(StatusInfoType);
create_serialize_impl(BootNotificationResponse);
create_serialize_impl(OcppTransactionEvent);
create_serialize_impl(MonitorVariableRequestList);
create_serialize_impl(SetVariablesArgs);
create_serialize_impl(EventData);
create_serialize_impl(ChargingSchedules);
create_serialize_impl(ChargingSchedule);
create_serialize_impl(ChargingSchedulePeriod);
create_serialize_impl(OperationMode);
create_serialize_impl(V2XSignalWattPointCurve);
create_serialize_impl(V2XFreqWattPointType);

#undef create_serialize_impl

} // namespace everest::lib::API::V1_0::types::ocpp
