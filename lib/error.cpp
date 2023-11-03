// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include <everest/logging.hpp>
#include <utils/error.hpp>
#include <utils/types.hpp>
#include <utils/yaml_loader.hpp>

#include <fmt/format.h>
#include <fstream>
#include <list>
#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace Everest {
namespace error {

std::string request_clear_error_option_to_string(const RequestClearErrorOption& o) {
    switch (o) {
    case RequestClearErrorOption::ClearUUID:
        return "ClearUUID";
    case RequestClearErrorOption::ClearAllOfTypeOfModule:
        return "ClearAllOfTypeOfModule";
    case RequestClearErrorOption::ClearAllOfModule:
        return "ClearAllOfModule";
    }
    throw std::runtime_error(fmt::format("RequestClearErrorOption is not valid."));
}
RequestClearErrorOption string_to_request_clear_error_option(const std::string& s) {
    if (s == "ClearUUID") {
        return RequestClearErrorOption::ClearUUID;
    } else if (s == "ClearAllOfTypeOfModule") {
        return RequestClearErrorOption::ClearAllOfTypeOfModule;
    } else if (s == "ClearAllOfModule") {
        return RequestClearErrorOption::ClearAllOfModule;
    } else {
        throw std::runtime_error(fmt::format("RequestClearErrorOption '{}' is not valid.", s));
    }
}

UUID::UUID() {
    uuid = boost::uuids::to_string(boost::uuids::random_generator()());
}

UUID::UUID(const std::string& uuid_) : uuid(uuid_) {
}

bool UUID::operator<(const UUID& other) const {
    return uuid < other.uuid;
}

std::string UUID::to_string() const {
    return uuid;
}

Error::Error(const std::string& type_, const std::string& message_, const std::string& description_,
             const ImplementationIdentifier& from_, const bool persistent_, const Severity& severity_,
             const time_point& timestamp_, const UUID& uuid_) :
    type(type_),
    message(message_),
    description(description_),
    from(from_),
    persistent(persistent_),
    severity(severity_),
    timestamp(timestamp_),
    uuid(uuid_) {
}

Error::Error(const std::string& type_, const std::string& message_, const std::string& description_,
             const ImplementationIdentifier& from_, const Severity& severity_) :
    Error(type_, message_, description_, from_, false, severity_, date::utc_clock::now(), UUID()) {
}

Error::Error(const std::string& type_, const std::string& message_, const std::string& description_,
             const std::string& from_module_, const std::string& from_implementation_, const Severity& severity_) :
    Error(type_, message_, description_, ImplementationIdentifier(from_module_, from_implementation_), severity_) {
}

Error json_to_error(const json& j) {
    BOOST_LOG_FUNCTION();

    std::string type = j.at("type");
    std::string message = j.at("message");
    std::string description = j.at("description");
    ImplementationIdentifier from =
        ImplementationIdentifier(j.at("from").at("module"), j.at("from").at("implementation"));
    bool persistent = j.at("persistent");
    Severity severity = string_to_severity(j.at("severity"));
    Error::time_point timestamp = Date::from_rfc3339(j.at("timestamp"));
    UUID uuid(j.at("uuid"));

    return Error(type, message, description, from, persistent, severity, timestamp, uuid);
}

json error_to_json(const Error& e) {
    json j;
    j["type"] = e.type;
    j["description"] = e.description;
    j["message"] = e.message;
    j["from"]["module"] = e.from.module_id;
    j["from"]["implementation"] = e.from.implementation_id;
    j["persistent"] = e.persistent;
    j["severity"] = severity_to_string(e.severity);
    j["timestamp"] = Date::to_rfc3339(e.timestamp);
    j["uuid"] = e.uuid.uuid;
    return j;
}

std::string severity_to_string(const Severity& s) {
    switch (s) {
    case Severity::Low:
        return "Low";
    case Severity::Mid:
        return "Mid";
    case Severity::High:
        return "High";
    }
    throw std::runtime_error(fmt::format("Severity is not valid."));
}

Severity string_to_severity(const std::string& s) {
    if (s == "Low") {
        return Severity::Low;
    } else if (s == "Mid") {
        return Severity::Mid;
    } else if (s == "High") {
        return Severity::High;
    } else {
        throw std::runtime_error(fmt::format("Severity '{}' is not valid.", s));
    }
}

ErrorTypeMap::ErrorTypeMap(std::filesystem::path error_types_dir) {
    load_error_types(error_types_dir);
}

void ErrorTypeMap::load_error_types(std::filesystem::path error_types_dir) {
    if (!std::filesystem::is_directory(error_types_dir) || !std::filesystem::exists(error_types_dir)) {
        throw std::runtime_error(fmt::format("Error types directory '{}' does not exist.", error_types_dir.string()));
    }
    for (const auto& entry : std::filesystem::directory_iterator(error_types_dir)) {
        if (!entry.is_regular_file()) {
            continue;
        }
        if (entry.path().extension() != ".yaml") {
            continue;
        }
        std::string prefix = entry.path().stem().string();
        json error_type_file = Everest::load_yaml(entry.path());
        if (!error_type_file.contains("errors")) {
            EVLOG_warning << "Error type file '" << entry.path().string() << "' does not contain 'errors' key.";
            continue;
        }
        if (!error_type_file["errors"].is_array()) {
            throw std::runtime_error(fmt::format("Error type file '{}' does not contain an array under 'errors' key.",
                                                 entry.path().string()));
        }
        for (const auto& error : error_type_file["errors"]) {
            if (!error.contains("name")) {
                throw std::runtime_error(
                    fmt::format("Error type file '{}' contains an error without a 'name' key.", entry.path().string()));
            }
            if (!error.contains("description")) {
                throw std::runtime_error(fmt::format(
                    "Error type file '{}' contains an error without a 'description' key.", entry.path().string()));
            }
            std::string complete_name = prefix + "/" + error["name"].get<std::string>();
            if (this->has(complete_name)) {
                throw std::runtime_error(
                    fmt::format("Error type file '{}' contains an error with the name '{}' which is already defined.",
                                entry.path().string(), complete_name));
            }
            std::string description = error["description"].get<std::string>();
            error_types[complete_name] = description;
        }
    }
}

std::string ErrorTypeMap::get_description(const std::string& error_type) const {
    return error_types.at(error_type);
}

bool ErrorTypeMap::has(const std::string& error_type) const {
    return error_types.find(error_type) != error_types.end();
}

} // namespace error
} // namespace Everest
