// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef UTILS_ERROR_JSON_HPP
#define UTILS_ERROR_JSON_HPP

#include <nlohmann/json.hpp>
#include <utils/error.hpp>

NLOHMANN_JSON_NAMESPACE_BEGIN
template <> struct adl_serializer<Everest::error::Error> {
    static void to_json(json& j, const Everest::error::Error& e) {
        j = {{"type", e.type},
             {"description", e.description},
             {"message", e.message},
             {"from", {{"module", e.from.module_id}, {"implementation", e.from.implementation_id}}},
             {"severity", Everest::error::severity_to_string(e.severity)},
             {"timestamp", Everest::Date::to_rfc3339(e.timestamp)},
             {"uuid", e.uuid.uuid},
             {"state", Everest::error::state_to_string(e.state)}};
    }
    static Everest::error::Error from_json(const json& j) {
        Everest::error::ErrorType type = j.at("type");
        std::string message = j.at("message");
        std::string description = j.at("description");
        ImplementationIdentifier from =
            ImplementationIdentifier(j.at("from").at("module"), j.at("from").at("implementation"));
        Everest::error::Severity severity = Everest::error::string_to_severity(j.at("severity"));
        Everest::error::Error::time_point timestamp = Everest::Date::from_rfc3339(j.at("timestamp"));
        Everest::error::UUID uuid(j.at("uuid"));
        Everest::error::State state = Everest::error::string_to_state(j.at("state"));

        return {type, message, description, from, severity, timestamp, uuid, state};
    }
};

NLOHMANN_JSON_NAMESPACE_END
#endif // UTILS_ERROR_JSON_HPP
