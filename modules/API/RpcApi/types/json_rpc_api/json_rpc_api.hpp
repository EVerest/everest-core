// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef TYPES_JSON_RPC_API_TYPES_HPP
#define TYPES_JSON_RPC_API_TYPES_HPP

//
// AUTO GENERATED - DO NOT EDIT!
// template version 5
//

#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

#include <nlohmann/json.hpp>


using nlohmann::json;


// enums of json_rpc_api

namespace types {
namespace json_rpc_api {
enum class ResponseErrorEnum
{
    NoError,
    ErrorInvalidParameter,
    ErrorOutOfRange,
    ErrorValuesNotApplied,
    ErrorInvalidEVSEIndex,
    ErrorInvalidConnectorIndex,
    ErrorNoDataAvailable,
    ErrorOperationNotSupported,
    ErrorUnknownError,
};

/// \brief Converts the given ResponseErrorEnum \p e to human readable string
/// \returns a string representation of the ResponseErrorEnum
inline std::string response_error_enum_to_string(ResponseErrorEnum e) {
    switch (e) {
        case ResponseErrorEnum::NoError: return "NoError";
        case ResponseErrorEnum::ErrorInvalidParameter: return "ErrorInvalidParameter";
        case ResponseErrorEnum::ErrorOutOfRange: return "ErrorOutOfRange";
        case ResponseErrorEnum::ErrorValuesNotApplied: return "ErrorValuesNotApplied";
        case ResponseErrorEnum::ErrorInvalidEVSEIndex: return "ErrorInvalidEVSEIndex";
        case ResponseErrorEnum::ErrorInvalidConnectorIndex: return "ErrorInvalidConnectorIndex";
        case ResponseErrorEnum::ErrorNoDataAvailable: return "ErrorNoDataAvailable";
        case ResponseErrorEnum::ErrorOperationNotSupported: return "ErrorOperationNotSupported";
        case ResponseErrorEnum::ErrorUnknownError: return "ErrorUnknownError";
    }

    throw std::out_of_range("No known string conversion for provided enum of type ResponseErrorEnum");
}

/// \brief Converts the given std::string \p s to ResponseErrorEnum
/// \returns a ResponseErrorEnum from a string representation
inline ResponseErrorEnum string_to_response_error_enum(const std::string& s) {
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

/// \brief Writes the string representation of the given ResponseErrorEnum \p response_error_enum to the given output stream \p os
/// \returns an output stream with the ResponseErrorEnum written to
inline std::ostream& operator<<(std::ostream& os, const types::json_rpc_api::ResponseErrorEnum& response_error_enum) {
    os << types::json_rpc_api::response_error_enum_to_string(response_error_enum);
    return os;
}

} // namespace json_rpc_api
} // namespace types

namespace types {
namespace json_rpc_api {
enum class ChargeProtocolEnum
{
    Unknown,
    IEC61851,
    DIN70121,
    ISO15118,
    ISO15118_20,
};

/// \brief Converts the given ChargeProtocolEnum \p e to human readable string
/// \returns a string representation of the ChargeProtocolEnum
inline std::string charge_protocol_enum_to_string(ChargeProtocolEnum e) {
    switch (e) {
        case ChargeProtocolEnum::Unknown: return "Unknown";
        case ChargeProtocolEnum::IEC61851: return "IEC61851";
        case ChargeProtocolEnum::DIN70121: return "DIN70121";
        case ChargeProtocolEnum::ISO15118: return "ISO15118";
        case ChargeProtocolEnum::ISO15118_20: return "ISO15118_20";
    }

    throw std::out_of_range("No known string conversion for provided enum of type ChargeProtocolEnum");
}

/// \brief Converts the given std::string \p s to ChargeProtocolEnum
/// \returns a ChargeProtocolEnum from a string representation
inline ChargeProtocolEnum string_to_charge_protocol_enum(const std::string& s) {
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

/// \brief Writes the string representation of the given ChargeProtocolEnum \p charge_protocol_enum to the given output stream \p os
/// \returns an output stream with the ChargeProtocolEnum written to
inline std::ostream& operator<<(std::ostream& os, const types::json_rpc_api::ChargeProtocolEnum& charge_protocol_enum) {
    os << types::json_rpc_api::charge_protocol_enum_to_string(charge_protocol_enum);
    return os;
}

} // namespace json_rpc_api
} // namespace types

namespace types {
namespace json_rpc_api {
enum class EVSEStateEnum
{
    Unknown,
    Unplugged,
    Disabled,
    Preparing,
    Reserved,
    AuthRequired,
    WaitingForEnergy,
    ChargingPausedEV,
    ChargingPausedEVSE,
    Charging,
    AuthTimeout,
    Finished,
    FinishedEVSE,
    FinishedEV,
    SwitchingPhases,
};

/// \brief Converts the given EVSEStateEnum \p e to human readable string
/// \returns a string representation of the EVSEStateEnum
inline std::string evsestate_enum_to_string(EVSEStateEnum e) {
    switch (e) {
        case EVSEStateEnum::Unknown: return "Unknown";
        case EVSEStateEnum::Unplugged: return "Unplugged";
        case EVSEStateEnum::Disabled: return "Disabled";
        case EVSEStateEnum::Preparing: return "Preparing";
        case EVSEStateEnum::Reserved: return "Reserved";
        case EVSEStateEnum::AuthRequired: return "AuthRequired";
        case EVSEStateEnum::WaitingForEnergy: return "WaitingForEnergy";
        case EVSEStateEnum::ChargingPausedEV: return "ChargingPausedEV";
        case EVSEStateEnum::ChargingPausedEVSE: return "ChargingPausedEVSE";
        case EVSEStateEnum::Charging: return "Charging";
        case EVSEStateEnum::AuthTimeout: return "AuthTimeout";
        case EVSEStateEnum::Finished: return "Finished";
        case EVSEStateEnum::FinishedEVSE: return "FinishedEVSE";
        case EVSEStateEnum::FinishedEV: return "FinishedEV";
        case EVSEStateEnum::SwitchingPhases: return "SwitchingPhases";
    }

    throw std::out_of_range("No known string conversion for provided enum of type EVSEStateEnum");
}

/// \brief Converts the given std::string \p s to EVSEStateEnum
/// \returns a EVSEStateEnum from a string representation
inline EVSEStateEnum string_to_evsestate_enum(const std::string& s) {
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

/// \brief Writes the string representation of the given EVSEStateEnum \p evsestate_enum to the given output stream \p os
/// \returns an output stream with the EVSEStateEnum written to
inline std::ostream& operator<<(std::ostream& os, const types::json_rpc_api::EVSEStateEnum& evsestate_enum) {
    os << types::json_rpc_api::evsestate_enum_to_string(evsestate_enum);
    return os;
}

} // namespace json_rpc_api
} // namespace types

namespace types {
namespace json_rpc_api {
enum class ConnectorTypeEnum
{
    cCCS1,
    cCCS2,
    cG105,
    cTesla,
    cType1,
    cType2,
    s309_1P_16A,
    s309_1P_32A,
    s309_3P_16A,
    s309_3P_32A,
    sBS1361,
    sCEE_7_7,
    sType2,
    sType3,
    Other1PhMax16A,
    Other1PhOver16A,
    Other3Ph,
    Pan,
    wInductive,
    wResonant,
    Undetermined,
    Unknown,
};

/// \brief Converts the given ConnectorTypeEnum \p e to human readable string
/// \returns a string representation of the ConnectorTypeEnum
inline std::string connector_type_enum_to_string(ConnectorTypeEnum e) {
    switch (e) {
        case ConnectorTypeEnum::cCCS1: return "cCCS1";
        case ConnectorTypeEnum::cCCS2: return "cCCS2";
        case ConnectorTypeEnum::cG105: return "cG105";
        case ConnectorTypeEnum::cTesla: return "cTesla";
        case ConnectorTypeEnum::cType1: return "cType1";
        case ConnectorTypeEnum::cType2: return "cType2";
        case ConnectorTypeEnum::s309_1P_16A: return "s309_1P_16A";
        case ConnectorTypeEnum::s309_1P_32A: return "s309_1P_32A";
        case ConnectorTypeEnum::s309_3P_16A: return "s309_3P_16A";
        case ConnectorTypeEnum::s309_3P_32A: return "s309_3P_32A";
        case ConnectorTypeEnum::sBS1361: return "sBS1361";
        case ConnectorTypeEnum::sCEE_7_7: return "sCEE_7_7";
        case ConnectorTypeEnum::sType2: return "sType2";
        case ConnectorTypeEnum::sType3: return "sType3";
        case ConnectorTypeEnum::Other1PhMax16A: return "Other1PhMax16A";
        case ConnectorTypeEnum::Other1PhOver16A: return "Other1PhOver16A";
        case ConnectorTypeEnum::Other3Ph: return "Other3Ph";
        case ConnectorTypeEnum::Pan: return "Pan";
        case ConnectorTypeEnum::wInductive: return "wInductive";
        case ConnectorTypeEnum::wResonant: return "wResonant";
        case ConnectorTypeEnum::Undetermined: return "Undetermined";
        case ConnectorTypeEnum::Unknown: return "Unknown";
    }

    throw std::out_of_range("No known string conversion for provided enum of type ConnectorTypeEnum");
}

/// \brief Converts the given std::string \p s to ConnectorTypeEnum
/// \returns a ConnectorTypeEnum from a string representation
inline ConnectorTypeEnum string_to_connector_type_enum(const std::string& s) {
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

/// \brief Writes the string representation of the given ConnectorTypeEnum \p connector_type_enum to the given output stream \p os
/// \returns an output stream with the ConnectorTypeEnum written to
inline std::ostream& operator<<(std::ostream& os, const types::json_rpc_api::ConnectorTypeEnum& connector_type_enum) {
    os << types::json_rpc_api::connector_type_enum_to_string(connector_type_enum);
    return os;
}

} // namespace json_rpc_api
} // namespace types

namespace types {
namespace json_rpc_api {
enum class EnergyTransferModeEnum
{
    AC_single_phase_core,
    AC_two_phase,
    AC_three_phase_core,
    DC_core,
    DC_extended,
    DC_combo_core,
    DC_unique,
    DC,
    AC_BPT,
    AC_BPT_DER,
    AC_DER,
    DC_BPT,
    DC_ACDP,
    DC_ACDP_BPT,
    WPT,
};

/// \brief Converts the given EnergyTransferModeEnum \p e to human readable string
/// \returns a string representation of the EnergyTransferModeEnum
inline std::string energy_transfer_mode_enum_to_string(EnergyTransferModeEnum e) {
    switch (e) {
        case EnergyTransferModeEnum::AC_single_phase_core: return "AC_single_phase_core";
        case EnergyTransferModeEnum::AC_two_phase: return "AC_two_phase";
        case EnergyTransferModeEnum::AC_three_phase_core: return "AC_three_phase_core";
        case EnergyTransferModeEnum::DC_core: return "DC_core";
        case EnergyTransferModeEnum::DC_extended: return "DC_extended";
        case EnergyTransferModeEnum::DC_combo_core: return "DC_combo_core";
        case EnergyTransferModeEnum::DC_unique: return "DC_unique";
        case EnergyTransferModeEnum::DC: return "DC";
        case EnergyTransferModeEnum::AC_BPT: return "AC_BPT";
        case EnergyTransferModeEnum::AC_BPT_DER: return "AC_BPT_DER";
        case EnergyTransferModeEnum::AC_DER: return "AC_DER";
        case EnergyTransferModeEnum::DC_BPT: return "DC_BPT";
        case EnergyTransferModeEnum::DC_ACDP: return "DC_ACDP";
        case EnergyTransferModeEnum::DC_ACDP_BPT: return "DC_ACDP_BPT";
        case EnergyTransferModeEnum::WPT: return "WPT";
    }

    throw std::out_of_range("No known string conversion for provided enum of type EnergyTransferModeEnum");
}

/// \brief Converts the given std::string \p s to EnergyTransferModeEnum
/// \returns a EnergyTransferModeEnum from a string representation
inline EnergyTransferModeEnum string_to_energy_transfer_mode_enum(const std::string& s) {
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

/// \brief Writes the string representation of the given EnergyTransferModeEnum \p energy_transfer_mode_enum to the given output stream \p os
/// \returns an output stream with the EnergyTransferModeEnum written to
inline std::ostream& operator<<(std::ostream& os, const types::json_rpc_api::EnergyTransferModeEnum& energy_transfer_mode_enum) {
    os << types::json_rpc_api::energy_transfer_mode_enum_to_string(energy_transfer_mode_enum);
    return os;
}

} // namespace json_rpc_api
} // namespace types

namespace types {
namespace json_rpc_api {
enum class Severity
{
    High,
    Medium,
    Low,
};

/// \brief Converts the given Severity \p e to human readable string
/// \returns a string representation of the Severity
inline std::string severity_to_string(Severity e) {
    switch (e) {
        case Severity::High: return "High";
        case Severity::Medium: return "Medium";
        case Severity::Low: return "Low";
    }

    throw std::out_of_range("No known string conversion for provided enum of type Severity");
}

/// \brief Converts the given std::string \p s to Severity
/// \returns a Severity from a string representation
inline Severity string_to_severity(const std::string& s) {
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

/// \brief Writes the string representation of the given Severity \p severity to the given output stream \p os
/// \returns an output stream with the Severity written to
inline std::ostream& operator<<(std::ostream& os, const types::json_rpc_api::Severity& severity) {
    os << types::json_rpc_api::severity_to_string(severity);
    return os;
}

} // namespace json_rpc_api
} // namespace types


// types of json_rpc_api
namespace types {
namespace json_rpc_api {

struct ImplementationIdentifier {
    std::string module_id; ///< TODO: description
    std::string implementation_id; ///< TODO: description
    std::optional<int32_t> evse_index; ///< TODO: description
    std::optional<int32_t> connector_index; ///< TODO: description

    /// \brief Conversion from a given ImplementationIdentifier \p k to a given json object \p j
    friend void to_json(json& j, const ImplementationIdentifier& k) {
        // the required parts of the type
        j = json{
            {"module_id", k.module_id},
            {"implementation_id", k.implementation_id},
        };
        // the optional parts of the type
        if (k.evse_index) {
            j["evse_index"] = k.evse_index.value();
        }
        if (k.connector_index) {
            j["connector_index"] = k.connector_index.value();
        }
    }

    /// \brief Conversion from a given json object \p j to a given ImplementationIdentifier \p k
    friend void from_json(const json& j, ImplementationIdentifier& k) {
        // the required parts of the type
        k.module_id = j.at("module_id");
        k.implementation_id = j.at("implementation_id");

        // the optional parts of the type
        if (j.contains("evse_index")) {
            k.evse_index.emplace(j.at("evse_index"));
        }
        if (j.contains("connector_index")) {
            k.connector_index.emplace(j.at("connector_index"));
        }
    }

    /// \brief Compares objects of type ImplementationIdentifier for equality
    friend constexpr bool operator==(const ImplementationIdentifier& k, const ImplementationIdentifier& l) {
        const auto& lhs_tuple = std::tie(
            k.module_id,
            k.implementation_id,
            k.evse_index,
            k.connector_index
        );
        const auto& rhs_tuple = std::tie(
            l.module_id,
            l.implementation_id,
            l.evse_index,
            l.connector_index
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type ImplementationIdentifier for inequality
    friend constexpr bool operator!=(const ImplementationIdentifier& k, const ImplementationIdentifier& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given ImplementationIdentifier \p k to the given output stream \p os
    /// \returns an output stream with the ImplementationIdentifier written to
    friend std::ostream& operator<<(std::ostream& os, const ImplementationIdentifier& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct ChargerInfoObj {
    std::string vendor; ///< EVSE vendor
    std::string model; ///< EVSE model
    std::string serial; ///< EVSE serial number
    std::string firmware_version; ///< EVSE firmware version
    std::optional<std::string> friendly_name; ///< EVSE friendly name
    std::optional<std::string> manufacturer; ///< EVSE manufacturer
    std::optional<std::string> manufacturer_url; ///< Manufacturer's URL
    std::optional<std::string> model_url; ///< EVSE model's URL
    std::optional<std::string> model_no; ///< EVSE model number
    std::optional<std::string> revision; ///< EVSE model revision
    std::optional<std::string> board_revision; ///< EVSE board revision

    /// \brief Conversion from a given ChargerInfoObj \p k to a given json object \p j
    friend void to_json(json& j, const ChargerInfoObj& k) {
        // the required parts of the type
        j = json{
            {"vendor", k.vendor},
            {"model", k.model},
            {"serial", k.serial},
            {"firmware_version", k.firmware_version},
        };
        // the optional parts of the type
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

    /// \brief Conversion from a given json object \p j to a given ChargerInfoObj \p k
    friend void from_json(const json& j, ChargerInfoObj& k) {
        // the required parts of the type
        k.vendor = j.at("vendor");
        k.model = j.at("model");
        k.serial = j.at("serial");
        k.firmware_version = j.at("firmware_version");

        // the optional parts of the type
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

    /// \brief Compares objects of type ChargerInfoObj for equality
    friend constexpr bool operator==(const ChargerInfoObj& k, const ChargerInfoObj& l) {
        const auto& lhs_tuple = std::tie(
            k.vendor,
            k.model,
            k.serial,
            k.firmware_version,
            k.friendly_name,
            k.manufacturer,
            k.manufacturer_url,
            k.model_url,
            k.model_no,
            k.revision,
            k.board_revision
        );
        const auto& rhs_tuple = std::tie(
            l.vendor,
            l.model,
            l.serial,
            l.firmware_version,
            l.friendly_name,
            l.manufacturer,
            l.manufacturer_url,
            l.model_url,
            l.model_no,
            l.revision,
            l.board_revision
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type ChargerInfoObj for inequality
    friend constexpr bool operator!=(const ChargerInfoObj& k, const ChargerInfoObj& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given ChargerInfoObj \p k to the given output stream \p os
    /// \returns an output stream with the ChargerInfoObj written to
    friend std::ostream& operator<<(std::ostream& os, const ChargerInfoObj& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct ErrorObj {
    std::string type; ///< TODO: description
    std::string description; ///< TODO: description
    std::string message; ///< TODO: description
    types::json_rpc_api::Severity severity; ///< TODO: description
    types::json_rpc_api::ImplementationIdentifier origin; ///< TODO: description
    std::string timestamp; ///< TODO: description
    std::string uuid; ///< TODO: description
    std::optional<std::string> sub_type; ///< TODO: description

    /// \brief Conversion from a given ErrorObj \p k to a given json object \p j
    friend void to_json(json& j, const ErrorObj& k) {
        // the required parts of the type
        j = json{
            {"type", k.type},
            {"description", k.description},
            {"message", k.message},
            {"severity", types::json_rpc_api::severity_to_string(k.severity)},
            {"origin", k.origin},
            {"timestamp", k.timestamp},
            {"uuid", k.uuid},
        };
        // the optional parts of the type
        if (k.sub_type) {
            j["sub_type"] = k.sub_type.value();
        }
    }

    /// \brief Conversion from a given json object \p j to a given ErrorObj \p k
    friend void from_json(const json& j, ErrorObj& k) {
        // the required parts of the type
        k.type = j.at("type");
        k.description = j.at("description");
        k.message = j.at("message");
        k.severity = types::json_rpc_api::string_to_severity(j.at("severity"));
        k.origin = j.at("origin");
        k.timestamp = j.at("timestamp");
        k.uuid = j.at("uuid");

        // the optional parts of the type
        if (j.contains("sub_type")) {
            k.sub_type.emplace(j.at("sub_type"));
        }
    }

    /// \brief Compares objects of type ErrorObj for equality
    friend constexpr bool operator==(const ErrorObj& k, const ErrorObj& l) {
        const auto& lhs_tuple = std::tie(
            k.type,
            k.description,
            k.message,
            k.severity,
            k.origin,
            k.timestamp,
            k.uuid,
            k.sub_type
        );
        const auto& rhs_tuple = std::tie(
            l.type,
            l.description,
            l.message,
            l.severity,
            l.origin,
            l.timestamp,
            l.uuid,
            l.sub_type
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type ErrorObj for inequality
    friend constexpr bool operator!=(const ErrorObj& k, const ErrorObj& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given ErrorObj \p k to the given output stream \p os
    /// \returns an output stream with the ErrorObj written to
    friend std::ostream& operator<<(std::ostream& os, const ErrorObj& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct ConnectorInfoObj {
    int32_t index; ///< Unique identifier
    types::json_rpc_api::ConnectorTypeEnum type; ///< Connector type
    std::optional<std::string> description; ///< Description

    /// \brief Conversion from a given ConnectorInfoObj \p k to a given json object \p j
    friend void to_json(json& j, const ConnectorInfoObj& k) {
        // the required parts of the type
        j = json{
            {"index", k.index},
            {"type", types::json_rpc_api::connector_type_enum_to_string(k.type)},
        };
        // the optional parts of the type
        if (k.description) {
            j["description"] = k.description.value();
        }
    }

    /// \brief Conversion from a given json object \p j to a given ConnectorInfoObj \p k
    friend void from_json(const json& j, ConnectorInfoObj& k) {
        // the required parts of the type
        k.index = j.at("index");
        k.type = types::json_rpc_api::string_to_connector_type_enum(j.at("type"));

        // the optional parts of the type
        if (j.contains("description")) {
            k.description.emplace(j.at("description"));
        }
    }

    /// \brief Compares objects of type ConnectorInfoObj for equality
    friend constexpr bool operator==(const ConnectorInfoObj& k, const ConnectorInfoObj& l) {
        const auto& lhs_tuple = std::tie(
            k.index,
            k.type,
            k.description
        );
        const auto& rhs_tuple = std::tie(
            l.index,
            l.type,
            l.description
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type ConnectorInfoObj for inequality
    friend constexpr bool operator!=(const ConnectorInfoObj& k, const ConnectorInfoObj& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given ConnectorInfoObj \p k to the given output stream \p os
    /// \returns an output stream with the ConnectorInfoObj written to
    friend std::ostream& operator<<(std::ostream& os, const ConnectorInfoObj& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct ACChargeParametersObj {
    float evse_max_current; ///< evse_max_current
    int32_t evse_max_phase_count; ///< evse_max_phase_count
    float evse_maximum_charge_power; ///< evse_maximum_charge_power
    float evse_minimum_charge_power; ///< evse_minimum_charge_power
    float evse_nominal_frequency; ///< evse_nominal_frequency
    std::optional<float> evse_nominal_voltage; ///< evse_nominal_voltage
    std::optional<float> evse_maximum_discharge_power; ///< evse_maximum_discharge_power
    std::optional<float> evse_minimum_discharge_power; ///< evse_minimum_discharge_power

    /// \brief Conversion from a given ACChargeParametersObj \p k to a given json object \p j
    friend void to_json(json& j, const ACChargeParametersObj& k) {
        // the required parts of the type
        j = json{
            {"evse_max_current", k.evse_max_current},
            {"evse_max_phase_count", k.evse_max_phase_count},
            {"evse_maximum_charge_power", k.evse_maximum_charge_power},
            {"evse_minimum_charge_power", k.evse_minimum_charge_power},
            {"evse_nominal_frequency", k.evse_nominal_frequency},
        };
        // the optional parts of the type
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

    /// \brief Conversion from a given json object \p j to a given ACChargeParametersObj \p k
    friend void from_json(const json& j, ACChargeParametersObj& k) {
        // the required parts of the type
        k.evse_max_current = j.at("evse_max_current");
        k.evse_max_phase_count = j.at("evse_max_phase_count");
        k.evse_maximum_charge_power = j.at("evse_maximum_charge_power");
        k.evse_minimum_charge_power = j.at("evse_minimum_charge_power");
        k.evse_nominal_frequency = j.at("evse_nominal_frequency");

        // the optional parts of the type
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

    /// \brief Compares objects of type ACChargeParametersObj for equality
    friend constexpr bool operator==(const ACChargeParametersObj& k, const ACChargeParametersObj& l) {
        const auto& lhs_tuple = std::tie(
            k.evse_max_current,
            k.evse_max_phase_count,
            k.evse_maximum_charge_power,
            k.evse_minimum_charge_power,
            k.evse_nominal_frequency,
            k.evse_nominal_voltage,
            k.evse_maximum_discharge_power,
            k.evse_minimum_discharge_power
        );
        const auto& rhs_tuple = std::tie(
            l.evse_max_current,
            l.evse_max_phase_count,
            l.evse_maximum_charge_power,
            l.evse_minimum_charge_power,
            l.evse_nominal_frequency,
            l.evse_nominal_voltage,
            l.evse_maximum_discharge_power,
            l.evse_minimum_discharge_power
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type ACChargeParametersObj for inequality
    friend constexpr bool operator!=(const ACChargeParametersObj& k, const ACChargeParametersObj& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given ACChargeParametersObj \p k to the given output stream \p os
    /// \returns an output stream with the ACChargeParametersObj written to
    friend std::ostream& operator<<(std::ostream& os, const ACChargeParametersObj& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct DCChargeParametersObj {
    float evse_maximum_charge_current; ///< evse_maximum_charge_current
    float evse_maximum_charge_power; ///< evse_maximum_charge_power
    float evse_maximum_voltage; ///< evse_maximum_voltage
    float evse_minimum_charge_current; ///< evse_minimum_charge_current
    float evse_minimum_charge_power; ///< evse_minimum_charge_power
    float evse_minimum_voltage; ///< evse_minimum_voltage
    std::optional<float> evse_energy_to_be_delivered; ///< evse_energy_to_be_delivered
    std::optional<float> evse_maximum_discharge_current; ///< evse_maximum_discharge_current
    std::optional<float> evse_maximum_discharge_power; ///< evse_maximum_discharge_power
    std::optional<float> evse_minimum_discharge_current; ///< evse_minimum_discharge_current
    std::optional<float> evse_minimum_discharge_power; ///< evse_minimum_discharge_power

    /// \brief Conversion from a given DCChargeParametersObj \p k to a given json object \p j
    friend void to_json(json& j, const DCChargeParametersObj& k) {
        // the required parts of the type
        j = json{
            {"evse_maximum_charge_current", k.evse_maximum_charge_current},
            {"evse_maximum_charge_power", k.evse_maximum_charge_power},
            {"evse_maximum_voltage", k.evse_maximum_voltage},
            {"evse_minimum_charge_current", k.evse_minimum_charge_current},
            {"evse_minimum_charge_power", k.evse_minimum_charge_power},
            {"evse_minimum_voltage", k.evse_minimum_voltage},
        };
        // the optional parts of the type
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

    /// \brief Conversion from a given json object \p j to a given DCChargeParametersObj \p k
    friend void from_json(const json& j, DCChargeParametersObj& k) {
        // the required parts of the type
        k.evse_maximum_charge_current = j.at("evse_maximum_charge_current");
        k.evse_maximum_charge_power = j.at("evse_maximum_charge_power");
        k.evse_maximum_voltage = j.at("evse_maximum_voltage");
        k.evse_minimum_charge_current = j.at("evse_minimum_charge_current");
        k.evse_minimum_charge_power = j.at("evse_minimum_charge_power");
        k.evse_minimum_voltage = j.at("evse_minimum_voltage");

        // the optional parts of the type
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

    /// \brief Compares objects of type DCChargeParametersObj for equality
    friend constexpr bool operator==(const DCChargeParametersObj& k, const DCChargeParametersObj& l) {
        const auto& lhs_tuple = std::tie(
            k.evse_maximum_charge_current,
            k.evse_maximum_charge_power,
            k.evse_maximum_voltage,
            k.evse_minimum_charge_current,
            k.evse_minimum_charge_power,
            k.evse_minimum_voltage,
            k.evse_energy_to_be_delivered,
            k.evse_maximum_discharge_current,
            k.evse_maximum_discharge_power,
            k.evse_minimum_discharge_current,
            k.evse_minimum_discharge_power
        );
        const auto& rhs_tuple = std::tie(
            l.evse_maximum_charge_current,
            l.evse_maximum_charge_power,
            l.evse_maximum_voltage,
            l.evse_minimum_charge_current,
            l.evse_minimum_charge_power,
            l.evse_minimum_voltage,
            l.evse_energy_to_be_delivered,
            l.evse_maximum_discharge_current,
            l.evse_maximum_discharge_power,
            l.evse_minimum_discharge_current,
            l.evse_minimum_discharge_power
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type DCChargeParametersObj for inequality
    friend constexpr bool operator!=(const DCChargeParametersObj& k, const DCChargeParametersObj& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given DCChargeParametersObj \p k to the given output stream \p os
    /// \returns an output stream with the DCChargeParametersObj written to
    friend std::ostream& operator<<(std::ostream& os, const DCChargeParametersObj& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct ACChargeStatusObj {
    int32_t evse_active_phase_count; ///< evse_active_phase_count

    /// \brief Conversion from a given ACChargeStatusObj \p k to a given json object \p j
    friend void to_json(json& j, const ACChargeStatusObj& k) {
        // the required parts of the type
        j = json{
            {"evse_active_phase_count", k.evse_active_phase_count},
        };
        // the optional parts of the type
    }

    /// \brief Conversion from a given json object \p j to a given ACChargeStatusObj \p k
    friend void from_json(const json& j, ACChargeStatusObj& k) {
        // the required parts of the type
        k.evse_active_phase_count = j.at("evse_active_phase_count");

        // the optional parts of the type
    }

    /// \brief Compares objects of type ACChargeStatusObj for equality
    friend constexpr bool operator==(const ACChargeStatusObj& k, const ACChargeStatusObj& l) {
        const auto& lhs_tuple = std::tie(
            k.evse_active_phase_count
        );
        const auto& rhs_tuple = std::tie(
            l.evse_active_phase_count
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type ACChargeStatusObj for inequality
    friend constexpr bool operator!=(const ACChargeStatusObj& k, const ACChargeStatusObj& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given ACChargeStatusObj \p k to the given output stream \p os
    /// \returns an output stream with the ACChargeStatusObj written to
    friend std::ostream& operator<<(std::ostream& os, const ACChargeStatusObj& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct DCChargeStatusObj {
    float evse_present_current; ///< evse_present_current
    float evse_present_voltage; ///< evse_present_voltage
    bool evse_power_limit_achieved; ///< evse_power_limit_achieved
    bool evse_current_limit_achieved; ///< evse_current_limit_achieved
    bool evse_voltage_limit_achieved; ///< evse_voltage_limit_achieved

    /// \brief Conversion from a given DCChargeStatusObj \p k to a given json object \p j
    friend void to_json(json& j, const DCChargeStatusObj& k) {
        // the required parts of the type
        j = json{
            {"evse_present_current", k.evse_present_current},
            {"evse_present_voltage", k.evse_present_voltage},
            {"evse_power_limit_achieved", k.evse_power_limit_achieved},
            {"evse_current_limit_achieved", k.evse_current_limit_achieved},
            {"evse_voltage_limit_achieved", k.evse_voltage_limit_achieved},
        };
        // the optional parts of the type
    }

    /// \brief Conversion from a given json object \p j to a given DCChargeStatusObj \p k
    friend void from_json(const json& j, DCChargeStatusObj& k) {
        // the required parts of the type
        k.evse_present_current = j.at("evse_present_current");
        k.evse_present_voltage = j.at("evse_present_voltage");
        k.evse_power_limit_achieved = j.at("evse_power_limit_achieved");
        k.evse_current_limit_achieved = j.at("evse_current_limit_achieved");
        k.evse_voltage_limit_achieved = j.at("evse_voltage_limit_achieved");

        // the optional parts of the type
    }

    /// \brief Compares objects of type DCChargeStatusObj for equality
    friend constexpr bool operator==(const DCChargeStatusObj& k, const DCChargeStatusObj& l) {
        const auto& lhs_tuple = std::tie(
            k.evse_present_current,
            k.evse_present_voltage,
            k.evse_power_limit_achieved,
            k.evse_current_limit_achieved,
            k.evse_voltage_limit_achieved
        );
        const auto& rhs_tuple = std::tie(
            l.evse_present_current,
            l.evse_present_voltage,
            l.evse_power_limit_achieved,
            l.evse_current_limit_achieved,
            l.evse_voltage_limit_achieved
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type DCChargeStatusObj for inequality
    friend constexpr bool operator!=(const DCChargeStatusObj& k, const DCChargeStatusObj& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given DCChargeStatusObj \p k to the given output stream \p os
    /// \returns an output stream with the DCChargeStatusObj written to
    friend std::ostream& operator<<(std::ostream& os, const DCChargeStatusObj& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct DisplayParametersObj {
    std::optional<int32_t> start_soc; ///< start_soc
    std::optional<int32_t> present_soc; ///< present_soc
    std::optional<int32_t> minimum_soc; ///< minimum_soc
    std::optional<int32_t> target_soc; ///< target_soc
    std::optional<int32_t> maximum_soc; ///< maximum_soc
    std::optional<int32_t> remaining_time_to_minimum_soc; ///< remaining_time_to_minimum_soc
    std::optional<int32_t> remaining_time_to_target_soc; ///< remaining_time_to_target_soc
    std::optional<int32_t> remaining_time_to_maximum_soc; ///< remaining_time_to_maximum_soc
    std::optional<bool> charging_complete; ///< charging_complete
    std::optional<float> battery_energy_capacity; ///< battery_energy_capacity
    std::optional<bool> inlet_hot; ///< inlet_hot

    /// \brief Conversion from a given DisplayParametersObj \p k to a given json object \p j
    friend void to_json(json& j, const DisplayParametersObj& k) {
        // the required parts of the type
        j = json ({});
        // the optional parts of the type
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

    /// \brief Conversion from a given json object \p j to a given DisplayParametersObj \p k
    friend void from_json(const json& j, DisplayParametersObj& k) {
        // the required parts of the type

        // the optional parts of the type
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

    /// \brief Compares objects of type DisplayParametersObj for equality
    friend constexpr bool operator==(const DisplayParametersObj& k, const DisplayParametersObj& l) {
        const auto& lhs_tuple = std::tie(
            k.start_soc,
            k.present_soc,
            k.minimum_soc,
            k.target_soc,
            k.maximum_soc,
            k.remaining_time_to_minimum_soc,
            k.remaining_time_to_target_soc,
            k.remaining_time_to_maximum_soc,
            k.charging_complete,
            k.battery_energy_capacity,
            k.inlet_hot
        );
        const auto& rhs_tuple = std::tie(
            l.start_soc,
            l.present_soc,
            l.minimum_soc,
            l.target_soc,
            l.maximum_soc,
            l.remaining_time_to_minimum_soc,
            l.remaining_time_to_target_soc,
            l.remaining_time_to_maximum_soc,
            l.charging_complete,
            l.battery_energy_capacity,
            l.inlet_hot
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type DisplayParametersObj for inequality
    friend constexpr bool operator!=(const DisplayParametersObj& k, const DisplayParametersObj& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given DisplayParametersObj \p k to the given output stream \p os
    /// \returns an output stream with the DisplayParametersObj written to
    friend std::ostream& operator<<(std::ostream& os, const DisplayParametersObj& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct HardwareCapabilitiesObj {
    float max_current_A_export; ///< TODO: description
    float max_current_A_import; ///< TODO: description
    int32_t max_phase_count_export; ///< TODO: description
    int32_t max_phase_count_import; ///< TODO: description
    float min_current_A_export; ///< TODO: description
    float min_current_A_import; ///< TODO: description
    int32_t min_phase_count_export; ///< TODO: description
    int32_t min_phase_count_import; ///< TODO: description
    bool phase_switch_during_charging; ///< TODO: description

    /// \brief Conversion from a given HardwareCapabilitiesObj \p k to a given json object \p j
    friend void to_json(json& j, const HardwareCapabilitiesObj& k) {
        // the required parts of the type
        j = json{
            {"max_current_A_export", k.max_current_A_export},
            {"max_current_A_import", k.max_current_A_import},
            {"max_phase_count_export", k.max_phase_count_export},
            {"max_phase_count_import", k.max_phase_count_import},
            {"min_current_A_export", k.min_current_A_export},
            {"min_current_A_import", k.min_current_A_import},
            {"min_phase_count_export", k.min_phase_count_export},
            {"min_phase_count_import", k.min_phase_count_import},
            {"phase_switch_during_charging", k.phase_switch_during_charging},
        };
        // the optional parts of the type
    }

    /// \brief Conversion from a given json object \p j to a given HardwareCapabilitiesObj \p k
    friend void from_json(const json& j, HardwareCapabilitiesObj& k) {
        // the required parts of the type
        k.max_current_A_export = j.at("max_current_A_export");
        k.max_current_A_import = j.at("max_current_A_import");
        k.max_phase_count_export = j.at("max_phase_count_export");
        k.max_phase_count_import = j.at("max_phase_count_import");
        k.min_current_A_export = j.at("min_current_A_export");
        k.min_current_A_import = j.at("min_current_A_import");
        k.min_phase_count_export = j.at("min_phase_count_export");
        k.min_phase_count_import = j.at("min_phase_count_import");
        k.phase_switch_during_charging = j.at("phase_switch_during_charging");

        // the optional parts of the type
    }

    /// \brief Compares objects of type HardwareCapabilitiesObj for equality
    friend constexpr bool operator==(const HardwareCapabilitiesObj& k, const HardwareCapabilitiesObj& l) {
        const auto& lhs_tuple = std::tie(
            k.max_current_A_export,
            k.max_current_A_import,
            k.max_phase_count_export,
            k.max_phase_count_import,
            k.min_current_A_export,
            k.min_current_A_import,
            k.min_phase_count_export,
            k.min_phase_count_import,
            k.phase_switch_during_charging
        );
        const auto& rhs_tuple = std::tie(
            l.max_current_A_export,
            l.max_current_A_import,
            l.max_phase_count_export,
            l.max_phase_count_import,
            l.min_current_A_export,
            l.min_current_A_import,
            l.min_phase_count_export,
            l.min_phase_count_import,
            l.phase_switch_during_charging
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type HardwareCapabilitiesObj for inequality
    friend constexpr bool operator!=(const HardwareCapabilitiesObj& k, const HardwareCapabilitiesObj& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given HardwareCapabilitiesObj \p k to the given output stream \p os
    /// \returns an output stream with the HardwareCapabilitiesObj written to
    friend std::ostream& operator<<(std::ostream& os, const HardwareCapabilitiesObj& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct EVSEInfoObj {
    int32_t index; ///< Unique index of the EVSE, used for identifying it
    std::string id; ///< Unique identifier string, as used in V2G communication
    std::vector<types::json_rpc_api::ConnectorInfoObj> available_connectors; ///< Available connectors
    std::vector<types::json_rpc_api::EnergyTransferModeEnum> supported_energy_transfer_modes; ///< Supported energy transfer modes of the EVSE
    std::optional<std::string> description; ///< Description

    /// \brief Conversion from a given EVSEInfoObj \p k to a given json object \p j
    friend void to_json(json& j, const EVSEInfoObj& k) {
        // the required parts of the type
        j = json{
            {"index", k.index},
            {"id", k.id},
            {"available_connectors", k.available_connectors},
            {"supported_energy_transfer_modes", k.supported_energy_transfer_modes},
        };
        // the optional parts of the type
        if (k.description) {
            j["description"] = k.description.value();
        }
    }

    /// \brief Conversion from a given json object \p j to a given EVSEInfoObj \p k
    friend void from_json(const json& j, EVSEInfoObj& k) {
        // the required parts of the type
        k.index = j.at("index");
        k.id = j.at("id");
        for (auto val : j.at("available_connectors")) {
            k.available_connectors.push_back(val);
        }
        for (auto val : j.at("supported_energy_transfer_modes")) {
            k.supported_energy_transfer_modes.push_back(val);
        }

        // the optional parts of the type
        if (j.contains("description")) {
            k.description.emplace(j.at("description"));
        }
    }

    /// \brief Compares objects of type EVSEInfoObj for equality
    friend constexpr bool operator==(const EVSEInfoObj& k, const EVSEInfoObj& l) {
        const auto& lhs_tuple = std::tie(
            k.index,
            k.id,
            k.available_connectors,
            k.supported_energy_transfer_modes,
            k.description
        );
        const auto& rhs_tuple = std::tie(
            l.index,
            l.id,
            l.available_connectors,
            l.supported_energy_transfer_modes,
            l.description
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type EVSEInfoObj for inequality
    friend constexpr bool operator!=(const EVSEInfoObj& k, const EVSEInfoObj& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given EVSEInfoObj \p k to the given output stream \p os
    /// \returns an output stream with the EVSEInfoObj written to
    friend std::ostream& operator<<(std::ostream& os, const EVSEInfoObj& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct EVSEStatusObj {
    float charged_energy_wh; ///< charged_energy_wh
    float discharged_energy_wh; ///< discharged_energy_wh
    int32_t charging_duration_s; ///< charging_duration_s
    bool charging_allowed; ///< charging_allowed
    bool available; ///< available
    int32_t active_connector_index; ///< active_connector_index
    bool error_present; ///< error_present
    types::json_rpc_api::ChargeProtocolEnum charge_protocol; ///< charge_protocol
    types::json_rpc_api::EVSEStateEnum state; ///< state
    std::optional<types::json_rpc_api::ACChargeParametersObj> ac_charge_param; ///< ac_charge_param
    std::optional<types::json_rpc_api::DCChargeParametersObj> dc_charge_param; ///< dc_charge_param
    std::optional<types::json_rpc_api::ACChargeStatusObj> ac_charge_status; ///< ac_charge_status
    std::optional<types::json_rpc_api::DCChargeStatusObj> dc_charge_status; ///< dc_charge_status
    std::optional<types::json_rpc_api::DisplayParametersObj> display_parameters; ///< display_parameters

    /// \brief Conversion from a given EVSEStatusObj \p k to a given json object \p j
    friend void to_json(json& j, const EVSEStatusObj& k) {
        // the required parts of the type
        j = json{
            {"charged_energy_wh", k.charged_energy_wh},
            {"discharged_energy_wh", k.discharged_energy_wh},
            {"charging_duration_s", k.charging_duration_s},
            {"charging_allowed", k.charging_allowed},
            {"available", k.available},
            {"active_connector_index", k.active_connector_index},
            {"error_present", k.error_present},
            {"charge_protocol", types::json_rpc_api::charge_protocol_enum_to_string(k.charge_protocol)},
            {"state", types::json_rpc_api::evsestate_enum_to_string(k.state)},
        };
        // the optional parts of the type
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

    /// \brief Conversion from a given json object \p j to a given EVSEStatusObj \p k
    friend void from_json(const json& j, EVSEStatusObj& k) {
        // the required parts of the type
        k.charged_energy_wh = j.at("charged_energy_wh");
        k.discharged_energy_wh = j.at("discharged_energy_wh");
        k.charging_duration_s = j.at("charging_duration_s");
        k.charging_allowed = j.at("charging_allowed");
        k.available = j.at("available");
        k.active_connector_index = j.at("active_connector_index");
        k.error_present = j.at("error_present");
        k.charge_protocol = types::json_rpc_api::string_to_charge_protocol_enum(j.at("charge_protocol"));
        k.state = types::json_rpc_api::string_to_evsestate_enum(j.at("state"));

        // the optional parts of the type
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

    /// \brief Compares objects of type EVSEStatusObj for equality
    friend constexpr bool operator==(const EVSEStatusObj& k, const EVSEStatusObj& l) {
        const auto& lhs_tuple = std::tie(
            k.charged_energy_wh,
            k.discharged_energy_wh,
            k.charging_duration_s,
            k.charging_allowed,
            k.available,
            k.active_connector_index,
            k.error_present,
            k.charge_protocol,
            k.state,
            k.ac_charge_param,
            k.dc_charge_param,
            k.ac_charge_status,
            k.dc_charge_status,
            k.display_parameters
        );
        const auto& rhs_tuple = std::tie(
            l.charged_energy_wh,
            l.discharged_energy_wh,
            l.charging_duration_s,
            l.charging_allowed,
            l.available,
            l.active_connector_index,
            l.error_present,
            l.charge_protocol,
            l.state,
            l.ac_charge_param,
            l.dc_charge_param,
            l.ac_charge_status,
            l.dc_charge_status,
            l.display_parameters
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type EVSEStatusObj for inequality
    friend constexpr bool operator!=(const EVSEStatusObj& k, const EVSEStatusObj& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given EVSEStatusObj \p k to the given output stream \p os
    /// \returns an output stream with the EVSEStatusObj written to
    friend std::ostream& operator<<(std::ostream& os, const EVSEStatusObj& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct Current_A {
    std::optional<float> L1; ///< AC L1 value only
    std::optional<float> L2; ///< AC L2 value only
    std::optional<float> L3; ///< AC L3 value only
    std::optional<float> N; ///< AC Neutral value only

    /// \brief Conversion from a given Current_A \p k to a given json object \p j
    friend void to_json(json& j, const Current_A& k) {
        // the required parts of the type
        j = json ({});
        // the optional parts of the type
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

    /// \brief Conversion from a given json object \p j to a given Current_A \p k
    friend void from_json(const json& j, Current_A& k) {
        // the required parts of the type

        // the optional parts of the type
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

    /// \brief Compares objects of type Current_A for equality
    friend constexpr bool operator==(const Current_A& k, const Current_A& l) {
        const auto& lhs_tuple = std::tie(
            k.L1,
            k.L2,
            k.L3,
            k.N
        );
        const auto& rhs_tuple = std::tie(
            l.L1,
            l.L2,
            l.L3,
            l.N
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type Current_A for inequality
    friend constexpr bool operator!=(const Current_A& k, const Current_A& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given Current_A \p k to the given output stream \p os
    /// \returns an output stream with the Current_A written to
    friend std::ostream& operator<<(std::ostream& os, const Current_A& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct Energy_Wh_import {
    float total; ///< DC / AC Sum value (which is relevant for billing)
    std::optional<float> L1; ///< AC L1 value only
    std::optional<float> L2; ///< AC L2 value only
    std::optional<float> L3; ///< AC L3 value only

    /// \brief Conversion from a given Energy_Wh_import \p k to a given json object \p j
    friend void to_json(json& j, const Energy_Wh_import& k) {
        // the required parts of the type
        j = json{
            {"total", k.total},
        };
        // the optional parts of the type
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

    /// \brief Conversion from a given json object \p j to a given Energy_Wh_import \p k
    friend void from_json(const json& j, Energy_Wh_import& k) {
        // the required parts of the type
        k.total = j.at("total");

        // the optional parts of the type
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

    /// \brief Compares objects of type Energy_Wh_import for equality
    friend constexpr bool operator==(const Energy_Wh_import& k, const Energy_Wh_import& l) {
        const auto& lhs_tuple = std::tie(
            k.total,
            k.L1,
            k.L2,
            k.L3
        );
        const auto& rhs_tuple = std::tie(
            l.total,
            l.L1,
            l.L2,
            l.L3
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type Energy_Wh_import for inequality
    friend constexpr bool operator!=(const Energy_Wh_import& k, const Energy_Wh_import& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given Energy_Wh_import \p k to the given output stream \p os
    /// \returns an output stream with the Energy_Wh_import written to
    friend std::ostream& operator<<(std::ostream& os, const Energy_Wh_import& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct Energy_Wh_export {
    float total; ///< DC / AC Sum value (which is relevant for billing)
    std::optional<float> L1; ///< AC L1 value only
    std::optional<float> L2; ///< AC L2 value only
    std::optional<float> L3; ///< AC L3 value only

    /// \brief Conversion from a given Energy_Wh_export \p k to a given json object \p j
    friend void to_json(json& j, const Energy_Wh_export& k) {
        // the required parts of the type
        j = json{
            {"total", k.total},
        };
        // the optional parts of the type
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

    /// \brief Conversion from a given json object \p j to a given Energy_Wh_export \p k
    friend void from_json(const json& j, Energy_Wh_export& k) {
        // the required parts of the type
        k.total = j.at("total");

        // the optional parts of the type
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

    /// \brief Compares objects of type Energy_Wh_export for equality
    friend constexpr bool operator==(const Energy_Wh_export& k, const Energy_Wh_export& l) {
        const auto& lhs_tuple = std::tie(
            k.total,
            k.L1,
            k.L2,
            k.L3
        );
        const auto& rhs_tuple = std::tie(
            l.total,
            l.L1,
            l.L2,
            l.L3
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type Energy_Wh_export for inequality
    friend constexpr bool operator!=(const Energy_Wh_export& k, const Energy_Wh_export& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given Energy_Wh_export \p k to the given output stream \p os
    /// \returns an output stream with the Energy_Wh_export written to
    friend std::ostream& operator<<(std::ostream& os, const Energy_Wh_export& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct Frequency_Hz {
    float L1; ///< AC L1 value
    std::optional<float> L2; ///< AC L2 value
    std::optional<float> L3; ///< AC L3 value

    /// \brief Conversion from a given Frequency_Hz \p k to a given json object \p j
    friend void to_json(json& j, const Frequency_Hz& k) {
        // the required parts of the type
        j = json{
            {"L1", k.L1},
        };
        // the optional parts of the type
        if (k.L2) {
            j["L2"] = k.L2.value();
        }
        if (k.L3) {
            j["L3"] = k.L3.value();
        }
    }

    /// \brief Conversion from a given json object \p j to a given Frequency_Hz \p k
    friend void from_json(const json& j, Frequency_Hz& k) {
        // the required parts of the type
        k.L1 = j.at("L1");

        // the optional parts of the type
        if (j.contains("L2")) {
            k.L2.emplace(j.at("L2"));
        }
        if (j.contains("L3")) {
            k.L3.emplace(j.at("L3"));
        }
    }

    /// \brief Compares objects of type Frequency_Hz for equality
    friend constexpr bool operator==(const Frequency_Hz& k, const Frequency_Hz& l) {
        const auto& lhs_tuple = std::tie(
            k.L1,
            k.L2,
            k.L3
        );
        const auto& rhs_tuple = std::tie(
            l.L1,
            l.L2,
            l.L3
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type Frequency_Hz for inequality
    friend constexpr bool operator!=(const Frequency_Hz& k, const Frequency_Hz& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given Frequency_Hz \p k to the given output stream \p os
    /// \returns an output stream with the Frequency_Hz written to
    friend std::ostream& operator<<(std::ostream& os, const Frequency_Hz& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct Power_W {
    float total; ///< DC / AC Sum value
    std::optional<float> L1; ///< AC L1 value only
    std::optional<float> L2; ///< AC L2 value only
    std::optional<float> L3; ///< AC L3 value only

    /// \brief Conversion from a given Power_W \p k to a given json object \p j
    friend void to_json(json& j, const Power_W& k) {
        // the required parts of the type
        j = json{
            {"total", k.total},
        };
        // the optional parts of the type
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

    /// \brief Conversion from a given json object \p j to a given Power_W \p k
    friend void from_json(const json& j, Power_W& k) {
        // the required parts of the type
        k.total = j.at("total");

        // the optional parts of the type
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

    /// \brief Compares objects of type Power_W for equality
    friend constexpr bool operator==(const Power_W& k, const Power_W& l) {
        const auto& lhs_tuple = std::tie(
            k.total,
            k.L1,
            k.L2,
            k.L3
        );
        const auto& rhs_tuple = std::tie(
            l.total,
            l.L1,
            l.L2,
            l.L3
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type Power_W for inequality
    friend constexpr bool operator!=(const Power_W& k, const Power_W& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given Power_W \p k to the given output stream \p os
    /// \returns an output stream with the Power_W written to
    friend std::ostream& operator<<(std::ostream& os, const Power_W& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct Voltage_V {
    std::optional<float> L1; ///< AC L1 value only
    std::optional<float> L2; ///< AC L2 value only
    std::optional<float> L3; ///< AC L3 value only

    /// \brief Conversion from a given Voltage_V \p k to a given json object \p j
    friend void to_json(json& j, const Voltage_V& k) {
        // the required parts of the type
        j = json ({});
        // the optional parts of the type
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

    /// \brief Conversion from a given json object \p j to a given Voltage_V \p k
    friend void from_json(const json& j, Voltage_V& k) {
        // the required parts of the type

        // the optional parts of the type
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

    /// \brief Compares objects of type Voltage_V for equality
    friend constexpr bool operator==(const Voltage_V& k, const Voltage_V& l) {
        const auto& lhs_tuple = std::tie(
            k.L1,
            k.L2,
            k.L3
        );
        const auto& rhs_tuple = std::tie(
            l.L1,
            l.L2,
            l.L3
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type Voltage_V for inequality
    friend constexpr bool operator!=(const Voltage_V& k, const Voltage_V& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given Voltage_V \p k to the given output stream \p os
    /// \returns an output stream with the Voltage_V written to
    friend std::ostream& operator<<(std::ostream& os, const Voltage_V& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct MeterDataObj {
    Energy_Wh_import energy_Wh_import; ///< Imported energy in Wh (from grid)
    std::string timestamp; ///< Timestamp of the meter values, as RFC3339 string
    std::optional<Current_A> current_A; ///< Current in Ampere
    std::optional<Energy_Wh_export> energy_Wh_export; ///< Exported energy in Wh (to grid)
    std::optional<Frequency_Hz> frequency_Hz; ///< Grid frequency in Hertz
    std::optional<std::string> meter_id; ///< TODO: description
    std::optional<std::string> serial_number; ///< TODO: description
    std::optional<bool> phase_seq_error; ///< TODO: description
    std::optional<Power_W> power_W; ///< Instantaneous power in Watt. Negative values are exported, positive values imported energy.
    std::optional<Voltage_V> voltage_V; ///< Voltage in Volts

    /// \brief Conversion from a given MeterDataObj \p k to a given json object \p j
    friend void to_json(json& j, const MeterDataObj& k) {
        // the required parts of the type
        j = json{
            {"energy_Wh_import", k.energy_Wh_import},
            {"timestamp", k.timestamp},
        };
        // the optional parts of the type
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

    /// \brief Conversion from a given json object \p j to a given MeterDataObj \p k
    friend void from_json(const json& j, MeterDataObj& k) {
        // the required parts of the type
        k.energy_Wh_import = j.at("energy_Wh_import");
        k.timestamp = j.at("timestamp");

        // the optional parts of the type
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

    /// \brief Compares objects of type MeterDataObj for equality
    friend constexpr bool operator==(const MeterDataObj& k, const MeterDataObj& l) {
        const auto& lhs_tuple = std::tie(
            k.energy_Wh_import,
            k.timestamp,
            k.current_A,
            k.energy_Wh_export,
            k.frequency_Hz,
            k.meter_id,
            k.serial_number,
            k.phase_seq_error,
            k.power_W,
            k.voltage_V
        );
        const auto& rhs_tuple = std::tie(
            l.energy_Wh_import,
            l.timestamp,
            l.current_A,
            l.energy_Wh_export,
            l.frequency_Hz,
            l.meter_id,
            l.serial_number,
            l.phase_seq_error,
            l.power_W,
            l.voltage_V
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type MeterDataObj for inequality
    friend constexpr bool operator!=(const MeterDataObj& k, const MeterDataObj& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given MeterDataObj \p k to the given output stream \p os
    /// \returns an output stream with the MeterDataObj written to
    friend std::ostream& operator<<(std::ostream& os, const MeterDataObj& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct HelloResObj {
    bool authentication_required; ///< Whether authentication is required
    std::string api_version; ///< Version of the JSON RPC API
    std::string everest_version; ///< The version of the running EVerest instance
    types::json_rpc_api::ChargerInfoObj charger_info; ///< Charger information
    std::optional<bool> authenticated; ///< Whether the client is properly authenticated

    /// \brief Conversion from a given HelloResObj \p k to a given json object \p j
    friend void to_json(json& j, const HelloResObj& k) {
        // the required parts of the type
        j = json{
            {"authentication_required", k.authentication_required},
            {"api_version", k.api_version},
            {"everest_version", k.everest_version},
            {"charger_info", k.charger_info},
        };
        // the optional parts of the type
        if (k.authenticated) {
            j["authenticated"] = k.authenticated.value();
        }
    }

    /// \brief Conversion from a given json object \p j to a given HelloResObj \p k
    friend void from_json(const json& j, HelloResObj& k) {
        // the required parts of the type
        k.authentication_required = j.at("authentication_required");
        k.api_version = j.at("api_version");
        k.everest_version = j.at("everest_version");
        k.charger_info = j.at("charger_info");

        // the optional parts of the type
        if (j.contains("authenticated")) {
            k.authenticated.emplace(j.at("authenticated"));
        }
    }

    /// \brief Compares objects of type HelloResObj for equality
    friend constexpr bool operator==(const HelloResObj& k, const HelloResObj& l) {
        const auto& lhs_tuple = std::tie(
            k.authentication_required,
            k.api_version,
            k.everest_version,
            k.charger_info,
            k.authenticated
        );
        const auto& rhs_tuple = std::tie(
            l.authentication_required,
            l.api_version,
            l.everest_version,
            l.charger_info,
            l.authenticated
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type HelloResObj for inequality
    friend constexpr bool operator!=(const HelloResObj& k, const HelloResObj& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given HelloResObj \p k to the given output stream \p os
    /// \returns an output stream with the HelloResObj written to
    friend std::ostream& operator<<(std::ostream& os, const HelloResObj& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct ChargePointGetEVSEInfosResObj {
    std::vector<types::json_rpc_api::EVSEInfoObj> infos; ///< Array of EVSE infos
    types::json_rpc_api::ResponseErrorEnum error; ///< Response error

    /// \brief Conversion from a given ChargePointGetEVSEInfosResObj \p k to a given json object \p j
    friend void to_json(json& j, const ChargePointGetEVSEInfosResObj& k) {
        // the required parts of the type
        j = json{
            {"infos", k.infos},
            {"error", types::json_rpc_api::response_error_enum_to_string(k.error)},
        };
        // the optional parts of the type
    }

    /// \brief Conversion from a given json object \p j to a given ChargePointGetEVSEInfosResObj \p k
    friend void from_json(const json& j, ChargePointGetEVSEInfosResObj& k) {
        // the required parts of the type
        for (auto val : j.at("infos")) {
            k.infos.push_back(val);
        }
        k.error = types::json_rpc_api::string_to_response_error_enum(j.at("error"));

        // the optional parts of the type
    }

    /// \brief Compares objects of type ChargePointGetEVSEInfosResObj for equality
    friend constexpr bool operator==(const ChargePointGetEVSEInfosResObj& k, const ChargePointGetEVSEInfosResObj& l) {
        const auto& lhs_tuple = std::tie(
            k.infos,
            k.error
        );
        const auto& rhs_tuple = std::tie(
            l.infos,
            l.error
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type ChargePointGetEVSEInfosResObj for inequality
    friend constexpr bool operator!=(const ChargePointGetEVSEInfosResObj& k, const ChargePointGetEVSEInfosResObj& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given ChargePointGetEVSEInfosResObj \p k to the given output stream \p os
    /// \returns an output stream with the ChargePointGetEVSEInfosResObj written to
    friend std::ostream& operator<<(std::ostream& os, const ChargePointGetEVSEInfosResObj& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct ChargePointGetActiveErrorsResObj {
    std::vector<types::json_rpc_api::ErrorObj> active_errors; ///< Array of active charge point errors
    types::json_rpc_api::ResponseErrorEnum error; ///< Response error

    /// \brief Conversion from a given ChargePointGetActiveErrorsResObj \p k to a given json object \p j
    friend void to_json(json& j, const ChargePointGetActiveErrorsResObj& k) {
        // the required parts of the type
        j = json{
            {"active_errors", k.active_errors},
            {"error", types::json_rpc_api::response_error_enum_to_string(k.error)},
        };
        // the optional parts of the type
    }

    /// \brief Conversion from a given json object \p j to a given ChargePointGetActiveErrorsResObj \p k
    friend void from_json(const json& j, ChargePointGetActiveErrorsResObj& k) {
        // the required parts of the type
        for (auto val : j.at("active_errors")) {
            k.active_errors.push_back(val);
        }
        k.error = types::json_rpc_api::string_to_response_error_enum(j.at("error"));

        // the optional parts of the type
    }

    /// \brief Compares objects of type ChargePointGetActiveErrorsResObj for equality
    friend constexpr bool operator==(const ChargePointGetActiveErrorsResObj& k, const ChargePointGetActiveErrorsResObj& l) {
        const auto& lhs_tuple = std::tie(
            k.active_errors,
            k.error
        );
        const auto& rhs_tuple = std::tie(
            l.active_errors,
            l.error
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type ChargePointGetActiveErrorsResObj for inequality
    friend constexpr bool operator!=(const ChargePointGetActiveErrorsResObj& k, const ChargePointGetActiveErrorsResObj& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given ChargePointGetActiveErrorsResObj \p k to the given output stream \p os
    /// \returns an output stream with the ChargePointGetActiveErrorsResObj written to
    friend std::ostream& operator<<(std::ostream& os, const ChargePointGetActiveErrorsResObj& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct EVSEGetInfoResObj {
    types::json_rpc_api::EVSEInfoObj info; ///< TODO: description
    types::json_rpc_api::ResponseErrorEnum error; ///< Response error

    /// \brief Conversion from a given EVSEGetInfoResObj \p k to a given json object \p j
    friend void to_json(json& j, const EVSEGetInfoResObj& k) {
        // the required parts of the type
        j = json{
            {"info", k.info},
            {"error", types::json_rpc_api::response_error_enum_to_string(k.error)},
        };
        // the optional parts of the type
    }

    /// \brief Conversion from a given json object \p j to a given EVSEGetInfoResObj \p k
    friend void from_json(const json& j, EVSEGetInfoResObj& k) {
        // the required parts of the type
        k.info = j.at("info");
        k.error = types::json_rpc_api::string_to_response_error_enum(j.at("error"));

        // the optional parts of the type
    }

    /// \brief Compares objects of type EVSEGetInfoResObj for equality
    friend constexpr bool operator==(const EVSEGetInfoResObj& k, const EVSEGetInfoResObj& l) {
        const auto& lhs_tuple = std::tie(
            k.info,
            k.error
        );
        const auto& rhs_tuple = std::tie(
            l.info,
            l.error
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type EVSEGetInfoResObj for inequality
    friend constexpr bool operator!=(const EVSEGetInfoResObj& k, const EVSEGetInfoResObj& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given EVSEGetInfoResObj \p k to the given output stream \p os
    /// \returns an output stream with the EVSEGetInfoResObj written to
    friend std::ostream& operator<<(std::ostream& os, const EVSEGetInfoResObj& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct EVSEGetStatusResObj {
    types::json_rpc_api::EVSEStatusObj status; ///< TODO: description
    types::json_rpc_api::ResponseErrorEnum error; ///< Response error

    /// \brief Conversion from a given EVSEGetStatusResObj \p k to a given json object \p j
    friend void to_json(json& j, const EVSEGetStatusResObj& k) {
        // the required parts of the type
        j = json{
            {"status", k.status},
            {"error", types::json_rpc_api::response_error_enum_to_string(k.error)},
        };
        // the optional parts of the type
    }

    /// \brief Conversion from a given json object \p j to a given EVSEGetStatusResObj \p k
    friend void from_json(const json& j, EVSEGetStatusResObj& k) {
        // the required parts of the type
        k.status = j.at("status");
        k.error = types::json_rpc_api::string_to_response_error_enum(j.at("error"));

        // the optional parts of the type
    }

    /// \brief Compares objects of type EVSEGetStatusResObj for equality
    friend constexpr bool operator==(const EVSEGetStatusResObj& k, const EVSEGetStatusResObj& l) {
        const auto& lhs_tuple = std::tie(
            k.status,
            k.error
        );
        const auto& rhs_tuple = std::tie(
            l.status,
            l.error
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type EVSEGetStatusResObj for inequality
    friend constexpr bool operator!=(const EVSEGetStatusResObj& k, const EVSEGetStatusResObj& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given EVSEGetStatusResObj \p k to the given output stream \p os
    /// \returns an output stream with the EVSEGetStatusResObj written to
    friend std::ostream& operator<<(std::ostream& os, const EVSEGetStatusResObj& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct EVSEGetHardwareCapabilitiesResObj {
    types::json_rpc_api::HardwareCapabilitiesObj hardware_capabilities; ///< TODO: description
    types::json_rpc_api::ResponseErrorEnum error; ///< Response error

    /// \brief Conversion from a given EVSEGetHardwareCapabilitiesResObj \p k to a given json object \p j
    friend void to_json(json& j, const EVSEGetHardwareCapabilitiesResObj& k) {
        // the required parts of the type
        j = json{
            {"hardware_capabilities", k.hardware_capabilities},
            {"error", types::json_rpc_api::response_error_enum_to_string(k.error)},
        };
        // the optional parts of the type
    }

    /// \brief Conversion from a given json object \p j to a given EVSEGetHardwareCapabilitiesResObj \p k
    friend void from_json(const json& j, EVSEGetHardwareCapabilitiesResObj& k) {
        // the required parts of the type
        k.hardware_capabilities = j.at("hardware_capabilities");
        k.error = types::json_rpc_api::string_to_response_error_enum(j.at("error"));

        // the optional parts of the type
    }

    /// \brief Compares objects of type EVSEGetHardwareCapabilitiesResObj for equality
    friend constexpr bool operator==(const EVSEGetHardwareCapabilitiesResObj& k, const EVSEGetHardwareCapabilitiesResObj& l) {
        const auto& lhs_tuple = std::tie(
            k.hardware_capabilities,
            k.error
        );
        const auto& rhs_tuple = std::tie(
            l.hardware_capabilities,
            l.error
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type EVSEGetHardwareCapabilitiesResObj for inequality
    friend constexpr bool operator!=(const EVSEGetHardwareCapabilitiesResObj& k, const EVSEGetHardwareCapabilitiesResObj& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given EVSEGetHardwareCapabilitiesResObj \p k to the given output stream \p os
    /// \returns an output stream with the EVSEGetHardwareCapabilitiesResObj written to
    friend std::ostream& operator<<(std::ostream& os, const EVSEGetHardwareCapabilitiesResObj& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct EVSEGetMeterDataResObj {
    types::json_rpc_api::MeterDataObj meter_data; ///< TODO: description
    types::json_rpc_api::ResponseErrorEnum error; ///< Response error

    /// \brief Conversion from a given EVSEGetMeterDataResObj \p k to a given json object \p j
    friend void to_json(json& j, const EVSEGetMeterDataResObj& k) {
        // the required parts of the type
        j = json{
            {"meter_data", k.meter_data},
            {"error", types::json_rpc_api::response_error_enum_to_string(k.error)},
        };
        // the optional parts of the type
    }

    /// \brief Conversion from a given json object \p j to a given EVSEGetMeterDataResObj \p k
    friend void from_json(const json& j, EVSEGetMeterDataResObj& k) {
        // the required parts of the type
        k.meter_data = j.at("meter_data");
        k.error = types::json_rpc_api::string_to_response_error_enum(j.at("error"));

        // the optional parts of the type
    }

    /// \brief Compares objects of type EVSEGetMeterDataResObj for equality
    friend constexpr bool operator==(const EVSEGetMeterDataResObj& k, const EVSEGetMeterDataResObj& l) {
        const auto& lhs_tuple = std::tie(
            k.meter_data,
            k.error
        );
        const auto& rhs_tuple = std::tie(
            l.meter_data,
            l.error
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type EVSEGetMeterDataResObj for inequality
    friend constexpr bool operator!=(const EVSEGetMeterDataResObj& k, const EVSEGetMeterDataResObj& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given EVSEGetMeterDataResObj \p k to the given output stream \p os
    /// \returns an output stream with the EVSEGetMeterDataResObj written to
    friend std::ostream& operator<<(std::ostream& os, const EVSEGetMeterDataResObj& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct ErrorResObj {
    types::json_rpc_api::ResponseErrorEnum error; ///< Response error

    /// \brief Conversion from a given ErrorResObj \p k to a given json object \p j
    friend void to_json(json& j, const ErrorResObj& k) {
        // the required parts of the type
        j = json{
            {"error", types::json_rpc_api::response_error_enum_to_string(k.error)},
        };
        // the optional parts of the type
    }

    /// \brief Conversion from a given json object \p j to a given ErrorResObj \p k
    friend void from_json(const json& j, ErrorResObj& k) {
        // the required parts of the type
        k.error = types::json_rpc_api::string_to_response_error_enum(j.at("error"));

        // the optional parts of the type
    }

    /// \brief Compares objects of type ErrorResObj for equality
    friend constexpr bool operator==(const ErrorResObj& k, const ErrorResObj& l) {
        const auto& lhs_tuple = std::tie(
            k.error
        );
        const auto& rhs_tuple = std::tie(
            l.error
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type ErrorResObj for inequality
    friend constexpr bool operator!=(const ErrorResObj& k, const ErrorResObj& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given ErrorResObj \p k to the given output stream \p os
    /// \returns an output stream with the ErrorResObj written to
    friend std::ostream& operator<<(std::ostream& os, const ErrorResObj& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct ChargePointActiveErrorsChangedObj {
    std::vector<types::json_rpc_api::ErrorObj> active_errors; ///< Array of active charge point errors

    /// \brief Conversion from a given ChargePointActiveErrorsChangedObj \p k to a given json object \p j
    friend void to_json(json& j, const ChargePointActiveErrorsChangedObj& k) {
        // the required parts of the type
        j = json{
            {"active_errors", k.active_errors},
        };
        // the optional parts of the type
    }

    /// \brief Conversion from a given json object \p j to a given ChargePointActiveErrorsChangedObj \p k
    friend void from_json(const json& j, ChargePointActiveErrorsChangedObj& k) {
        // the required parts of the type
        for (auto val : j.at("active_errors")) {
            k.active_errors.push_back(val);
        }

        // the optional parts of the type
    }

    /// \brief Compares objects of type ChargePointActiveErrorsChangedObj for equality
    friend constexpr bool operator==(const ChargePointActiveErrorsChangedObj& k, const ChargePointActiveErrorsChangedObj& l) {
        const auto& lhs_tuple = std::tie(
            k.active_errors
        );
        const auto& rhs_tuple = std::tie(
            l.active_errors
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type ChargePointActiveErrorsChangedObj for inequality
    friend constexpr bool operator!=(const ChargePointActiveErrorsChangedObj& k, const ChargePointActiveErrorsChangedObj& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given ChargePointActiveErrorsChangedObj \p k to the given output stream \p os
    /// \returns an output stream with the ChargePointActiveErrorsChangedObj written to
    friend std::ostream& operator<<(std::ostream& os, const ChargePointActiveErrorsChangedObj& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct EVSEHardwareCapabilitiesChangedObj {
    int32_t evse_index; ///< Index of the EVSE
    types::json_rpc_api::HardwareCapabilitiesObj hardware_capabilities; ///< TODO: description

    /// \brief Conversion from a given EVSEHardwareCapabilitiesChangedObj \p k to a given json object \p j
    friend void to_json(json& j, const EVSEHardwareCapabilitiesChangedObj& k) {
        // the required parts of the type
        j = json{
            {"evse_index", k.evse_index},
            {"hardware_capabilities", k.hardware_capabilities},
        };
        // the optional parts of the type
    }

    /// \brief Conversion from a given json object \p j to a given EVSEHardwareCapabilitiesChangedObj \p k
    friend void from_json(const json& j, EVSEHardwareCapabilitiesChangedObj& k) {
        // the required parts of the type
        k.evse_index = j.at("evse_index");
        k.hardware_capabilities = j.at("hardware_capabilities");

        // the optional parts of the type
    }

    /// \brief Compares objects of type EVSEHardwareCapabilitiesChangedObj for equality
    friend constexpr bool operator==(const EVSEHardwareCapabilitiesChangedObj& k, const EVSEHardwareCapabilitiesChangedObj& l) {
        const auto& lhs_tuple = std::tie(
            k.evse_index,
            k.hardware_capabilities
        );
        const auto& rhs_tuple = std::tie(
            l.evse_index,
            l.hardware_capabilities
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type EVSEHardwareCapabilitiesChangedObj for inequality
    friend constexpr bool operator!=(const EVSEHardwareCapabilitiesChangedObj& k, const EVSEHardwareCapabilitiesChangedObj& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given EVSEHardwareCapabilitiesChangedObj \p k to the given output stream \p os
    /// \returns an output stream with the EVSEHardwareCapabilitiesChangedObj written to
    friend std::ostream& operator<<(std::ostream& os, const EVSEHardwareCapabilitiesChangedObj& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct EVSEStatusChangedObj {
    int32_t evse_index; ///< Index of the EVSE
    types::json_rpc_api::EVSEStatusObj evse_status; ///< TODO: description

    /// \brief Conversion from a given EVSEStatusChangedObj \p k to a given json object \p j
    friend void to_json(json& j, const EVSEStatusChangedObj& k) {
        // the required parts of the type
        j = json{
            {"evse_index", k.evse_index},
            {"evse_status", k.evse_status},
        };
        // the optional parts of the type
    }

    /// \brief Conversion from a given json object \p j to a given EVSEStatusChangedObj \p k
    friend void from_json(const json& j, EVSEStatusChangedObj& k) {
        // the required parts of the type
        k.evse_index = j.at("evse_index");
        k.evse_status = j.at("evse_status");

        // the optional parts of the type
    }

    /// \brief Compares objects of type EVSEStatusChangedObj for equality
    friend constexpr bool operator==(const EVSEStatusChangedObj& k, const EVSEStatusChangedObj& l) {
        const auto& lhs_tuple = std::tie(
            k.evse_index,
            k.evse_status
        );
        const auto& rhs_tuple = std::tie(
            l.evse_index,
            l.evse_status
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type EVSEStatusChangedObj for inequality
    friend constexpr bool operator!=(const EVSEStatusChangedObj& k, const EVSEStatusChangedObj& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given EVSEStatusChangedObj \p k to the given output stream \p os
    /// \returns an output stream with the EVSEStatusChangedObj written to
    friend std::ostream& operator<<(std::ostream& os, const EVSEStatusChangedObj& k) {
        os << json(k).dump(4);
        return os;
    }

};
struct EVSEMeterDataChangedObj {
    int32_t evse_index; ///< Index of the EVSE
    types::json_rpc_api::MeterDataObj meter_data; ///< TODO: description

    /// \brief Conversion from a given EVSEMeterDataChangedObj \p k to a given json object \p j
    friend void to_json(json& j, const EVSEMeterDataChangedObj& k) {
        // the required parts of the type
        j = json{
            {"evse_index", k.evse_index},
            {"meter_data", k.meter_data},
        };
        // the optional parts of the type
    }

    /// \brief Conversion from a given json object \p j to a given EVSEMeterDataChangedObj \p k
    friend void from_json(const json& j, EVSEMeterDataChangedObj& k) {
        // the required parts of the type
        k.evse_index = j.at("evse_index");
        k.meter_data = j.at("meter_data");

        // the optional parts of the type
    }

    /// \brief Compares objects of type EVSEMeterDataChangedObj for equality
    friend constexpr bool operator==(const EVSEMeterDataChangedObj& k, const EVSEMeterDataChangedObj& l) {
        const auto& lhs_tuple = std::tie(
            k.evse_index,
            k.meter_data
        );
        const auto& rhs_tuple = std::tie(
            l.evse_index,
            l.meter_data
        );
        return lhs_tuple == rhs_tuple;
    }

    /// \brief Compares objects of type EVSEMeterDataChangedObj for inequality
    friend constexpr bool operator!=(const EVSEMeterDataChangedObj& k, const EVSEMeterDataChangedObj& l) {
        return not operator==(k, l);
    }

    /// \brief Writes the string representation of the given EVSEMeterDataChangedObj \p k to the given output stream \p os
    /// \returns an output stream with the EVSEMeterDataChangedObj written to
    friend std::ostream& operator<<(std::ostream& os, const EVSEMeterDataChangedObj& k) {
        os << json(k).dump(4);
        return os;
    }

};
} // namespace json_rpc_api
} // namespace types


#endif // TYPES_JSON_RPC_API_TYPES_HPP
