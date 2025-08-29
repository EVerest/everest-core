// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <everest_api_types/energy/API.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#include "generated/types/energy.hpp"
#include "generated/types/energy_price_information.hpp"
#pragma GCC diagnostic pop

namespace everest::lib::API::V1_0::types::energy {

using NumberWithSource_Internal = ::types::energy::NumberWithSource;
using NumberWithSource_External = NumberWithSource;

NumberWithSource_Internal toInternalApi(NumberWithSource_External const& val);
NumberWithSource_External toExternalApi(NumberWithSource_Internal const& val);

using IntegerWithSource_Internal = ::types::energy::IntegerWithSource;
using IntegerWithSource_External = IntegerWithSource;

IntegerWithSource_Internal toInternalApi(IntegerWithSource_External const& val);
IntegerWithSource_External toExternalApi(IntegerWithSource_Internal const& val);

using FrequencyWattPoint_Internal = ::types::energy::FrequencyWattPoint;
using FrequencyWattPoint_External = FrequencyWattPoint;

FrequencyWattPoint_Internal toInternalApi(FrequencyWattPoint_External const& val);
FrequencyWattPoint_External toExternalApi(FrequencyWattPoint_Internal const& val);

using SetpointType_Internal = ::types::energy::SetpointType;
using SetpointType_External = SetpointType;

SetpointType_Internal toInternalApi(SetpointType_External const& val);
SetpointType_External toExternalApi(SetpointType_Internal const& val);

using PricePerkWh_Internal = ::types::energy_price_information::PricePerkWh;
using PricePerkWh_External = PricePerkWh;

PricePerkWh_Internal toInternalApi(PricePerkWh_External const& val);
PricePerkWh_External toExternalApi(PricePerkWh_Internal const& val);

using LimitsReq_Internal = ::types::energy::LimitsReq;
using LimitsReq_External = LimitsReq;

LimitsReq_Internal toInternalApi(LimitsReq_External const& val);
LimitsReq_External toExternalApi(LimitsReq_Internal const& val);

using LimitsRes_Internal = ::types::energy::LimitsRes;
using LimitsRes_External = LimitsRes;

LimitsRes_Internal toInternalApi(LimitsRes_External const& val);
LimitsRes_External toExternalApi(LimitsRes_Internal const& val);

using ScheduleReqEntry_Internal = ::types::energy::ScheduleReqEntry;
using ScheduleReqEntry_External = ScheduleReqEntry;

ScheduleReqEntry_Internal toInternalApi(ScheduleReqEntry_External const& val);
ScheduleReqEntry_External toExternalApi(ScheduleReqEntry_Internal const& val);

using ScheduleResEntry_Internal = ::types::energy::ScheduleResEntry;
using ScheduleResEntry_External = ScheduleResEntry;

ScheduleResEntry_Internal toInternalApi(ScheduleResEntry_External const& val);
ScheduleResEntry_External toExternalApi(ScheduleResEntry_Internal const& val);

using ScheduleSetpointEntry_Internal = ::types::energy::ScheduleSetpointEntry;
using ScheduleSetpointEntry_External = ScheduleSetpointEntry;

ScheduleSetpointEntry_Internal toInternalApi(ScheduleSetpointEntry_External const& val);
ScheduleSetpointEntry_External toExternalApi(ScheduleSetpointEntry_Internal const& val);

using ExternalLimits_Internal = ::types::energy::ExternalLimits;
using ExternalLimits_External = ExternalLimits;

ExternalLimits_Internal toInternalApi(ExternalLimits_External const& val);
ExternalLimits_External toExternalApi(ExternalLimits_Internal const& val);

using EnforcedLimits_Internal = ::types::energy::EnforcedLimits;
using EnforcedLimits_External = EnforcedLimits;

EnforcedLimits_Internal toInternalApi(EnforcedLimits_External const& val);
EnforcedLimits_External toExternalApi(EnforcedLimits_Internal const& val);

} // namespace everest::lib::API::V1_0::types::energy
