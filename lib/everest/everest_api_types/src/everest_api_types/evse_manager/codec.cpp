// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "evse_manager/codec.hpp"
#include "evse_manager/API.hpp"
#include "evse_manager/json_codec.hpp"
#include "nlohmann/json.hpp"
#include "utilities/constants.hpp"
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::evse_manager {

std::string serialize(StopTransactionReason val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(StopTransactionRequest const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(StartSessionReason val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(SessionEventEnum val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(SessionEvent const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(Limits const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(EVInfo const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(CarManufacturer val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(SessionStarted const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(SessionFinished const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(TransactionStarted const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(TransactionFinished const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ChargingStateChangedEvent const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(AuthorizationEvent const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ErrorSeverity val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ErrorState val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ErrorOrigin const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(Error const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ConnectorTypeEnum const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(Connector const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(Evse const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(EnableSourceEnum const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(EnableStateEnum const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(EnableDisableSource const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(EnableDisableRequest const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(AuthorizeResponseArgs const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}
std::string serialize(PlugAndChargeConfiguration const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(EvseStateEnum const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(SessionInfo const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(PauseChargingEVSEReasonEnum val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(ChargingPausedEVSEReasons const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(HlcSessionFailedReasonEnum val) noexcept {
    return nlohmann::json(val).dump(json_indent);
}

std::string serialize(HlcSessionFailedEvent const& val) noexcept {
    return nlohmann::json(val).dump(json_indent);
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

std::ostream& operator<<(std::ostream& os, EvseStateEnum const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, SessionInfo const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, PauseChargingEVSEReasonEnum const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ChargingPausedEVSEReasons const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, HlcSessionFailedReasonEnum const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, HlcSessionFailedEvent const& val) {
    os << serialize(val);
    return os;
}

template <> StopTransactionReason deserialize(std::string const& val) {
    return json::parse(val);
}

template <> StopTransactionRequest deserialize(std::string const& val) {
    return json::parse(val);
}

template <> StartSessionReason deserialize(std::string const& val) {
    return json::parse(val);
}

template <> SessionEventEnum deserialize(std::string const& val) {
    return json::parse(val);
}

template <> SessionEvent deserialize(std::string const& val) {
    return json::parse(val);
}

template <> Limits deserialize(std::string const& val) {
    return json::parse(val);
}

template <> EVInfo deserialize(std::string const& val) {
    return json::parse(val);
}

template <> CarManufacturer deserialize(std::string const& val) {
    return json::parse(val);
}

template <> SessionStarted deserialize(std::string const& val) {
    return json::parse(val);
}

template <> SessionFinished deserialize(std::string const& val) {
    return json::parse(val);
}

template <> TransactionStarted deserialize(std::string const& val) {
    return json::parse(val);
}

template <> TransactionFinished deserialize(std::string const& val) {
    return json::parse(val);
}

template <> ChargingStateChangedEvent deserialize(std::string const& val) {
    return json::parse(val);
}

template <> AuthorizationEvent deserialize(std::string const& val) {
    return json::parse(val);
}

template <> ErrorSeverity deserialize(std::string const& val) {
    return json::parse(val);
}

template <> ErrorState deserialize(std::string const& val) {
    return json::parse(val);
}

template <> ErrorOrigin deserialize(std::string const& val) {
    return json::parse(val);
}

template <> Error deserialize(std::string const& val) {
    return json::parse(val);
}

template <> ConnectorTypeEnum deserialize(std::string const& val) {
    return json::parse(val);
}

template <> Connector deserialize(std::string const& val) {
    return json::parse(val);
}

template <> Evse deserialize(std::string const& val) {
    return json::parse(val);
}

template <> EnableSourceEnum deserialize(std::string const& val) {
    return json::parse(val);
}

template <> EnableStateEnum deserialize(std::string const& val) {
    return json::parse(val);
}

template <> EnableDisableSource deserialize(std::string const& val) {
    return json::parse(val);
}

template <> EnableDisableRequest deserialize(std::string const& val) {
    return json::parse(val);
}

template <> AuthorizeResponseArgs deserialize(std::string const& val) {
    return json::parse(val);
}

template <> PlugAndChargeConfiguration deserialize(std::string const& val) {
    return json::parse(val);
}

template <> EvseStateEnum deserialize(std::string const& val) {
    return json::parse(val);
}

template <> SessionInfo deserialize(std::string const& val) {
    return json::parse(val);
}

template <> PauseChargingEVSEReasonEnum deserialize(std::string const& val) {
    return json::parse(val);
}

template <> ChargingPausedEVSEReasons deserialize(std::string const& val) {
    return json::parse(val);
}

template <> HlcSessionFailedReasonEnum deserialize(std::string const& val) {
    return json::parse(val);
}

template <> HlcSessionFailedEvent deserialize(std::string const& val) {
    return json::parse(val);
}

} // namespace everest::lib::API::V1_0::types::evse_manager
