// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef UTILS_ERROR_COMM_BRIDGE_HPP
#define UTILS_ERROR_COMM_BRIDGE_HPP

#include <functional>
#include <utils/error/error_manager.hpp>
#include <utils/types.hpp>

namespace Everest {
namespace error {

class ErrorCommBridge {
public:
    using HandlerFunc = std::function<void(const json&)>;
    using SendMessageFunc = std::function<void(const std::string&, const json&)>;
    using RegisterHandlerFunc = std::function<void(const std::string&, HandlerFunc&)>;
    using RegisterCallHandlerFunc = RegisterHandlerFunc;
    using RegisterErrorHandlerFunc = RegisterHandlerFunc;

    void add_error_topic(const std::string& topic);
    ErrorCommBridge(std::shared_ptr<ErrorManager> error_manager_, SendMessageFunc send_json_message_,
                    RegisterCallHandlerFunc register_call_handler_, RegisterErrorHandlerFunc register_error_handler_,
                    const std::string& request_clear_error_topic_);

private:
    void handle_error(const json& data);
    void handle_request_clear_error(const json& data);

    std::shared_ptr<ErrorManager> error_manager;
    std::string request_clear_error_topic;

    RegisterCallHandlerFunc register_call_handler;
    RegisterErrorHandlerFunc register_error_handler;
    SendMessageFunc send_json_message;
};

} // namespace error
} // namespace Everest

#endif // UTILS_ERROR_COMM_BRIDGE_HPP
