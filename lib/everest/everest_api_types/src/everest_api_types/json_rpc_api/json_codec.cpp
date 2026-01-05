// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "json_rpc_api/json_codec.hpp"
#include "json_rpc_api/API.hpp"
#include "json_rpc_api/codec.hpp"
#include "nlohmann/json.hpp"
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::json_rpc_api {

void to_json(json& j, ResponseErrorEnum const& k) noexcept {
    switch (k) {
    case ResponseErrorEnum::NoError:
        j = "NoError";
        return;
    case ResponseErrorEnum::ErrorInvalidParameter:
        j = "ErrorInvalidParameter";
        return;
    case ResponseErrorEnum::ErrorOutOfRange:
        j = "ErrorOutOfRange";
        return;
    case ResponseErrorEnum::ErrorValuesNotApplied:
        j = "ErrorValuesNotApplied";
        return;
    case ResponseErrorEnum::ErrorInvalidEVSEIndex:
        j = "ErrorInvalidEVSEIndex";
        return;
    case ResponseErrorEnum::ErrorInvalidConnectorIndex:
        j = "ErrorInvalidConnectorIndex";
        return;
    case ResponseErrorEnum::ErrorNoDataAvailable:
        j = "ErrorNoDataAvailable";
        return;
    case ResponseErrorEnum::ErrorOperationNotSupported:
        j = "ErrorOperationNotSupported";
        return;
    case ResponseErrorEnum::ErrorUnknownError:
        j = "ErrorUnknownError";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::json_rpc_api::ResponseErrorEnum";
}

void from_json(json const& j, ResponseErrorEnum& k) {
    std::string s = j;
    if (s == "NoError") {
        k = ResponseErrorEnum::NoError;
        return;
    }
    if (s == "ErrorInvalidParameter") {
        k = ResponseErrorEnum::ErrorInvalidParameter;
        return;
    }
    if (s == "ErrorOutOfRange") {
        k = ResponseErrorEnum::ErrorOutOfRange;
        return;
    }
    if (s == "ErrorValuesNotApplied") {
        k = ResponseErrorEnum::ErrorValuesNotApplied;
        return;
    }
    if (s == "ErrorInvalidEVSEIndex") {
        k = ResponseErrorEnum::ErrorInvalidEVSEIndex;
        return;
    }
    if (s == "ErrorInvalidConnectorIndex") {
        k = ResponseErrorEnum::ErrorInvalidConnectorIndex;
        return;
    }
    if (s == "ErrorNoDataAvailable") {
        k = ResponseErrorEnum::ErrorNoDataAvailable;
        return;
    }
    if (s == "ErrorOperationNotSupported") {
        k = ResponseErrorEnum::ErrorOperationNotSupported;
        return;
    }
    if (s == "ErrorUnknownError") {
        k = ResponseErrorEnum::ErrorUnknownError;
        return;
    }

    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type ResponseErrorEnum");
}

void to_json(json& j, ChargeProtocolEnum const& k) noexcept {
    switch (k) {
    case ChargeProtocolEnum::Unknown:
        j = "Unknown";
        return;
    case ChargeProtocolEnum::IEC61851:
        j = "IEC61851";
        return;
    case ChargeProtocolEnum::DIN70121:
        j = "DIN70121";
        return;
    case ChargeProtocolEnum::ISO15118:
        j = "ISO15118";
        return;
    case ChargeProtocolEnum::ISO15118_20:
        j = "ISO15118_20";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::json_rpc_api::ChargeProtocolEnum";
}

void from_json(json const& j, ChargeProtocolEnum& k) {
    std::string s = j;
    if (s == "Unknown") {
        k = ChargeProtocolEnum::Unknown;
        return;
    }
    if (s == "IEC61851") {
        k = ChargeProtocolEnum::IEC61851;
        return;
    }
    if (s == "DIN70121") {
        k = ChargeProtocolEnum::DIN70121;
        return;
    }
    if (s == "ISO15118") {
        k = ChargeProtocolEnum::ISO15118;
        return;
    }
    if (s == "ISO15118_20") {
        k = ChargeProtocolEnum::ISO15118_20;
        return;
    }

    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type ChargeProtocolEnum");
}

void to_json(json& j, EVSEStateEnum const& k) noexcept {
    switch (k) {
    case EVSEStateEnum::Unknown:
        j = "Unknown";
        return;
    case EVSEStateEnum::Unplugged:
        j = "Unplugged";
        return;
    case EVSEStateEnum::Disabled:
        j = "Disabled";
        return;
    case EVSEStateEnum::Preparing:
        j = "Preparing";
        return;
    case EVSEStateEnum::Reserved:
        j = "Reserved";
        return;
    case EVSEStateEnum::AuthRequired:
        j = "AuthRequired";
        return;
    case EVSEStateEnum::WaitingForEnergy:
        j = "WaitingForEnergy";
        return;
    case EVSEStateEnum::ChargingPausedEV:
        j = "ChargingPausedEV";
        return;
    case EVSEStateEnum::ChargingPausedEVSE:
        j = "ChargingPausedEVSE";
        return;
    case EVSEStateEnum::Charging:
        j = "Charging";
        return;
    case EVSEStateEnum::AuthTimeout:
        j = "AuthTimeout";
        return;
    case EVSEStateEnum::Finished:
        j = "Finished";
        return;
    case EVSEStateEnum::FinishedEVSE:
        j = "FinishedEVSE";
        return;
    case EVSEStateEnum::FinishedEV:
        j = "FinishedEV";
        return;
    case EVSEStateEnum::SwitchingPhases:
        j = "SwitchingPhases";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::json_rpc_api::EVSEStateEnum";
}

void from_json(json const& j, EVSEStateEnum& k) {
    std::string s = j;
    if (s == "Unknown") {
        k = EVSEStateEnum::Unknown;
        return;
    }
    if (s == "Unplugged") {
        k = EVSEStateEnum::Unplugged;
        return;
    }
    if (s == "Disabled") {
        k = EVSEStateEnum::Disabled;
        return;
    }
    if (s == "Preparing") {
        k = EVSEStateEnum::Preparing;
        return;
    }
    if (s == "Reserved") {
        k = EVSEStateEnum::Reserved;
        return;
    }
    if (s == "AuthRequired") {
        k = EVSEStateEnum::AuthRequired;
        return;
    }
    if (s == "WaitingForEnergy") {
        k = EVSEStateEnum::WaitingForEnergy;
        return;
    }
    if (s == "ChargingPausedEV") {
        k = EVSEStateEnum::ChargingPausedEV;
        return;
    }
    if (s == "ChargingPausedEVSE") {
        k = EVSEStateEnum::ChargingPausedEVSE;
        return;
    }
    if (s == "Charging") {
        k = EVSEStateEnum::Charging;
        return;
    }
    if (s == "AuthTimeout") {
        k = EVSEStateEnum::AuthTimeout;
        return;
    }
    if (s == "Finished") {
        k = EVSEStateEnum::Finished;
        return;
    }
    if (s == "FinishedEVSE") {
        k = EVSEStateEnum::FinishedEVSE;
        return;
    }
    if (s == "FinishedEV") {
        k = EVSEStateEnum::FinishedEV;
        return;
    }
    if (s == "SwitchingPhases") {
        k = EVSEStateEnum::SwitchingPhases;
        return;
    }

    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type EVSEStateEnum");
}

void to_json(json& j, ConnectorTypeEnum const& k) noexcept {
    switch (k) {
    case ConnectorTypeEnum::cCCS1:
        j = "cCCS1";
        return;
    case ConnectorTypeEnum::cCCS2:
        j = "cCCS2";
        return;
    case ConnectorTypeEnum::cG105:
        j = "cG105";
        return;
    case ConnectorTypeEnum::cTesla:
        j = "cTesla";
        return;
    case ConnectorTypeEnum::cType1:
        j = "cType1";
        return;
    case ConnectorTypeEnum::cType2:
        j = "cType2";
        return;
    case ConnectorTypeEnum::s309_1P_16A:
        j = "s309_1P_16A";
        return;
    case ConnectorTypeEnum::s309_1P_32A:
        j = "s309_1P_32A";
        return;
    case ConnectorTypeEnum::s309_3P_16A:
        j = "s309_3P_16A";
        return;
    case ConnectorTypeEnum::s309_3P_32A:
        j = "s309_3P_32A";
        return;
    case ConnectorTypeEnum::sBS1361:
        j = "sBS1361";
        return;
    case ConnectorTypeEnum::sCEE_7_7:
        j = "sCEE_7_7";
        return;
    case ConnectorTypeEnum::sType2:
        j = "sType2";
        return;
    case ConnectorTypeEnum::sType3:
        j = "sType3";
        return;
    case ConnectorTypeEnum::Other1PhMax16A:
        j = "Other1PhMax16A";
        return;
    case ConnectorTypeEnum::Other1PhOver16A:
        j = "Other1PhOver16A";
        return;
    case ConnectorTypeEnum::Other3Ph:
        j = "Other3Ph";
        return;
    case ConnectorTypeEnum::Pan:
        j = "Pan";
        return;
    case ConnectorTypeEnum::wInductive:
        j = "wInductive";
        return;
    case ConnectorTypeEnum::wResonant:
        j = "wResonant";
        return;
    case ConnectorTypeEnum::Undetermined:
        j = "Undetermined";
        return;
    case ConnectorTypeEnum::Unknown:
        j = "Unknown";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::json_rpc_api::ConnectorTypeEnum";
}

void from_json(json const& j, ConnectorTypeEnum& k) {
    std::string s = j;
    if (s == "cCCS1") {
        k = ConnectorTypeEnum::cCCS1;
        return;
    }
    if (s == "cCCS2") {
        k = ConnectorTypeEnum::cCCS2;
        return;
    }
    if (s == "cG105") {
        k = ConnectorTypeEnum::cG105;
        return;
    }
    if (s == "cTesla") {
        k = ConnectorTypeEnum::cTesla;
        return;
    }
    if (s == "cType1") {
        k = ConnectorTypeEnum::cType1;
        return;
    }
    if (s == "cType2") {
        k = ConnectorTypeEnum::cType2;
        return;
    }
    if (s == "s309_1P_16A") {
        k = ConnectorTypeEnum::s309_1P_16A;
        return;
    }
    if (s == "s309_1P_32A") {
        k = ConnectorTypeEnum::s309_1P_32A;
        return;
    }
    if (s == "s309_3P_16A") {
        k = ConnectorTypeEnum::s309_3P_16A;
        return;
    }
    if (s == "s309_3P_32A") {
        k = ConnectorTypeEnum::s309_3P_32A;
        return;
    }
    if (s == "sBS1361") {
        k = ConnectorTypeEnum::sBS1361;
        return;
    }
    if (s == "sCEE_7_7") {
        k = ConnectorTypeEnum::sCEE_7_7;
        return;
    }
    if (s == "sType2") {
        k = ConnectorTypeEnum::sType2;
        return;
    }
    if (s == "sType3") {
        k = ConnectorTypeEnum::sType3;
        return;
    }
    if (s == "Other1PhMax16A") {
        k = ConnectorTypeEnum::Other1PhMax16A;
        return;
    }
    if (s == "Other1PhOver16A") {
        k = ConnectorTypeEnum::Other1PhOver16A;
        return;
    }
    if (s == "Other3Ph") {
        k = ConnectorTypeEnum::Other3Ph;
        return;
    }
    if (s == "Pan") {
        k = ConnectorTypeEnum::Pan;
        return;
    }
    if (s == "wInductive") {
        k = ConnectorTypeEnum::wInductive;
        return;
    }
    if (s == "wResonant") {
        k = ConnectorTypeEnum::wResonant;
        return;
    }
    if (s == "Undetermined") {
        k = ConnectorTypeEnum::Undetermined;
        return;
    }
    if (s == "Unknown") {
        k = ConnectorTypeEnum::Unknown;
        return;
    }

    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type ConnectorTypeEnum");
}

void to_json(json& j, EnergyTransferModeEnum const& k) noexcept {
    switch (k) {
    case EnergyTransferModeEnum::AC_single_phase_core:
        j = "AC_single_phase_core";
        return;
    case EnergyTransferModeEnum::AC_two_phase:
        j = "AC_two_phase";
        return;
    case EnergyTransferModeEnum::AC_three_phase_core:
        j = "AC_three_phase_core";
        return;
    case EnergyTransferModeEnum::DC_core:
        j = "DC_core";
        return;
    case EnergyTransferModeEnum::DC_extended:
        j = "DC_extended";
        return;
    case EnergyTransferModeEnum::DC_combo_core:
        j = "DC_combo_core";
        return;
    case EnergyTransferModeEnum::DC_unique:
        j = "DC_unique";
        return;
    case EnergyTransferModeEnum::DC:
        j = "DC";
        return;
    case EnergyTransferModeEnum::AC_BPT:
        j = "AC_BPT";
        return;
    case EnergyTransferModeEnum::AC_BPT_DER:
        j = "AC_BPT_DER";
        return;
    case EnergyTransferModeEnum::AC_DER:
        j = "AC_DER";
        return;
    case EnergyTransferModeEnum::DC_BPT:
        j = "DC_BPT";
        return;
    case EnergyTransferModeEnum::DC_ACDP:
        j = "DC_ACDP";
        return;
    case EnergyTransferModeEnum::DC_ACDP_BPT:
        j = "DC_ACDP_BPT";
        return;
    case EnergyTransferModeEnum::WPT:
        j = "WPT";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::json_rpc_api::EnergyTransferModeEnum";
}

void from_json(json const& j, EnergyTransferModeEnum& k) {
    std::string s = j;
    if (s == "AC_single_phase_core") {
        k = EnergyTransferModeEnum::AC_single_phase_core;
        return;
    }
    if (s == "AC_two_phase") {
        k = EnergyTransferModeEnum::AC_two_phase;
        return;
    }
    if (s == "AC_three_phase_core") {
        k = EnergyTransferModeEnum::AC_three_phase_core;
        return;
    }
    if (s == "DC_core") {
        k = EnergyTransferModeEnum::DC_core;
        return;
    }
    if (s == "DC_extended") {
        k = EnergyTransferModeEnum::DC_extended;
        return;
    }
    if (s == "DC_combo_core") {
        k = EnergyTransferModeEnum::DC_combo_core;
        return;
    }
    if (s == "DC_unique") {
        k = EnergyTransferModeEnum::DC_unique;
        return;
    }
    if (s == "DC") {
        k = EnergyTransferModeEnum::DC;
        return;
    }
    if (s == "AC_BPT") {
        k = EnergyTransferModeEnum::AC_BPT;
        return;
    }
    if (s == "AC_BPT_DER") {
        k = EnergyTransferModeEnum::AC_BPT_DER;
        return;
    }
    if (s == "AC_DER") {
        k = EnergyTransferModeEnum::AC_DER;
        return;
    }
    if (s == "DC_BPT") {
        k = EnergyTransferModeEnum::DC_BPT;
        return;
    }
    if (s == "DC_ACDP") {
        k = EnergyTransferModeEnum::DC_ACDP;
        return;
    }
    if (s == "DC_ACDP_BPT") {
        k = EnergyTransferModeEnum::DC_ACDP_BPT;
        return;
    }
    if (s == "WPT") {
        k = EnergyTransferModeEnum::WPT;
        return;
    }

    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type EnergyTransferModeEnum");
}

void to_json(json& j, Severity const& k) noexcept {
    switch (k) {
    case Severity::High:
        j = "High";
        return;
    case Severity::Medium:
        j = "Medium";
        return;
    case Severity::Low:
        j = "Low";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::json_rpc_api::Severity";
}

void from_json(json const& j, Severity& k) {
    std::string s = j;
    if (s == "High") {
        k = Severity::High;
        return;
    }
    if (s == "Medium") {
        k = Severity::Medium;
        return;
    }
    if (s == "Low") {
        k = Severity::Low;
        return;
    }

    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type Severity");
}

void to_json(json& j, ImplementationIdentifier const& k) noexcept {
    j = json{{"module_id", k.module_id}, {"implementation_id", k.implementation_id}};
    if (k.evse_index) {
        j["evse_index"] = k.evse_index.value();
    }
    if (k.connector_index) {
        j["connector_index"] = k.connector_index.value();
    }
}

void from_json(const json& j, ImplementationIdentifier& k) {
    k.module_id = j.at("module_id");
    k.implementation_id = j.at("implementation_id");

    if (j.contains("evse_index")) {
        k.evse_index.emplace(j.at("evse_index"));
    }
    if (j.contains("connector_index")) {
        k.connector_index.emplace(j.at("connector_index"));
    }
}

void to_json(json& j, ChargerInfoObj const& k) noexcept {
    j = json{{"vendor", k.vendor}, {"model", k.model}, {"serial", k.serial}, {"firmware_version", k.firmware_version}};
    if (k.friendly_name) {
        j["friendly_name"] = k.friendly_name.value();
    }
    if (k.manufacturer) {
        j["manufacturer"] = k.manufacturer.value();
    }
    if (k.manufacturer_url) {
        j["manufacturer_url"] = k.manufacturer_url.value();
    }
    if (k.model_url) {
        j["model_url"] = k.model_url.value();
    }
    if (k.model_no) {
        j["model_no"] = k.model_no.value();
    }
    if (k.revision) {
        j["revision"] = k.revision.value();
    }
    if (k.board_revision) {
        j["board_revision"] = k.board_revision.value();
    }
}

void from_json(const json& j, ChargerInfoObj& k) {
    k.vendor = j.at("vendor");
    k.model = j.at("model");
    k.serial = j.at("serial");
    k.firmware_version = j.at("firmware_version");

    if (j.contains("friendly_name")) {
        k.friendly_name.emplace(j.at("friendly_name"));
    }
    if (j.contains("manufacturer")) {
        k.manufacturer.emplace(j.at("manufacturer"));
    }
    if (j.contains("manufacturer_url")) {
        k.manufacturer_url.emplace(j.at("manufacturer_url"));
    }
    if (j.contains("model_url")) {
        k.model_url.emplace(j.at("model_url"));
    }
    if (j.contains("model_no")) {
        k.model_no.emplace(j.at("model_no"));
    }
    if (j.contains("revision")) {
        k.revision.emplace(j.at("revision"));
    }
    if (j.contains("board_revision")) {
        k.board_revision.emplace(j.at("board_revision"));
    }
}

void to_json(json& j, ErrorObj const& k) noexcept {
    j = json{{"type", k.type},     {"description", k.description}, {"message", k.message}, {"severity", k.severity},
             {"origin", k.origin}, {"timestamp", k.timestamp},     {"uuid", k.uuid}};
    if (k.sub_type) {
        j["sub_type"] = k.sub_type.value();
    }
}

void from_json(const json& j, ErrorObj& k) {
    k.type = j.at("type");
    k.description = j.at("description");
    k.message = j.at("message");
    k.severity = j.at("severity");
    k.origin = j.at("origin");
    k.timestamp = j.at("timestamp");
    k.uuid = j.at("uuid");

    if (j.contains("sub_type")) {
        k.sub_type.emplace(j.at("sub_type"));
    }
}

void to_json(json& j, ConnectorInfoObj const& k) noexcept {
    j = json{{"index", k.index}, {"type", k.type}};
    if (k.description) {
        j["description"] = k.description.value();
    }
}

void from_json(const json& j, ConnectorInfoObj& k) {
    k.index = j.at("index");
    k.type = j.at("type");

    if (j.contains("description")) {
        k.description.emplace(j.at("description"));
    }
}

void to_json(json& j, ACChargeParametersObj const& k) noexcept {
    j = json{{"evse_max_current", k.evse_max_current},
             {"evse_max_phase_count", k.evse_max_phase_count},
             {"evse_maximum_charge_power", k.evse_maximum_charge_power},
             {"evse_minimum_charge_power", k.evse_minimum_charge_power},
             {"evse_nominal_frequency", k.evse_nominal_frequency}};
    if (k.evse_nominal_voltage) {
        j["evse_nominal_voltage"] = k.evse_nominal_voltage.value();
    }
    if (k.evse_maximum_discharge_power) {
        j["evse_maximum_discharge_power"] = k.evse_maximum_discharge_power.value();
    }
    if (k.evse_minimum_discharge_power) {
        j["evse_minimum_discharge_power"] = k.evse_minimum_discharge_power.value();
    }
}

void from_json(const json& j, ACChargeParametersObj& k) {
    k.evse_max_current = j.at("evse_max_current");
    k.evse_max_phase_count = j.at("evse_max_phase_count");
    k.evse_maximum_charge_power = j.at("evse_maximum_charge_power");
    k.evse_minimum_charge_power = j.at("evse_minimum_charge_power");
    k.evse_nominal_frequency = j.at("evse_nominal_frequency");

    if (j.contains("evse_nominal_voltage")) {
        k.evse_nominal_voltage.emplace(j.at("evse_nominal_voltage"));
    }
    if (j.contains("evse_maximum_discharge_power")) {
        k.evse_maximum_discharge_power.emplace(j.at("evse_maximum_discharge_power"));
    }
    if (j.contains("evse_minimum_discharge_power")) {
        k.evse_minimum_discharge_power.emplace(j.at("evse_minimum_discharge_power"));
    }
}

void to_json(json& j, DCChargeParametersObj const& k) noexcept {
    j = json{{"evse_maximum_charge_current", k.evse_maximum_charge_current},
             {"evse_maximum_charge_power", k.evse_maximum_charge_power},
             {"evse_maximum_voltage", k.evse_maximum_voltage},
             {"evse_minimum_charge_current", k.evse_minimum_charge_current},
             {"evse_minimum_charge_power", k.evse_minimum_charge_power},
             {"evse_minimum_voltage", k.evse_minimum_voltage}};
    if (k.evse_energy_to_be_delivered) {
        j["evse_energy_to_be_delivered"] = k.evse_energy_to_be_delivered.value();
    }
    if (k.evse_maximum_discharge_current) {
        j["evse_maximum_discharge_current"] = k.evse_maximum_discharge_current.value();
    }
    if (k.evse_maximum_discharge_power) {
        j["evse_maximum_discharge_power"] = k.evse_maximum_discharge_power.value();
    }
    if (k.evse_minimum_discharge_current) {
        j["evse_minimum_discharge_current"] = k.evse_minimum_discharge_current.value();
    }
    if (k.evse_minimum_discharge_power) {
        j["evse_minimum_discharge_power"] = k.evse_minimum_discharge_power.value();
    }
}

void from_json(const json& j, DCChargeParametersObj& k) {
    k.evse_maximum_charge_current = j.at("evse_maximum_charge_current");
    k.evse_maximum_charge_power = j.at("evse_maximum_charge_power");
    k.evse_maximum_voltage = j.at("evse_maximum_voltage");
    k.evse_minimum_charge_current = j.at("evse_minimum_charge_current");
    k.evse_minimum_charge_power = j.at("evse_minimum_charge_power");
    k.evse_minimum_voltage = j.at("evse_minimum_voltage");

    if (j.contains("evse_energy_to_be_delivered")) {
        k.evse_energy_to_be_delivered.emplace(j.at("evse_energy_to_be_delivered"));
    }
    if (j.contains("evse_maximum_discharge_current")) {
        k.evse_maximum_discharge_current.emplace(j.at("evse_maximum_discharge_current"));
    }
    if (j.contains("evse_maximum_discharge_power")) {
        k.evse_maximum_discharge_power.emplace(j.at("evse_maximum_discharge_power"));
    }
    if (j.contains("evse_minimum_discharge_current")) {
        k.evse_minimum_discharge_current.emplace(j.at("evse_minimum_discharge_current"));
    }
    if (j.contains("evse_minimum_discharge_power")) {
        k.evse_minimum_discharge_power.emplace(j.at("evse_minimum_discharge_power"));
    }
}

void to_json(json& j, ACChargeStatusObj const& k) noexcept {
    j = json{{"evse_active_phase_count", k.evse_active_phase_count}};
}

void from_json(const json& j, ACChargeStatusObj& k) {
    k.evse_active_phase_count = j.at("evse_active_phase_count");
}

void to_json(json& j, DCChargeStatusObj const& k) noexcept {
    j = json{{"evse_present_current", k.evse_present_current},
             {"evse_present_voltage", k.evse_present_voltage},
             {"evse_power_limit_achieved", k.evse_power_limit_achieved},
             {"evse_current_limit_achieved", k.evse_current_limit_achieved},
             {"evse_voltage_limit_achieved", k.evse_voltage_limit_achieved}};
}

void from_json(const json& j, DCChargeStatusObj& k) {
    k.evse_present_current = j.at("evse_present_current");
    k.evse_present_voltage = j.at("evse_present_voltage");
    k.evse_power_limit_achieved = j.at("evse_power_limit_achieved");
    k.evse_current_limit_achieved = j.at("evse_current_limit_achieved");
    k.evse_voltage_limit_achieved = j.at("evse_voltage_limit_achieved");
}

void to_json(json& j, DisplayParametersObj const& k) noexcept {
    j = json({});
    if (k.start_soc) {
        j["start_soc"] = k.start_soc.value();
    }
    if (k.present_soc) {
        j["present_soc"] = k.present_soc.value();
    }
    if (k.minimum_soc) {
        j["minimum_soc"] = k.minimum_soc.value();
    }
    if (k.target_soc) {
        j["target_soc"] = k.target_soc.value();
    }
    if (k.maximum_soc) {
        j["maximum_soc"] = k.maximum_soc.value();
    }
    if (k.remaining_time_to_minimum_soc) {
        j["remaining_time_to_minimum_soc"] = k.remaining_time_to_minimum_soc.value();
    }
    if (k.remaining_time_to_target_soc) {
        j["remaining_time_to_target_soc"] = k.remaining_time_to_target_soc.value();
    }
    if (k.remaining_time_to_maximum_soc) {
        j["remaining_time_to_maximum_soc"] = k.remaining_time_to_maximum_soc.value();
    }
    if (k.charging_complete) {
        j["charging_complete"] = k.charging_complete.value();
    }
    if (k.battery_energy_capacity) {
        j["battery_energy_capacity"] = k.battery_energy_capacity.value();
    }
    if (k.inlet_hot) {
        j["inlet_hot"] = k.inlet_hot.value();
    }
}

void from_json(const json& j, DisplayParametersObj& k) {
    if (j.contains("start_soc")) {
        k.start_soc.emplace(j.at("start_soc"));
    }
    if (j.contains("present_soc")) {
        k.present_soc.emplace(j.at("present_soc"));
    }
    if (j.contains("minimum_soc")) {
        k.minimum_soc.emplace(j.at("minimum_soc"));
    }
    if (j.contains("target_soc")) {
        k.target_soc.emplace(j.at("target_soc"));
    }
    if (j.contains("maximum_soc")) {
        k.maximum_soc.emplace(j.at("maximum_soc"));
    }
    if (j.contains("remaining_time_to_minimum_soc")) {
        k.remaining_time_to_minimum_soc.emplace(j.at("remaining_time_to_minimum_soc"));
    }
    if (j.contains("remaining_time_to_target_soc")) {
        k.remaining_time_to_target_soc.emplace(j.at("remaining_time_to_target_soc"));
    }
    if (j.contains("remaining_time_to_maximum_soc")) {
        k.remaining_time_to_maximum_soc.emplace(j.at("remaining_time_to_maximum_soc"));
    }
    if (j.contains("charging_complete")) {
        k.charging_complete.emplace(j.at("charging_complete"));
    }
    if (j.contains("battery_energy_capacity")) {
        k.battery_energy_capacity.emplace(j.at("battery_energy_capacity"));
    }
    if (j.contains("inlet_hot")) {
        k.inlet_hot.emplace(j.at("inlet_hot"));
    }
}

void to_json(json& j, HardwareCapabilitiesObj const& k) noexcept {
    j = json{{"max_current_A_export", k.max_current_A_export},
             {"max_current_A_import", k.max_current_A_import},
             {"max_phase_count_export", k.max_phase_count_export},
             {"max_phase_count_import", k.max_phase_count_import},
             {"min_current_A_export", k.min_current_A_export},
             {"min_current_A_import", k.min_current_A_import},
             {"min_phase_count_export", k.min_phase_count_export},
             {"min_phase_count_import", k.min_phase_count_import},
             {"phase_switch_during_charging", k.phase_switch_during_charging}};
}

void from_json(const json& j, HardwareCapabilitiesObj& k) {
    k.max_current_A_export = j.at("max_current_A_export");
    k.max_current_A_import = j.at("max_current_A_import");
    k.max_phase_count_export = j.at("max_phase_count_export");
    k.max_phase_count_import = j.at("max_phase_count_import");
    k.min_current_A_export = j.at("min_current_A_export");
    k.min_current_A_import = j.at("min_current_A_import");
    k.min_phase_count_export = j.at("min_phase_count_export");
    k.min_phase_count_import = j.at("min_phase_count_import");
    k.phase_switch_during_charging = j.at("phase_switch_during_charging");
}

void to_json(json& j, EVSEInfoObj const& k) noexcept {
    j = json{{"index", k.index},
             {"id", k.id},
             {"available_connectors", k.available_connectors},
             {"supported_energy_transfer_modes", k.supported_energy_transfer_modes}};
    if (k.description) {
        j["description"] = k.description.value();
    }
}

void from_json(const json& j, EVSEInfoObj& k) {
    k.index = j.at("index");
    k.id = j.at("id");
    for (auto val : j.at("available_connectors")) {
        k.available_connectors.push_back(val);
    }
    for (auto val : j.at("supported_energy_transfer_modes")) {
        k.supported_energy_transfer_modes.push_back(val);
    }

    if (j.contains("description")) {
        k.description.emplace(j.at("description"));
    }
}

void to_json(json& j, EVSEStatusObj const& k) noexcept {
    j = json{{"charged_energy_wh", k.charged_energy_wh},
             {"discharged_energy_wh", k.discharged_energy_wh},
             {"charging_duration_s", k.charging_duration_s},
             {"charging_allowed", k.charging_allowed},
             {"available", k.available},
             {"active_connector_index", k.active_connector_index},
             {"error_present", k.error_present},
             {"charge_protocol", k.charge_protocol},
             {"state", k.state}};
    if (k.ac_charge_param) {
        j["ac_charge_param"] = k.ac_charge_param.value();
    }
    if (k.dc_charge_param) {
        j["dc_charge_param"] = k.dc_charge_param.value();
    }
    if (k.ac_charge_status) {
        j["ac_charge_status"] = k.ac_charge_status.value();
    }
    if (k.dc_charge_status) {
        j["dc_charge_status"] = k.dc_charge_status.value();
    }
    if (k.display_parameters) {
        j["display_parameters"] = k.display_parameters.value();
    }
}

void from_json(const json& j, EVSEStatusObj& k) {
    k.charged_energy_wh = j.at("charged_energy_wh");
    k.discharged_energy_wh = j.at("discharged_energy_wh");
    k.charging_duration_s = j.at("charging_duration_s");
    k.charging_allowed = j.at("charging_allowed");
    k.available = j.at("available");
    k.active_connector_index = j.at("active_connector_index");
    k.error_present = j.at("error_present");
    k.charge_protocol = j.at("charge_protocol");
    k.state = j.at("state");

    if (j.contains("ac_charge_param")) {
        k.ac_charge_param.emplace(j.at("ac_charge_param"));
    }
    if (j.contains("dc_charge_param")) {
        k.dc_charge_param.emplace(j.at("dc_charge_param"));
    }
    if (j.contains("ac_charge_status")) {
        k.ac_charge_status.emplace(j.at("ac_charge_status"));
    }
    if (j.contains("dc_charge_status")) {
        k.dc_charge_status.emplace(j.at("dc_charge_status"));
    }
    if (j.contains("display_parameters")) {
        k.display_parameters.emplace(j.at("display_parameters"));
    }
}

void to_json(json& j, Current_A const& k) noexcept {
    j = json({});
    if (k.L1) {
        j["L1"] = k.L1.value();
    }
    if (k.L2) {
        j["L2"] = k.L2.value();
    }
    if (k.L3) {
        j["L3"] = k.L3.value();
    }
    if (k.N) {
        j["N"] = k.N.value();
    }
}

void from_json(const json& j, Current_A& k) {
    if (j.contains("L1")) {
        k.L1.emplace(j.at("L1"));
    }
    if (j.contains("L2")) {
        k.L2.emplace(j.at("L2"));
    }
    if (j.contains("L3")) {
        k.L3.emplace(j.at("L3"));
    }
    if (j.contains("N")) {
        k.N.emplace(j.at("N"));
    }
}

void to_json(json& j, Energy_Wh_import const& k) noexcept {
    j = json{{"total", k.total}};
    if (k.L1) {
        j["L1"] = k.L1.value();
    }
    if (k.L2) {
        j["L2"] = k.L2.value();
    }
    if (k.L3) {
        j["L3"] = k.L3.value();
    }
}

void from_json(const json& j, Energy_Wh_import& k) {
    k.total = j.at("total");

    if (j.contains("L1")) {
        k.L1.emplace(j.at("L1"));
    }
    if (j.contains("L2")) {
        k.L2.emplace(j.at("L2"));
    }
    if (j.contains("L3")) {
        k.L3.emplace(j.at("L3"));
    }
}

void to_json(json& j, Energy_Wh_export const& k) noexcept {
    j = json{{"total", k.total}};
    if (k.L1) {
        j["L1"] = k.L1.value();
    }
    if (k.L2) {
        j["L2"] = k.L2.value();
    }
    if (k.L3) {
        j["L3"] = k.L3.value();
    }
}

void from_json(const json& j, Energy_Wh_export& k) {
    k.total = j.at("total");

    if (j.contains("L1")) {
        k.L1.emplace(j.at("L1"));
    }
    if (j.contains("L2")) {
        k.L2.emplace(j.at("L2"));
    }
    if (j.contains("L3")) {
        k.L3.emplace(j.at("L3"));
    }
}

void to_json(json& j, Frequency_Hz const& k) noexcept {
    j = json{{"L1", k.L1}};
    if (k.L2) {
        j["L2"] = k.L2.value();
    }
    if (k.L3) {
        j["L3"] = k.L3.value();
    }
}

void from_json(const json& j, Frequency_Hz& k) {
    k.L1 = j.at("L1");

    if (j.contains("L2")) {
        k.L2.emplace(j.at("L2"));
    }
    if (j.contains("L3")) {
        k.L3.emplace(j.at("L3"));
    }
}

void to_json(json& j, Power_W const& k) noexcept {
    j = json{{"total", k.total}};
    if (k.L1) {
        j["L1"] = k.L1.value();
    }
    if (k.L2) {
        j["L2"] = k.L2.value();
    }
    if (k.L3) {
        j["L3"] = k.L3.value();
    }
}

void from_json(const json& j, Power_W& k) {
    k.total = j.at("total");

    if (j.contains("L1")) {
        k.L1.emplace(j.at("L1"));
    }
    if (j.contains("L2")) {
        k.L2.emplace(j.at("L2"));
    }
    if (j.contains("L3")) {
        k.L3.emplace(j.at("L3"));
    }
}

void to_json(json& j, Voltage_V const& k) noexcept {
    j = json({});
    if (k.L1) {
        j["L1"] = k.L1.value();
    }
    if (k.L2) {
        j["L2"] = k.L2.value();
    }
    if (k.L3) {
        j["L3"] = k.L3.value();
    }
}

void from_json(const json& j, Voltage_V& k) {
    if (j.contains("L1")) {
        k.L1.emplace(j.at("L1"));
    }
    if (j.contains("L2")) {
        k.L2.emplace(j.at("L2"));
    }
    if (j.contains("L3")) {
        k.L3.emplace(j.at("L3"));
    }
}

void to_json(json& j, MeterDataObj const& k) noexcept {
    j = json{{"energy_Wh_import", k.energy_Wh_import}, {"timestamp", k.timestamp}};
    if (k.current_A) {
        j["current_A"] = k.current_A.value();
    }
    if (k.energy_Wh_export) {
        j["energy_Wh_export"] = k.energy_Wh_export.value();
    }
    if (k.frequency_Hz) {
        j["frequency_Hz"] = k.frequency_Hz.value();
    }
    if (k.meter_id) {
        j["meter_id"] = k.meter_id.value();
    }
    if (k.serial_number) {
        j["serial_number"] = k.serial_number.value();
    }
    if (k.phase_seq_error) {
        j["phase_seq_error"] = k.phase_seq_error.value();
    }
    if (k.power_W) {
        j["power_W"] = k.power_W.value();
    }
    if (k.voltage_V) {
        j["voltage_V"] = k.voltage_V.value();
    }
}

void from_json(const json& j, MeterDataObj& k) {
    k.energy_Wh_import = j.at("energy_Wh_import");
    k.timestamp = j.at("timestamp");

    if (j.contains("current_A")) {
        k.current_A.emplace(j.at("current_A"));
    }
    if (j.contains("energy_Wh_export")) {
        k.energy_Wh_export.emplace(j.at("energy_Wh_export"));
    }
    if (j.contains("frequency_Hz")) {
        k.frequency_Hz.emplace(j.at("frequency_Hz"));
    }
    if (j.contains("meter_id")) {
        k.meter_id.emplace(j.at("meter_id"));
    }
    if (j.contains("serial_number")) {
        k.serial_number.emplace(j.at("serial_number"));
    }
    if (j.contains("phase_seq_error")) {
        k.phase_seq_error.emplace(j.at("phase_seq_error"));
    }
    if (j.contains("power_W")) {
        k.power_W.emplace(j.at("power_W"));
    }
    if (j.contains("voltage_V")) {
        k.voltage_V.emplace(j.at("voltage_V"));
    }
}

void to_json(json& j, HelloResObj const& k) noexcept {
    j = json{{"authentication_required", k.authentication_required},
             {"api_version", k.api_version},
             {"everest_version", k.everest_version},
             {"charger_info", k.charger_info}};
    if (k.authenticated) {
        j["authenticated"] = k.authenticated.value();
    }
}

void from_json(const json& j, HelloResObj& k) {
    k.authentication_required = j.at("authentication_required");
    k.api_version = j.at("api_version");
    k.everest_version = j.at("everest_version");
    k.charger_info = j.at("charger_info");

    if (j.contains("authenticated")) {
        k.authenticated.emplace(j.at("authenticated"));
    }
}

void to_json(json& j, ChargePointGetEVSEInfosResObj const& k) noexcept {
    j = json{{"infos", k.infos}, {"error", k.error}};
}

void from_json(const json& j, ChargePointGetEVSEInfosResObj& k) {
    for (auto val : j.at("infos")) {
        k.infos.push_back(val);
    }
    k.error = j.at("error");
}

void to_json(json& j, ChargePointGetActiveErrorsResObj const& k) noexcept {
    j = json{{"active_errors", k.active_errors}, {"error", k.error}};
}

void from_json(const json& j, ChargePointGetActiveErrorsResObj& k) {
    for (auto val : j.at("active_errors")) {
        k.active_errors.push_back(val);
    }
    k.error = j.at("error");
}

void to_json(json& j, EVSEGetInfoResObj const& k) noexcept {
    j = json{{"info", k.info}, {"error", k.error}};
}

void from_json(const json& j, EVSEGetInfoResObj& k) {
    k.info = j.at("info");
    k.error = j.at("error");
}

void to_json(json& j, EVSEGetStatusResObj const& k) noexcept {
    j = json{{"status", k.status}, {"error", k.error}};
}

void from_json(const json& j, EVSEGetStatusResObj& k) {
    k.status = j.at("status");
    k.error = j.at("error");
}

void to_json(json& j, EVSEGetHardwareCapabilitiesResObj const& k) noexcept {
    j = json{{"hardware_capabilities", k.hardware_capabilities}, {"error", k.error}};
}

void from_json(const json& j, EVSEGetHardwareCapabilitiesResObj& k) {
    k.hardware_capabilities = j.at("hardware_capabilities");
    k.error = j.at("error");
}

void to_json(json& j, EVSEGetMeterDataResObj const& k) noexcept {
    j = json{{"meter_data", k.meter_data}, {"error", k.error}};
}

void from_json(const json& j, EVSEGetMeterDataResObj& k) {
    k.meter_data = j.at("meter_data");
    k.error = j.at("error");
}

void to_json(json& j, ErrorResObj const& k) noexcept {
    j = json{{"error", k.error}};
}

void from_json(const json& j, ErrorResObj& k) {
    k.error = j.at("error");
}

void to_json(json& j, ChargePointActiveErrorsChangedObj const& k) noexcept {
    j = json{{"active_errors", k.active_errors}};
}

void from_json(const json& j, ChargePointActiveErrorsChangedObj& k) {
    for (auto val : j.at("active_errors")) {
        k.active_errors.push_back(val);
    }
}

void to_json(json& j, EVSEHardwareCapabilitiesChangedObj const& k) noexcept {
    j = json{{"evse_index", k.evse_index}, {"hardware_capabilities", k.hardware_capabilities}};
}

void from_json(const json& j, EVSEHardwareCapabilitiesChangedObj& k) {
    k.evse_index = j.at("evse_index");
    k.hardware_capabilities = j.at("hardware_capabilities");
}

void to_json(json& j, EVSEStatusChangedObj const& k) noexcept {
    j = json{{"evse_index", k.evse_index}, {"evse_status", k.evse_status}};
}

void from_json(const json& j, EVSEStatusChangedObj& k) {
    k.evse_index = j.at("evse_index");
    k.evse_status = j.at("evse_status");
}

void to_json(json& j, EVSEMeterDataChangedObj const& k) noexcept {
    j = json{{"evse_index", k.evse_index}, {"meter_data", k.meter_data}};
}

void from_json(const json& j, EVSEMeterDataChangedObj& k) {
    k.evse_index = j.at("evse_index");
    k.meter_data = j.at("meter_data");
}

} // namespace everest::lib::API::V1_0::types::json_rpc_api
