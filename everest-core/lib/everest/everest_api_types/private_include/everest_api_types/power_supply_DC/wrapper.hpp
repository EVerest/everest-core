// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <everest_api_types/power_supply_DC/API.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#include "generated/types/power_supply_DC.hpp"
#pragma GCC diagnostic pop

namespace everest::lib::API::V1_0::types::power_supply_DC {

using Capabilities_Internal = ::types::power_supply_DC::Capabilities;
using Capabilities_External = Capabilities;

Capabilities_Internal to_internal_api(Capabilities_External const& val);
Capabilities_External to_external_api(Capabilities_Internal const& val);

using Mode_Internal = ::types::power_supply_DC::Mode;
using Mode_External = Mode;

Mode_Internal to_internal_api(Mode_External val);
Mode_External to_external_api(Mode_Internal val);

using ChargingPhase_Internal = ::types::power_supply_DC::ChargingPhase;
using ChargingPhase_External = ChargingPhase;

ChargingPhase_Internal to_internal_api(ChargingPhase_External val);
ChargingPhase_External to_external_api(ChargingPhase_Internal val);

using VoltageCurrent_Internal = ::types::power_supply_DC::VoltageCurrent;
using VoltageCurrent_External = VoltageCurrent;

VoltageCurrent_Internal to_internal_api(VoltageCurrent_External const& val);
VoltageCurrent_External to_external_api(VoltageCurrent_Internal const& val);

} // namespace everest::lib::API::V1_0::types::power_supply_DC
