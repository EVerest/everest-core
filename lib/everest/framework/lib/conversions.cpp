// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <utils/conversions.hpp>

namespace Everest {
namespace conversions {
constexpr auto CMD_ERROR_TYPE_MESSAGE_PARSING_ERROR = "MessageParsingError";
constexpr auto CMD_ERROR_TYPE_SCHEMA_VALIDATION_ERROR = "SchemaValidationError";
constexpr auto CMD_ERROR_TYPE_HANDLER_EXCEPTION = "HandlerException";
constexpr auto CMD_ERROR_TYPE_CMD_TIMEOUT = "CmdTimeout";
constexpr auto CMD_ERROR_TYPE_SHUTDOWN = "Shutdown";
constexpr auto CMD_ERROR_TYPE_NOT_READY = "NotReady";

std::string cmd_error_type_to_string(CmdErrorType cmd_error) {
    switch (cmd_error) {
    case CmdErrorType::MessageParsingError:
        return CMD_ERROR_TYPE_MESSAGE_PARSING_ERROR;
    case CmdErrorType::SchemaValidationError:
        return CMD_ERROR_TYPE_SCHEMA_VALIDATION_ERROR;
    case CmdErrorType::HandlerException:
        return CMD_ERROR_TYPE_HANDLER_EXCEPTION;
    case CmdErrorType::CmdTimeout:
        return CMD_ERROR_TYPE_CMD_TIMEOUT;
    case CmdErrorType::Shutdown:
        return CMD_ERROR_TYPE_SHUTDOWN;
    case CmdErrorType::NotReady:
        return CMD_ERROR_TYPE_NOT_READY;
    }

    throw std::runtime_error("Unknown CmdError");
}

CmdErrorType string_to_cmd_error_type(const std::string& cmd_error_string) {
    if (cmd_error_string == CMD_ERROR_TYPE_MESSAGE_PARSING_ERROR) {
        return CmdErrorType::MessageParsingError;
    } else if (cmd_error_string == CMD_ERROR_TYPE_SCHEMA_VALIDATION_ERROR) {
        return CmdErrorType::SchemaValidationError;
    } else if (cmd_error_string == CMD_ERROR_TYPE_HANDLER_EXCEPTION) {
        return CmdErrorType::HandlerException;
    } else if (cmd_error_string == CMD_ERROR_TYPE_CMD_TIMEOUT) {
        return CmdErrorType::CmdTimeout;
    } else if (cmd_error_string == CMD_ERROR_TYPE_SHUTDOWN) {
        return CmdErrorType::Shutdown;
    } else if (cmd_error_string == CMD_ERROR_TYPE_NOT_READY) {
        return CmdErrorType::NotReady;
    }

    throw std::runtime_error("Unknown CmdError");
}
} // namespace conversions

void to_json(nlohmann::json& j, const CmdResultError& e) {
    j = {{conversions::ERROR_TYPE, conversions::cmd_error_type_to_string(e.event)}, {conversions::ERROR_MSG, e.msg}};
}

void from_json(const nlohmann::json& j, CmdResultError& e) {
    e.event = conversions::string_to_cmd_error_type(j.at(conversions::ERROR_TYPE));
    e.msg = j.at(conversions::ERROR_MSG);
}

} // namespace Everest
