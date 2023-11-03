// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <utils/error_manager.hpp>

#include <utils/error.hpp>
#include <utils/error_json.hpp>

#include <everest/logging.hpp>

namespace Everest {
namespace error {

ErrorManager::ErrorManager(SendMessageFunc send_json_message_, RegisterCallHandlerFunc register_call_handler_,
                           RegisterErrorHandlerFunc register_error_handler_,
                           const std::string& request_clear_error_topic_) :
    error_database(),
    request_clear_error_topic(request_clear_error_topic_),
    send_json_message(send_json_message_),
    register_call_handler(register_call_handler_),
    register_error_handler(register_error_handler_) {
    BOOST_LOG_FUNCTION();

    HandlerFunc handler = [this](const json& data) { this->handle_request_clear_error(data); };
    this->register_call_handler(this->request_clear_error_topic, handler);
    EVLOG_debug << "request_clear_error_topic: " << this->request_clear_error_topic;
}

void ErrorManager::add_error_topic(const std::string& topic) {
    BOOST_LOG_FUNCTION();

    EVLOG_debug << "Add error topic: " << topic;

    HandlerFunc handler = [this](const json& data) { this->handle_error(data); };
    this->register_error_handler(topic, handler);
}

void ErrorManager::handle_error(const json& data) {
    BOOST_LOG_FUNCTION();

    EVLOG_debug << "Received error: " << data.dump(1);

    auto error = std::make_shared<Error>(json_to_error(data));
    this->error_database.add_error(error);
}

void ErrorManager::handle_request_clear_error(const json& data) {
    BOOST_LOG_FUNCTION();

    EVLOG_debug << "Received request clear error: " << data.dump(1);

    RequestClearErrorOption request_type = string_to_request_clear_error_option(data.at("request-clear-type"));
    ImplementationIdentifier impl =
        ImplementationIdentifier(data.at("origin").at("module"), data.at("origin").at("implementation"));

    bool result = true;
    std::list<std::shared_ptr<Error>> cleared_errors;
    switch (request_type) {
    case RequestClearErrorOption::ClearUUID: {
        ErrorHandle handle(data.at("error_id"));
        try {
            std::shared_ptr<Error> cleared_error = this->error_database.clear_error_handle(impl, handle);
            cleared_errors.push_back(cleared_error);
        } catch (EverestErrorDoesNotExistsError& e) {
            EVLOG_error << e.what();
        } catch (EverestFalseRequestorError& e) {
            EVLOG_error << e.what();
        }
    } break;
    case RequestClearErrorOption::ClearAllOfTypeOfModule: {
        std::string error_type = data.at("error_type");
        std::list<std::shared_ptr<Error>> tmp_cleared_errors;
        tmp_cleared_errors = this->error_database.clear_all_errors_of_type_of_module(impl, error_type);
        cleared_errors.splice(cleared_errors.end(), tmp_cleared_errors);
        if (cleared_errors.empty()) {
            EVLOG_info << "There are no errors of type " + error_type + " and of module " + impl.to_string() +
                              ", so no errors are cleared";
        }
    } break;
    case RequestClearErrorOption::ClearAllOfModule: {
        std::list<std::shared_ptr<Error>> tmp_cleared_errors;
        tmp_cleared_errors = this->error_database.clear_all_errors_of_module(impl);
        cleared_errors.splice(cleared_errors.end(), tmp_cleared_errors);
        if (cleared_errors.empty()) {
            EVLOG_info << "There are no errors of module " + impl.to_string() + ", so no errors are cleared";
        }
    } break;
    default: {
        EVLOG_error << "RequestClearErrorOption '" << request_clear_error_option_to_string(request_type)
                    << "' is not supported.";
    } break;
    }
    if (cleared_errors.empty()) {
        result = false;
    }
    for (auto& error : cleared_errors) {
        std::string error_cleared_topic =
            error->from.module_id + "/" + error->from.implementation_id + "/error-cleared/" + error->type;
        this->send_json_message(error_cleared_topic, error_to_json(*error));
    }
    // send result
    json result_data;
    result_data["id"] = data.at("request-id");
    result_data["success"] = result;
    json result_msg;
    result_msg["name"] = "request-clear-error";
    result_msg["type"] = "result";
    result_msg["data"] = result_data;
    this->send_json_message(this->request_clear_error_topic, result_msg);
}

} // namespace error
} // namespace Everest
