// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest
#ifndef CONVERSIONS_HPP
#define CONVERSIONS_HPP

#include <generated/types/evse_manager.hpp>
#include <generated/types/json_rpc_api.hpp>

#include "ld-ev.hpp"

namespace types {
namespace json_rpc_api {
EVSEStateEnum evse_manager_session_event_to_evse_state(types::evse_manager::SessionEvent state);
ChargeProtocolEnum evse_manager_protocol_to_charge_protocol(std::string protocol);
types::json_rpc_api::ErrorObj everest_error_to_rpc_error(const Everest::error::Error& error_object);
std::vector<types::json_rpc_api::EnergyTransferModeEnum> iso15118_energy_transfer_modes_to_json_rpc_api(const std::vector<types::iso15118::EnergyTransferMode>& supported_energy_transfer_modes, bool& is_ac_transfer_mode);

/**
 * @brief Serializes an EnergyTransferModeEnum object to a JSON representation.
 *
 * This function converts the given EnergyTransferModeEnum value into its corresponding
 * JSON format and assigns it to the provided json object. This function is necessary
 * for properly serializing the data for JSON-RPC API responses.
 *
 * @param j Reference to a json object where the serialized data will be stored.
 * @param k The EnergyTransferModeEnum value to be serialized.
 */
void to_json(json& j, const types::json_rpc_api::EnergyTransferModeEnum& k);
} // namespace json_rpc_api
} // namespace types

#endif // CONVERSIONS_HPP
