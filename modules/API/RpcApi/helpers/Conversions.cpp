// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#include "Conversions.hpp"

namespace types {
namespace json_rpc_api {

EVSEStateEnum evse_manager_session_event_to_evse_state(types::evse_manager::SessionEvent state) {
    using Event = types::evse_manager::SessionEventEnum;

    switch (state.event) {
    case Event::Enabled:
        return EVSEStateEnum::Unplugged;
    case Event::Disabled:
        return EVSEStateEnum::Disabled;
    case Event::AuthRequired:
        return EVSEStateEnum::AuthRequired;
    case Event::PrepareCharging:
        [[fallthrough]];
    case Event::SessionStarted:
        [[fallthrough]];
    case Event::SessionResumed:
        [[fallthrough]];
    case Event::TransactionStarted:
        return EVSEStateEnum::Preparing;
    case Event::ChargingResumed:
        [[fallthrough]];
    case Event::ChargingStarted:
        return EVSEStateEnum::Charging;
    case Event::ChargingPausedEV:
        return EVSEStateEnum::ChargingPausedEV;
    case Event::ChargingPausedEVSE:
        return EVSEStateEnum::ChargingPausedEVSE;
    case Event::WaitingForEnergy:
        return EVSEStateEnum::WaitingForEnergy;
    case Event::ChargingFinished:
        return EVSEStateEnum::Finished;
    case Event::StoppingCharging:
        return EVSEStateEnum::FinishedEV;
    case Event::TransactionFinished: {
        if (state.transaction_finished.has_value() &&
            state.transaction_finished->reason == types::evse_manager::StopTransactionReason::Local) {
            return EVSEStateEnum::FinishedEVSE;
        } else {
            return EVSEStateEnum::Finished;
        }
        break;
    }
    case Event::PluginTimeout:
        return EVSEStateEnum::AuthTimeout;
    case Event::ReservationStart:
        return EVSEStateEnum::Reserved;
    case Event::ReservationEnd:
        [[fallthrough]];
    case Event::SessionFinished:
        return EVSEStateEnum::Unplugged;
    case Event::SwitchingPhases:
        return EVSEStateEnum::SwitchingPhases;
    case Event::ReplugStarted:
        [[fallthrough]];
    case Event::ReplugFinished:
        [[fallthrough]];
    case Event::Authorized:
        [[fallthrough]];
    case Event::Deauthorized:
        [[fallthrough]];
    default:
        return EVSEStateEnum::Unknown;
    }
}

ChargeProtocolEnum evse_manager_protocol_to_charge_protocol(const std::string& protocol) {
    if (protocol == "IEC61851-1") {
        return ChargeProtocolEnum::IEC61851;
    } else if (protocol == "DIN70121") {
        return ChargeProtocolEnum::DIN70121;
    } else if (protocol.compare(0, 11, "ISO15118-20") == 0) {
        return ChargeProtocolEnum::ISO15118_20;
    }
    // This check must be after the ISO15118-20 check
    else if (protocol.compare(0, 10, "ISO15118-2") == 0) {
        return ChargeProtocolEnum::ISO15118;
    } else {
        return ChargeProtocolEnum::Unknown;
    }
}

ErrorObj everest_error_to_rpc_error(const Everest::error::Error& error_object) {
    std::vector<ErrorObj> rpc_errors;

    ErrorObj rpc_error;
    rpc_error.type = error_object.type;
    rpc_error.description = error_object.description;
    rpc_error.message = error_object.message;

    switch (error_object.severity) {
    case Everest::error::Severity::High:
        rpc_error.severity = Severity::High;
        break;
    case Everest::error::Severity::Medium:
        rpc_error.severity = Severity::Medium;
        break;
    case Everest::error::Severity::Low:
        rpc_error.severity = Severity::Low;
        break;
    default:
        throw std::out_of_range("Provided severity " + std::to_string(static_cast<int>(error_object.severity)) +
                                " could not be converted to enum of type SeverityEnum");
    }
    rpc_error.origin.module_id = error_object.origin.module_id;
    rpc_error.origin.implementation_id = error_object.origin.implementation_id;

    if (error_object.origin.mapping.has_value()) {
        rpc_error.origin.evse_index = error_object.origin.mapping.value().evse;

        if (error_object.origin.mapping.value().connector.has_value()) {
            rpc_error.origin.connector_index = error_object.origin.mapping.value().connector.value();
        }
    }

    rpc_error.origin.evse_index = 0;
    rpc_error.origin.connector_index = 0;

    rpc_error.timestamp = Everest::Date::to_rfc3339(error_object.timestamp);
    rpc_error.uuid = error_object.uuid.to_string();

    return rpc_error;
}

std::vector<EnergyTransferModeEnum> iso15118_energy_transfer_modes_to_json_rpc_api(
    const std::vector<types::iso15118::EnergyTransferMode>& supported_energy_transfer_modes,
    bool& is_ac_transfer_mode) {
    std::vector<EnergyTransferModeEnum> tmp{};
    is_ac_transfer_mode = false;

    for (const auto& mode : supported_energy_transfer_modes) {
        switch (mode) {
        case types::iso15118::EnergyTransferMode::AC_single_phase_core:
            tmp.push_back(EnergyTransferModeEnum::AC_single_phase_core);
            is_ac_transfer_mode = true;
            break;
        case types::iso15118::EnergyTransferMode::AC_two_phase:
            tmp.push_back(EnergyTransferModeEnum::AC_two_phase);
            is_ac_transfer_mode = true;
            break;
        case types::iso15118::EnergyTransferMode::AC_three_phase_core:
            tmp.push_back(EnergyTransferModeEnum::AC_three_phase_core);
            is_ac_transfer_mode = true;
            break;
        case types::iso15118::EnergyTransferMode::DC_core:
            tmp.push_back(EnergyTransferModeEnum::DC_core);
            break;
        case types::iso15118::EnergyTransferMode::DC_extended:
            tmp.push_back(EnergyTransferModeEnum::DC_extended);
            break;
        case types::iso15118::EnergyTransferMode::DC_combo_core:
            tmp.push_back(EnergyTransferModeEnum::DC_combo_core);
            break;
        case types::iso15118::EnergyTransferMode::DC_unique:
            tmp.push_back(EnergyTransferModeEnum::DC_unique);
            break;
        case types::iso15118::EnergyTransferMode::DC:
            tmp.push_back(EnergyTransferModeEnum::DC);
            break;
        case types::iso15118::EnergyTransferMode::AC_BPT:
            tmp.push_back(EnergyTransferModeEnum::AC_BPT);
            is_ac_transfer_mode = true;
            break;
        case types::iso15118::EnergyTransferMode::AC_BPT_DER:
            tmp.push_back(EnergyTransferModeEnum::AC_BPT_DER);
            is_ac_transfer_mode = true;
            break;
        case types::iso15118::EnergyTransferMode::AC_DER:
            tmp.push_back(EnergyTransferModeEnum::AC_DER);
            is_ac_transfer_mode = true;
            break;
        case types::iso15118::EnergyTransferMode::DC_BPT:
            tmp.push_back(EnergyTransferModeEnum::DC_BPT);
            break;
        case types::iso15118::EnergyTransferMode::DC_ACDP:
            tmp.push_back(EnergyTransferModeEnum::DC_ACDP);
            break;
        case types::iso15118::EnergyTransferMode::DC_ACDP_BPT:
            tmp.push_back(EnergyTransferModeEnum::DC_ACDP_BPT);
            break;
        case types::iso15118::EnergyTransferMode::WPT:
            tmp.push_back(EnergyTransferModeEnum::WPT);
            is_ac_transfer_mode = true; // TBD
            break;
        default:
            throw std::invalid_argument("Unsupported energy transfer mode");
        }
    }

    return tmp;
}

void to_json(json& j, const EnergyTransferModeEnum& k) {
    // the required parts of the type
    j = energy_transfer_mode_enum_to_string(k);
}

} // namespace json_rpc_api
} // namespace types
