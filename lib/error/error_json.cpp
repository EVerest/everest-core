// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <utils/error/error_json.hpp>

#include <everest/logging.hpp>

namespace Everest {
namespace error {

Error json_to_error(const json& j) {
    BOOST_LOG_FUNCTION();

    ErrorType type = j.at("type");
    std::string message = j.at("message");
    std::string description = j.at("description");
    ImplementationIdentifier from =
        ImplementationIdentifier(j.at("from").at("module"), j.at("from").at("implementation"));
    Severity severity = string_to_severity(j.at("severity"));
    Error::time_point timestamp = Date::from_rfc3339(j.at("timestamp"));
    UUID uuid(j.at("uuid"));
    State state = string_to_state(j.at("state"));

    return Error(type, message, description, from, severity, timestamp, uuid, state);
}

json error_to_json(const Error& e) {
    json j;
    j["type"] = e.type;
    j["description"] = e.description;
    j["message"] = e.message;
    j["from"]["module"] = e.from.module_id;
    j["from"]["implementation"] = e.from.implementation_id;
    j["severity"] = severity_to_string(e.severity);
    j["timestamp"] = Date::to_rfc3339(e.timestamp);
    j["uuid"] = e.uuid.uuid;
    j["state"] = state_to_string(e.state);
    return j;
}

} // namespace error
} // namespace Everest
