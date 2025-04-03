// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef METHODS_HPP
#define METHODS_HPP

// drafts for methods, possibly obsolete
namespace data {
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

#endif // METHODS_HPP
