// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "nlohmann/json_fwd.hpp"
#include <everest_api_types/json_rpc_api/API.hpp>

namespace everest::lib::API::V1_0::types::json_rpc_api {

using json = nlohmann::json;

void from_json(const json& j, ResponseErrorEnum& k);
void to_json(json& j, const ResponseErrorEnum& k) noexcept;

void from_json(const json& j, ChargeProtocolEnum& k);
void to_json(json& j, const ChargeProtocolEnum& k) noexcept;

void from_json(const json& j, EVSEStateEnum& k);
void to_json(json& j, const EVSEStateEnum& k) noexcept;

void from_json(const json& j, ConnectorTypeEnum& k);
void to_json(json& j, const ConnectorTypeEnum& k) noexcept;

void from_json(const json& j, EnergyTransferModeEnum& k);
void to_json(json& j, const EnergyTransferModeEnum& k) noexcept;

void from_json(const json& j, Severity& k);
void to_json(json& j, const Severity& k) noexcept;

void from_json(const json& j, ImplementationIdentifier& k);
void to_json(json& j, const ImplementationIdentifier& k) noexcept;

void from_json(const json& j, ChargerInfoObj& k);
void to_json(json& j, const ChargerInfoObj& k) noexcept;

void from_json(const json& j, ErrorObj& k);
void to_json(json& j, const ErrorObj& k) noexcept;

void from_json(const json& j, ConnectorInfoObj& k);
void to_json(json& j, const ConnectorInfoObj& k) noexcept;

void from_json(const json& j, ACChargeParametersObj& k);
void to_json(json& j, const ACChargeParametersObj& k) noexcept;

void from_json(const json& j, DCChargeParametersObj& k);
void to_json(json& j, const DCChargeParametersObj& k) noexcept;

void from_json(const json& j, ACChargeStatusObj& k);
void to_json(json& j, const ACChargeStatusObj& k) noexcept;

void from_json(const json& j, DCChargeStatusObj& k);
void to_json(json& j, const DCChargeStatusObj& k) noexcept;

void from_json(const json& j, DisplayParametersObj& k);
void to_json(json& j, const DisplayParametersObj& k) noexcept;

void from_json(const json& j, HardwareCapabilitiesObj& k);
void to_json(json& j, const HardwareCapabilitiesObj& k) noexcept;

void from_json(const json& j, EVSEInfoObj& k);
void to_json(json& j, const EVSEInfoObj& k) noexcept;

void from_json(const json& j, EVSEStatusObj& k);
void to_json(json& j, const EVSEStatusObj& k) noexcept;

void from_json(const json& j, Current_A& k);
void to_json(json& j, const Current_A& k) noexcept;

void from_json(const json& j, Energy_Wh_import& k);
void to_json(json& j, const Energy_Wh_import& k) noexcept;

void from_json(const json& j, Energy_Wh_export& k);
void to_json(json& j, const Energy_Wh_export& k) noexcept;

void from_json(const json& j, Frequency_Hz& k);
void to_json(json& j, const Frequency_Hz& k) noexcept;

void from_json(const json& j, Power_W& k);
void to_json(json& j, const Power_W& k) noexcept;

void from_json(const json& j, Voltage_V& k);
void to_json(json& j, const Voltage_V& k) noexcept;

void from_json(const json& j, MeterDataObj& k);
void to_json(json& j, const MeterDataObj& k) noexcept;

void from_json(const json& j, HelloResObj& k);
void to_json(json& j, const HelloResObj& k) noexcept;

void from_json(const json& j, ChargePointGetEVSEInfosResObj& k);
void to_json(json& j, const ChargePointGetEVSEInfosResObj& k) noexcept;

void from_json(const json& j, ChargePointGetActiveErrorsResObj& k);
void to_json(json& j, const ChargePointGetActiveErrorsResObj& k) noexcept;

void from_json(const json& j, EVSEGetInfoResObj& k);
void to_json(json& j, const EVSEGetInfoResObj& k) noexcept;

void from_json(const json& j, EVSEGetStatusResObj& k);
void to_json(json& j, const EVSEGetStatusResObj& k) noexcept;

void from_json(const json& j, EVSEGetHardwareCapabilitiesResObj& k);
void to_json(json& j, const EVSEGetHardwareCapabilitiesResObj& k) noexcept;

void from_json(const json& j, EVSEGetMeterDataResObj& k);
void to_json(json& j, const EVSEGetMeterDataResObj& k) noexcept;

void from_json(const json& j, ErrorResObj& k);
void to_json(json& j, const ErrorResObj& k) noexcept;

void from_json(const json& j, ChargePointActiveErrorsChangedObj& k);
void to_json(json& j, const ChargePointActiveErrorsChangedObj& k) noexcept;

void from_json(const json& j, EVSEHardwareCapabilitiesChangedObj& k);
void to_json(json& j, const EVSEHardwareCapabilitiesChangedObj& k) noexcept;

void from_json(const json& j, EVSEStatusChangedObj& k);
void to_json(json& j, const EVSEStatusChangedObj& k) noexcept;

void from_json(const json& j, EVSEMeterDataChangedObj& k);
void to_json(json& j, const EVSEMeterDataChangedObj& k) noexcept;

} // namespace everest::lib::API::V1_0::types::json_rpc_api
