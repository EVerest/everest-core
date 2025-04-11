#include "rpcApi.hpp"

namespace rpc {

RpcApi::RpcApi() {
    // Constructor implementation
}
RpcApi::~RpcApi() {
    // Destructor implementation
}

clientHelloRes RpcApi::hello() {
    clientHelloRes res;
    res.authentication_required = false;
    // std::optional<bool> authenticated;

    // initializing the optional authenticated value std::optional<bool> authenticated;
    res.authenticated = true;
    res.api_version = "1.0";
    res.everest_version = "2024.9.0";
    // ChargerInfoObj charger_info; // Not yet defined
    return res;
}
}

