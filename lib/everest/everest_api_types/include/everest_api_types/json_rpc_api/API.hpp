// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

namespace everest::lib::API::V1_0::types::json_rpc_api {

enum class ResponseErrorEnum {
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

enum class ChargeProtocolEnum {
    Unknown,
    IEC61851,
    DIN70121,
    ISO15118,
    ISO15118_20,
};

enum class EVSEStateEnum {
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

enum class ConnectorTypeEnum {
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

enum class EnergyTransferModeEnum {
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

enum class Severity {
    High,
    Medium,
    Low,
};

struct ImplementationIdentifier {
    std::string module_id;                  ///< TODO: description
    std::string implementation_id;          ///< TODO: description
    std::optional<int32_t> evse_index;      ///< TODO: description
    std::optional<int32_t> connector_index; ///< TODO: description

    friend constexpr bool operator==(const ImplementationIdentifier& k, const ImplementationIdentifier& l) {
        const auto& lhs_tuple = std::tie(k.module_id, k.implementation_id, k.evse_index, k.connector_index);
        const auto& rhs_tuple = std::tie(l.module_id, l.implementation_id, l.evse_index, l.connector_index);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const ImplementationIdentifier& k, const ImplementationIdentifier& l) {
        return not operator==(k, l);
    }
};

struct ChargerInfoObj {
    std::string vendor;                          ///< EVSE vendor
    std::string model;                           ///< EVSE model
    std::string serial;                          ///< EVSE serial number
    std::string firmware_version;                ///< EVSE firmware version
    std::optional<std::string> friendly_name;    ///< EVSE friendly name
    std::optional<std::string> manufacturer;     ///< EVSE manufacturer
    std::optional<std::string> manufacturer_url; ///< Manufacturer's URL
    std::optional<std::string> model_url;        ///< EVSE model's URL
    std::optional<std::string> model_no;         ///< EVSE model number
    std::optional<std::string> revision;         ///< EVSE model revision
    std::optional<std::string> board_revision;   ///< EVSE board revision

    friend constexpr bool operator==(const ChargerInfoObj& k, const ChargerInfoObj& l) {
        const auto& lhs_tuple =
            std::tie(k.vendor, k.model, k.serial, k.firmware_version, k.friendly_name, k.manufacturer,
                     k.manufacturer_url, k.model_url, k.model_no, k.revision, k.board_revision);
        const auto& rhs_tuple =
            std::tie(l.vendor, l.model, l.serial, l.firmware_version, l.friendly_name, l.manufacturer,
                     l.manufacturer_url, l.model_url, l.model_no, l.revision, l.board_revision);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const ChargerInfoObj& k, const ChargerInfoObj& l) {
        return not operator==(k, l);
    }
};

struct ErrorObj {
    std::string type;                    ///< TODO: description
    std::string description;             ///< TODO: description
    std::string message;                 ///< TODO: description
    Severity severity;                   ///< TODO: description
    ImplementationIdentifier origin;     ///< TODO: description
    std::string timestamp;               ///< TODO: description
    std::string uuid;                    ///< TODO: description
    std::optional<std::string> sub_type; ///< TODO: description

    friend constexpr bool operator==(const ErrorObj& k, const ErrorObj& l) {
        const auto& lhs_tuple =
            std::tie(k.type, k.description, k.message, k.severity, k.origin, k.timestamp, k.uuid, k.sub_type);
        const auto& rhs_tuple =
            std::tie(l.type, l.description, l.message, l.severity, l.origin, l.timestamp, l.uuid, l.sub_type);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const ErrorObj& k, const ErrorObj& l) {
        return not operator==(k, l);
    }
};

struct ConnectorInfoObj {
    int32_t index;                          ///< Unique identifier
    ConnectorTypeEnum type;                 ///< Connector type
    std::optional<std::string> description; ///< Description

    friend constexpr bool operator==(const ConnectorInfoObj& k, const ConnectorInfoObj& l) {
        const auto& lhs_tuple = std::tie(k.index, k.type, k.description);
        const auto& rhs_tuple = std::tie(l.index, l.type, l.description);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const ConnectorInfoObj& k, const ConnectorInfoObj& l) {
        return not operator==(k, l);
    }
};

struct ACChargeParametersObj {
    float evse_max_current;                            ///< evse_max_current
    int32_t evse_max_phase_count;                      ///< evse_max_phase_count
    float evse_maximum_charge_power;                   ///< evse_maximum_charge_power
    float evse_minimum_charge_power;                   ///< evse_minimum_charge_power
    float evse_nominal_frequency;                      ///< evse_nominal_frequency
    std::optional<float> evse_nominal_voltage;         ///< evse_nominal_voltage
    std::optional<float> evse_maximum_discharge_power; ///< evse_maximum_discharge_power
    std::optional<float> evse_minimum_discharge_power; ///< evse_minimum_discharge_power

    friend constexpr bool operator==(const ACChargeParametersObj& k, const ACChargeParametersObj& l) {
        const auto& lhs_tuple = std::tie(k.evse_max_current, k.evse_max_phase_count, k.evse_maximum_charge_power,
                                         k.evse_minimum_charge_power, k.evse_nominal_frequency, k.evse_nominal_voltage,
                                         k.evse_maximum_discharge_power, k.evse_minimum_discharge_power);
        const auto& rhs_tuple = std::tie(l.evse_max_current, l.evse_max_phase_count, l.evse_maximum_charge_power,
                                         l.evse_minimum_charge_power, l.evse_nominal_frequency, l.evse_nominal_voltage,
                                         l.evse_maximum_discharge_power, l.evse_minimum_discharge_power);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const ACChargeParametersObj& k, const ACChargeParametersObj& l) {
        return not operator==(k, l);
    }
};

struct DCChargeParametersObj {
    float evse_maximum_charge_current;                   ///< evse_maximum_charge_current
    float evse_maximum_charge_power;                     ///< evse_maximum_charge_power
    float evse_maximum_voltage;                          ///< evse_maximum_voltage
    float evse_minimum_charge_current;                   ///< evse_minimum_charge_current
    float evse_minimum_charge_power;                     ///< evse_minimum_charge_power
    float evse_minimum_voltage;                          ///< evse_minimum_voltage
    std::optional<float> evse_energy_to_be_delivered;    ///< evse_energy_to_be_delivered
    std::optional<float> evse_maximum_discharge_current; ///< evse_maximum_discharge_current
    std::optional<float> evse_maximum_discharge_power;   ///< evse_maximum_discharge_power
    std::optional<float> evse_minimum_discharge_current; ///< evse_minimum_discharge_current
    std::optional<float> evse_minimum_discharge_power;   ///< evse_minimum_discharge_power

    friend constexpr bool operator==(const DCChargeParametersObj& k, const DCChargeParametersObj& l) {
        const auto& lhs_tuple =
            std::tie(k.evse_maximum_charge_current, k.evse_maximum_charge_power, k.evse_maximum_voltage,
                     k.evse_minimum_charge_current, k.evse_minimum_charge_power, k.evse_minimum_voltage,
                     k.evse_energy_to_be_delivered, k.evse_maximum_discharge_current, k.evse_maximum_discharge_power,
                     k.evse_minimum_discharge_current, k.evse_minimum_discharge_power);
        const auto& rhs_tuple =
            std::tie(l.evse_maximum_charge_current, l.evse_maximum_charge_power, l.evse_maximum_voltage,
                     l.evse_minimum_charge_current, l.evse_minimum_charge_power, l.evse_minimum_voltage,
                     l.evse_energy_to_be_delivered, l.evse_maximum_discharge_current, l.evse_maximum_discharge_power,
                     l.evse_minimum_discharge_current, l.evse_minimum_discharge_power);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const DCChargeParametersObj& k, const DCChargeParametersObj& l) {
        return not operator==(k, l);
    }
};

struct ACChargeStatusObj {
    int32_t evse_active_phase_count; ///< evse_active_phase_count

    friend constexpr bool operator==(const ACChargeStatusObj& k, const ACChargeStatusObj& l) {
        const auto& lhs_tuple = std::tie(k.evse_active_phase_count);
        const auto& rhs_tuple = std::tie(l.evse_active_phase_count);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const ACChargeStatusObj& k, const ACChargeStatusObj& l) {
        return not operator==(k, l);
    }
};

struct DCChargeStatusObj {
    float evse_present_current;       ///< evse_present_current
    float evse_present_voltage;       ///< evse_present_voltage
    bool evse_power_limit_achieved;   ///< evse_power_limit_achieved
    bool evse_current_limit_achieved; ///< evse_current_limit_achieved
    bool evse_voltage_limit_achieved; ///< evse_voltage_limit_achieved

    friend constexpr bool operator==(const DCChargeStatusObj& k, const DCChargeStatusObj& l) {
        const auto& lhs_tuple = std::tie(k.evse_present_current, k.evse_present_voltage, k.evse_power_limit_achieved,
                                         k.evse_current_limit_achieved, k.evse_voltage_limit_achieved);
        const auto& rhs_tuple = std::tie(l.evse_present_current, l.evse_present_voltage, l.evse_power_limit_achieved,
                                         l.evse_current_limit_achieved, l.evse_voltage_limit_achieved);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const DCChargeStatusObj& k, const DCChargeStatusObj& l) {
        return not operator==(k, l);
    }
};

struct DisplayParametersObj {
    std::optional<int32_t> start_soc;                     ///< start_soc
    std::optional<int32_t> present_soc;                   ///< present_soc
    std::optional<int32_t> minimum_soc;                   ///< minimum_soc
    std::optional<int32_t> target_soc;                    ///< target_soc
    std::optional<int32_t> maximum_soc;                   ///< maximum_soc
    std::optional<int32_t> remaining_time_to_minimum_soc; ///< remaining_time_to_minimum_soc
    std::optional<int32_t> remaining_time_to_target_soc;  ///< remaining_time_to_target_soc
    std::optional<int32_t> remaining_time_to_maximum_soc; ///< remaining_time_to_maximum_soc
    std::optional<bool> charging_complete;                ///< charging_complete
    std::optional<float> battery_energy_capacity;         ///< battery_energy_capacity
    std::optional<bool> inlet_hot;                        ///< inlet_hot

    friend constexpr bool operator==(const DisplayParametersObj& k, const DisplayParametersObj& l) {
        const auto& lhs_tuple =
            std::tie(k.start_soc, k.present_soc, k.minimum_soc, k.target_soc, k.maximum_soc,
                     k.remaining_time_to_minimum_soc, k.remaining_time_to_target_soc, k.remaining_time_to_maximum_soc,
                     k.charging_complete, k.battery_energy_capacity, k.inlet_hot);
        const auto& rhs_tuple =
            std::tie(l.start_soc, l.present_soc, l.minimum_soc, l.target_soc, l.maximum_soc,
                     l.remaining_time_to_minimum_soc, l.remaining_time_to_target_soc, l.remaining_time_to_maximum_soc,
                     l.charging_complete, l.battery_energy_capacity, l.inlet_hot);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const DisplayParametersObj& k, const DisplayParametersObj& l) {
        return not operator==(k, l);
    }
};

struct HardwareCapabilitiesObj {
    float max_current_A_export;        ///< TODO: description
    float max_current_A_import;        ///< TODO: description
    int32_t max_phase_count_export;    ///< TODO: description
    int32_t max_phase_count_import;    ///< TODO: description
    float min_current_A_export;        ///< TODO: description
    float min_current_A_import;        ///< TODO: description
    int32_t min_phase_count_export;    ///< TODO: description
    int32_t min_phase_count_import;    ///< TODO: description
    bool phase_switch_during_charging; ///< TODO: description

    friend constexpr bool operator==(const HardwareCapabilitiesObj& k, const HardwareCapabilitiesObj& l) {
        const auto& lhs_tuple =
            std::tie(k.max_current_A_export, k.max_current_A_import, k.max_phase_count_export, k.max_phase_count_import,
                     k.min_current_A_export, k.min_current_A_import, k.min_phase_count_export, k.min_phase_count_import,
                     k.phase_switch_during_charging);
        const auto& rhs_tuple =
            std::tie(l.max_current_A_export, l.max_current_A_import, l.max_phase_count_export, l.max_phase_count_import,
                     l.min_current_A_export, l.min_current_A_import, l.min_phase_count_export, l.min_phase_count_import,
                     l.phase_switch_during_charging);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const HardwareCapabilitiesObj& k, const HardwareCapabilitiesObj& l) {
        return not operator==(k, l);
    }
};

struct EVSEInfoObj {
    int32_t index;                                      ///< Unique index of the EVSE, used for identifying it
    std::string id;                                     ///< Unique identifier string, as used in V2G communication
    std::vector<ConnectorInfoObj> available_connectors; ///< Available connectors
    std::vector<EnergyTransferModeEnum>
        supported_energy_transfer_modes;    ///< Supported energy transfer modes of the EVSE
    std::optional<std::string> description; ///< Description

    friend constexpr bool operator==(const EVSEInfoObj& k, const EVSEInfoObj& l) {
        const auto& lhs_tuple =
            std::tie(k.index, k.id, k.available_connectors, k.supported_energy_transfer_modes, k.description);
        const auto& rhs_tuple =
            std::tie(l.index, l.id, l.available_connectors, l.supported_energy_transfer_modes, l.description);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const EVSEInfoObj& k, const EVSEInfoObj& l) {
        return not operator==(k, l);
    }
};

struct EVSEStatusObj {
    float charged_energy_wh;                                ///< charged_energy_wh
    float discharged_energy_wh;                             ///< discharged_energy_wh
    int32_t charging_duration_s;                            ///< charging_duration_s
    bool charging_allowed;                                  ///< charging_allowed
    bool available;                                         ///< available
    int32_t active_connector_index;                         ///< active_connector_index
    bool error_present;                                     ///< error_present
    ChargeProtocolEnum charge_protocol;                     ///< charge_protocol
    EVSEStateEnum state;                                    ///< state
    std::optional<ACChargeParametersObj> ac_charge_param;   ///< ac_charge_param
    std::optional<DCChargeParametersObj> dc_charge_param;   ///< dc_charge_param
    std::optional<ACChargeStatusObj> ac_charge_status;      ///< ac_charge_status
    std::optional<DCChargeStatusObj> dc_charge_status;      ///< dc_charge_status
    std::optional<DisplayParametersObj> display_parameters; ///< display_parameters

    friend constexpr bool operator==(const EVSEStatusObj& k, const EVSEStatusObj& l) {
        const auto& lhs_tuple = std::tie(k.charged_energy_wh, k.discharged_energy_wh, k.charging_duration_s,
                                         k.charging_allowed, k.available, k.active_connector_index, k.error_present,
                                         k.charge_protocol, k.state, k.ac_charge_param, k.dc_charge_param,
                                         k.ac_charge_status, k.dc_charge_status, k.display_parameters);
        const auto& rhs_tuple = std::tie(l.charged_energy_wh, l.discharged_energy_wh, l.charging_duration_s,
                                         l.charging_allowed, l.available, l.active_connector_index, l.error_present,
                                         l.charge_protocol, l.state, l.ac_charge_param, l.dc_charge_param,
                                         l.ac_charge_status, l.dc_charge_status, l.display_parameters);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const EVSEStatusObj& k, const EVSEStatusObj& l) {
        return not operator==(k, l);
    }
};

struct Current_A {
    std::optional<float> L1; ///< AC L1 value only
    std::optional<float> L2; ///< AC L2 value only
    std::optional<float> L3; ///< AC L3 value only
    std::optional<float> N;  ///< AC Neutral value only

    friend constexpr bool operator==(const Current_A& k, const Current_A& l) {
        const auto& lhs_tuple = std::tie(k.L1, k.L2, k.L3, k.N);
        const auto& rhs_tuple = std::tie(l.L1, l.L2, l.L3, l.N);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const Current_A& k, const Current_A& l) {
        return not operator==(k, l);
    }
};

struct Energy_Wh_import {
    float total;             ///< DC / AC Sum value (which is relevant for billing)
    std::optional<float> L1; ///< AC L1 value only
    std::optional<float> L2; ///< AC L2 value only
    std::optional<float> L3; ///< AC L3 value only

    friend constexpr bool operator==(const Energy_Wh_import& k, const Energy_Wh_import& l) {
        const auto& lhs_tuple = std::tie(k.total, k.L1, k.L2, k.L3);
        const auto& rhs_tuple = std::tie(l.total, l.L1, l.L2, l.L3);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const Energy_Wh_import& k, const Energy_Wh_import& l) {
        return not operator==(k, l);
    }
};

struct Energy_Wh_export {
    float total;             ///< DC / AC Sum value (which is relevant for billing)
    std::optional<float> L1; ///< AC L1 value only
    std::optional<float> L2; ///< AC L2 value only
    std::optional<float> L3; ///< AC L3 value only

    friend constexpr bool operator==(const Energy_Wh_export& k, const Energy_Wh_export& l) {
        const auto& lhs_tuple = std::tie(k.total, k.L1, k.L2, k.L3);
        const auto& rhs_tuple = std::tie(l.total, l.L1, l.L2, l.L3);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const Energy_Wh_export& k, const Energy_Wh_export& l) {
        return not operator==(k, l);
    }
};

struct Frequency_Hz {
    float L1;                ///< AC L1 value
    std::optional<float> L2; ///< AC L2 value
    std::optional<float> L3; ///< AC L3 value

    friend constexpr bool operator==(const Frequency_Hz& k, const Frequency_Hz& l) {
        const auto& lhs_tuple = std::tie(k.L1, k.L2, k.L3);
        const auto& rhs_tuple = std::tie(l.L1, l.L2, l.L3);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const Frequency_Hz& k, const Frequency_Hz& l) {
        return not operator==(k, l);
    }
};

struct Power_W {
    float total;             ///< DC / AC Sum value
    std::optional<float> L1; ///< AC L1 value only
    std::optional<float> L2; ///< AC L2 value only
    std::optional<float> L3; ///< AC L3 value only

    friend constexpr bool operator==(const Power_W& k, const Power_W& l) {
        const auto& lhs_tuple = std::tie(k.total, k.L1, k.L2, k.L3);
        const auto& rhs_tuple = std::tie(l.total, l.L1, l.L2, l.L3);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const Power_W& k, const Power_W& l) {
        return not operator==(k, l);
    }
};

struct Voltage_V {
    std::optional<float> L1; ///< AC L1 value only
    std::optional<float> L2; ///< AC L2 value only
    std::optional<float> L3; ///< AC L3 value only

    friend constexpr bool operator==(const Voltage_V& k, const Voltage_V& l) {
        const auto& lhs_tuple = std::tie(k.L1, k.L2, k.L3);
        const auto& rhs_tuple = std::tie(l.L1, l.L2, l.L3);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const Voltage_V& k, const Voltage_V& l) {
        return not operator==(k, l);
    }
};

struct MeterDataObj {
    Energy_Wh_import energy_Wh_import;                ///< Imported energy in Wh (from grid)
    std::string timestamp;                            ///< Timestamp of the meter values, as RFC3339 string
    std::optional<Current_A> current_A;               ///< Current in Ampere
    std::optional<Energy_Wh_export> energy_Wh_export; ///< Exported energy in Wh (to grid)
    std::optional<Frequency_Hz> frequency_Hz;         ///< Grid frequency in Hertz
    std::optional<std::string> meter_id;              ///< TODO: description
    std::optional<std::string> serial_number;         ///< TODO: description
    std::optional<bool> phase_seq_error;              ///< TODO: description
    std::optional<Power_W>
        power_W; ///< Instantaneous power in Watt. Negative values are exported, positive values imported energy.
    std::optional<Voltage_V> voltage_V; ///< Voltage in Volts

    friend constexpr bool operator==(const MeterDataObj& k, const MeterDataObj& l) {
        const auto& lhs_tuple =
            std::tie(k.energy_Wh_import, k.timestamp, k.current_A, k.energy_Wh_export, k.frequency_Hz, k.meter_id,
                     k.serial_number, k.phase_seq_error, k.power_W, k.voltage_V);
        const auto& rhs_tuple =
            std::tie(l.energy_Wh_import, l.timestamp, l.current_A, l.energy_Wh_export, l.frequency_Hz, l.meter_id,
                     l.serial_number, l.phase_seq_error, l.power_W, l.voltage_V);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const MeterDataObj& k, const MeterDataObj& l) {
        return not operator==(k, l);
    }
};

struct HelloResObj {
    bool authentication_required;      ///< Whether authentication is required
    std::string api_version;           ///< Version of the JSON RPC API
    std::string everest_version;       ///< The version of the running EVerest instance
    ChargerInfoObj charger_info;       ///< Charger information
    std::optional<bool> authenticated; ///< Whether the client is properly authenticated

    friend constexpr bool operator==(const HelloResObj& k, const HelloResObj& l) {
        const auto& lhs_tuple =
            std::tie(k.authentication_required, k.api_version, k.everest_version, k.charger_info, k.authenticated);
        const auto& rhs_tuple =
            std::tie(l.authentication_required, l.api_version, l.everest_version, l.charger_info, l.authenticated);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const HelloResObj& k, const HelloResObj& l) {
        return not operator==(k, l);
    }
};

struct ChargePointGetEVSEInfosResObj {
    std::vector<EVSEInfoObj> infos; ///< Array of EVSE infos
    ResponseErrorEnum error;        ///< Response error

    friend constexpr bool operator==(const ChargePointGetEVSEInfosResObj& k, const ChargePointGetEVSEInfosResObj& l) {
        const auto& lhs_tuple = std::tie(k.infos, k.error);
        const auto& rhs_tuple = std::tie(l.infos, l.error);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const ChargePointGetEVSEInfosResObj& k, const ChargePointGetEVSEInfosResObj& l) {
        return not operator==(k, l);
    }
};

struct ChargePointGetActiveErrorsResObj {
    std::vector<ErrorObj> active_errors; ///< Array of active charge point errors
    ResponseErrorEnum error;             ///< Response error

    friend constexpr bool operator==(const ChargePointGetActiveErrorsResObj& k,
                                     const ChargePointGetActiveErrorsResObj& l) {
        const auto& lhs_tuple = std::tie(k.active_errors, k.error);
        const auto& rhs_tuple = std::tie(l.active_errors, l.error);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const ChargePointGetActiveErrorsResObj& k,
                                     const ChargePointGetActiveErrorsResObj& l) {
        return not operator==(k, l);
    }
};

struct EVSEGetInfoResObj {
    EVSEInfoObj info;        ///< TODO: description
    ResponseErrorEnum error; ///< Response error

    friend constexpr bool operator==(const EVSEGetInfoResObj& k, const EVSEGetInfoResObj& l) {
        const auto& lhs_tuple = std::tie(k.info, k.error);
        const auto& rhs_tuple = std::tie(l.info, l.error);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const EVSEGetInfoResObj& k, const EVSEGetInfoResObj& l) {
        return not operator==(k, l);
    }
};

struct EVSEGetStatusResObj {
    EVSEStatusObj status;    ///< TODO: description
    ResponseErrorEnum error; ///< Response error

    friend constexpr bool operator==(const EVSEGetStatusResObj& k, const EVSEGetStatusResObj& l) {
        const auto& lhs_tuple = std::tie(k.status, k.error);
        const auto& rhs_tuple = std::tie(l.status, l.error);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const EVSEGetStatusResObj& k, const EVSEGetStatusResObj& l) {
        return not operator==(k, l);
    }
};

struct EVSEGetHardwareCapabilitiesResObj {
    HardwareCapabilitiesObj hardware_capabilities; ///< TODO: description
    ResponseErrorEnum error;                       ///< Response error

    friend constexpr bool operator==(const EVSEGetHardwareCapabilitiesResObj& k,
                                     const EVSEGetHardwareCapabilitiesResObj& l) {
        const auto& lhs_tuple = std::tie(k.hardware_capabilities, k.error);
        const auto& rhs_tuple = std::tie(l.hardware_capabilities, l.error);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const EVSEGetHardwareCapabilitiesResObj& k,
                                     const EVSEGetHardwareCapabilitiesResObj& l) {
        return not operator==(k, l);
    }
};

struct EVSEGetMeterDataResObj {
    MeterDataObj meter_data; ///< TODO: description
    ResponseErrorEnum error; ///< Response error

    friend constexpr bool operator==(const EVSEGetMeterDataResObj& k, const EVSEGetMeterDataResObj& l) {
        const auto& lhs_tuple = std::tie(k.meter_data, k.error);
        const auto& rhs_tuple = std::tie(l.meter_data, l.error);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const EVSEGetMeterDataResObj& k, const EVSEGetMeterDataResObj& l) {
        return not operator==(k, l);
    }
};

struct ErrorResObj {
    ResponseErrorEnum error; ///< Response error

    friend constexpr bool operator==(const ErrorResObj& k, const ErrorResObj& l) {
        const auto& lhs_tuple = std::tie(k.error);
        const auto& rhs_tuple = std::tie(l.error);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const ErrorResObj& k, const ErrorResObj& l) {
        return not operator==(k, l);
    }
};

struct ChargePointActiveErrorsChangedObj {
    std::vector<ErrorObj> active_errors; ///< Array of active charge point errors

    friend constexpr bool operator==(const ChargePointActiveErrorsChangedObj& k,
                                     const ChargePointActiveErrorsChangedObj& l) {
        const auto& lhs_tuple = std::tie(k.active_errors);
        const auto& rhs_tuple = std::tie(l.active_errors);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const ChargePointActiveErrorsChangedObj& k,
                                     const ChargePointActiveErrorsChangedObj& l) {
        return not operator==(k, l);
    }
};

struct EVSEHardwareCapabilitiesChangedObj {
    int32_t evse_index;                            ///< Index of the EVSE
    HardwareCapabilitiesObj hardware_capabilities; ///< TODO: description

    friend constexpr bool operator==(const EVSEHardwareCapabilitiesChangedObj& k,
                                     const EVSEHardwareCapabilitiesChangedObj& l) {
        const auto& lhs_tuple = std::tie(k.evse_index, k.hardware_capabilities);
        const auto& rhs_tuple = std::tie(l.evse_index, l.hardware_capabilities);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const EVSEHardwareCapabilitiesChangedObj& k,
                                     const EVSEHardwareCapabilitiesChangedObj& l) {
        return not operator==(k, l);
    }
};

struct EVSEStatusChangedObj {
    int32_t evse_index;        ///< Index of the EVSE
    EVSEStatusObj evse_status; ///< TODO: description

    friend constexpr bool operator==(const EVSEStatusChangedObj& k, const EVSEStatusChangedObj& l) {
        const auto& lhs_tuple = std::tie(k.evse_index, k.evse_status);
        const auto& rhs_tuple = std::tie(l.evse_index, l.evse_status);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const EVSEStatusChangedObj& k, const EVSEStatusChangedObj& l) {
        return not operator==(k, l);
    }
};

struct EVSEMeterDataChangedObj {
    int32_t evse_index;      ///< Index of the EVSE
    MeterDataObj meter_data; ///< TODO: description

    friend constexpr bool operator==(const EVSEMeterDataChangedObj& k, const EVSEMeterDataChangedObj& l) {
        const auto& lhs_tuple = std::tie(k.evse_index, k.meter_data);
        const auto& rhs_tuple = std::tie(l.evse_index, l.meter_data);
        return lhs_tuple == rhs_tuple;
    }

    friend constexpr bool operator!=(const EVSEMeterDataChangedObj& k, const EVSEMeterDataChangedObj& l) {
        return not operator==(k, l);
    }
};

} // namespace everest::lib::API::V1_0::types::json_rpc_api
