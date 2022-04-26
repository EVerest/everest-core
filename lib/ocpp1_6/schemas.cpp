// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <ocpp1_6/schemas.hpp>

#include <fstream>
#include <stdexcept>

#include <everest/logging.hpp>

namespace ocpp1_6 {
Schemas::Schemas(std::string main_dir) {
    this->profile_schemas_path = boost::filesystem::path(main_dir) / "profile_schemas" / "ocpp1_6";
    if (!boost::filesystem::exists(this->profile_schemas_path) ||
        !boost::filesystem::is_directory(this->profile_schemas_path)) {
        EVLOG(error) << this->profile_schemas_path << " does not exist";
        // FIXME(kai): exception?
    } else {
        for (auto file : boost::filesystem::directory_iterator(this->profile_schemas_path)) {
            available_schemas_paths.insert(file.path());
        }
        this->load_root_schema();
    }
}

void Schemas::load_root_schema() {
    boost::filesystem::path profile_path = this->profile_schemas_path / "Config.json";

    EVLOG(debug) << "parsing root schema file: " << boost::filesystem::canonical(profile_path);

    std::ifstream ifs(profile_path.c_str());
    std::string schema_file((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    this->profile_schema = json::parse(schema_file);

    this->profile_validator = std::make_shared<json_validator>(
        [this](const json_uri& uri, json& schema) { this->loader(uri, schema); }, Schemas::format_checker);
    this->profile_validator->set_root_schema(this->profile_schema);
}

json Schemas::get_profile_schema() {
    return this->profile_schema;
}

std::shared_ptr<json_validator> Schemas::get_profile_validator() {
    return this->profile_validator;
}

void Schemas::loader(const json_uri& uri, json& schema) {
    std::string location = uri.location();
    if (location == "http://json-schema.org/draft-07/schema") {
        schema = nlohmann::json_schema::draft7_schema_builtin;
        return;
    }

    boost::filesystem::path profile_path = this->profile_schemas_path / uri.location();
    if (available_schemas_paths.count(profile_path) != 0) {
        std::ifstream ifs(profile_path.c_str());
        std::string schema_file((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        schema = json::parse(schema_file);
        return;
    }

    throw std::runtime_error(uri.url() + " is not supported for schema loading at the moment");
}

void Schemas::format_checker(const std::string& format, const std::string& value) {
    if (format == "date-time") {
        if (!std::regex_match(value, Schemas::date_time_regex)) {
            throw std::runtime_error("No format checker available for date-time");
        }
    } else {
        nlohmann::json_schema::default_string_format_check(format, value);
    }
}

// NOLINTNEXTLINE(cert-err58-cpp)
const std::regex Schemas::date_time_regex =
    std::regex(R"(^((?:(\d{4}-\d{2}-\d{2})T(\d{2}:\d{2}:\d{2}(?:\.\d{1,3})?))(Z|[\+-]\d{2}:\d{2})?)$)");
} // namespace ocpp1_6
