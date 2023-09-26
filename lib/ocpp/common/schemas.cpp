// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include <ocpp/common/schemas.hpp>

#include <fstream>
#include <stdexcept>

#include <everest/logging.hpp>

namespace ocpp {

Schemas::Schemas(std::filesystem::path schemas_path) : schemas_path(schemas_path) {
    if (!std::filesystem::exists(this->schemas_path) || !std::filesystem::is_directory(this->schemas_path)) {
        EVLOG_error << this->schemas_path << " does not exist";
        // FIXME(kai): exception?
    } else {
        for (auto file : std::filesystem::directory_iterator(this->schemas_path)) {
            available_schemas_paths.insert(file.path());
        }
        this->load_root_schema();
    }
}

void Schemas::load_root_schema() {
    std::filesystem::path config_schema_path = this->schemas_path / "Config.json";

    EVLOG_debug << "parsing root schema file: " << std::filesystem::canonical(config_schema_path);

    std::ifstream ifs(config_schema_path.c_str());
    std::string schema_file((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    this->schema = json::parse(schema_file);

    const auto custom_schema_path = schemas_path / "Custom.json";
    if (std::filesystem::exists(custom_schema_path)) {
        json custom_object = {{"type", "object"}, {"$ref", "Custom.json"}};
        this->schema["properties"]["Custom"] = custom_object;
    }

    this->validator = std::make_shared<json_validator>(
        [this](const json_uri& uri, json& schema) { this->loader(uri, schema); }, Schemas::format_checker);
    this->validator->set_root_schema(this->schema);
}

json Schemas::get_schema() {
    return this->schema;
}

std::shared_ptr<json_validator> Schemas::get_validator() {
    return this->validator;
}

void Schemas::loader(const json_uri& uri, json& schema) {
    std::string location = uri.location();
    if (location == "http://json-schema.org/draft-07/schema") {
        schema = nlohmann::json_schema::draft7_schema_builtin;
        return;
    } else if (location.rfind("/", 0) == 0) {
        // remove leading /
        location.erase(0, 1);
    }

    std::filesystem::path schema_path = this->schemas_path / std::filesystem::path(location);
    if (available_schemas_paths.count(schema_path) != 0) {
        std::ifstream ifs(schema_path.string().c_str());
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
} // namespace ocpp
