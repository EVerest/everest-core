// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest
#ifndef ERROR_HANDLER_HPP
#define ERROR_HANDLER_HPP

#include "../data/DataStore.hpp"

namespace everest_api_types = everest::lib::API::V1_0::types;
namespace RPCDataTypes = everest_api_types::json_rpc_api;
namespace helpers {
void handle_error_raised(data::DataStoreCharger& data, const RPCDataTypes::ErrorObj& error);
void handle_error_cleared(data::DataStoreCharger& data, const RPCDataTypes::ErrorObj& error);
} // namespace helpers
#endif // ERROR_HANDLER_HPP
