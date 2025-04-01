// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

// This contains types for all the JSON objects
#include <optional>
#include <string>
#include <vector>

namespace data {

// automatically created

// Enums
enum class ResponseErrorEnum {
    NoError,
    ErrorInvalidParameter,
    ErrorOutOfRange,
    ErrorValuesNotApplied,
    ErrorInvalidEVSEID,
    ErrorNoDataAvailable,
    ErrorUnknownError
};

enum class ChargeProtocolEnum {
    Unknown,
    IEC61851,
    DIN70121,
    ISO15118,
    ISO15118_20
};

enum class EVSEStateEnum {
    Unplugged,
    Disabled,
    Preparing,
    Reserved,
    AuthRequired,
    WaitingForEnergy,
    Charging,
    ChargingPausedEV,
    ChargingPausedEVSE,
    Finished
};

enum class ConnectorTypeEnum {
    ACSinglePhaseCable,
    ACThreePhaseCable,
    ACSinglePhaseSocket,
    ACThreePhaseSocket,
    DCCore,
    DCExtended,
    DCDual2,
    DCDual4,
    Other
};

enum class EVSEErrorEnum {
    NoError,
    power_supply_DC_HardwareFault,
    power_supply_DC_OverTemperature,
    power_supply_DC_UnderTemperature,
    power_supply_DC_UnderVoltageAC,
    power_supply_DC_OverVoltageAC,
    power_supply_DC_UnderVoltageDC,
    power_supply_DC_OverVoltageDC,
    power_supply_DC_OverCurrentAC,
    power_supply_DC_OverCurrentDC,
    power_supply_DC_VendorError,
    power_supply_DC_VendorWarning,
    evse_board_support_MREC2GroundFailure,
    evse_board_support_MREC3HighTemperature,
    evse_board_support_MREC4OverCurrentFailure,
    evse_board_support_MREC5OverVoltage,
    evse_board_support_MREC6UnderVoltage,
    evse_board_support_MREC8EmergencyStop,
    evse_board_support_MREC10InvalidVehicleMode,
    evse_board_support_MREC14PilotFault,
    evse_board_support_MREC15PowerLoss,
    evse_board_support_MREC17EVSEContactorFault,
    evse_board_support_MREC18CableOverTempDerate,
    evse_board_support_MREC19CableOverTempStop,
    evse_board_support_MREC20PartialInsertion,
    evse_board_support_MREC23ProximityFault,
    evse_board_support_MREC24ConnectorVoltageHigh,
    evse_board_support_MREC25BrokenLatch,
    evse_board_support_MREC26CutCable
    // ... Add remaining EVSEErrorEnum values
};

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

struct ChargerInfoObj {
    std::string vendor;
    std::string model;
    std::string serial;
    std::optional<std::string> friendly_name;
    std::optional<std::string> manufacturer;
    std::optional<std::string> manufacturer_url;
    std::optional<std::string> model_url;
    std::optional<std::string> model_no;
    std::optional<std::string> revision;
    std::optional<std::string> board_revision;
    std::string firmware_version;
};

struct APIHelloResponse {
    bool authentication_required;
    bool authenticated;
    std::optional<std::vector<std::string>> permission_scopes; // Placeholder until PermissionScopes is defined
    std::string api_version;
    std::string everst_version;
    ChargerInfoObj charger_info;
};

// Method Signatures (Pseudo RPC)

// API.Hello
APIHelloResponse API_Hello();

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
EVSEGetStatusResponse EVSE_GetStatus(const std::string& evse_id);

// EVSE.GetHardwareCapabilities
struct EVSEGetHardwareCapabilitiesResponse {
    HardwareCapabilitiesObj hardware_capabilities;
    ResponseErrorEnum error;
};
EVSEGetHardwareCapabilitiesResponse EVSE_GetHardwareCapabilities(const std::string& evse_id);

// EVSE.SetChargingAllowed
ResponseErrorEnum EVSE_SetChargingAllowed(const std::string& evse_id, bool charging_allowed);

// EVSE.GetMeterData
struct EVSEGetMeterDataResponse {
    MeterDataObj meter_data;
    ResponseErrorEnum error;
};
EVSEGetMeterDataResponse EVSE_GetMeterData(const std::string& evse_id);

// EVSE.SetACCharging
ResponseErrorEnum EVSE_SetACCharging(bool charging_allowed, float max_current, std::optional<int> phase_count);

// EVSE.SetACChargingCurrent
ResponseErrorEnum EVSE_SetACChargingCurrent(const std::string& evse_id, float max_current);

// EVSE.SetACChargingPhaseCount
ResponseErrorEnum EVSE_SetACChargingPhaseCount(const std::string& evse_id, int phase_count);

// EVSE.SetDCCharging
ResponseErrorEnum EVSE_SetDCCharging(const std::string& evse_id, bool charging_allowed, int max_power);

// EVSE.SetDCChargingPower
ResponseErrorEnum EVSE_SetDCChargingPower(const std::string& evse_id, int max_power);

// EVSE.EnableConnector
ResponseErrorEnum EVSE_EnableConnector(int connector_id, bool enable, int priority);

// Notification Structures (Pseudo RPC)
void EVSE_HardwareCapabilitiesChanged(const std::string& evse_id, const HardwareCapabilitiesObj& hardware_capabilities);
void EVSE_StatusChanged(const std::string& evse_id, const EVSEStatusObj& evse_status);
void EVSE_MeterDataChanged(const std::string& evse_id, const MeterDataObj& meter_data);

} // namespace data
