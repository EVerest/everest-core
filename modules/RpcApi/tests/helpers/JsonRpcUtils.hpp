#ifndef JSON_RPC_UTILS_HPP
#define JSON_RPC_UTILS_HPP

#include <nlohmann/json.hpp>

namespace json_rpc_utils {

inline nlohmann::json create_json_rpc_request(const std::string& method, const nlohmann::json& params, int id) {
    nlohmann::json request;
    request["jsonrpc"] = "2.0";
    request["method"] = method;
    request["params"] = params;
    request["id"] = id;
    return request;
}

inline nlohmann::json create_json_rpc_response(const nlohmann::json& result, int id) {
    nlohmann::json response;
    response["jsonrpc"] = "2.0";
    response["result"] = result;
    response["id"] = id;
    return response;
}

inline nlohmann::json create_json_rpc_error_response(int code, const std::string& message, int id) {
    nlohmann::json error_response;
    error_response["jsonrpc"] = "2.0";
    error_response["error"]["code"] = code;
    error_response["error"]["message"] = message;
    error_response["id"] = id;
    return error_response;
}

} // namespace json_rpc_utils

#endif // JSON_RPC_UTILS_HPP