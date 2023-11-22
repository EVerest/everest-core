// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <catch2/catch_all.hpp>

#include <framework/runtime.hpp>
#include <tests/helpers.hpp>
#include <utils/config.hpp>

namespace fs = std::filesystem;

SCENARIO("Check RuntimeSetting Constructor", "[!throws]") {
    std::string bin_dir = Everest::tests::get_bin_dir().string() + "/";
    GIVEN("An invalid prefix, but a valid config file") {
        THEN("It should throw BootException") {
            CHECK_THROWS_AS(
                Everest::RuntimeSettings(bin_dir + "non-valid-prefix/", bin_dir + "valid_config/config.yaml"),
                Everest::BootException);
        }
    }
    GIVEN("A valid prefix, but a non existing config file") {
        THEN("It should throw BootException") {
            CHECK_THROWS_AS(Everest::RuntimeSettings(bin_dir + "valid_config/", bin_dir + "non-existing-config.yaml"),
                            Everest::BootException);
        }
    }
    GIVEN("A valid prefix and a valid config file") {
        THEN("It should not throw") {
            CHECK_NOTHROW(Everest::RuntimeSettings(bin_dir + "valid_config/", bin_dir + "valid_config/config.yaml"));
        }
    }
    GIVEN("A broken yaml file") {
        THEN("It should throw") {
            CHECK_THROWS(Everest::RuntimeSettings(bin_dir + "broken_yaml/", bin_dir + "broken_yaml/config.yaml"));
        }
    }
    GIVEN("A empty yaml file") {
        THEN("It shouldn't throw") {
            CHECK_NOTHROW(Everest::RuntimeSettings(bin_dir + "empty_yaml/", bin_dir + "empty_yaml/config.yaml"));
        }
    }
    GIVEN("A empty yaml object file") {
        THEN("It shouldn't throw") {
            CHECK_NOTHROW(
                Everest::RuntimeSettings(bin_dir + "empty_yaml_object/", bin_dir + "empty_yaml_object/config.yaml"));
        }
    }
    GIVEN("A null yaml file") {
        THEN("It shouldn't throw") {
            CHECK_NOTHROW(Everest::RuntimeSettings(bin_dir + "null_yaml/", bin_dir + "null_yaml/config.yaml"));
        }
    }
    GIVEN("A string yaml file") {
        THEN("It should throw Everest::EverestConfigError") {
            CHECK_THROWS_AS(Everest::RuntimeSettings(bin_dir + "string_yaml/", bin_dir + "string_yaml/config.yaml"),
                            Everest::BootException);
        }
    }
}
SCENARIO("Check Config Constructor", "[!throws]") {
    std::string bin_dir = Everest::tests::get_bin_dir().string() + "/";
    GIVEN("A config without modules") {
        std::shared_ptr<Everest::RuntimeSettings> rs = std::make_shared<Everest::RuntimeSettings>(
            Everest::RuntimeSettings(bin_dir + "empty_config/", bin_dir + "empty_config/config.yaml"));
        Everest::Config config = Everest::Config(rs);
        THEN("It should not contain the module some_module") {
            CHECK(!config.contains("some_module"));
        }
    }
    GIVEN("A config file referencing a non existent module") {
        std::shared_ptr<Everest::RuntimeSettings> rs = std::make_shared<Everest::RuntimeSettings>(
            Everest::RuntimeSettings(bin_dir + "missing_module/", bin_dir + "missing_module/config.yaml"));
        THEN("It should throw Everest::EverestConfigError") {
            CHECK_THROWS_AS(Everest::Config(rs), Everest::EverestConfigError);
        }
    }
    GIVEN("A config file using a module with broken manifest (missing meta data)") {
        std::shared_ptr<Everest::RuntimeSettings> rs = std::make_shared<Everest::RuntimeSettings>(
            Everest::RuntimeSettings(bin_dir + "broken_manifest_1/", bin_dir + "broken_manifest_1/config.yaml"));
        THEN("It should throw Everest::EverestConfigError") {
            CHECK_THROWS_AS(Everest::Config(rs), Everest::EverestConfigError);
        }
    }
    GIVEN("A config file using a module with broken manifest (empty file)") {
        std::shared_ptr<Everest::RuntimeSettings> rs = std::make_shared<Everest::RuntimeSettings>(
            Everest::RuntimeSettings(bin_dir + "broken_manifest_2/", bin_dir + "broken_manifest_2/config.yaml"));
        THEN("It should throw Everest::EverestConfigError") {
            // FIXME: an empty manifest breaks the test?
            CHECK_THROWS_AS(Everest::Config(rs), Everest::EverestConfigError);
        }
    }
    GIVEN("A config file using a module with an invalid interface (missing "
          "interface)") {
        std::shared_ptr<Everest::RuntimeSettings> rs = std::make_shared<Everest::RuntimeSettings>(
            Everest::RuntimeSettings(bin_dir + "missing_interface/", bin_dir + "missing_interface/config.yaml"));
        THEN("It should throw Everest::EverestConfigError") {
            CHECK_THROWS_AS(Everest::Config(rs), Everest::EverestConfigError);
        }
    }
    GIVEN("A valid config") {
        std::shared_ptr<Everest::RuntimeSettings> rs = std::make_shared<Everest::RuntimeSettings>(
            Everest::RuntimeSettings(bin_dir + "valid_config/", bin_dir + "valid_config/config.yaml"));
        THEN("It should not throw at all") {
            CHECK_NOTHROW(Everest::Config(rs));
        }
    }
}
