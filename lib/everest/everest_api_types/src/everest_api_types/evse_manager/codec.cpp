// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "evse_manager/codec.hpp"
#include "evse_manager/API.hpp"
#include "evse_manager/json_codec.hpp"
#include "nlohmann/json.hpp"
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::evse_manager {

std::string serialize(StopTransactionReason val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(StopTransactionRequest const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(StartSessionReason val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(SessionEventEnum val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(SessionEvent const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(Limits const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(EVInfo const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(CarManufacturer val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(SessionStarted const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(SessionFinished const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(TransactionStarted const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(TransactionFinished const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(ChargingStateChangedEvent const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(AuthorizationEvent const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(ErrorSeverity val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(ErrorState val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(ErrorOrigin const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(Error const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(ConnectorTypeEnum const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(Connector const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(Evse const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(EnableSourceEnum const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(EnableStateEnum const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(EnableDisableSource const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(EnableDisableRequest const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::string serialize(AuthorizeResponseArgs const& val) noexcept {
    json result = val;
    return result.dump(4);
}
std::string serialize(PlugAndChargeConfiguration const& val) noexcept {
    json result = val;
    return result.dump(4);
}

std::ostream& operator<<(std::ostream& os, StopTransactionReason const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, StopTransactionRequest const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, StartSessionReason const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, SessionEventEnum const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, SessionEvent const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, Limits const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, EVInfo const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, CarManufacturer const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, SessionStarted const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, SessionFinished const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, TransactionStarted const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, TransactionFinished const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ChargingStateChangedEvent const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, AuthorizationEvent const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ErrorSeverity const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ErrorState const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ErrorOrigin const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, Error const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ConnectorTypeEnum const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, Connector const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, Evse const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, EnableSourceEnum const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, EnableStateEnum const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, EnableDisableSource const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, EnableDisableRequest const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, AuthorizeResponseArgs const& val) {
    os << serialize(val);
    return os;
}
std::ostream& operator<<(std::ostream& os, PlugAndChargeConfiguration const& val) {
    os << serialize(val);
    return os;
}

template <> StopTransactionReason deserialize(std::string const& s) {
    auto data = json::parse(s);
    StopTransactionReason result = data;
    return result;
}

template <> StopTransactionRequest deserialize(std::string const& s) {
    auto data = json::parse(s);
    StopTransactionRequest result = data;
    return result;
}

template <> StartSessionReason deserialize(std::string const& s) {
    auto data = json::parse(s);
    StartSessionReason result = data;
    return result;
}

template <> SessionEventEnum deserialize(std::string const& s) {
    auto data = json::parse(s);
    SessionEventEnum result = data;
    return result;
}

template <> SessionEvent deserialize(std::string const& s) {
    auto data = json::parse(s);
    SessionEvent result = data;
    return result;
}

template <> Limits deserialize(std::string const& s) {
    auto data = json::parse(s);
    Limits result = data;
    return result;
}

template <> EVInfo deserialize(std::string const& s) {
    auto data = json::parse(s);
    EVInfo result = data;
    return result;
}

template <> CarManufacturer deserialize(const std::string& s) {
    auto data = json::parse(s);
    CarManufacturer result = data;
    return result;
}

template <> SessionStarted deserialize(std::string const& s) {
    auto data = json::parse(s);
    SessionStarted result = data;
    return result;
}

template <> SessionFinished deserialize(std::string const& s) {
    auto data = json::parse(s);
    SessionFinished result = data;
    return result;
}

template <> TransactionStarted deserialize(std::string const& s) {
    auto data = json::parse(s);
    TransactionStarted result = data;
    return result;
}

template <> TransactionFinished deserialize(std::string const& s) {
    auto data = json::parse(s);
    TransactionFinished result = data;
    return result;
}

template <> ChargingStateChangedEvent deserialize(std::string const& s) {
    auto data = json::parse(s);
    ChargingStateChangedEvent result = data;
    return result;
}

template <> AuthorizationEvent deserialize(std::string const& s) {
    auto data = json::parse(s);
    AuthorizationEvent result = data;
    return result;
}

template <> ErrorSeverity deserialize(const std::string& s) {
    auto data = json::parse(s);
    ErrorSeverity result = data;
    return result;
}

template <> ErrorState deserialize(const std::string& s) {
    auto data = json::parse(s);
    ErrorState result = data;
    return result;
}

template <> ErrorOrigin deserialize(std::string const& s) {
    auto data = json::parse(s);
    ErrorOrigin result = data;
    return result;
}

template <> Error deserialize(std::string const& s) {
    auto data = json::parse(s);
    Error result = data;
    return result;
}

template <> ConnectorTypeEnum deserialize(std::string const& s) {
    auto data = json::parse(s);
    ConnectorTypeEnum result = data;
    return result;
}

template <> Connector deserialize(std::string const& s) {
    auto data = json::parse(s);
    Connector result = data;
    return result;
}

template <> Evse deserialize(std::string const& s) {
    auto data = json::parse(s);
    Evse result = data;
    return result;
}

template <> EnableSourceEnum deserialize(std::string const& s) {
    auto data = json::parse(s);
    EnableSourceEnum result = data;
    return result;
}

template <> EnableStateEnum deserialize(std::string const& s) {
    auto data = json::parse(s);
    EnableStateEnum result = data;
    return result;
}

template <> EnableDisableSource deserialize(std::string const& s) {
    auto data = json::parse(s);
    EnableDisableSource result = data;
    return result;
}

template <> EnableDisableRequest deserialize(std::string const& s) {
    auto data = json::parse(s);
    EnableDisableRequest result = data;
    return result;
}

template <> AuthorizeResponseArgs deserialize(std::string const& s) {
    auto data = json::parse(s);
    AuthorizeResponseArgs result = data;
    return result;
}

template <> PlugAndChargeConfiguration deserialize(std::string const& s) {
    auto data = json::parse(s);
    PlugAndChargeConfiguration result = data;
    return result;
}

} // namespace everest::lib::API::V1_0::types::evse_manager
