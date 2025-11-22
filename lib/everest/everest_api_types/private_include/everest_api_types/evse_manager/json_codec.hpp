// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "nlohmann/json_fwd.hpp"
#include <everest_api_types/evse_manager/API.hpp>

namespace everest::lib::API::V1_0::types::evse_manager {

using json = nlohmann::json;

void to_json(json& j, StopTransactionReason const& k) noexcept;
void from_json(json const& j, StopTransactionReason& k);

void to_json(json& j, StopTransactionRequest const& k) noexcept;
void from_json(json const& j, StopTransactionRequest& k);

void to_json(json& j, StartSessionReason const& k) noexcept;
void from_json(json const& j, StartSessionReason& k);

void to_json(json& j, SessionEventEnum const& k) noexcept;
void from_json(json const& j, SessionEventEnum& k);

void to_json(json& j, SessionEvent const& k) noexcept;
void from_json(json const& j, SessionEvent& k);

void to_json(json& j, Limits const& k) noexcept;
void from_json(json const& j, Limits& k);

void to_json(json& j, EVInfo const& k) noexcept;
void from_json(json const& j, EVInfo& k);

void to_json(json& j, CarManufacturer const& k) noexcept;
void from_json(json const& j, CarManufacturer& k);

void to_json(json& j, SessionStarted const& k) noexcept;
void from_json(json const& j, SessionStarted& k);

void to_json(json& j, SessionFinished const& k) noexcept;
void from_json(json const& j, SessionFinished& k);

void to_json(json& j, TransactionStarted const& k) noexcept;
void from_json(json const& j, TransactionStarted& k);

void to_json(json& j, TransactionFinished const& k) noexcept;
void from_json(json const& j, TransactionFinished& k);

void to_json(json& j, ChargingStateChangedEvent const& k) noexcept;
void from_json(json const& j, ChargingStateChangedEvent& k);

void to_json(json& j, AuthorizationEvent const& k) noexcept;
void from_json(json const& j, AuthorizationEvent& k);

void to_json(json& j, ErrorSeverity const& k) noexcept;
void from_json(json const& j, ErrorSeverity& k);

void to_json(json& j, ErrorState const& k) noexcept;
void from_json(json const& j, ErrorState& k);

void to_json(json& j, ErrorOrigin const& k) noexcept;
void from_json(json const& j, ErrorOrigin& k);

void to_json(json& j, Error const& k) noexcept;
void from_json(json const& j, Error& k);

void to_json(json& j, ConnectorTypeEnum const& k) noexcept;
void from_json(json const& j, ConnectorTypeEnum& k);

void to_json(json& j, Connector const& k) noexcept;
void from_json(json const& j, Connector& k);

void to_json(json& j, Evse const& k) noexcept;
void from_json(json const& j, Evse& k);

void to_json(json& j, EnableSourceEnum const& k) noexcept;
void from_json(json const& j, EnableSourceEnum& k);

void to_json(json& j, EnableStateEnum const& k) noexcept;
void from_json(json const& j, EnableStateEnum& k);

void to_json(json& j, EnableDisableSource const& k) noexcept;
void from_json(json const& j, EnableDisableSource& k);

void to_json(json& j, EnableDisableRequest const& k) noexcept;
void from_json(json const& j, EnableDisableRequest& k);

void to_json(json& j, AuthorizeResponseArgs const& k) noexcept;
void from_json(json const& j, AuthorizeResponseArgs& k);

void to_json(json& j, PlugAndChargeConfiguration const& k) noexcept;
void from_json(json const& j, PlugAndChargeConfiguration& k);

void to_json(json& j, EvseStateEnum const& k) noexcept;
void from_json(json const& j, EvseStateEnum& k);

void to_json(json& j, SessionInfo const& k) noexcept;
void from_json(json const& j, SessionInfo& k);

} // namespace everest::lib::API::V1_0::types::evse_manager
