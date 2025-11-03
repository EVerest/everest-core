// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <everest_api_types/evse_board_support/API.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#include "generated/types/board_support_common.hpp"
#include "generated/types/evse_board_support.hpp"
#pragma GCC diagnostic pop

namespace everest::lib::API::V1_0::types::evse_board_support {

using Event_Internal = ::types::board_support_common::Event;
using Event_External = Event;

Event_Internal to_internal_api(Event_External const& val);
Event_External to_external_api(Event_Internal const& val);

using BspEvent_Internal = ::types::board_support_common::BspEvent;
using BspEvent_External = BspEvent;

BspEvent_Internal to_internal_api(BspEvent_External const& val);
BspEvent_External to_external_api(BspEvent_Internal const& val);

using Connector_type_Internal = ::types::evse_board_support::Connector_type;
using Connector_type_External = Connector_type;

Connector_type_Internal to_internal_api(Connector_type_External const& val);
Connector_type_External to_external_api(Connector_type_Internal const& val);

using HardwareCapabilities_Internal = ::types::evse_board_support::HardwareCapabilities;
using HardwareCapabilities_External = HardwareCapabilities;

HardwareCapabilities_Internal to_internal_api(HardwareCapabilities_External const& val);
HardwareCapabilities_External to_external_api(HardwareCapabilities_Internal const& val);

using Reason_Internal = ::types::evse_board_support::Reason;
using Reason_External = Reason;

Reason_Internal to_internal_api(Reason_External const& val);
Reason_External to_external_api(Reason_Internal const& val);

using PowerOnOff_Internal = ::types::evse_board_support::PowerOnOff;
using PowerOnOff_External = PowerOnOff;

PowerOnOff_Internal to_internal_api(PowerOnOff_External const& val);
PowerOnOff_External to_external_api(PowerOnOff_Internal const& val);

using Ampacity_Internal = ::types::board_support_common::Ampacity;
using Ampacity_External = Ampacity;

Ampacity_Internal to_internal_api(Ampacity_External const& val);
Ampacity_External to_external_api(Ampacity_Internal const& val);

using ProximityPilot_Internal = ::types::board_support_common::ProximityPilot;
using ProximityPilot_External = ProximityPilot;

ProximityPilot_Internal to_internal_api(ProximityPilot_External const& val);
ProximityPilot_External to_external_api(ProximityPilot_Internal const& val);

} // namespace everest::lib::API::V1_0::types::evse_board_support
