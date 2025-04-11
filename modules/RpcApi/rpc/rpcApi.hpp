// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#ifndef RPC_API_HPP
#define RPC_API_HPP

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

// {
//     "authentication_required": "bool",
//     o:"authenticated": "bool",
//     o:"permission_scopes": "$ref:PermissionScopes" (Not yet defined),
//     "api_version": "string" (e.g. 1.0),
//     "everst_version": "string" (e.g. 2024.9.0),
//     "charger_info": "$ChargerInfoObj"
// }


namespace rpc {


struct clientHelloRes {
    bool authentication_required;
    bool authenticated;
    std::string api_version;
    std::string everest_version;
    // ChargerInfoObj charger_info;
};

inline void to_json(nlohmann::json &j, const clientHelloRes &r) {
    j = nlohmann::json{
        {"authentication_required", r.authentication_required},
        {"authenticated", r.authenticated},
        {"api_version", r.api_version},
        {"everest_version", r.everest_version}
    };
}
inline void from_json(const nlohmann::json &j, clientHelloRes &r) {
    j.at("authentication_required").get_to(r.authentication_required);
    j.at("authenticated").get_to(r.authenticated);
    j.at("api_version").get_to(r.api_version);
    j.at("everest_version").get_to(r.everest_version);
}

class RpcApi {
public:
    // Constructor and Destructor
    RpcApi();
    ~RpcApi();
    // Methods
    clientHelloRes hello();

};
}

#endif // RPC_API_HPP