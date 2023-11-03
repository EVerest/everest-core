// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef UTILS_ERROR_MANAGER_HPP
#define UTILS_ERROR_MANAGER_HPP

#include <functional>

#include <utils/error_database.hpp>
#include <utils/types.hpp>

namespace Everest {
namespace error {

class ErrorManager {
public:
    using HandlerFunc = std::function<void(const json&)>;
    using SendMessageFunc = std::function<void(const std::string&, const json&)>;
    using RegisterHandlerFunc = std::function<void(const std::string&, HandlerFunc&)>;
    using RegisterCallHandlerFunc = RegisterHandlerFunc;
    using RegisterErrorHandlerFunc = RegisterHandlerFunc;

public:
    void add_error_topic(const std::string& topic);
    ErrorManager(SendMessageFunc send_json_message_, RegisterCallHandlerFunc register_call_handler_,
                 RegisterErrorHandlerFunc register_error_handler_, const std::string& request_clear_error_topic_);

private:
    void handle_error(const json& data);
    void handle_request_clear_error(const json& data);

    ErrorDatabase error_database;
    std::string request_clear_error_topic;

    RegisterCallHandlerFunc register_call_handler;
    RegisterErrorHandlerFunc register_error_handler;
    SendMessageFunc send_json_message;
};

} // namespace error
} // namespace Everest

#endif // UTILS_ERROR_MANAGER_HPP
