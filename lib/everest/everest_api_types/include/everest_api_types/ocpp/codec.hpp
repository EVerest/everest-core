// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "API.hpp"
#include <optional>
#include <string>

namespace everest::lib::API::V1_0::types::ocpp {

std::string serialize(AttributeEnum val) noexcept;
std::string serialize(GetVariableStatusEnumType val) noexcept;
std::string serialize(SetVariableStatusEnumType val) noexcept;
std::string serialize(EventTriggerEnum const& val) noexcept;
std::string serialize(EventNotificationType const& val) noexcept;
std::string serialize(DataTransferStatus val) noexcept;
std::string serialize(RegistrationStatus val) noexcept;
std::string serialize(TransactionEvent val) noexcept;
std::string serialize(CustomData const& val) noexcept;
std::string serialize(DataTransferRequest const& val) noexcept;
std::string serialize(DataTransferResponse const& val) noexcept;
std::string serialize(EVSE const& val) noexcept;
std::string serialize(Component const& val) noexcept;
std::string serialize(Variable const& val) noexcept;
std::string serialize(ComponentVariable const& val) noexcept;
std::string serialize(GetVariableRequest const& val) noexcept;
std::string serialize(GetVariableResult const& val) noexcept;
std::string serialize(SetVariableRequest const& val) noexcept;
std::string serialize(SetVariableResult const& val) noexcept;
std::string serialize(GetVariableRequestList const& val) noexcept;
std::string serialize(GetVariableResultList const& val) noexcept;
std::string serialize(SetVariableRequestList const& val) noexcept;
std::string serialize(SetVariableResultList const& val) noexcept;
std::string serialize(SetVariablesArgs const& val) noexcept;
std::string serialize(MonitorVariableRequestList const& val) noexcept;
std::string serialize(SecurityEvent const& val) noexcept;
std::string serialize(StatusInfoType const& val) noexcept;
std::string serialize(BootNotificationResponse const& val) noexcept;
std::string serialize(OcppTransactionEvent const& val) noexcept;
std::string serialize(EventData const& val) noexcept;

std::ostream& operator<<(std::ostream& os, AttributeEnum const& val);
std::ostream& operator<<(std::ostream& os, GetVariableStatusEnumType const& val);
std::ostream& operator<<(std::ostream& os, SetVariableStatusEnumType const& val);
std::ostream& operator<<(std::ostream& os, EventTriggerEnum const& val);
std::ostream& operator<<(std::ostream& os, EventNotificationType const& val);
std::ostream& operator<<(std::ostream& os, DataTransferStatus const& val);
std::ostream& operator<<(std::ostream& os, RegistrationStatus const& val);
std::ostream& operator<<(std::ostream& os, TransactionEvent const& val);
std::ostream& operator<<(std::ostream& os, CustomData const& val);
std::ostream& operator<<(std::ostream& os, DataTransferRequest const& val);
std::ostream& operator<<(std::ostream& os, DataTransferResponse const& val);
std::ostream& operator<<(std::ostream& os, EVSE const& val);
std::ostream& operator<<(std::ostream& os, Component const& val);
std::ostream& operator<<(std::ostream& os, Variable const& val);
std::ostream& operator<<(std::ostream& os, ComponentVariable const& val);
std::ostream& operator<<(std::ostream& os, GetVariableRequest const& val);
std::ostream& operator<<(std::ostream& os, GetVariableResult const& val);
std::ostream& operator<<(std::ostream& os, SetVariableRequest const& val);
std::ostream& operator<<(std::ostream& os, SetVariableResult const& val);
std::ostream& operator<<(std::ostream& os, GetVariableRequestList const& val);
std::ostream& operator<<(std::ostream& os, GetVariableResultList const& val);
std::ostream& operator<<(std::ostream& os, SetVariableRequestList const& val);
std::ostream& operator<<(std::ostream& os, SetVariableResultList const& val);
std::ostream& operator<<(std::ostream& os, MonitorVariableRequestList const& val);
std::ostream& operator<<(std::ostream& os, SetVariablesArgs const& val);
std::ostream& operator<<(std::ostream& os, SecurityEvent const& val);
std::ostream& operator<<(std::ostream& os, StatusInfoType const& val);
std::ostream& operator<<(std::ostream& os, BootNotificationResponse const& val);
std::ostream& operator<<(std::ostream& os, OcppTransactionEvent const& val);
std::ostream& operator<<(std::ostream& os, EventData const& val);

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
