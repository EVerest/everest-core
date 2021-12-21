// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
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
class Schemas {
private:
    std::map<std::string, json> profile_schemas;
    std::map<std::string, std::shared_ptr<json_validator>> profile_validators;
    boost::filesystem::path profile_schemas_path;
    std::set<boost::filesystem::path> available_schemas_paths;
    const static std::vector<std::string> profiles;
    const static std::regex date_time_regex;
    void load_root_schema();
    void loader(const json_uri& uri, json& schema);

public:
    Schemas(std::string main_dir);
    json get_profile_schema();
    std::shared_ptr<json_validator> get_profile_validator();

    static void format_checker(const std::string& format, const std::string& value);
};
} // namespace ocpp1_6

#endif // OCPP1_6_SCHEMAS_HPP
