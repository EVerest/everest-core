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

NumberWithSource_Internal to_internal_api(NumberWithSource_External const& val);
NumberWithSource_External to_external_api(NumberWithSource_Internal const& val);

using IntegerWithSource_Internal = ::types::energy::IntegerWithSource;
using IntegerWithSource_External = IntegerWithSource;

IntegerWithSource_Internal to_internal_api(IntegerWithSource_External const& val);
IntegerWithSource_External to_external_api(IntegerWithSource_Internal const& val);

using FrequencyWattPoint_Internal = ::types::energy::FrequencyWattPoint;
using FrequencyWattPoint_External = FrequencyWattPoint;

FrequencyWattPoint_Internal to_internal_api(FrequencyWattPoint_External const& val);
FrequencyWattPoint_External to_external_api(FrequencyWattPoint_Internal const& val);

using SetpointType_Internal = ::types::energy::SetpointType;
using SetpointType_External = SetpointType;

SetpointType_Internal to_internal_api(SetpointType_External const& val);
SetpointType_External to_external_api(SetpointType_Internal const& val);

using PricePerkWh_Internal = ::types::energy_price_information::PricePerkWh;
using PricePerkWh_External = PricePerkWh;

PricePerkWh_Internal to_internal_api(PricePerkWh_External const& val);
PricePerkWh_External to_external_api(PricePerkWh_Internal const& val);

using LimitsReq_Internal = ::types::energy::LimitsReq;
using LimitsReq_External = LimitsReq;

LimitsReq_Internal to_internal_api(LimitsReq_External const& val);
LimitsReq_External to_external_api(LimitsReq_Internal const& val);

using LimitsRes_Internal = ::types::energy::LimitsRes;
using LimitsRes_External = LimitsRes;

LimitsRes_Internal to_internal_api(LimitsRes_External const& val);
LimitsRes_External to_external_api(LimitsRes_Internal const& val);

using ScheduleReqEntry_Internal = ::types::energy::ScheduleReqEntry;
using ScheduleReqEntry_External = ScheduleReqEntry;

ScheduleReqEntry_Internal to_internal_api(ScheduleReqEntry_External const& val);
ScheduleReqEntry_External to_external_api(ScheduleReqEntry_Internal const& val);

using ScheduleResEntry_Internal = ::types::energy::ScheduleResEntry;
using ScheduleResEntry_External = ScheduleResEntry;

ScheduleResEntry_Internal to_internal_api(ScheduleResEntry_External const& val);
ScheduleResEntry_External to_external_api(ScheduleResEntry_Internal const& val);

using ScheduleSetpointEntry_Internal = ::types::energy::ScheduleSetpointEntry;
using ScheduleSetpointEntry_External = ScheduleSetpointEntry;

ScheduleSetpointEntry_Internal to_internal_api(ScheduleSetpointEntry_External const& val);
ScheduleSetpointEntry_External to_external_api(ScheduleSetpointEntry_Internal const& val);

using ExternalLimits_Internal = ::types::energy::ExternalLimits;
using ExternalLimits_External = ExternalLimits;

ExternalLimits_Internal to_internal_api(ExternalLimits_External const& val);
ExternalLimits_External to_external_api(ExternalLimits_Internal const& val);

using EnforcedLimits_Internal = ::types::energy::EnforcedLimits;
using EnforcedLimits_External = EnforcedLimits;

EnforcedLimits_Internal to_internal_api(EnforcedLimits_External const& val);
EnforcedLimits_External to_external_api(EnforcedLimits_Internal const& val);

} // namespace everest::lib::API::V1_0::types::energy
