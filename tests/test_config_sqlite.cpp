// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <catch2/catch_all.hpp>

#include <everest/database/exceptions.hpp>
#include <everest/database/sqlite/connection.hpp>
#include <tests/helpers.hpp>
#include <utils/config/storage_sqlite.hpp>
#include <utils/yaml_loader.hpp>

using namespace everest::config;

Everest::ManagerSettings get_example_settings() {
    auto bin_dir = Everest::tests::get_bin_dir().string() + "/";
    return Everest::ManagerSettings(bin_dir + "valid_config/", bin_dir + "valid_config/config.yaml");
}

std::map<ModuleId, ModuleConfig> get_example_module_configs() {
    std::map<ModuleId, ModuleConfig> module_configs;
    ModuleConfig module_config;
    module_config.module_name = "example_module";
    module_config.standalone = true;
    module_config.capabilities = "capability1,capability2";
    module_config.telemetry_enabled = true;
    module_config.telemetry_config = TelemetryConfig(1);

    Fulfillment fulfillment;
    fulfillment.module_id = "module_id1";
    fulfillment.implementation_id = "implementation_id1";
    fulfillment.requirement = {"requirement_id1", 0};
    module_config.connections.insert({"connection1", {fulfillment}});

    Mapping module_mapping = {1};
    Mapping impl_mapping = {1, 1};

    module_config.mapping.module = module_mapping;
    module_config.mapping.implementations.insert({"implementation_id1", impl_mapping});

    ConfigurationParameterCharacteristics characteristics1;
    characteristics1.datatype = Datatype::Integer;
    characteristics1.mutability = Mutability::ReadWrite;
    characteristics1.unit = "ms";

    ConfigurationParameterCharacteristics characteristics2;
    characteristics2.datatype = Datatype::String;
    characteristics2.mutability = Mutability::ReadOnly;

    ConfigurationParameterCharacteristics characteristics3;
    characteristics3.datatype = Datatype::Path;
    characteristics3.mutability = Mutability::ReadWrite;

    ConfigurationParameterCharacteristics characteristics4;
    characteristics4.datatype = Datatype::Decimal;
    characteristics4.mutability = Mutability::ReadWrite;

    ConfigurationParameterCharacteristics characteristics5;
    characteristics5.datatype = Datatype::Boolean;
    characteristics5.mutability = Mutability::ReadWrite;

    ConfigurationParameter param1;
    param1.name = "integer_param";
    param1.value = 10;
    param1.characteristics = characteristics1;

    ConfigurationParameter param2;
    param2.name = "string_param";
    param2.value = std::string("example_value");
    param2.characteristics = characteristics2;

    ConfigurationParameter param3;
    param3.name = "path_param";
    param3.value = fs::path("/example/path");
    param3.characteristics = characteristics3;

    ConfigurationParameter param4;
    param4.name = "decimal_param";
    param4.value = 42.23;
    param4.characteristics = characteristics4;

    ConfigurationParameter param5;
    param5.name = "boolean_param";
    param5.value = true;
    param5.characteristics = characteristics5;

    module_config.configuration_parameters["!module"].push_back({param1});
    module_config.configuration_parameters["implementation_id1"].push_back({param2});
    module_config.configuration_parameters["!module"].push_back({param3});
    module_config.configuration_parameters["!module"].push_back({param4});
    module_config.configuration_parameters["!module"].push_back({param5});

    module_configs["example_module"] = module_config;

    ModuleConfig module_config2;
    module_config2.module_name = "Module1";
    module_config2.standalone = false;
    module_config2.telemetry_enabled = false;
    module_configs["module1"] = module_config2;

    return module_configs;
}

SCENARIO("Database initialization", "[db_initialization]") {
    const auto bin_dir = Everest::tests::get_bin_dir().string() + "/";
    const auto migrations_dir = bin_dir + "migrations";
    GIVEN("A valid migration path") {
        THEN("It should not throw") {
            CHECK_NOTHROW(SqliteStorage("file::memory:?cache=shared", migrations_dir));
        }
    }
    GIVEN("An invalid migration path") {
        THEN("It should throw") {
            CHECK_THROWS_AS(SqliteStorage("file::memory:?cache=shared", "invalid_migrations"),
                            everest::db::MigrationException);
        }
    }
}

TEST_CASE("Database operations", "[db_operation]") {
    auto bin_dir = Everest::tests::get_bin_dir().string() + "/";
    const auto migrations_dir = bin_dir + "migrations";
    everest::db::sqlite::Connection c("file::memory:?cache=shared");
    c.open_connection(); // keep at least one connection to keep the in-memory database alive
    SqliteStorage storage("file::memory:?cache=shared", migrations_dir);

    SECTION("Empty settings can not be retrieved") {
        auto response = storage.get_settings();
        REQUIRE(response.status == GenericResponseStatus::Failed);
    }

    SECTION("Empty module config can be retrieved") {
        auto response = storage.get_module_configs();
        REQUIRE(response.status == GenericResponseStatus::OK);
        REQUIRE(response.module_configs.size() == 0);
    }

    const auto module_configs = get_example_module_configs();
    const auto settings = get_example_settings();

    // valid config and settings can be successfully written
    REQUIRE(storage.write_module_configs(module_configs) == GenericResponseStatus::OK);
    REQUIRE(storage.write_settings(settings) == GenericResponseStatus::OK);

    SECTION("Module configurations can be written and correctly retrieved") {
        auto response = storage.get_module_configs();
        REQUIRE(response.status == GenericResponseStatus::OK);
        REQUIRE(response.module_configs.size() == 2);
    }
    SECTION("Configuration parameters can be retrieved") {
        auto response1 = storage.get_configuration_parameter({"example_module", "integer_param"});
        REQUIRE(response1.status == GetSetResponseStatus::OK);
        REQUIRE(response1.configuration_parameter.has_value());
        REQUIRE(std::get<int>(response1.configuration_parameter.value().value) == 10);

        auto response2 = storage.get_configuration_parameter({"example_module", "string_param", "implementation_id1"});
        REQUIRE(response2.status == GetSetResponseStatus::OK);
        REQUIRE(response2.configuration_parameter.has_value());
        REQUIRE(std::get<std::string>(response2.configuration_parameter.value().value) == "example_value");

        auto response3 = storage.get_configuration_parameter({"example_module", "path_param"});
        REQUIRE(response3.status == GetSetResponseStatus::OK);
        REQUIRE(response3.configuration_parameter.has_value());
        REQUIRE(std::get<fs::path>(response3.configuration_parameter.value().value) == fs::path("/example/path"));

        auto response4 = storage.get_configuration_parameter({"example_module", "decimal_param"});
        REQUIRE(response4.status == GetSetResponseStatus::OK);
        REQUIRE(response4.configuration_parameter.has_value());
        REQUIRE(std::get<double>(response4.configuration_parameter.value().value) == 42.23);

        auto response5 = storage.get_configuration_parameter({"example_module", "boolean_param"});
        REQUIRE(response5.status == GetSetResponseStatus::OK);
        REQUIRE(response5.configuration_parameter.has_value());
        REQUIRE(std::get<bool>(response5.configuration_parameter.value().value) == true);
    }
    SECTION("Unknown configuration can not be found") {
        auto response =
            storage.get_configuration_parameter({"module_that_does_not_exist", "param_that_does_not_exist"});
        REQUIRE(response.status == GetSetResponseStatus::NotFound);
    }
    SECTION("Configuration parameters can be updated") {
        auto response = storage.update_configuration_parameter({"example_module", "integer_param"}, "20");
        REQUIRE(response == GetSetResponseStatus::OK);
    }
    SECTION("Unknown configuration can not be updated") {
        auto response =
            storage.update_configuration_parameter({"module_that_does_not_exist", "param_that_does_not_exist"}, "20");
        REQUIRE(response == GetSetResponseStatus::NotFound);
    }
    SECTION("Settings can be retrieved") {
        auto response = storage.get_settings();
        REQUIRE(response.status == GenericResponseStatus::OK);
        REQUIRE(response.settings.has_value());
    }
    SECTION("Unknown module config can not be retrieved") {
        auto response = storage.get_module_config("unknown_module_id");
        REQUIRE(response.status == GenericResponseStatus::Failed);
    }
    SECTION("Configuration parameter for unknown configuration parameter identifier can not be retrieved") {
        ConfigurationParameterIdentifier id;
        id.module_id = "unknown_module_id";
        auto response = storage.get_configuration_parameter(id);
        REQUIRE(response.status == GetSetResponseStatus::NotFound);
    }
    SECTION("Configuration parameter for unknown configuration parameter identifier can not be written") {
        ConfigurationParameterIdentifier id;
        id.module_id = "unknown_module_id";
        ConfigurationParameterCharacteristics characteristics;
        characteristics.datatype = Datatype::String;
        characteristics.mutability = Mutability::ReadWrite;
        auto response = storage.write_configuration_parameter(id, characteristics, "value");
        REQUIRE(response == GetSetResponseStatus::NotFound);
    }
    SECTION("Configuration parameter for wrong type can not be retrieved") {
        ConfigurationParameterIdentifier id;
        id.module_id = "example_module";
        id.configuration_parameter_name = "integer_param";
        id.module_implementation_id = "!module";
        ConfigurationParameterCharacteristics characteristics;
        characteristics.datatype = Datatype::Integer;
        characteristics.mutability = Mutability::ReadWrite;
        auto write_response = storage.write_configuration_parameter(id, characteristics, "value");
        REQUIRE(write_response == GetSetResponseStatus::OK);
        auto get_response = storage.get_configuration_parameter(id);
        REQUIRE(get_response.status == GetSetResponseStatus::Failed);
    }
    SECTION("Config is not valid if not marked as valid") {
        REQUIRE(storage.contains_valid_config() == false);
        storage.mark_valid(false, "Test", std::nullopt);
        REQUIRE(storage.contains_valid_config() == false);
    }
    SECTION("Config is valid if marked as valid") {
        storage.mark_valid(true, "Test", "Test");
        REQUIRE(storage.contains_valid_config() == true);
    }
    SECTION("Config can be wiped from the database") {
        REQUIRE(storage.wipe() == GenericResponseStatus::OK);
    }
}
