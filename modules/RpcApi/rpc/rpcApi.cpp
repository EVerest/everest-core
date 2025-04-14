#include "rpcApi.hpp"

namespace rpc {

RpcApi::RpcApi() {
    // Constructor implementation
}
RpcApi::~RpcApi() {
    // Destructor implementation
}

types::json_rpc_api::HelloResObj RpcApi::hello() {
    types::json_rpc_api::HelloResObj res;
    res.authentication_required = false;
    // std::optional<bool> authenticated;

    // initializing the optional authenticated value std::optional<bool> authenticated;
    // FIXME clarify what happens if non-optional members are not explicitly initialized
    res.authenticated = true;
    res.api_version = "1.0";
    res.everest_version = "2024.9.0";
    res.charger_info.vendor = "chargebyte";
    res.charger_info.model = "DavyBox";
    res.charger_info.serial = "1";
    res.charger_info.firmware_version = "1.1.1beta";
    return res;
}
}

