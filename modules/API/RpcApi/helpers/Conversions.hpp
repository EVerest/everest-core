// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest
#ifndef CONVERSIONS_HPP
#define CONVERSIONS_HPP

#include <everest_api_types/json_rpc_api/codec.hpp>
#include <generated/types/evse_manager.hpp>
#include <generated/types/iso15118.hpp>

#include <utils/error.hpp>
#include <vector>

namespace everest::lib::API::V1_0::types::json_rpc_api {
EVSEStateEnum evse_manager_session_event_to_evse_state(::types::evse_manager::SessionEvent state);
ChargeProtocolEnum evse_manager_protocol_to_charge_protocol(const std::string& protocol);
ErrorObj everest_error_to_rpc_error(const Everest::error::Error& error_object);
std::vector<EnergyTransferModeEnum> iso15118_energy_transfer_modes_to_json_rpc_api(
    const std::vector<::types::iso15118::EnergyTransferMode>& supported_energy_transfer_modes,
    bool& is_ac_transfer_mode);
} // namespace everest::lib::API::V1_0::types::json_rpc_api

#endif // CONVERSIONS_HPP
