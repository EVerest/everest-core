// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP1_6_SCHEMAS_HPP
#define OCPP1_6_SCHEMAS_HPP

#include <map>
#include <regex>
#include <vector>

#include <boost/filesystem.hpp>
#include <everest/logging.hpp>
#include <nlohmann/json-schema.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using json_uri = nlohmann::json_uri;
using json_validator = nlohmann::json_schema::json_validator;

namespace ocpp1_6 {

/// \brief Contains the json schema validation for the libocpp config
class Schemas {
private:
    std::map<std::string, json> profile_schemas;
    std::map<std::string, std::shared_ptr<json_validator>> profile_validators;
    boost::filesystem::path profile_schemas_path;
    std::set<boost::filesystem::path> available_schemas_paths;
    const static std::vector<std::string> profiles;
    const static std::regex date_time_regex;

    /// \brief Loads the root schema "Config.json" from the profile schemas path
    void load_root_schema();

    /// \brief A custom json schema loader that loads \p schema files relative to the provided \p uri
    void loader(const json_uri& uri, json& schema);

public:
    /// \brief Creates a new Schemas object looking for the root schema file in relation to the provided \p main_dir
    Schemas(std::string main_dir);

    /// \brief Provides the config profile schema
    /// \returns the config profile schema as as json object
    json get_profile_schema();

    /// \brief Provides the config profile schema validator
    /// \returns a json_validator for the config profile
    std::shared_ptr<json_validator> get_profile_validator();

    /// \brief Provides a format checker for the given \p format and \p value
    static void format_checker(const std::string& format, const std::string& value);
};
} // namespace ocpp1_6

#endif // OCPP1_6_SCHEMAS_HPP
