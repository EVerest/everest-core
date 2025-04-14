// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#ifndef RPC_API_HPP
#define RPC_API_HPP

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

#include <generated/types/json_rpc_api.hpp>

// {
//     "authentication_required": "bool",
//     o:"authenticated": "bool",
//     o:"permission_scopes": "$ref:PermissionScopes" (Not yet defined),
//     "api_version": "string" (e.g. 1.0),
//     "everst_version": "string" (e.g. 2024.9.0),
//     "charger_info": "$ChargerInfoObj"
// }


namespace rpc {
class RpcApi {
public:
    // Constructor and Destructor
    RpcApi();
    ~RpcApi();
    // Methods
    types::json_rpc_api::HelloResObj hello();

};
}

#endif // RPC_API_HPP