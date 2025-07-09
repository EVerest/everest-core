// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <utils/conversions.hpp>

namespace Everest {
namespace conversions {
constexpr auto CMD_EVENT_MESSAGE_PARSING_FAILED = "MessageParsingFailed";
constexpr auto CMD_EVENT_SCHEMA_VALIDATION_FAILED = "SchemaValidationFailed";
constexpr auto CMD_EVENT_HANDLER_EXCEPTION = "HandlerException";
constexpr auto CMD_EVENT_TIMEOUT = "Timeout";
constexpr auto CMD_EVENT_SHUTDOWN = "Shutdown";
constexpr auto CMD_EVENT_NOT_READY = "NotReady";

std::string cmd_event_to_string(CmdEvent cmd_event) {
    switch (cmd_event) {
    case CmdEvent::MessageParsingFailed:
        return CMD_EVENT_MESSAGE_PARSING_FAILED;
    case CmdEvent::SchemaValidationFailed:
        return CMD_EVENT_SCHEMA_VALIDATION_FAILED;
    case CmdEvent::HandlerException:
        return CMD_EVENT_HANDLER_EXCEPTION;
    case CmdEvent::Timeout:
        return CMD_EVENT_TIMEOUT;
    case CmdEvent::Shutdown:
        return CMD_EVENT_SHUTDOWN;
    case CmdEvent::NotReady:
        return CMD_EVENT_NOT_READY;
    }

    throw std::runtime_error("Unknown CmdEvent");
}

CmdEvent string_to_cmd_event(const std::string& cmd_event_string) {
    if (cmd_event_string == CMD_EVENT_MESSAGE_PARSING_FAILED) {
        return CmdEvent::MessageParsingFailed;
    } else if (cmd_event_string == CMD_EVENT_SCHEMA_VALIDATION_FAILED) {
        return CmdEvent::SchemaValidationFailed;
    } else if (cmd_event_string == CMD_EVENT_HANDLER_EXCEPTION) {
        return CmdEvent::HandlerException;
    } else if (cmd_event_string == CMD_EVENT_TIMEOUT) {
        return CmdEvent::Timeout;
    } else if (cmd_event_string == CMD_EVENT_SHUTDOWN) {
        return CmdEvent::Shutdown;
    } else if (cmd_event_string == CMD_EVENT_NOT_READY) {
        return CmdEvent::NotReady;
    }

    throw std::runtime_error("Unknown CmdEvent");
}
} // namespace conversions

void to_json(nlohmann::json& j, const CmdResultError& e) {
    j = {{"event", conversions::cmd_event_to_string(e.event)}, {"msg", e.msg}};
}

void from_json(const nlohmann::json& j, CmdResultError& e) {
    e.event = conversions::string_to_cmd_event(j.at("event"));
    e.msg = j.at("msg");
}

} // namespace Everest
