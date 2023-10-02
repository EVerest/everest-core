// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest

#include "rpc.hpp"

#include "command_api.hpp"
#include "ipc.hpp"

enum class RPCRequestErrorCode {
    ParseError = -32700,
    InvalidRequest = -32600,
    MethodNotFound = -32601,
    InvalidParams = -32602,
    InternalError = -32603
};

class RPCRequestError : public std::runtime_error {
public:
    RPCRequestError(RPCRequestErrorCode error_code, const std::string& message, const nlohmann::json& id) :
        std::runtime_error(
            "Error code: " + std::to_string(static_cast<std::underlying_type_t<RPCRequestErrorCode>>(error_code)) +
            "\n" + message),
        error_code(error_code),
        message(message),
        id(id) {
    }

    RPCRequestErrorCode get_error_code() const {
        return error_code;
    }

    const std::string& get_message() const {
        return this->message;
    }

    const nlohmann::json& get_id() const {
        return this->id;
    }

private:
    RPCRequestErrorCode error_code;
    std::string message;
    nlohmann::json id;
};

class RPCRequest {
public:
    RPCRequest(const std::string& request_string) {
        const auto request = nlohmann::json::parse(request_string, nullptr, false, false);

        if (request.is_discarded()) {
            throw RPCRequestError(RPCRequestErrorCode::ParseError, "", nullptr);
        }

        this->id = request.value("id", nlohmann::json(nullptr));

        if (!this->id.is_number() && !this->id.is_string() && !this->id.is_null()) {
            throw RPCRequestError(RPCRequestErrorCode::InvalidRequest, "Invalid ID type", nullptr);
        }

        if (!request.contains("method") || !request.at("method").is_string()) {
            throw RPCRequestError(RPCRequestErrorCode::InvalidRequest, "Missing 'method' key or invalid type",
                                  this->id);
        }

        this->method = request.at("method");
        this->params = request.value("params", nlohmann::json(nullptr));
    }

    bool is_notification() const {
        return this->id.is_null();
    }

    std::string get_method() const {
        return this->method;
    }

    const nlohmann::json& get_params() const {
        return this->params;
    }

    const nlohmann::json& get_id() const {
        return this->id;
    }

private:
    nlohmann::json id;
    std::string method;
    nlohmann::json params;
};

RPC::RPC(int ipc_fd, const CommandApi::Config& config) :
    ipc_fd(ipc_fd), rpc_timeout(std::chrono::milliseconds(config.controller_rpc_timeout_ms)) {
    this->api = std::make_unique<CommandApi>(config, *this);
}

void RPC::run(const NotificationHandler& notification_handler) {
    if (!notification_handler) {
        throw std::runtime_error("Could not run the RPC loop with a null notification handler");
    }

    while (true) {
        // polling on command api ..
        auto msg = Everest::controller_ipc::receive_message(this->ipc_fd);

        if (msg.status != Everest::controller_ipc::MESSAGE_RETURN_STATUS::OK) {
            // FIXME (aw): proper error handling!
            continue;
        }

        const auto& payload = msg.json;
        if (!payload.contains("id")) {
            // probably only a simple notification
            this->notification_handler(payload);
        }

        // otherwise a result
        const std::mt19937::result_type id = payload.at("id");

        const std::lock_guard<std::mutex> lock(this->ipc_mutex);
        auto call = this->ipc_calls.find(id);

        if (call == this->ipc_calls.end()) {
            // does not exists (anymore)
            continue;
        }

        try {
            call->second.set_value(payload.value("result", nlohmann::json(nullptr)));
        } catch (const std::future_error& e) {
            if (e.code() == std::future_errc::promise_already_satisfied) {
                // this could, but should not happen
                continue;
            }
            throw;
        }
    }
}

nlohmann::json RPC::handle_json_rpc(const std::string& payload) {
    // FIXME (aw): these nested tries look like code smell
    try {
        const auto rpc_request = RPCRequest(payload);

        try {
            nlohmann::json reply{
                {"id", rpc_request.get_id()},
                {"result", this->api->handle(rpc_request.get_method(), rpc_request.get_params())},
            };

            if (rpc_request.is_notification()) {
                return nullptr;
            } else {
                return reply;
            }
        } catch (const CommandApiParamsError& e) {
            throw RPCRequestError(RPCRequestErrorCode::InvalidParams, e.what(), rpc_request.get_id());
        } catch (const CommandApiMethodNotFound& e) {
            throw RPCRequestError(RPCRequestErrorCode::MethodNotFound, e.what(), rpc_request.get_id());
        } catch (const std::exception& e) {
            throw RPCRequestError(RPCRequestErrorCode::InternalError, e.what(), rpc_request.get_id());
        }
    } catch (const RPCRequestError& e) {
        const auto error_code = e.get_error_code();
        const auto error_code_int = static_cast<std::underlying_type_t<RPCRequestErrorCode>>(error_code);
        return {
            {"error",
             {
                 {"code", error_code_int},
                 {"message", e.get_message()},
             }},
            {"id", e.get_id()},
        };
    }
}

nlohmann::json RPC::ipc_request(const std::string& method, const nlohmann::json& params, bool only_notify) {
    if (only_notify) {
        Everest::controller_ipc::send_message(this->ipc_fd, {{"method", method}, {"params", params}});
        return nullptr;
    }

    std::unique_lock<std::mutex> lock(this->ipc_mutex);
    auto new_call = this->ipc_calls.emplace(this->rng(), std::promise<nlohmann::json>());
    while (!new_call.second) {
        new_call = this->ipc_calls.emplace(this->rng(), std::promise<nlohmann::json>());
    }

    auto id = new_call.first->first;
    auto call_result_future = std::move(new_call.first->second.get_future());

    lock.unlock();

    Everest::controller_ipc::send_message(this->ipc_fd, {{"method", method}, {"params", params}, {"id", id}});

    auto status = call_result_future.wait_for(this->rpc_timeout);

    lock.lock();
    this->ipc_calls.erase(new_call.first);
    lock.unlock();

    if (status == std::future_status::timeout) {
        throw std::runtime_error("Promise timeout");
    }

    return call_result_future.get();
}
