// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <catch2/catch_all.hpp>

#include <framework/runtime.hpp>
#include <tests/helpers.hpp>
#include <utils/config.hpp>

namespace fs = std::filesystem;

SCENARIO("Check ManagerSettings Constructor", "[!throws]") {
    auto bin_dir = Everest::tests::get_bin_dir().string() + "/";
    GIVEN("An invalid prefix, but a valid config file") {
        THEN("It should throw BootException") {
            CHECK_THROWS_AS(
                Everest::ManagerSettings(bin_dir + "non-valid-prefix/", bin_dir + "valid_config/config.yaml"),
                Everest::BootException);
        }
    }
    GIVEN("A valid prefix, but a non existing config file") {
        THEN("It should throw BootException") {
            CHECK_THROWS_AS(Everest::ManagerSettings(bin_dir + "valid_config/", bin_dir + "non-existing-config.yaml"),
                            Everest::BootException);
        }
    }
    GIVEN("A valid prefix and a valid config file") {
        THEN("It should not throw") {
            CHECK_NOTHROW(Everest::ManagerSettings(bin_dir + "valid_config/", bin_dir + "valid_config/config.yaml"));
        }
    }
    GIVEN("A valid prefix and a valid config file with a custom prefix") {
        THEN("It should not throw") {
            auto ms = Everest::ManagerSettings(bin_dir + "valid_config_custom_prefix/usr",
                                               bin_dir + "valid_config_custom_prefix/usr/config.yaml");
            CHECK(ms.runtime_settings.etc_dir == bin_dir + "valid_config_custom_prefix/etc/everest");
        }
    }
    GIVEN("A broken yaml file") {
        // FIXME (aw): this also throws, if the folder doesn't even exists or some other things fail
        THEN("It should throw") {
            CHECK_THROWS(Everest::ManagerSettings(bin_dir + "broken_yaml/", bin_dir + "broken_yaml/config.yaml"));
        }
    }
    GIVEN("A empty yaml file") {
        THEN("It shouldn't throw") {
            CHECK_NOTHROW(Everest::ManagerSettings(bin_dir + "empty_yaml/", bin_dir + "empty_yaml/config.yaml"));
        }
    }
    GIVEN("A empty yaml object file") {
        THEN("It shouldn't throw") {
            CHECK_NOTHROW(
                Everest::ManagerSettings(bin_dir + "empty_yaml_object/", bin_dir + "empty_yaml_object/config.yaml"));
        }
    }
    GIVEN("A null yaml file") {
        THEN("It shouldn't throw") {
            CHECK_NOTHROW(Everest::ManagerSettings(bin_dir + "null_yaml/", bin_dir + "null_yaml/config.yaml"));
        }
    }
    GIVEN("A string yaml file") {
        THEN("It should throw Everest::EverestConfigError") {
            CHECK_THROWS_AS(Everest::ManagerSettings(bin_dir + "string_yaml/", bin_dir + "string_yaml/config.yaml"),
                            Everest::BootException);
        }
    }
    GIVEN("A non-exsiting database file with ConfigurationBootMode::DatabaseInit") {
        THEN("It should not throw and create the file") {
            CHECK_NOTHROW(Everest::ManagerSettings(bin_dir + "valid_config/", bin_dir + "valid_config/config.yaml",
                                                   "valid_config/non_existing.db"));
        }
    }
}
SCENARIO("Check ManagerConfig Constructor", "[!throws]") {
    auto bin_dir = Everest::tests::get_bin_dir().string() + "/";
    GIVEN("A config without modules") {
        auto ms = Everest::ManagerSettings(bin_dir + "empty_config/", bin_dir + "empty_config/config.yaml");
        auto config = Everest::ManagerConfig(ms);
        THEN("It should not contain the module some_module") {
            CHECK(!config.contains("some_module"));
        }
    }
    GIVEN("A config file referencing a non existent module") {
        auto ms = Everest::ManagerSettings(bin_dir + "missing_module/", bin_dir + "missing_module/config.yaml");
        THEN("It should throw Everest::EverestConfigError") {
            CHECK_THROWS_AS(Everest::ManagerConfig(ms), Everest::EverestConfigError);
        }
    }
    GIVEN("A config file using a module with broken manifest (missing meta data)") {
        auto ms = Everest::ManagerSettings(bin_dir + "broken_manifest_1/", bin_dir + "broken_manifest_1/config.yaml");
        THEN("It should throw Everest::EverestConfigError") {
            CHECK_THROWS_AS(Everest::ManagerConfig(ms), Everest::EverestConfigError);
        }
    }
    GIVEN("A config file using a module with broken manifest (empty file)") {
        auto ms = Everest::ManagerSettings(bin_dir + "broken_manifest_2/", bin_dir + "broken_manifest_2/config.yaml");
        THEN("It should throw Everest::EverestConfigError") {
            // FIXME: an empty manifest breaks the test?
            CHECK_THROWS_AS(Everest::ManagerConfig(ms), Everest::EverestConfigError);
        }
    }
    GIVEN("A config file using a module with broken manifest (broken module config)") {
        auto ms = Everest::ManagerSettings(bin_dir + "broken_manifest_3/", bin_dir + "broken_manifest_3/config.yaml");
        THEN("It should throw Everest::EverestConfigError") {
            CHECK_THROWS_AS(Everest::ManagerConfig(ms), Everest::EverestConfigError);
        }
    }
    GIVEN("A config file using a module with broken manifest (broken implementation config)") {
        auto ms = Everest::ManagerSettings(bin_dir + "broken_manifest_4/", bin_dir + "broken_manifest_4/config.yaml");
        THEN("It should throw Everest::EverestConfigError") {
            CHECK_THROWS_AS(Everest::ManagerConfig(ms), Everest::EverestConfigError);
        }
    }
    GIVEN("A config file with an unknown implementation config") {
        auto ms = Everest::ManagerSettings(bin_dir + "unknown_impls/", bin_dir + "unknown_impls/config.yaml");
        THEN("It should throw Everest::EverestConfigError") {
            CHECK_THROWS_AS(Everest::ManagerConfig(ms), Everest::EverestConfigError);
        }
    }
    GIVEN("A config file with an missing config entry") {
        auto ms =
            Everest::ManagerSettings(bin_dir + "missing_config_entry/", bin_dir + "missing_config_entry/config.yaml");
        THEN("It should throw Everest::EverestConfigError") {
            CHECK_THROWS_AS(Everest::ManagerConfig(ms), Everest::EverestConfigError);
        }
    }
    GIVEN("A config file with an missing implementation config entry") {
        auto ms = Everest::ManagerSettings(bin_dir + "missing_impl_config_entry/",
                                           bin_dir + "missing_impl_config_entry/config.yaml");
        THEN("It should throw Everest::EverestConfigError") {
            CHECK_THROWS_AS(Everest::ManagerConfig(ms), Everest::EverestConfigError);
        }
    }
    GIVEN("A config file with an invalid type of an implementation config entry") {
        auto ms = Everest::ManagerSettings(bin_dir + "invalid_config_entry_type/",
                                           bin_dir + "invalid_config_entry_type/config.yaml");
        THEN("It should throw Everest::EverestConfigError") {
            CHECK_THROWS_AS(Everest::ManagerConfig(ms), Everest::EverestConfigError);
        }
    }
    GIVEN("A config file using a module with an invalid interface (missing "
          "interface)") {
        auto ms = Everest::ManagerSettings(bin_dir + "missing_interface/", bin_dir + "missing_interface/config.yaml");
        THEN("It should throw Everest::EverestConfigError") {
            CHECK_THROWS_AS(Everest::ManagerConfig(ms), Everest::EverestConfigError);
        }
    }
    GIVEN("A valid config") {
        auto ms = Everest::ManagerSettings(bin_dir + "valid_config/", bin_dir + "valid_config/config.yaml");
        THEN("It should not throw at all") {
            CHECK_NOTHROW(Everest::ManagerConfig(ms));
        }
    }
    GIVEN("A valid config with a valid module") {
        auto ms =
            Everest::ManagerSettings(bin_dir + "valid_module_config/", bin_dir + "valid_module_config/config.yaml");
        THEN("It should not throw at all") {
            CHECK_NOTHROW(Everest::ManagerConfig(ms));
        }
    }
    GIVEN("A valid config with a valid module and a user-config applied") {
        auto ms = Everest::ManagerSettings(bin_dir + "valid_module_config_userconfig/",
                                           bin_dir + "valid_module_config_userconfig/config.yaml");
        THEN("It should not throw at all") {
            CHECK_NOTHROW([&]() {
                auto mc = Everest::ManagerConfig(ms);
                auto module_configs = mc.get_module_configurations();

                bool found = false;
                const auto config_params = module_configs.at("valid_module").configuration_parameters;
                for (const auto& param : config_params.at("!module")) {
                    if (param.name == "valid_config_entry") {
                        found = true;
                        CHECK(std::get<std::string>(param.value) == "hi");
                    }
                }

                if (!found) {
                    FAIL("Expected configuration parameter 'valid_config_entry' not found.");
                }
            }());
        }
    }
    GIVEN("A valid config with a valid module and enabled schema validation") {
        auto ms = Everest::ManagerSettings(bin_dir + "valid_module_config_validate/",
                                           bin_dir + "valid_module_config_validate/config.yaml");
        THEN("It should not throw at all") {
            CHECK_NOTHROW([&]() {
                auto mc = Everest::ManagerConfig(ms);
                auto interfaces = mc.get_interfaces();
                CHECK(interfaces.size() == 1);
                CHECK(interfaces.contains("TESTValidManifestCmdVar"));
                CHECK(interfaces.at("TESTValidManifestCmdVar").at("main") == "test_interface_cmd_var");
                auto types = mc.get_types();
                CHECK(types.size() == 1);
                CHECK(types.contains("/test_type"));
            }());
        }
    }
    GIVEN("A valid config in legacy json format with a valid module") {
        auto ms = Everest::ManagerSettings(bin_dir + "valid_module_config_json/",
                                           bin_dir + "valid_module_config_json/config.json");
        THEN("It should not throw at all") {
            CHECK_NOTHROW(Everest::ManagerConfig(ms));
        }
    }
    GIVEN("A config file that does not exist") {
        THEN("It should throw Everest::BootException") {
            CHECK_THROWS_AS(Everest::ManagerSettings(bin_dir + "valid_module_config_json/",
                                                     bin_dir + "valid_module_config_json/config.yaml"),
                            Everest::BootException);
        }
    }
    GIVEN("A valid config in legacy json format with multiple connected valid modules") {
        auto ms =
            Everest::ManagerSettings(bin_dir + "valid_complete_config/", bin_dir + "valid_complete_config/config.json");
        THEN("It should not throw at all") {
            CHECK_NOTHROW(Everest::ManagerConfig(ms));
        }
    }
    GIVEN("ManagerSettings are instantiated two times - first with fallback to init from config file, second with "
          "database") {
        auto db_path = bin_dir + "valid_config/everest.db";

        // Clean up before test
        if (fs::exists(db_path)) {
            fs::remove(db_path);
        }
        auto ms = Everest::ManagerSettings(bin_dir + "valid_config/", bin_dir + "valid_config/config.yaml", db_path);
        CHECK(ms.storage->contains_valid_config() == false);
        THEN("In the first intstantiation the database is not initialized") {
            CHECK_NOTHROW(Everest::ManagerConfig(ms));

            THEN("In the second instantiation the database is initialized and valid") {
                ms = Everest::ManagerSettings(bin_dir + "valid_config/", bin_dir + "valid_config/config.yaml", db_path);
                CHECK(ms.storage->contains_valid_config() == true);
                CHECK_NOTHROW(Everest::ManagerConfig(ms));
            }
            THEN("It should be possible to construct the ManagerSettings with a database path") {
                CHECK_NOTHROW(Everest::ManagerSettings(bin_dir + "valid_config/", db_path, Everest::DatabaseTag{}));
            }
        }
    }
}

SCENARIO("Check everest config parsing", "[!throws]") {
    auto bin_dir = Everest::tests::get_bin_dir().string() + "/";
    auto valid_complete_config_json = bin_dir + "valid_complete_config/config.json";
    GIVEN("A complete and valid config") {
        auto config = Everest::load_yaml(valid_complete_config_json);
        THEN("It should not throw") {
            CHECK_NOTHROW(everest::config::parse_module_configs(config.value("active_modules", json::object())));
        }
    }
    GIVEN("A valid config that misses module connections") {
        auto config = Everest::load_yaml(valid_complete_config_json);
        config["active_modules"]["valid_module_requires"].erase("connections");
        THEN("It should not throw") {
            CHECK_NOTHROW(everest::config::parse_module_configs(config.value("active_modules", json::object())));
        }
    }
    GIVEN("A valid config that misses a mapping") {
        auto config = Everest::load_yaml(valid_complete_config_json);
        config["active_modules"]["valid_module"].erase("mapping");
        THEN("It should not throw") {
            CHECK_NOTHROW(everest::config::parse_module_configs(config.value("active_modules", json::object())));
        }
    }
    GIVEN("A config where a module is missing the 'module' field") {
        auto config = Everest::load_yaml(valid_complete_config_json);
        config["active_modules"]["valid_module"].erase("module");
        THEN("It should throw ConfigParseException for missing 'module'") {
            CHECK_THROWS_AS(everest::config::parse_module_configs(config.value("active_modules", json::object())),
                            ConfigParseException);
        }
    }
    GIVEN("A config with only 'active_modules' and no 'settings'") {
        json config;
        config["active_modules"] = {{"valid_module", {{"module", "TESTValidManifest"}}}};
        THEN("It should not throw and parse default settings") {
            CHECK_NOTHROW(everest::config::parse_module_configs(config.value("active_modules", json::object())));
        }
    }
    GIVEN("A config with empty 'active_modules'") {
        json config;
        config["active_modules"] = json::object(); // empty object
        THEN("It should not throw and result in no modules") {
            auto result = everest::config::parse_module_configs(config.value("active_modules", json::object()));
            CHECK(result.empty());
        }
    }

    GIVEN("A config with unsupported JSON type in configuration parameter") {
        json config;
        config["active_modules"] = {
            {"test_module", {{"module", "test"}, {"config_module", {{"param1", json::array({1, 2, 3})}}}}}};
        THEN("It should throw due to unsupported config parameter type") {
            CHECK_THROWS(everest::config::parse_module_configs(config.value("active_modules", json::object())));
        }
    }
}

json complete_serialized_mod_config(json& serialized_mod_config, Everest::ManagerConfig& mc) {
    serialized_mod_config["interface_definitions"] = mc.get_interface_definitions();
    serialized_mod_config["types"] = mc.get_types();
    serialized_mod_config["module_provides"] = mc.get_interfaces();
    serialized_mod_config["settings"] = mc.get_settings();
    serialized_mod_config["schemas"] = mc.get_schemas();
    serialized_mod_config["module_names"] = mc.get_module_names();
    serialized_mod_config["manifests"] = mc.get_manifests();
    serialized_mod_config["error_map"] = mc.get_error_types();
    return serialized_mod_config;
}

SCENARIO("Check config constructor and functions", "[!throws]") {
    auto bin_dir = Everest::tests::get_bin_dir().string() + "/";
    auto ms = Everest::ManagerSettings(bin_dir + "two_module_test/", bin_dir + "two_module_test/config.yaml");
    GIVEN("A config with two connected modules") {
        THEN("It should not throw") {
            CHECK_NOTHROW([&]() {
                auto mc = Everest::ManagerConfig(ms);
                auto serialized_mod_config =
                    Everest::get_serialized_module_config("module_a", mc.get_module_configurations());
                complete_serialized_mod_config(serialized_mod_config, mc);
                Everest::MQTTSettings mqtt_settings;
                const auto config = Everest::Config(mqtt_settings, serialized_mod_config);
                config.get_requirement_initialization("module_a");
            }());
        }
    }
}

SCENARIO("Config constructor throws on missing required fields in serialized config", "[Config][throws]") {
    GIVEN("A serialized config missing required fields") {
        Everest::MQTTSettings mqtt_settings;
        Everest::json serialized_config = Everest::json::object();
        serialized_config["module_config"] = Everest::json::object();
        serialized_config["module_config"]["module_a"] = Everest::json::object();

        THEN("It should throw an exception") {
            CHECK_THROWS_AS(Everest::Config(mqtt_settings, serialized_config), json::exception);
        }
    }
}

SCENARIO("Config returns correct module info", "[Config]") {
    auto bin_dir = Everest::tests::get_bin_dir().string() + "/";
    auto ms = Everest::ManagerSettings(bin_dir + "two_module_test/", bin_dir + "two_module_test/config.yaml");
    auto mc = Everest::ManagerConfig(ms);
    auto serialized = Everest::get_serialized_module_config("module_a", mc.get_module_configurations());
    complete_serialized_mod_config(serialized, mc);
    Everest::MQTTSettings mqtt_settings;

    GIVEN("A valid serialized config") {
        Everest::Config config(mqtt_settings, serialized);

        WHEN("Calling get_module_info") {
            auto info = config.get_module_info("module_a");

            THEN("It should return the correct name and license") {
                CHECK(info.id == "module_a");
                CHECK(info.name == "TESTModuleA");
                CHECK(info.license == "https://opensource.org/licenses/Apache-2.0");
                CHECK(info.authors.at(0) == "author@example.com");
            }
        }
    }
}

SCENARIO("Config returns parsed module configs", "[Config]") {
    auto bin_dir = Everest::tests::get_bin_dir().string() + "/";
    auto ms = Everest::ManagerSettings(bin_dir + "two_module_test/", bin_dir + "two_module_test/config.yaml");
    auto mc = Everest::ManagerConfig(ms);
    auto serialized = Everest::get_serialized_module_config("module_a", mc.get_module_configurations());
    complete_serialized_mod_config(serialized, mc);
    Everest::MQTTSettings mqtt_settings;
    Everest::Config config(mqtt_settings, serialized);

    GIVEN("A valid config for module_a") {
        auto configs = config.get_module_configs("module_a");

        THEN("It should contain the correct config values") {
            CHECK(configs.find("main") != configs.end());
            CHECK(configs.find("!module") != configs.end());
            CHECK(std::get<std::string>(configs["!module"]["valid_module_config_entry"]) == "test");
            CHECK(std::get<int>(configs["main"]["valid_impl_config_entry"]) == 42);
        }
    }
}
