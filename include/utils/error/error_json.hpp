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
             {"origin", {{"module_id", e.origin.module_id}, {"implementation_id", e.origin.implementation_id}}},
             {"severity", Everest::error::severity_to_string(e.severity)},
             {"timestamp", Everest::Date::to_rfc3339(e.timestamp)},
             {"uuid", e.uuid.uuid},
             {"state", Everest::error::state_to_string(e.state)},
             {"sub_type", e.sub_type},
             {"vendor_id", e.vendor_id}};
    }
    static Everest::error::Error from_json(const json& j) {
        Everest::error::ErrorType type = j.at("type");
        std::string message = j.at("message");
        std::string description = j.at("description");
        ImplementationIdentifier origin =
            ImplementationIdentifier(j.at("origin").at("module_id"), j.at("origin").at("implementation_id"));
        Everest::error::Severity severity = Everest::error::string_to_severity(j.at("severity"));
        Everest::error::Error::time_point timestamp = Everest::Date::from_rfc3339(j.at("timestamp"));
        Everest::error::UUID uuid(j.at("uuid"));
        Everest::error::State state = Everest::error::string_to_state(j.at("state"));
        Everest::error::ErrorSubType sub_type(j.at("sub_type"));
        std::string vendor_id = j.at("vendor_id");

        return {type, sub_type, message, description, origin, vendor_id, severity, timestamp, uuid, state};
    }
};

NLOHMANN_JSON_NAMESPACE_END
#endif // UTILS_ERROR_JSON_HPP
