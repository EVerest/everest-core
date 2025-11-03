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

StopTransactionReason_Internal to_internal_api(StopTransactionReason_External const& val);
StopTransactionReason_External to_external_api(StopTransactionReason_Internal const& val);

using StopTransactionRequest_Internal = ::types::evse_manager::StopTransactionRequest;
using StopTransactionRequest_External = StopTransactionRequest;

StopTransactionRequest_Internal to_internal_api(StopTransactionRequest_External const& val);
StopTransactionRequest_External to_external_api(StopTransactionRequest_Internal const& val);

using StartSessionReason_Internal = ::types::evse_manager::StartSessionReason;
using StartSessionReason_External = StartSessionReason;

StartSessionReason_Internal to_internal_api(StartSessionReason_External const& val);
StartSessionReason_External to_external_api(StartSessionReason_Internal const& val);

using SessionEventEnum_Internal = ::types::evse_manager::SessionEventEnum;
using SessionEventEnum_External = SessionEventEnum;

SessionEventEnum_Internal to_internal_api(SessionEventEnum_External const& val);
SessionEventEnum_External to_external_api(SessionEventEnum_Internal const& val);

using SessionEvent_Internal = ::types::evse_manager::SessionEvent;
using SessionEvent_External = SessionEvent;

SessionEvent_Internal to_internal_api(SessionEvent_External const& val);
SessionEvent_External to_external_api(SessionEvent_Internal const& val);

using Limits_Internal = ::types::evse_manager::Limits;
using Limits_External = Limits;

Limits_Internal to_internal_api(Limits_External const& val);
Limits_External to_external_api(Limits_Internal const& val);

using EVInfo_Internal = ::types::evse_manager::EVInfo;
using EVInfo_External = EVInfo;

EVInfo_Internal to_internal_api(EVInfo_External const& val);
EVInfo_External to_external_api(EVInfo_Internal const& val);

using CarManufacturer_Internal = ::types::evse_manager::CarManufacturer;
using CarManufacturer_External = CarManufacturer;

CarManufacturer_Internal to_internal_api(CarManufacturer_External const& val);
CarManufacturer_External to_external_api(CarManufacturer_Internal const& val);

using SessionStarted_Internal = ::types::evse_manager::SessionStarted;
using SessionStarted_External = SessionStarted;

SessionStarted_Internal to_internal_api(SessionStarted_External const& val);
SessionStarted_External to_external_api(SessionStarted_Internal const& val);

using SessionFinished_Internal = ::types::evse_manager::SessionFinished;
using SessionFinished_External = SessionFinished;

SessionFinished_Internal to_internal_api(SessionFinished_External const& val);
SessionFinished_External to_external_api(SessionFinished_Internal const& val);

using TransactionStarted_Internal = ::types::evse_manager::TransactionStarted;
using TransactionStarted_External = TransactionStarted;

TransactionStarted_Internal to_internal_api(TransactionStarted_External const& val);
TransactionStarted_External to_external_api(TransactionStarted_Internal const& val);

using TransactionFinished_Internal = ::types::evse_manager::TransactionFinished;
using TransactionFinished_External = TransactionFinished;

TransactionFinished_Internal to_internal_api(TransactionFinished_External const& val);
TransactionFinished_External to_external_api(TransactionFinished_Internal const& val);

using ChargingStateChangedEvent_Internal = ::types::evse_manager::ChargingStateChangedEvent;
using ChargingStateChangedEvent_External = ChargingStateChangedEvent;

ChargingStateChangedEvent_Internal to_internal_api(ChargingStateChangedEvent_External const& val);
ChargingStateChangedEvent_External to_external_api(ChargingStateChangedEvent_Internal const& val);

using AuthorizationEvent_Internal = ::types::evse_manager::AuthorizationEvent;
using AuthorizationEvent_External = AuthorizationEvent;

AuthorizationEvent_Internal to_internal_api(AuthorizationEvent_External const& val);
AuthorizationEvent_External to_external_api(AuthorizationEvent_Internal const& val);

using ConnectorTypeEnum_Internal = ::types::evse_manager::ConnectorTypeEnum;
using ConnectorTypeEnum_External = ConnectorTypeEnum;

ConnectorTypeEnum_Internal to_internal_api(ConnectorTypeEnum_External const& val);
ConnectorTypeEnum_External to_external_api(ConnectorTypeEnum_Internal const& val);

using Connector_Internal = ::types::evse_manager::Connector;
using Connector_External = Connector;

Connector_Internal to_internal_api(Connector_External const& val);
Connector_External to_external_api(Connector_Internal const& val);

using Evse_Internal = ::types::evse_manager::Evse;
using Evse_External = Evse;

Evse_Internal to_internal_api(Evse_External const& val);
Evse_External to_external_api(Evse_Internal const& val);

using EnableSourceEnum_Internal = ::types::evse_manager::Enable_source;
using EnableSourceEnum_External = EnableSourceEnum;

EnableSourceEnum_Internal to_internal_api(EnableSourceEnum_External const& val);
EnableSourceEnum_External to_external_api(EnableSourceEnum_Internal const& val);

using EnableStateEnum_Internal = ::types::evse_manager::Enable_state;
using EnableStateEnum_External = EnableStateEnum;

EnableStateEnum_Internal to_internal_api(EnableStateEnum_External const& val);
EnableStateEnum_External to_external_api(EnableStateEnum_Internal const& val);

using EnableDisableSource_Internal = ::types::evse_manager::EnableDisableSource;
using EnableDisableSource_External = EnableDisableSource;

EnableDisableSource_Internal to_internal_api(EnableDisableSource_External const& val);
EnableDisableSource_External to_external_api(EnableDisableSource_Internal const& val);

using PlugAndChargeConfiguration_Internal = ::types::evse_manager::PlugAndChargeConfiguration;
using PlugAndChargeConfiguration_External = PlugAndChargeConfiguration;

PlugAndChargeConfiguration_Internal to_internal_api(PlugAndChargeConfiguration_External const& val);
PlugAndChargeConfiguration_External to_external_api(PlugAndChargeConfiguration_Internal const& val);

} // namespace everest::lib::API::V1_0::types::evse_manager
