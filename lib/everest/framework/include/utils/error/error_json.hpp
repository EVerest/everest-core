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
        if (e.origin.mapping.has_value()) {
            j["origin"]["mapping"] = e.origin.mapping.value();
        }
    }
    static Everest::error::Error from_json(const json& j) {
        const Everest::error::ErrorType type = j.at("type");
        const std::string message = j.at("message");
        const std::string description = j.at("description");
        std::optional<Mapping> mapping;
        if (j.at("origin").contains("mapping")) {
            mapping = j.at("origin").at("mapping");
        }
        const ImplementationIdentifier origin =
            ImplementationIdentifier(j.at("origin").at("module_id"), j.at("origin").at("implementation_id"), mapping);
        const Everest::error::Severity severity = Everest::error::string_to_severity(j.at("severity"));
        const Everest::error::Error::time_point timestamp = Everest::Date::from_rfc3339(j.at("timestamp"));
        const Everest::error::UUID uuid(j.at("uuid"));
        const Everest::error::State state = Everest::error::string_to_state(j.at("state"));
        const Everest::error::ErrorSubType sub_type(j.at("sub_type"));
        const std::string vendor_id = j.at("vendor_id");

        return {type, sub_type, message, description, origin, vendor_id, severity, timestamp, uuid, state};
    }
};

NLOHMANN_JSON_NAMESPACE_END
#endif // UTILS_ERROR_JSON_HPP
