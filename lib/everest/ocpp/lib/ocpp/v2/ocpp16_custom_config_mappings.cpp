// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ocpp/v2/ocpp16_custom_config_mappings.hpp>

#include <fstream>
#include <sstream>

#include <nlohmann/json-schema.hpp>
#include <ryml.hpp>
#include <ryml_std.hpp>

namespace ocpp::v2 {

namespace {

void yaml_error_handler(const char* msg, std::size_t len, ryml::Location loc, void*) {
    std::stringstream error_message;
    error_message << "YAML parsing error: ";

    if (loc) {
        if (!loc.name.empty()) {
            error_message.write(loc.name.str, static_cast<std::streamsize>(loc.name.len));
            error_message << ':';
        }
        error_message << loc.line << ':';
        if (loc.col) {
            error_message << loc.col << ':';
        }
        if (loc.offset) {
            error_message << " (" << loc.offset << "B):";
        }
    }

    error_message.write(msg, static_cast<std::streamsize>(len));
    throw Ocpp16CustomConfigMappingsError(error_message.str());
}

struct RymlCallbackInitializer {
    RymlCallbackInitializer() {
        ryml::set_callbacks({nullptr, nullptr, nullptr, yaml_error_handler});
    }
};

nlohmann::ordered_json ryml_to_nlohmann_json(const c4::yml::ConstNodeRef& ryml_node) {
    if (ryml_node.is_map()) {
        auto object = nlohmann::ordered_json::object();
        for (const auto& child : ryml_node) {
            object[std::string(child.key().data(), child.key().len)] = ryml_to_nlohmann_json(child);
        }
        return object;
    }

    if (ryml_node.is_seq()) {
        auto array = nlohmann::ordered_json::array();
        for (const auto& child : ryml_node) {
            array.emplace_back(ryml_to_nlohmann_json(child));
        }
        return array;
    }

    if (ryml_node.empty() || ryml_node.val_is_null()) {
        return nullptr;
    }

    const auto& value = ryml_node.val();
    std::string value_string(value.data(), value.len);
    const auto value_quoted = ryml_node.is_val_quoted();

    if (!value_quoted) {
        if (ryml_node.val().is_integer()) {
            return std::stoi(value_string);
        }
        if (ryml_node.val().is_number()) {
            return std::stod(value_string);
        }
        if (value_string == "true") {
            return true;
        }
        if (value_string == "false") {
            return false;
        }
    }

    return value_string;
}

nlohmann::ordered_json load_yaml(const std::filesystem::path& path) {
    const static RymlCallbackInitializer ryml_callback_initializer;

    if (!std::filesystem::exists(path)) {
        throw Ocpp16CustomConfigMappingsError("YAML mapping file not found: " + path.string());
    }

    std::ifstream ifs(path);
    const std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));

    const auto tree = ryml::parse_in_arena(ryml::to_csubstr(content));
    return ryml_to_nlohmann_json(tree.rootref());
}

} // namespace

nlohmann::json get_ocpp16_custom_mapping_schema() {
    return nlohmann::json::parse(R"json(
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "type": "object",
  "required": ["mappings"],
  "additionalProperties": false,
  "properties": {
    "mappings": {
      "type": "array",
      "items": {
        "type": "object",
        "required": ["ocpp16_key", "component", "variable"],
        "additionalProperties": false,
        "properties": {
          "ocpp16_key": {
            "type": "string",
            "minLength": 1
          },
          "component": {
            "type": "object",
            "required": ["name"],
            "additionalProperties": false,
            "properties": {
              "name": {
                "type": "string",
                "minLength": 1
              },
              "instance": {
                "type": "string",
                "minLength": 1
              },
              "evse": {
                "type": "object",
                "required": ["id"],
                "additionalProperties": false,
                "properties": {
                  "id": {
                    "type": "integer"
                  },
                  "connectorId": {
                    "type": "integer"
                  }
                }
              }
            }
          },
          "variable": {
            "type": "object",
            "required": ["name"],
            "additionalProperties": false,
            "properties": {
              "name": {
                "type": "string",
                "minLength": 1
              },
              "instance": {
                "type": "string",
                "minLength": 1
              }
            }
          }
        }
      }
    }
  }
}
)json");
}

Ocpp16CustomConfigMappings
load_ocpp16_custom_config_mappings_from_yaml(const std::filesystem::path& mapping_file_path) {
    const auto yaml_json = load_yaml(mapping_file_path);

    nlohmann::json_schema::json_validator validator;
    validator.set_root_schema(get_ocpp16_custom_mapping_schema());

    try {
        validator.validate(yaml_json);
    } catch (const std::exception& e) {
        throw Ocpp16CustomConfigMappingsError("Invalid OCPP16 custom mapping file '" + mapping_file_path.string() +
                                              "': " + e.what());
    }

    // The schema validation ensures that the following usage of .at(...) is safe
    Ocpp16CustomConfigMappings mappings;
    for (const auto& entry : yaml_json.at("mappings")) {
        const auto ocpp16_key = entry.at("ocpp16_key").get<std::string>();

        Component component;
        component.name = entry.at("component").at("name").get<std::string>();
        if (entry.at("component").contains("instance")) {
            component.instance = entry.at("component").at("instance").get<std::string>();
        }
        if (entry.at("component").contains("evse")) {
            EVSE evse;
            const auto& evse_json = entry.at("component").at("evse");
            evse.id = evse_json.at("id").get<std::int32_t>();
            if (evse_json.contains("connectorId")) {
                evse.connectorId = evse_json.at("connectorId").get<std::int32_t>();
            }
            component.evse = evse;
        }

        Variable variable;
        variable.name = entry.at("variable").at("name").get<std::string>();
        if (entry.at("variable").contains("instance")) {
            variable.instance = entry.at("variable").at("instance").get<std::string>();
        }

        if (!mappings.emplace(ocpp16_key, std::make_pair(component, variable)).second) {
            throw Ocpp16CustomConfigMappingsError("Duplicate ocpp16_key in custom mapping file '" +
                                                  mapping_file_path.string() + "': " + ocpp16_key);
        }
    }

    return mappings;
}

} // namespace ocpp::v2
