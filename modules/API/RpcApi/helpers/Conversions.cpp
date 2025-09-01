// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#include "Conversions.hpp"

namespace types {
namespace json_rpc_api {

EVSEStateEnum evse_manager_session_event_to_evse_state(types::evse_manager::SessionEvent state) {
    using Event = types::evse_manager::SessionEventEnum;
    using State = types::json_rpc_api::EVSEStateEnum;

    switch (state.event) {
    case Event::Enabled:
        return State::Unplugged;
    case Event::Disabled:
        return State::Disabled;
    case Event::AuthRequired:
        return State::AuthRequired;
    case Event::PrepareCharging:
        [[fallthrough]];
    case Event::SessionStarted:
        [[fallthrough]];
    case Event::SessionResumed:
        [[fallthrough]];
    case Event::TransactionStarted:
        return State::Preparing;
    case Event::ChargingResumed:
        [[fallthrough]];
    case Event::ChargingStarted:
        return State::Charging;
    case Event::ChargingPausedEV:
        return State::ChargingPausedEV;
    case Event::ChargingPausedEVSE:
        return State::ChargingPausedEVSE;
    case Event::WaitingForEnergy:
        return State::WaitingForEnergy;
    case Event::ChargingFinished:
        return State::Finished;
    case Event::StoppingCharging:
        return State::FinishedEV;
    case Event::TransactionFinished: {
        if (state.transaction_finished.has_value() &&
            state.transaction_finished->reason == types::evse_manager::StopTransactionReason::Local) {
            return State::FinishedEVSE;
        } else {
            return State::Finished;
        }
        break;
    }
    case Event::PluginTimeout:
        return State::AuthTimeout;
    case Event::ReservationStart:
        return State::Reserved;
    case Event::ReservationEnd:
        [[fallthrough]];
    case Event::SessionFinished:
        return State::Unplugged;
    case Event::SwitchingPhases:
        return State::SwitchingPhases;
    case Event::ReplugStarted:
        [[fallthrough]];
    case Event::ReplugFinished:
        [[fallthrough]];
    case Event::Authorized:
        [[fallthrough]];
    case Event::Deauthorized:
        [[fallthrough]];
    default:
        return State::Unknown;
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

types::json_rpc_api::ErrorObj everest_error_to_rpc_error(const Everest::error::Error& error_object) {
    std::vector<types::json_rpc_api::ErrorObj> rpc_errors;

    types::json_rpc_api::ErrorObj rpc_error;
    rpc_error.type = error_object.type;
    rpc_error.description = error_object.description;
    rpc_error.message = error_object.message;

    switch (error_object.severity) {
    case Everest::error::Severity::High:
        rpc_error.severity = types::json_rpc_api::Severity::High;
        break;
    case Everest::error::Severity::Medium:
        rpc_error.severity = types::json_rpc_api::Severity::Medium;
        break;
    case Everest::error::Severity::Low:
        rpc_error.severity = types::json_rpc_api::Severity::Low;
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

std::vector<types::json_rpc_api::EnergyTransferModeEnum> iso15118_energy_transfer_modes_to_json_rpc_api(
    const std::vector<types::iso15118::EnergyTransferMode>& supported_energy_transfer_modes,
    bool& is_ac_transfer_mode) {
    std::vector<types::json_rpc_api::EnergyTransferModeEnum> tmp{};
    is_ac_transfer_mode = false;

    for (const auto& mode : supported_energy_transfer_modes) {
        switch (mode) {
        case types::iso15118::EnergyTransferMode::AC_single_phase_core:
            tmp.push_back(types::json_rpc_api::EnergyTransferModeEnum::AC_single_phase_core);
            is_ac_transfer_mode = true;
            break;
        case types::iso15118::EnergyTransferMode::AC_two_phase:
            tmp.push_back(types::json_rpc_api::EnergyTransferModeEnum::AC_two_phase);
            is_ac_transfer_mode = true;
            break;
        case types::iso15118::EnergyTransferMode::AC_three_phase_core:
            tmp.push_back(types::json_rpc_api::EnergyTransferModeEnum::AC_three_phase_core);
            is_ac_transfer_mode = true;
            break;
        case types::iso15118::EnergyTransferMode::DC_core:
            tmp.push_back(types::json_rpc_api::EnergyTransferModeEnum::DC_core);
            break;
        case types::iso15118::EnergyTransferMode::DC_extended:
            tmp.push_back(types::json_rpc_api::EnergyTransferModeEnum::DC_extended);
            break;
        case types::iso15118::EnergyTransferMode::DC_combo_core:
            tmp.push_back(types::json_rpc_api::EnergyTransferModeEnum::DC_combo_core);
            break;
        case types::iso15118::EnergyTransferMode::DC_unique:
            tmp.push_back(types::json_rpc_api::EnergyTransferModeEnum::DC_unique);
            break;
        case types::iso15118::EnergyTransferMode::DC:
            tmp.push_back(types::json_rpc_api::EnergyTransferModeEnum::DC);
            break;
        case types::iso15118::EnergyTransferMode::AC_BPT:
            tmp.push_back(types::json_rpc_api::EnergyTransferModeEnum::AC_BPT);
            is_ac_transfer_mode = true;
            break;
        case types::iso15118::EnergyTransferMode::AC_BPT_DER:
            tmp.push_back(types::json_rpc_api::EnergyTransferModeEnum::AC_BPT_DER);
            is_ac_transfer_mode = true;
            break;
        case types::iso15118::EnergyTransferMode::AC_DER:
            tmp.push_back(types::json_rpc_api::EnergyTransferModeEnum::AC_DER);
            is_ac_transfer_mode = true;
            break;
        case types::iso15118::EnergyTransferMode::DC_BPT:
            tmp.push_back(types::json_rpc_api::EnergyTransferModeEnum::DC_BPT);
            break;
        case types::iso15118::EnergyTransferMode::DC_ACDP:
            tmp.push_back(types::json_rpc_api::EnergyTransferModeEnum::DC_ACDP);
            break;
        case types::iso15118::EnergyTransferMode::DC_ACDP_BPT:
            tmp.push_back(types::json_rpc_api::EnergyTransferModeEnum::DC_ACDP_BPT);
            break;
        case types::iso15118::EnergyTransferMode::WPT:
            tmp.push_back(types::json_rpc_api::EnergyTransferModeEnum::WPT);
            is_ac_transfer_mode = true; // TBD
            break;
        default:
            throw std::invalid_argument("Unsupported energy transfer mode");
        }
    }

    return tmp;
}

void to_json(json& j, const types::json_rpc_api::EnergyTransferModeEnum& k) {
    // the required parts of the type
    j = types::json_rpc_api::energy_transfer_mode_enum_to_string(k);
}

} // namespace json_rpc_api
} // namespace types
