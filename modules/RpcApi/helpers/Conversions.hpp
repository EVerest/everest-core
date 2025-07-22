// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest
#ifndef CONVERSIONS_HPP
#define CONVERSIONS_HPP

#include <generated/types/json_rpc_api.hpp>
#include <generated/types/evse_manager.hpp>

#include "ld-ev.hpp"

namespace types {
namespace json_rpc_api {
    EVSEStateEnum evse_manager_session_event_to_evse_state(types::evse_manager::SessionEvent state);
    ChargeProtocolEnum evse_manager_protocol_to_charge_protocol(std::string protocol);
    types::json_rpc_api::ErrorObj everest_error_to_rpc_error(const Everest::error::Error& error_object);
} // namespace json_rpc_api
} // namespace types

#endif // CONVERSIONS_HPP
