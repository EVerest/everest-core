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

std::string serialize(AttributeEnum val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(GetVariableStatusEnumType val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(SetVariableStatusEnumType val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(DataTransferStatus val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(RegistrationStatus val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(TransactionEvent val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(CustomData const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(DataTransferRequest const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(DataTransferResponse const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(EVSE const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(Component const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(Variable const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ComponentVariable const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(GetVariableRequest const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(GetVariableResult const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(SetVariableRequest const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(SetVariableResult const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(GetVariableRequestList const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(GetVariableResultList const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(SetVariableRequestList const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(SetVariableResultList const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(MonitorVariableRequestList const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(SecurityEvent const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(SetVariablesArgs const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(StatusInfoType const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(BootNotificationResponse const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(OcppTransactionEvent const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::ostream& operator<<(std::ostream& os, AttributeEnum const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, GetVariableStatusEnumType const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, SetVariableStatusEnumType const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, DataTransferStatus const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, RegistrationStatus const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, TransactionEvent const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, CustomData const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, DataTransferRequest const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, DataTransferResponse const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, EVSE const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, Component const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, Variable const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ComponentVariable const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, GetVariableRequest const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, GetVariableResult const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, SetVariableRequest const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, SetVariableResult const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, GetVariableRequestList const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, GetVariableResultList const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, SetVariableRequestList const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, SetVariableResultList const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, SetVariablesArgs const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, MonitorVariableRequestList const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, SecurityEvent const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, StatusInfoType const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, BootNotificationResponse const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, OcppTransactionEvent const& val) {
    os << serialize(val);
    return os;
}

template <> AttributeEnum deserialize(std::string const& val) {
    auto data = json::parse(val);
    AttributeEnum obj = data;
    return obj;
}

template <> GetVariableStatusEnumType deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    GetVariableStatusEnumType obj = data;
    return obj;
}

template <> SetVariableStatusEnumType deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    SetVariableStatusEnumType obj = data;
    return obj;
}

template <> DataTransferStatus deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    DataTransferStatus obj = data;
    return obj;
}

template <> RegistrationStatus deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    RegistrationStatus obj = data;
    return obj;
}

template <> TransactionEvent deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    TransactionEvent obj = data;
    return obj;
}

template <> CustomData deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    CustomData obj = data;
    return obj;
}

template <> DataTransferRequest deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    DataTransferRequest obj = data;
    return obj;
}

template <> DataTransferResponse deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    DataTransferResponse obj = data;
    return obj;
}

template <> EVSE deserialize<>(const std::string& val) {
    auto data = json::parse(val);
    EVSE obj = data;
    return obj;
}

template <> Component deserialize<>(const std::string& val) {
    auto data = json::parse(val);
    Component obj = data;
    return obj;
}

template <> Variable deserialize<>(const std::string& val) {
    auto data = json::parse(val);
    Variable obj = data;
    return obj;
}

template <> ComponentVariable deserialize<>(const std::string& val) {
    auto data = json::parse(val);
    ComponentVariable obj = data;
    return obj;
}

template <> GetVariableRequest deserialize<>(const std::string& val) {
    auto data = json::parse(val);
    GetVariableRequest obj = data;
    return obj;
}

template <> GetVariableResult deserialize<>(const std::string& val) {
    auto data = json::parse(val);
    GetVariableResult obj = data;
    return obj;
}

template <> SetVariableRequest deserialize<>(const std::string& val) {
    auto data = json::parse(val);
    SetVariableRequest obj = data;
    return obj;
}

template <> SetVariableResult deserialize<>(const std::string& val) {
    auto data = json::parse(val);
    SetVariableResult obj = data;
    return obj;
}

template <> GetVariableRequestList deserialize<>(const std::string& val) {
    auto data = json::parse(val);
    GetVariableRequestList obj = data;
    return obj;
}

template <> GetVariableResultList deserialize<>(const std::string& val) {
    auto data = json::parse(val);
    GetVariableResultList obj = data;
    return obj;
}

template <> SetVariableRequestList deserialize<>(const std::string& val) {
    auto data = json::parse(val);
    SetVariableRequestList obj = data;
    return obj;
}

template <> SetVariableResultList deserialize<>(const std::string& val) {
    auto data = json::parse(val);
    SetVariableResultList obj = data;
    return obj;
}

template <> SetVariablesArgs deserialize<>(const std::string& val) {
    auto data = json::parse(val);
    SetVariablesArgs obj = data;
    return obj;
}

template <> MonitorVariableRequestList deserialize<>(const std::string& val) {
    auto data = json::parse(val);
    MonitorVariableRequestList obj = data;
    return obj;
}

template <> SecurityEvent deserialize<>(const std::string& val) {
    auto data = json::parse(val);
    SecurityEvent obj = data;
    return obj;
}

template <> StatusInfoType deserialize<>(const std::string& val) {
    auto data = json::parse(val);
    StatusInfoType obj = data;
    return obj;
}

template <> BootNotificationResponse deserialize<>(const std::string& val) {
    auto data = json::parse(val);
    BootNotificationResponse obj = data;
    return obj;
}

template <> OcppTransactionEvent deserialize<>(const std::string& val) {
    auto data = json::parse(val);
    OcppTransactionEvent obj = data;
    return obj;
}

} // namespace everest::lib::API::V1_0::types::ocpp
