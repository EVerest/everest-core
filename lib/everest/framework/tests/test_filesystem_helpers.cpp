// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <catch2/catch_all.hpp>

#include <tests/helpers.hpp>
#include <utils/exceptions.hpp>
#include <utils/filesystem.hpp>

namespace fs = std::filesystem;

SCENARIO("Check module config helper functions", "[!throws]") {
    std::string bin_dir = Everest::tests::get_bin_dir().string() + "/";
    auto dir = fs::path(bin_dir);
    auto file = dir / "empty_yaml" / "config.yaml";
    GIVEN("A file instead of a directory") {
        THEN("It should throw") {
            CHECK_THROWS_AS(Everest::assert_dir(file, "alias"), Everest::BootException);
        }
    }
    GIVEN("A directory") {
        THEN("It shouldn't throw") {
            CHECK_NOTHROW(Everest::assert_dir(dir, "alias"));
        }
    }
    GIVEN("A directory instead of a file") {
        THEN("It should throw") {
            CHECK_THROWS_AS(Everest::assert_file(dir, "alias"), Everest::BootException);
        }
    }
    GIVEN("A file") {
        THEN("It shouldn't throw") {
            CHECK_NOTHROW(Everest::assert_file(file, "alias"));
        }
    }
    GIVEN("A file with yaml extension and expecting yaml") {
        THEN("It should have the expected extension") {
            CHECK(Everest::has_extension(file.string(), ".yaml"));
        }
    }

    GIVEN("A file with yaml extension and expecting json") {
        THEN("It should not have the expected extension") {
            CHECK_FALSE(Everest::has_extension(file.string(), ".json"));
        }
    }
}
