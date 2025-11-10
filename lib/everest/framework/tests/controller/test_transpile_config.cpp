// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest
#include <sstream>
#include <string>

#include <catch2/catch_all.hpp>
#include <nlohmann/json.hpp>
#include <ryml.hpp>
#include <ryml_std.hpp>

#include "transpile_config.hpp"

using json = nlohmann::json;

const std::string json_strings = R"(
    "string": {
        "complex name": "it is",
        "empty": "",
        "complex-name": "with hypen!",
        "couldBeANumber": "e"
    }
)";

const std::string yaml_strings = R"(string:
  complex name: it is
  'complex-name': with hypen!
  couldBeANumber: e
  empty: ''
)";

const std::string json_numbers = R"(
    "number": {
        "intger": 1,
        "float": 1.5,
        "eNotation": 6.02214086E23
    }
)";

const std::string yaml_numbers = R"(number:
  eNotation: 6.02214086e+23
  float: 1.5
  intger: 1
)";

const std::string json_booleans = R"(
    "boolean": {
        "true": true,
        "false": false
    }
)";

const std::string yaml_booleans = R"(boolean:
  false: false
  true: true
)";

const std::string json_arrays = R"(
    "array": {
        "strings": ["The", "quick", "brown", "fox", "jumps", "over", "the", "lazy", "dog", ""],
        "number": [1, 2, 3, 4, 5, 6, 7, 8, 9],
        "boolean": [true, false, true, true, false, false, false],
        "mixed": [true, "a string", 42, "", null]
    }
)";

const std::string yaml_arrays = R"(array:
  boolean:
    - true
    - false
    - true
    - true
    - false
    - false
    - false
  mixed:
    - true
    - a string
    - 42
    - ''
    - null
  number:
    - 1
    - 2
    - 3
    - 4
    - 5
    - 6
    - 7
    - 8
    - 9
  strings:
    - The
    - quick
    - brown
    - fox
    - jumps
    - over
    - the
    - lazy
    - dog
    - ''
)";

const std::string json_null = R"(
    "null": null
)";

const std::string yaml_null = R"(null: null
)";

SCENARIO("Check config transpiler", "[!throws]") {
    GIVEN("only strings") {
        std::string json_serialized = "{";
        json_serialized += json_strings;
        json_serialized += "}";
        json parsed = json::parse(json_serialized);
        auto result_yaml = ryml::emitrs<std::string>(transpile_config(parsed));
        THEN("It should contain the relevant data") {
            CHECK(result_yaml == yaml_strings);
        }
    }
    GIVEN("only numbers") {
        std::string json_serialized = "{";
        json_serialized += json_numbers;
        json_serialized += "}";
        json parsed = json::parse(json_serialized);
        auto result_yaml = ryml::emitrs<std::string>(transpile_config(parsed));
        THEN("It should contain the relevant data") {
            CHECK(result_yaml == yaml_numbers);
        }
    }
    GIVEN("only booleans") {
        std::string json_serialized = "{";
        json_serialized += json_booleans;
        json_serialized += "}";
        json parsed = json::parse(json_serialized);
        auto result_yaml = ryml::emitrs<std::string>(transpile_config(parsed));
        THEN("It should contain the relevant data") {
            CHECK(result_yaml == yaml_booleans);
        }
    }
    GIVEN("only arrays") {
        std::string json_serialized = "{";
        json_serialized += json_arrays;
        json_serialized += "}";
        json parsed = json::parse(json_serialized);
        auto result_yaml = ryml::emitrs<std::string>(transpile_config(parsed));
        THEN("It should contain the relevant data") {
            CHECK(result_yaml == yaml_arrays);
        }
    }
    GIVEN("only null") {
        std::string json_serialized = "{";
        json_serialized += json_null;
        json_serialized += "}";
        json parsed = json::parse(json_serialized);
        auto result_yaml = ryml::emitrs<std::string>(transpile_config(parsed));
        THEN("It should contain the relevant data") {
            CHECK(result_yaml == yaml_null);
        }
    }
    GIVEN("everything") {
        std::string json_serialized = "{";
        json_serialized += json_arrays;
        json_serialized += ",";
        json_serialized += json_booleans;
        json_serialized += ",";
        json_serialized += json_null;
        json_serialized += ",";
        json_serialized += json_numbers;
        json_serialized += ",";
        json_serialized += json_strings;
        json_serialized += "}";
        json parsed = json::parse(json_serialized);
        std::string expected_yaml = "";
        expected_yaml += yaml_arrays;
        expected_yaml += yaml_booleans;
        expected_yaml += yaml_null;
        expected_yaml += yaml_numbers;
        expected_yaml += yaml_strings;
        auto result_yaml = ryml::emitrs<std::string>(transpile_config(parsed));
        THEN("It should contain the relevant data") {
            CHECK(result_yaml == expected_yaml);
        }
    }
}
