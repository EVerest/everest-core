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

Event_Internal toInternalApi(Event_External const& val);
Event_External toExternalApi(Event_Internal const& val);

using BspEvent_Internal = ::types::board_support_common::BspEvent;
using BspEvent_External = BspEvent;

BspEvent_Internal toInternalApi(BspEvent_External const& val);
BspEvent_External toExternalApi(BspEvent_Internal const& val);

using Connector_type_Internal = ::types::evse_board_support::Connector_type;
using Connector_type_External = Connector_type;

Connector_type_Internal toInternalApi(Connector_type_External const& val);
Connector_type_External toExternalApi(Connector_type_Internal const& val);

using HardwareCapabilities_Internal = ::types::evse_board_support::HardwareCapabilities;
using HardwareCapabilities_External = HardwareCapabilities;

HardwareCapabilities_Internal toInternalApi(HardwareCapabilities_External const& val);
HardwareCapabilities_External toExternalApi(HardwareCapabilities_Internal const& val);

using Reason_Internal = ::types::evse_board_support::Reason;
;
using Reason_External = Reason;

Reason_Internal toInternalApi(Reason_External const& val);
Reason_External toExternalApi(Reason_Internal const& val);

using PowerOnOff_Internal = ::types::evse_board_support::PowerOnOff;
;
using PowerOnOff_External = PowerOnOff;

PowerOnOff_Internal toInternalApi(PowerOnOff_External const& val);
PowerOnOff_External toExternalApi(PowerOnOff_Internal const& val);

using Ampacity_Internal = ::types::board_support_common::Ampacity;
using Ampacity_External = Ampacity;

Ampacity_Internal toInternalApi(Ampacity_External const& val);
Ampacity_External toExternalApi(Ampacity_Internal const& val);

using ProximityPilot_Internal = ::types::board_support_common::ProximityPilot;
using ProximityPilot_External = ProximityPilot;

ProximityPilot_Internal toInternalApi(ProximityPilot_External const& val);
ProximityPilot_External toExternalApi(ProximityPilot_Internal const& val);

} // namespace everest::lib::API::V1_0::types::evse_board_support
