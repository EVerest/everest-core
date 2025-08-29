// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

// #include "utils/types.hpp"
#include <everest_api_types/evse_manager/API.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#include "generated/types/evse_manager.hpp"
#pragma GCC diagnostic pop

namespace everest::lib::API::V1_0::types::evse_manager {

using StopTransactionReason_Internal = ::types::evse_manager::StopTransactionReason;
using StopTransactionReason_External = StopTransactionReason;

StopTransactionReason_Internal toInternalApi(StopTransactionReason_External const& val);
StopTransactionReason_External toExternalApi(StopTransactionReason_Internal const& val);

using StopTransactionRequest_Internal = ::types::evse_manager::StopTransactionRequest;
using StopTransactionRequest_External = StopTransactionRequest;

StopTransactionRequest_Internal toInternalApi(StopTransactionRequest_External const& val);
StopTransactionRequest_External toExternalApi(StopTransactionRequest_Internal const& val);

using StartSessionReason_Internal = ::types::evse_manager::StartSessionReason;
using StartSessionReason_External = StartSessionReason;

StartSessionReason_Internal toInternalApi(StartSessionReason_External const& val);
StartSessionReason_External toExternalApi(StartSessionReason_Internal const& val);

using SessionEventEnum_Internal = ::types::evse_manager::SessionEventEnum;
using SessionEventEnum_External = SessionEventEnum;

SessionEventEnum_Internal toInternalApi(SessionEventEnum_External const& val);
SessionEventEnum_External toExternalApi(SessionEventEnum_Internal const& val);

using SessionEvent_Internal = ::types::evse_manager::SessionEvent;
using SessionEvent_External = SessionEvent;

SessionEvent_Internal toInternalApi(SessionEvent_External const& val);
SessionEvent_External toExternalApi(SessionEvent_Internal const& val);

using Limits_Internal = ::types::evse_manager::Limits;
using Limits_External = Limits;

Limits_Internal toInternalApi(Limits_External const& val);
Limits_External toExternalApi(Limits_Internal const& val);

using EVInfo_Internal = ::types::evse_manager::EVInfo;
using EVInfo_External = EVInfo;

EVInfo_Internal toInternalApi(EVInfo_External const& val);
EVInfo_External toExternalApi(EVInfo_Internal const& val);

using CarManufacturer_Internal = ::types::evse_manager::CarManufacturer;
using CarManufacturer_External = CarManufacturer;

CarManufacturer_Internal toInternalApi(CarManufacturer_External const& val);
CarManufacturer_External toExternalApi(CarManufacturer_Internal const& val);

using SessionStarted_Internal = ::types::evse_manager::SessionStarted;
using SessionStarted_External = SessionStarted;

SessionStarted_Internal toInternalApi(SessionStarted_External const& val);
SessionStarted_External toExternalApi(SessionStarted_Internal const& val);

using SessionFinished_Internal = ::types::evse_manager::SessionFinished;
using SessionFinished_External = SessionFinished;

SessionFinished_Internal toInternalApi(SessionFinished_External const& val);
SessionFinished_External toExternalApi(SessionFinished_Internal const& val);

using TransactionStarted_Internal = ::types::evse_manager::TransactionStarted;
using TransactionStarted_External = TransactionStarted;

TransactionStarted_Internal toInternalApi(TransactionStarted_External const& val);
TransactionStarted_External toExternalApi(TransactionStarted_Internal const& val);

using TransactionFinished_Internal = ::types::evse_manager::TransactionFinished;
using TransactionFinished_External = TransactionFinished;

TransactionFinished_Internal toInternalApi(TransactionFinished_External const& val);
TransactionFinished_External toExternalApi(TransactionFinished_Internal const& val);

using ChargingStateChangedEvent_Internal = ::types::evse_manager::ChargingStateChangedEvent;
using ChargingStateChangedEvent_External = ChargingStateChangedEvent;

ChargingStateChangedEvent_Internal toInternalApi(ChargingStateChangedEvent_External const& val);
ChargingStateChangedEvent_External toExternalApi(ChargingStateChangedEvent_Internal const& val);

using AuthorizationEvent_Internal = ::types::evse_manager::AuthorizationEvent;
using AuthorizationEvent_External = AuthorizationEvent;

AuthorizationEvent_Internal toInternalApi(AuthorizationEvent_External const& val);
AuthorizationEvent_External toExternalApi(AuthorizationEvent_Internal const& val);

using ConnectorTypeEnum_Internal = ::types::evse_manager::ConnectorTypeEnum;
using ConnectorTypeEnum_External = ConnectorTypeEnum;

ConnectorTypeEnum_Internal toInternalApi(ConnectorTypeEnum_External const& val);
ConnectorTypeEnum_External toExternalApi(ConnectorTypeEnum_Internal const& val);

using Connector_Internal = ::types::evse_manager::Connector;
using Connector_External = Connector;

Connector_Internal toInternalApi(Connector_External const& val);
Connector_External toExternalApi(Connector_Internal const& val);

using Evse_Internal = ::types::evse_manager::Evse;
using Evse_External = Evse;

Evse_Internal toInternalApi(Evse_External const& val);
Evse_External toExternalApi(Evse_Internal const& val);

using EnableSourceEnum_Internal = ::types::evse_manager::Enable_source;
using EnableSourceEnum_External = EnableSourceEnum;

EnableSourceEnum_Internal toInternalApi(EnableSourceEnum_External const& val);
EnableSourceEnum_External toExternalApi(EnableSourceEnum_Internal const& val);

using EnableStateEnum_Internal = ::types::evse_manager::Enable_state;
using EnableStateEnum_External = EnableStateEnum;

EnableStateEnum_Internal toInternalApi(EnableStateEnum_External const& val);
EnableStateEnum_External toExternalApi(EnableStateEnum_Internal const& val);

using EnableDisableSource_Internal = ::types::evse_manager::EnableDisableSource;
using EnableDisableSource_External = EnableDisableSource;

EnableDisableSource_Internal toInternalApi(EnableDisableSource_External const& val);
EnableDisableSource_External toExternalApi(EnableDisableSource_Internal const& val);

using PlugAndChargeConfiguration_Internal = ::types::evse_manager::PlugAndChargeConfiguration;
using PlugAndChargeConfiguration_External = PlugAndChargeConfiguration;

PlugAndChargeConfiguration_Internal toInternalApi(PlugAndChargeConfiguration_External const& val);
PlugAndChargeConfiguration_External toExternalApi(PlugAndChargeConfiguration_Internal const& val);

} // namespace everest::lib::API::V1_0::types::evse_manager
