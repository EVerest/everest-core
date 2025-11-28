// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include "API.hpp"
#include <optional>
#include <string>

namespace everest::lib::API::V1_0::types::evse_manager {

std::string serialize(StopTransactionReason val) noexcept;
std::string serialize(StopTransactionRequest const& val) noexcept;
std::string serialize(StartSessionReason val) noexcept;
std::string serialize(SessionEventEnum val) noexcept;
std::string serialize(SessionEvent const& val) noexcept;
std::string serialize(Limits const& val) noexcept;
std::string serialize(EVInfo const& val) noexcept;
std::string serialize(CarManufacturer val) noexcept;
std::string serialize(SessionStarted const& val) noexcept;
std::string serialize(SessionFinished const& val) noexcept;
std::string serialize(TransactionStarted const& val) noexcept;
std::string serialize(TransactionFinished const& val) noexcept;
std::string serialize(ChargingStateChangedEvent const& val) noexcept;
std::string serialize(AuthorizationEvent const& val) noexcept;
std::string serialize(ErrorSeverity val) noexcept;
std::string serialize(ErrorState val) noexcept;
std::string serialize(ErrorOrigin const& val) noexcept;
std::string serialize(Error const& val) noexcept;
std::string serialize(ConnectorTypeEnum const& val) noexcept;
std::string serialize(Connector const& val) noexcept;
std::string serialize(Evse const& val) noexcept;
std::string serialize(EnableSourceEnum const& val) noexcept;
std::string serialize(EnableStateEnum const& val) noexcept;
std::string serialize(EnableDisableSource const& val) noexcept;
std::string serialize(EnableDisableRequest const& val) noexcept;
std::string serialize(AuthorizeResponseArgs const& val) noexcept;
std::string serialize(PlugAndChargeConfiguration const& val) noexcept;
std::string serialize(EvseStateEnum const& val) noexcept;
std::string serialize(SessionInfo const& val) noexcept;

std::ostream& operator<<(std::ostream& os, StopTransactionReason const& val);
std::ostream& operator<<(std::ostream& os, StopTransactionRequest const& val);
std::ostream& operator<<(std::ostream& os, StartSessionReason const& val);
std::ostream& operator<<(std::ostream& os, SessionEventEnum const& val);
std::ostream& operator<<(std::ostream& os, SessionEvent const& val);
std::ostream& operator<<(std::ostream& os, Limits const& val);
std::ostream& operator<<(std::ostream& os, EVInfo const& val);
std::ostream& operator<<(std::ostream& os, CarManufacturer const& val);
std::ostream& operator<<(std::ostream& os, SessionStarted const& val);
std::ostream& operator<<(std::ostream& os, SessionFinished const& val);
std::ostream& operator<<(std::ostream& os, TransactionStarted const& val);
std::ostream& operator<<(std::ostream& os, TransactionFinished const& val);
std::ostream& operator<<(std::ostream& os, ChargingStateChangedEvent const& val);
std::ostream& operator<<(std::ostream& os, AuthorizationEvent const& val);
std::ostream& operator<<(std::ostream& os, ErrorSeverity const& val);
std::ostream& operator<<(std::ostream& os, ErrorState const& val);
std::ostream& operator<<(std::ostream& os, ErrorOrigin const& val);
std::ostream& operator<<(std::ostream& os, Error const& val);
std::ostream& operator<<(std::ostream& os, ConnectorTypeEnum const& val);
std::ostream& operator<<(std::ostream& os, Connector const& val);
std::ostream& operator<<(std::ostream& os, Evse const& val);
std::ostream& operator<<(std::ostream& os, EnableSourceEnum const& val);
std::ostream& operator<<(std::ostream& os, EnableStateEnum const& val);
std::ostream& operator<<(std::ostream& os, EnableDisableSource const& val);
std::ostream& operator<<(std::ostream& os, EnableDisableRequest const& val);
std::ostream& operator<<(std::ostream& os, AuthorizeResponseArgs const& val);
std::ostream& operator<<(std::ostream& os, PlugAndChargeConfiguration const& val);
std::ostream& operator<<(std::ostream& os, EvseStateEnum const& val);
std::ostream& operator<<(std::ostream& os, SessionInfo const& val);

template <class T> T deserialize(std::string const& val);
template <class T> std::optional<T> try_deserialize(std::string const& val) noexcept {
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

} // namespace everest::lib::API::V1_0::types::evse_manager
