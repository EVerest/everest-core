
// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>

#include <everest/logging.hpp>
#include <ocpp/common/schemas.hpp>

#include <memory>
#include <nlohmann/json-schema.hpp>
#include <nlohmann/json.hpp>

namespace {
using nlohmann::basic_json;
using nlohmann::json;
using nlohmann::json_uri;
using nlohmann::json_schema::basic_error_handler;
using nlohmann::json_schema::json_validator;

constexpr const char* test_schema = R"({
  "$schema": "http://json-schema.org/draft-07/schema#",
  "description": "Json schema for Custom configuration keys",
  "$comment": "This is just an example schema and can be modified according to custom requirements",
  "type": "object",
  "required": [],
  "properties": {
    "ConnectorType": {
      "type": "string",
      "enum": [
        "cType2",
        "sType2"
      ],
      "default": "sType2",
      "description": "Used to indicate the type of connector used by the unit",
      "readOnly": true
    },
    "ConfigLastUpdatedBy": {
      "type": "array",
      "items": {
        "type": "string",
        "enum": [
          "LOCAL",
          "CPMS"
        ]
      },
      "description": "Variable used to indicate how the Charge Points configuration was last updated",
      "readOnly": true
    }
  }
})";

class SchemaTest : public testing::Test {
    static void format_checker(const std::string& format, const std::string& value) {
        EVLOG_error << "format_checker: '" << format << "' '" << value << '\'';
    }

    static void loader(const json_uri& uri, json& schema) {
        schema = nlohmann::json_schema::draft7_schema_builtin;
    }

    class custom_error_handler : public basic_error_handler {
    private:
        void error(const json::json_pointer& pointer, const json& instance, const std::string& message) override {
            basic_error_handler::error(pointer, instance, message);
            EVLOG_error << "'" << pointer << "' - '" << instance << "': " << message;
            errors = true;
        }

    public:
        bool errors{false};
        constexpr bool has_errors() const {
            return errors;
        }
    };

protected:
    std::unique_ptr<json_validator> validator;
    json schema;
    custom_error_handler err;

    void SetUp() override {
        schema = json::parse(test_schema);
        validator = std::make_unique<json_validator>(&loader, &format_checker);
        validator->set_root_schema(schema);
        err.errors = false;
    }
};

TEST_F(SchemaTest, ValidationText) {
    json model = R"({"ConnectorType":"cType2"})"_json;
    validator->validate(model, err);
    EXPECT_FALSE(err.has_errors());
}

TEST_F(SchemaTest, ValidationObj) {
    json model;
    model["ConnectorType"] = "cType2";
    validator->validate(model, err);
    EXPECT_FALSE(err.has_errors());
}

TEST_F(SchemaTest, ValidationObjErr) {
    json model;
    model["ConnectorType"] = "cType3";
    validator->validate(model, err);
    EXPECT_TRUE(err.has_errors());
}

TEST(SchemaObj, Success) {
    ocpp::Schemas schema(std::move(json::parse(test_schema)));
    auto validator = schema.get_validator();
    json model;
    model["ConnectorType"] = "cType2";
    EXPECT_NO_THROW(validator->validate(model));
}

TEST(SchemaObj, Fail) {
    ocpp::Schemas schema(std::move(json::parse(test_schema)));
    auto validator = schema.get_validator();
    json model;
    model["ConnectorType"] = "cType3";
    EXPECT_ANY_THROW(validator->validate(model));
}

} // namespace
