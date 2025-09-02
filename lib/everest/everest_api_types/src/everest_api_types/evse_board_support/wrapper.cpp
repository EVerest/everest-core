// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "evse_board_support/wrapper.hpp"
#include "evse_board_support/API.hpp"

namespace everest::lib::API::V1_0::types::evse_board_support {

Event_Internal toInternalApi(Event_External const& val) {
    using SrcT = Event_External;
    using TarT = Event_Internal;

    switch (val) {
    case SrcT::A:
        return TarT::A;
    case SrcT::B:
        return TarT::B;
    case SrcT::C:
        return TarT::C;
    case SrcT::D:
        return TarT::D;
    case SrcT::E:
        return TarT::E;
    case SrcT::F:
        return TarT::F;
    case SrcT::PowerOn:
        return TarT::PowerOn;
    case SrcT::PowerOff:
        return TarT::PowerOff;
    case SrcT::EvseReplugStarted:
        return TarT::EvseReplugStarted;
    case SrcT::EvseReplugFinished:
        return TarT::EvseReplugFinished;
    case SrcT::Disconnected:
        return TarT::Disconnected;
    }

    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::evse_board_support::Event_External");
}

Event_External toExternalApi(Event_Internal const& val) {
    using SrcT = Event_Internal;
    using TarT = Event_External;

    switch (val) {
    case SrcT::A:
        return TarT::A;
    case SrcT::B:
        return TarT::B;
    case SrcT::C:
        return TarT::C;
    case SrcT::D:
        return TarT::D;
    case SrcT::E:
        return TarT::E;
    case SrcT::F:
        return TarT::F;
    case SrcT::PowerOn:
        return TarT::PowerOn;
    case SrcT::PowerOff:
        return TarT::PowerOff;
    case SrcT::EvseReplugStarted:
        return TarT::EvseReplugStarted;
    case SrcT::EvseReplugFinished:
        return TarT::EvseReplugFinished;
    case SrcT::Disconnected:
        return TarT::Disconnected;
    }

    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::evse_board_support::Event_Internal");
}

BspEvent_Internal toInternalApi(BspEvent_External const& val) {
    BspEvent_Internal result;
    result.event = toInternalApi(val.event);
    return result;
}

BspEvent_External toExternalApi(BspEvent_Internal const& val) {
    BspEvent_External result;
    result.event = toExternalApi(val.event);
    return result;
}

Connector_type_Internal toInternalApi(Connector_type_External const& val) {
    using SrcT = Connector_type_External;
    using TarT = Connector_type_Internal;

    switch (val) {
    case SrcT::IEC62196Type2Cable:
        return TarT::IEC62196Type2Cable;
    case SrcT::IEC62196Type2Socket:
        return TarT::IEC62196Type2Socket;
    }

    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::evse_board_support::Connector_type_External");
}

Connector_type_External toExternalApi(Connector_type_Internal const& val) {
    using SrcT = Connector_type_Internal;
    using TarT = Connector_type_External;

    switch (val) {
    case SrcT::IEC62196Type2Cable:
        return TarT::IEC62196Type2Cable;
    case SrcT::IEC62196Type2Socket:
        return TarT::IEC62196Type2Socket;
    }

    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::evse_board_support::Connector_type_Internal");
}

HardwareCapabilities_Internal toInternalApi(HardwareCapabilities_External const& val) {
    HardwareCapabilities_Internal result;
    result.max_current_A_import = val.max_current_A_import;
    result.min_current_A_import = val.min_current_A_import;
    result.max_phase_count_import = val.max_phase_count_import;
    result.min_phase_count_import = val.min_phase_count_import;
    result.max_current_A_export = val.max_current_A_export;
    result.min_current_A_export = val.min_current_A_export;
    result.max_phase_count_export = val.max_phase_count_export;
    result.min_phase_count_export = val.min_phase_count_export;
    result.supports_changing_phases_during_charging = val.supports_changing_phases_during_charging;
    result.connector_type = toInternalApi(val.connector_type);
    result.max_plug_temperature_C = val.max_plug_temperature_C;

    return result;
}

HardwareCapabilities_External toExternalApi(HardwareCapabilities_Internal const& val) {
    HardwareCapabilities_External result;
    result.max_current_A_import = val.max_current_A_import;
    result.min_current_A_import = val.min_current_A_import;
    result.max_phase_count_import = val.max_phase_count_import;
    result.min_phase_count_import = val.min_phase_count_import;
    result.max_current_A_export = val.max_current_A_export;
    result.min_current_A_export = val.min_current_A_export;
    result.max_phase_count_export = val.max_phase_count_export;
    result.min_phase_count_export = val.min_phase_count_export;
    result.supports_changing_phases_during_charging = val.supports_changing_phases_during_charging;
    result.connector_type = toExternalApi(val.connector_type);
    result.max_plug_temperature_C = val.max_plug_temperature_C;

    return result;
}

Reason_Internal toInternalApi(Reason_External const& val) {
    using SrcT = Reason_External;
    using TarT = Reason_Internal;

    switch (val) {
    case SrcT::DCCableCheck:
        return TarT::DCCableCheck;
    case SrcT::DCPreCharge:
        return TarT::DCPreCharge;
    case SrcT::FullPowerCharging:
        return TarT::FullPowerCharging;
    case SrcT::PowerOff:
        return TarT::PowerOff;
    }

    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::evse_board_support::Reason_External");
}

Reason_External toExternalApi(Reason_Internal const& val) {
    using SrcT = Reason_Internal;
    using TarT = Reason_External;

    switch (val) {
    case SrcT::DCCableCheck:
        return TarT::DCCableCheck;
    case SrcT::DCPreCharge:
        return TarT::DCPreCharge;
    case SrcT::FullPowerCharging:
        return TarT::FullPowerCharging;
    case SrcT::PowerOff:
        return TarT::PowerOff;
    }

    throw std::out_of_range("Unexpected value for everest::lib::API::V1_0::types::evse_board_support::Reason_Internal");
}

PowerOnOff_Internal toInternalApi(PowerOnOff_External const& val) {
    PowerOnOff_Internal result;

    result.allow_power_on = val.allow_power_on;
    result.reason = toInternalApi(val.reason);

    return result;
}

PowerOnOff_External toExternalApi(PowerOnOff_Internal const& val) {
    PowerOnOff_External result;

    result.allow_power_on = val.allow_power_on;
    result.reason = toExternalApi(val.reason);

    return result;
}

Ampacity_Internal toInternalApi(Ampacity_External const& val) {
    using SrcT = Ampacity_External;
    using TarT = Ampacity_Internal;

    switch (val) {
    case SrcT::None:
        return TarT::None;
    case SrcT::A_13:
        return TarT::A_13;
    case SrcT::A_20:
        return TarT::A_20;
    case SrcT::A_32:
        return TarT::A_32;
    case SrcT::A_63_3ph_70_1ph:
        return TarT::A_63_3ph_70_1ph;
    }

    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::evse_board_support::Ampacity_External");
}

Ampacity_External toExternalApi(Ampacity_Internal const& val) {
    using SrcT = Ampacity_Internal;
    using TarT = Ampacity_External;

    switch (val) {
    case SrcT::None:
        return TarT::None;
    case SrcT::A_13:
        return TarT::A_13;
    case SrcT::A_20:
        return TarT::A_20;
    case SrcT::A_32:
        return TarT::A_32;
    case SrcT::A_63_3ph_70_1ph:
        return TarT::A_63_3ph_70_1ph;
    }

    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::evse_board_support::Ampacity_Internal");
}

ProximityPilot_Internal toInternalApi(ProximityPilot_External const& val) {
    ProximityPilot_Internal result;
    result.ampacity = toInternalApi(val.ampacity);
    return result;
}

ProximityPilot_External toExternalApi(ProximityPilot_Internal const& val) {
    ProximityPilot_External result;
    result.ampacity = toExternalApi(val.ampacity);
    return result;
}

} // namespace everest::lib::API::V1_0::types::evse_board_support
