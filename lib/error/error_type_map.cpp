// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <utils/error/error_type_map.hpp>

#include <utils/error.hpp>
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
        EVLOG_error << "Error types directory '" << error_types_dir.string()
                    << "' does not exist, error types not loaded.";
        return;
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
            EVLOG_error << "Error type file '" << entry.path().string()
                        << "' does not contain an array with key 'errors', skipped.";
            continue;
        }
        for (const auto& error : error_type_file["errors"]) {
            if (!error.contains("name")) {
                EVLOG_error << "Error type file '" << entry.path().string()
                            << "' contains an error without a 'name' key, skipped.";
                continue;
            }
            std::string description;
            if (!error.contains("description")) {
                EVLOG_error << "Error type file '" << entry.path().string()
                            << "' contains an error without a 'description' key, using default description";
                description = "No description found";
            } else {
                description = error.at("description").get<std::string>();
            }
            ErrorType complete_name = prefix + "/" + error.at("name").get<std::string>();
            if (this->has(complete_name)) {
                EVLOG_error << "Error type file '" << entry.path().string() << "' contains an error with the name '"
                            << complete_name << "' which is already defined, skipped.";
                continue;
            }
            error_types[complete_name] = description;
        }
    }
}

void ErrorTypeMap::load_error_types_map(std::map<ErrorType, std::string> error_types_map) {
    this->error_types = error_types_map;
}

std::string ErrorTypeMap::get_description(const ErrorType& error_type) const {
    std::string description;
    try {
        description = error_types.at(error_type);
    } catch (...) {
        EVLOG_error << "Error type '" << error_type << "' is not defined, returning default description.";
        description = "No description found";
    }
    return description;
}

bool ErrorTypeMap::has(const ErrorType& error_type) const {
    return error_types.find(error_type) != error_types.end();
}

std::map<ErrorType, std::string> ErrorTypeMap::get_error_types() {
    return error_types;
}

} // namespace error
} // namespace Everest
