// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <utils/error/error_comm_bridge.hpp>

#include <utils/error/error_json.hpp>

#include <everest/exceptions.hpp>
#include <everest/logging.hpp>

namespace Everest {
namespace error {

ErrorCommBridge::ErrorCommBridge(std::shared_ptr<ErrorManager> error_manager_, SendMessageFunc send_json_message_,
                                 RegisterCallHandlerFunc register_call_handler_,
                                 RegisterErrorHandlerFunc register_error_handler_,
                                 const std::string& request_clear_error_topic_) :
    error_manager(error_manager_),
    request_clear_error_topic(request_clear_error_topic_),
    send_json_message(send_json_message_),
    register_call_handler(register_call_handler_),
    register_error_handler(register_error_handler_) {
    BOOST_LOG_FUNCTION();

    HandlerFunc handler = [this](const json& data) { this->handle_request_clear_error(data); };
    this->register_call_handler(this->request_clear_error_topic, handler);
    EVLOG_debug << "request_clear_error_topic: " << this->request_clear_error_topic;
}

void ErrorCommBridge::add_error_topic(const std::string& topic) {
    BOOST_LOG_FUNCTION();

    EVLOG_debug << "Add error topic: " << topic;

    HandlerFunc handler = [this](const json& data) { this->handle_error(data); };
    this->register_error_handler(topic, handler);
}

void ErrorCommBridge::handle_error(const json& data) {
    BOOST_LOG_FUNCTION();
    EVLOG_debug << "Received error: " << data.dump(1);

    ErrorPtr error = std::make_shared<Error>(json_to_error(data));
    this->error_manager->raise_error(error);
}

void ErrorCommBridge::handle_request_clear_error(const json& data) {
    BOOST_LOG_FUNCTION();
    EVLOG_debug << "Received request clear error: " << data.dump(1);

    ImplementationIdentifier impl =
        ImplementationIdentifier(data.at("origin").at("module"), data.at("origin").at("implementation"));
    RequestClearErrorOption request_type = string_to_request_clear_error_option(data.at("request-clear-type"));

    std::optional<ErrorHandle> handle = std::nullopt;
    std::optional<ErrorType> type = std::nullopt;
    if (data.contains("error_type")) {
        type = ErrorType(data.at("error_type"));
    }
    if (data.contains("error_id")) {
        handle = ErrorHandle(data.at("error_id"));
    }

    std::list<ErrorPtr> cleared_errors;
    try {
        cleared_errors = this->error_manager->clear_errors(request_type, impl, handle, type);
        if (cleared_errors.empty()) {
            throw EverestBaseLogicError("No errors matched the request.");
        }
    } catch (const std::exception& e) {
        EVLOG_error << "Error while clearing errors: " << e.what();
    }
    bool result = not cleared_errors.empty();

    for (auto& error : cleared_errors) {
        std::string error_cleared_topic =
            error->from.module_id + "/" + error->from.implementation_id + "/error-cleared/" + error->type;
        this->send_json_message(error_cleared_topic, error_to_json(*error));
    }

    // send result
    json result_data = {{"id", data.at("request-id")}, {"success", result}};
    json result_msg = {{"name", "request-clear-error"}, {"type", "result"}, {"data", result_data}};
    this->send_json_message(this->request_clear_error_topic, result_msg);
}

} // namespace error
} // namespace Everest
