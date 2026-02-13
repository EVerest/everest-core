// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "evse_board_support/wrapper.hpp"
#include "evse_board_support/API.hpp"

namespace everest::lib::API::V1_0::types::evse_board_support {

Event_Internal to_internal_api(Event_External const& val) {
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

Event_External to_external_api(Event_Internal const& val) {
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

BspEvent_Internal to_internal_api(BspEvent_External const& val) {
    BspEvent_Internal result;
    result.event = to_internal_api(val.event);
    return result;
}

BspEvent_External to_external_api(BspEvent_Internal const& val) {
    BspEvent_External result;
    result.event = to_external_api(val.event);
    return result;
}

Connector_type_Internal to_internal_api(Connector_type_External const& val) {
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

Connector_type_External to_external_api(Connector_type_Internal const& val) {
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

HardwareCapabilities_Internal to_internal_api(HardwareCapabilities_External const& val) {
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
    result.supports_cp_state_E = val.supports_cp_state_E;
    result.connector_type = to_internal_api(val.connector_type);
    result.max_plug_temperature_C = val.max_plug_temperature_C;

    return result;
}

HardwareCapabilities_External to_external_api(HardwareCapabilities_Internal const& val) {
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
    result.supports_cp_state_E = val.supports_cp_state_E;
    result.connector_type = to_external_api(val.connector_type);
    result.max_plug_temperature_C = val.max_plug_temperature_C;

    return result;
}

Reason_Internal to_internal_api(Reason_External const& val) {
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

Reason_External to_external_api(Reason_Internal const& val) {
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

PowerOnOff_Internal to_internal_api(PowerOnOff_External const& val) {
    PowerOnOff_Internal result;

    result.allow_power_on = val.allow_power_on;
    result.reason = to_internal_api(val.reason);

    return result;
}

PowerOnOff_External to_external_api(PowerOnOff_Internal const& val) {
    PowerOnOff_External result;

    result.allow_power_on = val.allow_power_on;
    result.reason = to_external_api(val.reason);

    return result;
}

Ampacity_Internal to_internal_api(Ampacity_External const& val) {
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

Ampacity_External to_external_api(Ampacity_Internal const& val) {
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

ProximityPilot_Internal to_internal_api(ProximityPilot_External const& val) {
    ProximityPilot_Internal result;
    result.ampacity = to_internal_api(val.ampacity);
    return result;
}

ProximityPilot_External to_external_api(ProximityPilot_Internal const& val) {
    ProximityPilot_External result;
    result.ampacity = to_external_api(val.ampacity);
    return result;
}

} // namespace everest::lib::API::V1_0::types::evse_board_support
