// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest
#ifndef ERROR_HANDLER_HPP
#define ERROR_HANDLER_HPP

#include "../data/DataStore.hpp"

namespace helpers {
void handle_error_raised(data::DataStoreCharger& data, const types::json_rpc_api::ErrorObj& error);
void handle_error_cleared(data::DataStoreCharger& data, const types::json_rpc_api::ErrorObj& error);
} // namespace helpers
#endif // ERROR_HANDLER_HPP
