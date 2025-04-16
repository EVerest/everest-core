// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef DATATYPES_HPP
#define DATATYPES_HPP

#include <generated/types/json_rpc_api.hpp>

using namespace types::json_rpc_api;

// This contains types for all the JSON objects

namespace data {
// Structs
struct ConnectorInfoObj {
    int id;
    ConnectorTypeEnum type;
    std::optional<std::string> description;
};

struct HardwareCapabilitiesObj {
    float max_current_A_export;
    float max_current_A_import;
    float max_phase_count_export;
    float max_phase_count_import;
    float min_current_A_export;
    float min_current_A_import;
    int min_phase_count_export;
    int min_phase_count_import;
    bool phase_switch_during_charging;
};

struct MeterDataObj {
    struct CurrentA {
        float L1;
        float L2;
        float L3;
        float N;
    };
    struct EnergyWhImport {
        float L1;
        float L2;
        float L3;
        float total;
    };
    struct EnergyWhExport {
        float L1;
        float L2;
        float L3;
        float total;
    };
    struct FrequencyHz {
        float L1;
        float L2;
        float L3;
    };
    struct PowerW {
        float L1;
        float L2;
        float L3;
        float total;
    };
    struct VoltageV {
        float L1;
        float L2;
        float L3;
    };

    CurrentA current_A;
    EnergyWhImport energy_Wh_import;
    EnergyWhExport energy_Wh_export;
    FrequencyHz frequency_Hz;
    std::string meter_id;
    std::optional<std::string> serial_number;
    std::optional<bool> phase_seq_error;
    PowerW power_W;
    double timestamp;
    VoltageV voltage_V;
};

struct ACChargeParametersObj {
    std::optional<float> evse_nominal_voltage;
    std::optional<float> evse_max_current;
    float evse_maximum_charge_power;
    float evse_minimum_charge_power;
    float evse_nominal_frequency;
    std::optional<float> evse_maximum_discharge_power;
    std::optional<float> evse_minimum_discharge_power;
};

struct DCChargeParametersObj {
    float evse_maximum_charge_current;
    float evse_maximum_charge_power;
    float evse_maximum_voltage;
    float evse_minimum_charge_current;
    float evse_minimum_charge_power;
    float evse_minimum_voltage;
    std::optional<float> evse_energy_to_be_delivered;
    std::optional<float> evse_maximum_discharge_current;
    std::optional<float> evse_maximum_discharge_power;
    std::optional<float> evse_minimum_discharge_current;
    std::optional<float> evse_minimum_discharge_power;
};

struct ACChargeLoopObj {
    int evse_active_phase_count;
};

struct DCChargeLoopObj {
    float evse_present_current;
    float evse_present_voltage;
    bool evse_power_limit_achieved;
    bool evse_current_limit_achieved;
    bool evse_voltage_limit_achieved;
};

struct DisplayParametersObj {
    std::optional<int> start_soc;
    std::optional<int> present_soc;
    std::optional<int> minimum_soc;
    std::optional<int> target_soc;
    std::optional<int> maximum_soc;
    std::optional<int> remaining_time_to_minimum_soc;
    std::optional<int> remaining_time_to_target_soc;
    std::optional<int> remaining_time_to_maximum_soc;
    std::optional<bool> charging_complete;
    std::optional<float> battery_energy_capacity;
    std::optional<bool> inlet_hot;
};

struct EVSEStatusObj {
    float charged_energy_wh;
    float discharged_energy_wh;
    int charging_duration_s;
    bool charging_allowed;
    bool available;
    int active_connector_id;
    EVSEErrorEnum evse_error;
    ChargeProtocolEnum charge_protocol;
    std::optional<ACChargeParametersObj> ac_charge_param;
    std::optional<DCChargeParametersObj> dc_charge_param;
    std::optional<ACChargeLoopObj> ac_charge_loop;
    std::optional<DCChargeLoopObj> dc_charge_loop;
    std::optional<DisplayParametersObj> display_parameters;
    EVSEStateEnum state;
};

struct EVSEInfoObj {
    std::string id;
    std::optional<std::string> description;
    std::vector<ConnectorInfoObj> available_connectors;
    bool bidi_charging;
};

// Method Signatures (Pseudo RPC)

// ChargePoint.GetEVSEInfos
struct ChargePointGetEVSEInfosResponse {
    std::vector<EVSEInfoObj> infos;
    ResponseErrorEnum error;
};
ChargePointGetEVSEInfosResponse ChargePoint_GetEVSEInfos();

// EVSE.GetInfo
struct EVSEGetInfoResponse {
    EVSEInfoObj status;
    ResponseErrorEnum error;
};
EVSEGetInfoResponse EVSE_GetInfo(const std::string& evse_id);

// EVSE.GetStatus
struct EVSEGetStatusResponse {
    EVSEStatusObj status;
    ResponseErrorEnum error;
};
} // namespace data

#endif // DATATYPES_HPP
