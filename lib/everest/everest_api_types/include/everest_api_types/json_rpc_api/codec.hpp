// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "API.hpp"
#include <optional>
#include <string>

namespace everest::lib::API::V1_0::types::json_rpc_api {

std::string response_error_enum_to_string(ResponseErrorEnum e);
ResponseErrorEnum string_to_response_error_enum(const std::string& s);
std::string charge_protocol_enum_to_string(ChargeProtocolEnum e);
ChargeProtocolEnum string_to_charge_protocol_enum(const std::string& s);
std::string evsestate_enum_to_string(EVSEStateEnum e);
EVSEStateEnum string_to_evsestate_enum(const std::string& s);
std::string connector_type_enum_to_string(ConnectorTypeEnum e);
ConnectorTypeEnum string_to_connector_type_enum(const std::string& s);
std::string energy_transfer_mode_enum_to_string(EnergyTransferModeEnum e);
EnergyTransferModeEnum string_to_energy_transfer_mode_enum(const std::string& s);
std::string severity_to_string(Severity e);
Severity string_to_severity(const std::string& s);

std::string serialize(ResponseErrorEnum val) noexcept;
std::string serialize(ChargeProtocolEnum val) noexcept;
std::string serialize(EVSEStateEnum val) noexcept;
std::string serialize(ConnectorTypeEnum val) noexcept;
std::string serialize(EnergyTransferModeEnum val) noexcept;
std::string serialize(Severity val) noexcept;
std::string serialize(ImplementationIdentifier const& val) noexcept;
std::string serialize(ChargerInfoObj const& val) noexcept;
std::string serialize(ErrorObj const& val) noexcept;
std::string serialize(ConnectorInfoObj const& val) noexcept;
std::string serialize(ACChargeParametersObj const& val) noexcept;
std::string serialize(DCChargeParametersObj const& val) noexcept;
std::string serialize(ACChargeStatusObj const& val) noexcept;
std::string serialize(DCChargeStatusObj const& val) noexcept;
std::string serialize(DisplayParametersObj const& val) noexcept;
std::string serialize(HardwareCapabilitiesObj const& val) noexcept;
std::string serialize(EVSEInfoObj const& val) noexcept;
std::string serialize(EVSEStatusObj const& val) noexcept;
std::string serialize(Current_A const& val) noexcept;
std::string serialize(Energy_Wh_import const& val) noexcept;
std::string serialize(Energy_Wh_export const& val) noexcept;
std::string serialize(Frequency_Hz const& val) noexcept;
std::string serialize(Power_W const& val) noexcept;
std::string serialize(Voltage_V const& val) noexcept;
std::string serialize(MeterDataObj const& val) noexcept;
std::string serialize(HelloResObj const& val) noexcept;
std::string serialize(ChargePointGetEVSEInfosResObj const& val) noexcept;
std::string serialize(ChargePointGetActiveErrorsResObj const& val) noexcept;
std::string serialize(EVSEGetInfoResObj const& val) noexcept;
std::string serialize(EVSEGetStatusResObj const& val) noexcept;
std::string serialize(EVSEGetHardwareCapabilitiesResObj const& val) noexcept;
std::string serialize(EVSEGetMeterDataResObj const& val) noexcept;
std::string serialize(ErrorResObj const& val) noexcept;
std::string serialize(ChargePointActiveErrorsChangedObj const& val) noexcept;
std::string serialize(EVSEHardwareCapabilitiesChangedObj const& val) noexcept;
std::string serialize(EVSEStatusChangedObj const& val) noexcept;
std::string serialize(EVSEMeterDataChangedObj const& val) noexcept;

std::ostream& operator<<(std::ostream& os, ResponseErrorEnum const& val);
std::ostream& operator<<(std::ostream& os, ChargeProtocolEnum const& val);
std::ostream& operator<<(std::ostream& os, EVSEStateEnum const& val);
std::ostream& operator<<(std::ostream& os, ConnectorTypeEnum const& val);
std::ostream& operator<<(std::ostream& os, EnergyTransferModeEnum const& val);
std::ostream& operator<<(std::ostream& os, Severity const& val);
std::ostream& operator<<(std::ostream& os, ImplementationIdentifier const& val);
std::ostream& operator<<(std::ostream& os, ChargerInfoObj const& val);
std::ostream& operator<<(std::ostream& os, ErrorObj const& val);
std::ostream& operator<<(std::ostream& os, ConnectorInfoObj const& val);
std::ostream& operator<<(std::ostream& os, ACChargeParametersObj const& val);
std::ostream& operator<<(std::ostream& os, DCChargeParametersObj const& val);
std::ostream& operator<<(std::ostream& os, ACChargeStatusObj const& val);
std::ostream& operator<<(std::ostream& os, DCChargeStatusObj const& val);
std::ostream& operator<<(std::ostream& os, DisplayParametersObj const& val);
std::ostream& operator<<(std::ostream& os, HardwareCapabilitiesObj const& val);
std::ostream& operator<<(std::ostream& os, EVSEInfoObj const& val);
std::ostream& operator<<(std::ostream& os, EVSEStatusObj const& val);
std::ostream& operator<<(std::ostream& os, Current_A const& val);
std::ostream& operator<<(std::ostream& os, Energy_Wh_import const& val);
std::ostream& operator<<(std::ostream& os, Energy_Wh_export const& val);
std::ostream& operator<<(std::ostream& os, Frequency_Hz const& val);
std::ostream& operator<<(std::ostream& os, Power_W const& val);
std::ostream& operator<<(std::ostream& os, Voltage_V const& val);
std::ostream& operator<<(std::ostream& os, MeterDataObj const& val);
std::ostream& operator<<(std::ostream& os, HelloResObj const& val);
std::ostream& operator<<(std::ostream& os, ChargePointGetEVSEInfosResObj const& val);
std::ostream& operator<<(std::ostream& os, ChargePointGetActiveErrorsResObj const& val);
std::ostream& operator<<(std::ostream& os, EVSEGetInfoResObj const& val);
std::ostream& operator<<(std::ostream& os, EVSEGetStatusResObj const& val);
std::ostream& operator<<(std::ostream& os, EVSEGetHardwareCapabilitiesResObj const& val);
std::ostream& operator<<(std::ostream& os, EVSEGetMeterDataResObj const& val);
std::ostream& operator<<(std::ostream& os, ErrorResObj const& val);
std::ostream& operator<<(std::ostream& os, ChargePointActiveErrorsChangedObj const& val);
std::ostream& operator<<(std::ostream& os, EVSEHardwareCapabilitiesChangedObj const& val);
std::ostream& operator<<(std::ostream& os, EVSEStatusChangedObj const& val);
std::ostream& operator<<(std::ostream& os, EVSEMeterDataChangedObj const& val);

template <class T> T deserialize(std::string const& val);

template <class T> std::optional<T> try_deserialize(std::string const& val) noexcept {
    try {
        return deserialize<T>(val);
    } catch (...) {
        return std::nullopt;
    }
}

template <class T> bool adl_deserialize(std::string const& json_data, T& obj) {
    auto opt = try_deserialize<T>(json_data);
    if (opt) {
        obj = opt.value();
        return true;
    }
    return false;
}

} // namespace everest::lib::API::V1_0::types::json_rpc_api
