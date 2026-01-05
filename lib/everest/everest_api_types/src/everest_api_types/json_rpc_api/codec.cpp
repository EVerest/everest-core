// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "json_rpc_api/codec.hpp"
#include "json_rpc_api/API.hpp"
#include "json_rpc_api/json_codec.hpp"
#include "nlohmann/json.hpp"
#include "utilities/constants.hpp"
#include <stdexcept>
#include <string>

namespace everest::lib::API::V1_0::types::json_rpc_api {

std::string response_error_enum_to_string(ResponseErrorEnum e) {
    switch (e) {
    case ResponseErrorEnum::NoError:
        return "NoError";
    case ResponseErrorEnum::ErrorInvalidParameter:
        return "ErrorInvalidParameter";
    case ResponseErrorEnum::ErrorOutOfRange:
        return "ErrorOutOfRange";
    case ResponseErrorEnum::ErrorValuesNotApplied:
        return "ErrorValuesNotApplied";
    case ResponseErrorEnum::ErrorInvalidEVSEIndex:
        return "ErrorInvalidEVSEIndex";
    case ResponseErrorEnum::ErrorInvalidConnectorIndex:
        return "ErrorInvalidConnectorIndex";
    case ResponseErrorEnum::ErrorNoDataAvailable:
        return "ErrorNoDataAvailable";
    case ResponseErrorEnum::ErrorOperationNotSupported:
        return "ErrorOperationNotSupported";
    case ResponseErrorEnum::ErrorUnknownError:
        return "ErrorUnknownError";
    }

    throw std::out_of_range("No known string conversion for provided enum of type ResponseErrorEnum");
}

ResponseErrorEnum string_to_response_error_enum(const std::string& s) {
    if (s == "NoError") {
        return ResponseErrorEnum::NoError;
    }
    if (s == "ErrorInvalidParameter") {
        return ResponseErrorEnum::ErrorInvalidParameter;
    }
    if (s == "ErrorOutOfRange") {
        return ResponseErrorEnum::ErrorOutOfRange;
    }
    if (s == "ErrorValuesNotApplied") {
        return ResponseErrorEnum::ErrorValuesNotApplied;
    }
    if (s == "ErrorInvalidEVSEIndex") {
        return ResponseErrorEnum::ErrorInvalidEVSEIndex;
    }
    if (s == "ErrorInvalidConnectorIndex") {
        return ResponseErrorEnum::ErrorInvalidConnectorIndex;
    }
    if (s == "ErrorNoDataAvailable") {
        return ResponseErrorEnum::ErrorNoDataAvailable;
    }
    if (s == "ErrorOperationNotSupported") {
        return ResponseErrorEnum::ErrorOperationNotSupported;
    }
    if (s == "ErrorUnknownError") {
        return ResponseErrorEnum::ErrorUnknownError;
    }

    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type ResponseErrorEnum");
}

std::string charge_protocol_enum_to_string(ChargeProtocolEnum e) {
    switch (e) {
    case ChargeProtocolEnum::Unknown:
        return "Unknown";
    case ChargeProtocolEnum::IEC61851:
        return "IEC61851";
    case ChargeProtocolEnum::DIN70121:
        return "DIN70121";
    case ChargeProtocolEnum::ISO15118:
        return "ISO15118";
    case ChargeProtocolEnum::ISO15118_20:
        return "ISO15118_20";
    }

    throw std::out_of_range("No known string conversion for provided enum of type ChargeProtocolEnum");
}

ChargeProtocolEnum string_to_charge_protocol_enum(const std::string& s) {
    if (s == "Unknown") {
        return ChargeProtocolEnum::Unknown;
    }
    if (s == "IEC61851") {
        return ChargeProtocolEnum::IEC61851;
    }
    if (s == "DIN70121") {
        return ChargeProtocolEnum::DIN70121;
    }
    if (s == "ISO15118") {
        return ChargeProtocolEnum::ISO15118;
    }
    if (s == "ISO15118_20") {
        return ChargeProtocolEnum::ISO15118_20;
    }

    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type ChargeProtocolEnum");
}

std::string evsestate_enum_to_string(EVSEStateEnum e) {
    switch (e) {
    case EVSEStateEnum::Unknown:
        return "Unknown";
    case EVSEStateEnum::Unplugged:
        return "Unplugged";
    case EVSEStateEnum::Disabled:
        return "Disabled";
    case EVSEStateEnum::Preparing:
        return "Preparing";
    case EVSEStateEnum::Reserved:
        return "Reserved";
    case EVSEStateEnum::AuthRequired:
        return "AuthRequired";
    case EVSEStateEnum::WaitingForEnergy:
        return "WaitingForEnergy";
    case EVSEStateEnum::ChargingPausedEV:
        return "ChargingPausedEV";
    case EVSEStateEnum::ChargingPausedEVSE:
        return "ChargingPausedEVSE";
    case EVSEStateEnum::Charging:
        return "Charging";
    case EVSEStateEnum::AuthTimeout:
        return "AuthTimeout";
    case EVSEStateEnum::Finished:
        return "Finished";
    case EVSEStateEnum::FinishedEVSE:
        return "FinishedEVSE";
    case EVSEStateEnum::FinishedEV:
        return "FinishedEV";
    case EVSEStateEnum::SwitchingPhases:
        return "SwitchingPhases";
    }

    throw std::out_of_range("No known string conversion for provided enum of type EVSEStateEnum");
}

EVSEStateEnum string_to_evsestate_enum(const std::string& s) {
    if (s == "Unknown") {
        return EVSEStateEnum::Unknown;
    }
    if (s == "Unplugged") {
        return EVSEStateEnum::Unplugged;
    }
    if (s == "Disabled") {
        return EVSEStateEnum::Disabled;
    }
    if (s == "Preparing") {
        return EVSEStateEnum::Preparing;
    }
    if (s == "Reserved") {
        return EVSEStateEnum::Reserved;
    }
    if (s == "AuthRequired") {
        return EVSEStateEnum::AuthRequired;
    }
    if (s == "WaitingForEnergy") {
        return EVSEStateEnum::WaitingForEnergy;
    }
    if (s == "ChargingPausedEV") {
        return EVSEStateEnum::ChargingPausedEV;
    }
    if (s == "ChargingPausedEVSE") {
        return EVSEStateEnum::ChargingPausedEVSE;
    }
    if (s == "Charging") {
        return EVSEStateEnum::Charging;
    }
    if (s == "AuthTimeout") {
        return EVSEStateEnum::AuthTimeout;
    }
    if (s == "Finished") {
        return EVSEStateEnum::Finished;
    }
    if (s == "FinishedEVSE") {
        return EVSEStateEnum::FinishedEVSE;
    }
    if (s == "FinishedEV") {
        return EVSEStateEnum::FinishedEV;
    }
    if (s == "SwitchingPhases") {
        return EVSEStateEnum::SwitchingPhases;
    }

    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type EVSEStateEnum");
}

std::string connector_type_enum_to_string(ConnectorTypeEnum e) {
    switch (e) {
    case ConnectorTypeEnum::cCCS1:
        return "cCCS1";
    case ConnectorTypeEnum::cCCS2:
        return "cCCS2";
    case ConnectorTypeEnum::cG105:
        return "cG105";
    case ConnectorTypeEnum::cTesla:
        return "cTesla";
    case ConnectorTypeEnum::cType1:
        return "cType1";
    case ConnectorTypeEnum::cType2:
        return "cType2";
    case ConnectorTypeEnum::s309_1P_16A:
        return "s309_1P_16A";
    case ConnectorTypeEnum::s309_1P_32A:
        return "s309_1P_32A";
    case ConnectorTypeEnum::s309_3P_16A:
        return "s309_3P_16A";
    case ConnectorTypeEnum::s309_3P_32A:
        return "s309_3P_32A";
    case ConnectorTypeEnum::sBS1361:
        return "sBS1361";
    case ConnectorTypeEnum::sCEE_7_7:
        return "sCEE_7_7";
    case ConnectorTypeEnum::sType2:
        return "sType2";
    case ConnectorTypeEnum::sType3:
        return "sType3";
    case ConnectorTypeEnum::Other1PhMax16A:
        return "Other1PhMax16A";
    case ConnectorTypeEnum::Other1PhOver16A:
        return "Other1PhOver16A";
    case ConnectorTypeEnum::Other3Ph:
        return "Other3Ph";
    case ConnectorTypeEnum::Pan:
        return "Pan";
    case ConnectorTypeEnum::wInductive:
        return "wInductive";
    case ConnectorTypeEnum::wResonant:
        return "wResonant";
    case ConnectorTypeEnum::Undetermined:
        return "Undetermined";
    case ConnectorTypeEnum::Unknown:
        return "Unknown";
    }

    throw std::out_of_range("No known string conversion for provided enum of type ConnectorTypeEnum");
}

ConnectorTypeEnum string_to_connector_type_enum(const std::string& s) {
    if (s == "cCCS1") {
        return ConnectorTypeEnum::cCCS1;
    }
    if (s == "cCCS2") {
        return ConnectorTypeEnum::cCCS2;
    }
    if (s == "cG105") {
        return ConnectorTypeEnum::cG105;
    }
    if (s == "cTesla") {
        return ConnectorTypeEnum::cTesla;
    }
    if (s == "cType1") {
        return ConnectorTypeEnum::cType1;
    }
    if (s == "cType2") {
        return ConnectorTypeEnum::cType2;
    }
    if (s == "s309_1P_16A") {
        return ConnectorTypeEnum::s309_1P_16A;
    }
    if (s == "s309_1P_32A") {
        return ConnectorTypeEnum::s309_1P_32A;
    }
    if (s == "s309_3P_16A") {
        return ConnectorTypeEnum::s309_3P_16A;
    }
    if (s == "s309_3P_32A") {
        return ConnectorTypeEnum::s309_3P_32A;
    }
    if (s == "sBS1361") {
        return ConnectorTypeEnum::sBS1361;
    }
    if (s == "sCEE_7_7") {
        return ConnectorTypeEnum::sCEE_7_7;
    }
    if (s == "sType2") {
        return ConnectorTypeEnum::sType2;
    }
    if (s == "sType3") {
        return ConnectorTypeEnum::sType3;
    }
    if (s == "Other1PhMax16A") {
        return ConnectorTypeEnum::Other1PhMax16A;
    }
    if (s == "Other1PhOver16A") {
        return ConnectorTypeEnum::Other1PhOver16A;
    }
    if (s == "Other3Ph") {
        return ConnectorTypeEnum::Other3Ph;
    }
    if (s == "Pan") {
        return ConnectorTypeEnum::Pan;
    }
    if (s == "wInductive") {
        return ConnectorTypeEnum::wInductive;
    }
    if (s == "wResonant") {
        return ConnectorTypeEnum::wResonant;
    }
    if (s == "Undetermined") {
        return ConnectorTypeEnum::Undetermined;
    }
    if (s == "Unknown") {
        return ConnectorTypeEnum::Unknown;
    }

    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type ConnectorTypeEnum");
}

std::string energy_transfer_mode_enum_to_string(EnergyTransferModeEnum e) {
    switch (e) {
    case EnergyTransferModeEnum::AC_single_phase_core:
        return "AC_single_phase_core";
    case EnergyTransferModeEnum::AC_two_phase:
        return "AC_two_phase";
    case EnergyTransferModeEnum::AC_three_phase_core:
        return "AC_three_phase_core";
    case EnergyTransferModeEnum::DC_core:
        return "DC_core";
    case EnergyTransferModeEnum::DC_extended:
        return "DC_extended";
    case EnergyTransferModeEnum::DC_combo_core:
        return "DC_combo_core";
    case EnergyTransferModeEnum::DC_unique:
        return "DC_unique";
    case EnergyTransferModeEnum::DC:
        return "DC";
    case EnergyTransferModeEnum::AC_BPT:
        return "AC_BPT";
    case EnergyTransferModeEnum::AC_BPT_DER:
        return "AC_BPT_DER";
    case EnergyTransferModeEnum::AC_DER:
        return "AC_DER";
    case EnergyTransferModeEnum::DC_BPT:
        return "DC_BPT";
    case EnergyTransferModeEnum::DC_ACDP:
        return "DC_ACDP";
    case EnergyTransferModeEnum::DC_ACDP_BPT:
        return "DC_ACDP_BPT";
    case EnergyTransferModeEnum::WPT:
        return "WPT";
    }

    throw std::out_of_range("No known string conversion for provided enum of type EnergyTransferModeEnum");
}

EnergyTransferModeEnum string_to_energy_transfer_mode_enum(const std::string& s) {
    if (s == "AC_single_phase_core") {
        return EnergyTransferModeEnum::AC_single_phase_core;
    }
    if (s == "AC_two_phase") {
        return EnergyTransferModeEnum::AC_two_phase;
    }
    if (s == "AC_three_phase_core") {
        return EnergyTransferModeEnum::AC_three_phase_core;
    }
    if (s == "DC_core") {
        return EnergyTransferModeEnum::DC_core;
    }
    if (s == "DC_extended") {
        return EnergyTransferModeEnum::DC_extended;
    }
    if (s == "DC_combo_core") {
        return EnergyTransferModeEnum::DC_combo_core;
    }
    if (s == "DC_unique") {
        return EnergyTransferModeEnum::DC_unique;
    }
    if (s == "DC") {
        return EnergyTransferModeEnum::DC;
    }
    if (s == "AC_BPT") {
        return EnergyTransferModeEnum::AC_BPT;
    }
    if (s == "AC_BPT_DER") {
        return EnergyTransferModeEnum::AC_BPT_DER;
    }
    if (s == "AC_DER") {
        return EnergyTransferModeEnum::AC_DER;
    }
    if (s == "DC_BPT") {
        return EnergyTransferModeEnum::DC_BPT;
    }
    if (s == "DC_ACDP") {
        return EnergyTransferModeEnum::DC_ACDP;
    }
    if (s == "DC_ACDP_BPT") {
        return EnergyTransferModeEnum::DC_ACDP_BPT;
    }
    if (s == "WPT") {
        return EnergyTransferModeEnum::WPT;
    }

    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type EnergyTransferModeEnum");
}

std::string severity_to_string(Severity e) {
    switch (e) {
    case Severity::High:
        return "High";
    case Severity::Medium:
        return "Medium";
    case Severity::Low:
        return "Low";
    }

    throw std::out_of_range("No known string conversion for provided enum of type Severity");
}

Severity string_to_severity(const std::string& s) {
    if (s == "High") {
        return Severity::High;
    }
    if (s == "Medium") {
        return Severity::Medium;
    }
    if (s == "Low") {
        return Severity::Low;
    }

    throw std::out_of_range("Provided string " + s + " could not be converted to enum of type Severity");
}

std::string serialize(ResponseErrorEnum val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ChargeProtocolEnum val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(EVSEStateEnum val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ConnectorTypeEnum val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(EnergyTransferModeEnum val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(Severity val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ImplementationIdentifier const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ChargerInfoObj const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ErrorObj const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ConnectorInfoObj const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ACChargeParametersObj const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(DCChargeParametersObj const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ACChargeStatusObj const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(DCChargeStatusObj const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(DisplayParametersObj const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(HardwareCapabilitiesObj const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(EVSEInfoObj const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(EVSEStatusObj const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(Current_A const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(Energy_Wh_import const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(Energy_Wh_export const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(Frequency_Hz const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(Power_W const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(Voltage_V const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(MeterDataObj const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(HelloResObj const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ChargePointGetEVSEInfosResObj const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ChargePointGetActiveErrorsResObj const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(EVSEGetInfoResObj const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(EVSEGetStatusResObj const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(EVSEGetHardwareCapabilitiesResObj const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(EVSEGetMeterDataResObj const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ErrorResObj const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ChargePointActiveErrorsChangedObj const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(EVSEHardwareCapabilitiesChangedObj const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(EVSEStatusChangedObj const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(EVSEMeterDataChangedObj const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

template <> ResponseErrorEnum deserialize(std::string const& s) {
    auto data = json::parse(s);
    ResponseErrorEnum result = data;
    return result;
}

template <> ChargeProtocolEnum deserialize(std::string const& s) {
    auto data = json::parse(s);
    ChargeProtocolEnum result = data;
    return result;
}

template <> EVSEStateEnum deserialize(std::string const& s) {
    auto data = json::parse(s);
    EVSEStateEnum result = data;
    return result;
}

template <> ConnectorTypeEnum deserialize(std::string const& s) {
    auto data = json::parse(s);
    ConnectorTypeEnum result = data;
    return result;
}

template <> EnergyTransferModeEnum deserialize(std::string const& s) {
    auto data = json::parse(s);
    EnergyTransferModeEnum result = data;
    return result;
}

template <> Severity deserialize(std::string const& s) {
    auto data = json::parse(s);
    Severity result = data;
    return result;
}

template <> ImplementationIdentifier deserialize(std::string const& s) {
    auto data = json::parse(s);
    ImplementationIdentifier result = data;
    return result;
}

template <> ChargerInfoObj deserialize(std::string const& s) {
    auto data = json::parse(s);
    ChargerInfoObj result = data;
    return result;
}

template <> ErrorObj deserialize(std::string const& s) {
    auto data = json::parse(s);
    ErrorObj result = data;
    return result;
}

template <> ConnectorInfoObj deserialize(std::string const& s) {
    auto data = json::parse(s);
    ConnectorInfoObj result = data;
    return result;
}

template <> ACChargeParametersObj deserialize(std::string const& s) {
    auto data = json::parse(s);
    ACChargeParametersObj result = data;
    return result;
}

template <> DCChargeParametersObj deserialize(std::string const& s) {
    auto data = json::parse(s);
    DCChargeParametersObj result = data;
    return result;
}

template <> ACChargeStatusObj deserialize(std::string const& s) {
    auto data = json::parse(s);
    ACChargeStatusObj result = data;
    return result;
}

template <> DCChargeStatusObj deserialize(std::string const& s) {
    auto data = json::parse(s);
    DCChargeStatusObj result = data;
    return result;
}

template <> DisplayParametersObj deserialize(std::string const& s) {
    auto data = json::parse(s);
    DisplayParametersObj result = data;
    return result;
}

template <> HardwareCapabilitiesObj deserialize(std::string const& s) {
    auto data = json::parse(s);
    HardwareCapabilitiesObj result = data;
    return result;
}

template <> EVSEInfoObj deserialize(std::string const& s) {
    auto data = json::parse(s);
    EVSEInfoObj result = data;
    return result;
}

template <> EVSEStatusObj deserialize(std::string const& s) {
    auto data = json::parse(s);
    EVSEStatusObj result = data;
    return result;
}

template <> Current_A deserialize(std::string const& s) {
    auto data = json::parse(s);
    Current_A result = data;
    return result;
}

template <> Energy_Wh_import deserialize(std::string const& s) {
    auto data = json::parse(s);
    Energy_Wh_import result = data;
    return result;
}

template <> Energy_Wh_export deserialize(std::string const& s) {
    auto data = json::parse(s);
    Energy_Wh_export result = data;
    return result;
}

template <> Frequency_Hz deserialize(std::string const& s) {
    auto data = json::parse(s);
    Frequency_Hz result = data;
    return result;
}

template <> Power_W deserialize(std::string const& s) {
    auto data = json::parse(s);
    Power_W result = data;
    return result;
}

template <> Voltage_V deserialize(std::string const& s) {
    auto data = json::parse(s);
    Voltage_V result = data;
    return result;
}

template <> MeterDataObj deserialize(std::string const& s) {
    auto data = json::parse(s);
    MeterDataObj result = data;
    return result;
}

template <> HelloResObj deserialize(std::string const& s) {
    auto data = json::parse(s);
    HelloResObj result = data;
    return result;
}

template <> ChargePointGetEVSEInfosResObj deserialize(std::string const& s) {
    auto data = json::parse(s);
    ChargePointGetEVSEInfosResObj result = data;
    return result;
}

template <> ChargePointGetActiveErrorsResObj deserialize(std::string const& s) {
    auto data = json::parse(s);
    ChargePointGetActiveErrorsResObj result = data;
    return result;
}

template <> EVSEGetInfoResObj deserialize(std::string const& s) {
    auto data = json::parse(s);
    EVSEGetInfoResObj result = data;
    return result;
}

template <> EVSEGetStatusResObj deserialize(std::string const& s) {
    auto data = json::parse(s);
    EVSEGetStatusResObj result = data;
    return result;
}

template <> EVSEGetHardwareCapabilitiesResObj deserialize(std::string const& s) {
    auto data = json::parse(s);
    EVSEGetHardwareCapabilitiesResObj result = data;
    return result;
}

template <> EVSEGetMeterDataResObj deserialize(std::string const& s) {
    auto data = json::parse(s);
    EVSEGetMeterDataResObj result = data;
    return result;
}

template <> ErrorResObj deserialize(std::string const& s) {
    auto data = json::parse(s);
    ErrorResObj result = data;
    return result;
}

template <> ChargePointActiveErrorsChangedObj deserialize(std::string const& s) {
    auto data = json::parse(s);
    ChargePointActiveErrorsChangedObj result = data;
    return result;
}

template <> EVSEHardwareCapabilitiesChangedObj deserialize(std::string const& s) {
    auto data = json::parse(s);
    EVSEHardwareCapabilitiesChangedObj result = data;
    return result;
}

template <> EVSEStatusChangedObj deserialize(std::string const& s) {
    auto data = json::parse(s);
    EVSEStatusChangedObj result = data;
    return result;
}

template <> EVSEMeterDataChangedObj deserialize(std::string const& s) {
    auto data = json::parse(s);
    EVSEMeterDataChangedObj result = data;
    return result;
}

std::ostream& operator<<(std::ostream& os, ResponseErrorEnum const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ChargeProtocolEnum const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, EVSEStateEnum const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ConnectorTypeEnum const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, EnergyTransferModeEnum const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, Severity const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ImplementationIdentifier const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ChargerInfoObj const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ErrorObj const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ConnectorInfoObj const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ACChargeParametersObj const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, DCChargeParametersObj const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ACChargeStatusObj const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, DCChargeStatusObj const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, DisplayParametersObj const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, HardwareCapabilitiesObj const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, EVSEInfoObj const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, EVSEStatusObj const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, Current_A const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, Energy_Wh_import const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, Energy_Wh_export const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, Frequency_Hz const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, Power_W const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, Voltage_V const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, MeterDataObj const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, HelloResObj const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ChargePointGetEVSEInfosResObj const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ChargePointGetActiveErrorsResObj const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, EVSEGetInfoResObj const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, EVSEGetStatusResObj const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, EVSEGetHardwareCapabilitiesResObj const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, EVSEGetMeterDataResObj const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ErrorResObj const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ChargePointActiveErrorsChangedObj const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, EVSEHardwareCapabilitiesChangedObj const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, EVSEStatusChangedObj const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, EVSEMeterDataChangedObj const& val) {
    os << serialize(val);
    return os;
}

} // namespace everest::lib::API::V1_0::types::json_rpc_api
