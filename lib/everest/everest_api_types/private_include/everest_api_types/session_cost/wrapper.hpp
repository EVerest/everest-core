// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <everest_api_types/session_cost/API.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#include "generated/types/session_cost.hpp"
#pragma GCC diagnostic pop

namespace everest::lib::API::V1_0::types::session_cost {

using TariffMessage_Internal = ::types::session_cost::TariffMessage;
using TariffMessage_External = TariffMessage;

TariffMessage_Internal to_internal_api(TariffMessage_External const& val);
TariffMessage_External to_external_api(TariffMessage_Internal const& val);

using IdlePrice_Internal = ::types::session_cost::IdlePrice;
using IdlePrice_External = IdlePrice;

IdlePrice_Internal to_internal_api(IdlePrice_External const& val);
IdlePrice_External to_external_api(IdlePrice_Internal const& val);

using CostCategory_Internal = ::types::session_cost::CostCategory;
using CostCategory_External = CostCategory;

CostCategory_Internal to_internal_api(CostCategory_External const& val);
CostCategory_External to_external_api(CostCategory_Internal const& val);

using ChargingPriceComponent_Internal = ::types::session_cost::ChargingPriceComponent;
using ChargingPriceComponent_External = ChargingPriceComponent;

ChargingPriceComponent_Internal to_internal_api(ChargingPriceComponent_External const& val);
ChargingPriceComponent_External to_external_api(ChargingPriceComponent_Internal const& val);

using NextPeriodPrice_Internal = ::types::session_cost::NextPeriodPrice;
using NextPeriodPrice_External = NextPeriodPrice;

NextPeriodPrice_Internal to_internal_api(NextPeriodPrice_External const& val);
NextPeriodPrice_External to_external_api(NextPeriodPrice_Internal const& val);

using SessionCostChunk_Internal = ::types::session_cost::SessionCostChunk;
using SessionCostChunk_External = SessionCostChunk;

SessionCostChunk_Internal to_internal_api(SessionCostChunk_External const& val);
SessionCostChunk_External to_external_api(SessionCostChunk_Internal const& val);

using SessionStatus_Internal = ::types::session_cost::SessionStatus;
using SessionStatus_External = SessionStatus;

SessionStatus_Internal to_internal_api(SessionStatus_External const& val);
SessionStatus_External to_external_api(SessionStatus_Internal const& val);

using SessionCost_Internal = ::types::session_cost::SessionCost;
using SessionCost_External = SessionCost;

SessionCost_Internal to_internal_api(SessionCost_External const& val);
SessionCost_External to_external_api(SessionCost_Internal const& val);

} // namespace everest::lib::API::V1_0::types::session_cost
