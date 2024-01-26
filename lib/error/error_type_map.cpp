// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <utils/error/error_type_map.hpp>

#include <utils/error.hpp>
#include <utils/error/error_exceptions.hpp>
#include <utils/yaml_loader.hpp>

#include <everest/logging.hpp>

namespace Everest {
namespace error {

ErrorTypeMap::ErrorTypeMap(std::filesystem::path error_types_dir) {
    load_error_types(error_types_dir);
}

void ErrorTypeMap::load_error_types(std::filesystem::path error_types_dir) {
    BOOST_LOG_FUNCTION();

    if (!std::filesystem::is_directory(error_types_dir) || !std::filesystem::exists(error_types_dir)) {
        throw EverestDirectoryNotFoundError(
            fmt::format("Error types directory '{}' does not exist.", error_types_dir.string()));
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
        if (!error_type_file.at("errors").is_array()) {
            throw EverestParseError(fmt::format("Error type file '{}' does not contain an array with key 'errors'.",
                                                entry.path().string()));
        }
        for (const auto& error : error_type_file["errors"]) {
            if (!error.contains("name")) {
                throw EverestParseError(
                    fmt::format("Error type file '{}' contains an error without a 'name' key.", entry.path().string()));
            }
            if (!error.contains("description")) {
                throw EverestParseError(fmt::format(
                    "Error type file '{}' contains an error without a 'description' key.", entry.path().string()));
            }
            ErrorType complete_name = prefix + "/" + error.at("name").get<std::string>();
            if (this->has(complete_name)) {
                throw EverestAlreadyExistsError(
                    fmt::format("Error type file '{}' contains an error with the name '{}' which is already defined.",
                                entry.path().string(), complete_name));
            }
            std::string description = error.at("description").get<std::string>();
            error_types[complete_name] = description;
        }
    }
}

std::string ErrorTypeMap::get_description(const ErrorType& error_type) const {
    return error_types.at(error_type);
}

bool ErrorTypeMap::has(const ErrorType& error_type) const {
    return error_types.find(error_type) != error_types.end();
}

} // namespace error
} // namespace Everest
