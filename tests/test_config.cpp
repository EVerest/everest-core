// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include <boost/bind/bind.hpp>
#include <boost/test/included/unit_test.hpp>

#include <everest/logging.hpp>
#include <framework/everest.hpp>
#include <nlohmann/json.hpp>
#include <utils/config.hpp>
#include <utils/mqtt_abstraction.hpp>

using namespace boost::unit_test;
using json = nlohmann::json;

///
/// \brief tests for the EVerest Config class
///
class test_config {

public:
    ///
    /// \brief tests that an empty config works
    ///
    void test_empty_config() {
        Everest::Config config =
            Everest::Config("../dist/schemas", "valid/config.json", "test_modules", "test_interfaces");

        BOOST_TEST(!config.contains("some_module"));

        json main_config = json({});

        // BOOST_TEST(config.get_main_config() == main_config);
    }

    ///
    /// \brief an EverestInternalError exception is expected when files are missing from the maindir
    ///
    void test_wrong_maindir() {
        BOOST_CHECK_THROW(Everest::Config("../dist/schemas", "wrong_maindir", "test_modules", "test_interfaces"),
                          Everest::EverestConfigError);
    }

    ///
    /// \brief an EverestConfigError exception is expected when config.js is missing
    ///
    void test_missing_config() {
        BOOST_CHECK_THROW(Everest::Config("../dist/schemas", "missing_config", "test_modules", "test_interfaces"),
                          Everest::EverestConfigError);
    }

    ///
    /// \brief an EverestConfigError exception is expected when config.js is broken
    ///
    void test_broken_config() {
        BOOST_CHECK_THROW(
            Everest::Config("../dist/schemas", "broken_config/config.json", "test_modules", "test_interfaces"),
            Everest::EverestConfigError);
    }

    ///
    /// \brief an EverestConfigError exception is expected when config.js references a missing module
    ///
    void test_config_with_missing_module() {
        BOOST_CHECK_THROW(
            Everest::Config("../dist/schemas", "missing_module_config/config.json", "test_modules", "test_interfaces"),
            Everest::EverestConfigError);
    }

    ///
    /// \brief an EverestConfigError exception is expected when config.js references a module with a broken manifest
    ///
    void test_config_with_broken_manifest() {
        BOOST_CHECK_THROW(Everest::Config("../dist/schemas", "broken_manifest/config.json", "broken_manifest/modules",
                                          "test_interfaces"),
                          Everest::EverestConfigError);
        BOOST_CHECK_THROW(Everest::Config("../dist/schemas", "broken_manifest2/config.json", "broken_manifest2/modules",
                                          "test_interfaces"),
                          Everest::EverestConfigError);
    }

    ///
    /// \brief an EverestConfigError exception is expected when config.js references a module with a broken manifest
    ///
    void test_config_with_valid_manifest_missing_class() {
        BOOST_CHECK_THROW(Everest::Config("../dist/schemas", "valid_manifest_missing_interface/config.json",
                                          "valid_manifest_missing_interface/modules", "test_interfaces"),
                          Everest::EverestConfigError);
    }

    ///
    /// \brief This should work perfectly fine
    ///
    void test_config_with_valid_manifest_valid_class() {
        Everest::Config("../dist/schemas", "valid_manifest_valid_interface/config.json",
                        "valid_manifest_valid_interface/modules", "valid_manifest_valid_interface/interfaces");
    }

    //    ///
    //    /// \brief loads the default config from the dist directory, this should always work
    //    ///
    //    void test_dist_config() {
    //        Everest::Config config = Everest::Config("dist");
    //
    //        BOOST_TEST(config.contains("store"));
    //    }
};
//
test_suite* init_unit_test_suite(int /*argc*/, char* /*argv*/[]) {
    boost::shared_ptr<test_config> tester(new test_config);

    framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(&test_config::test_empty_config, tester)));
    framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(&test_config::test_wrong_maindir, tester)));
    framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(&test_config::test_missing_config, tester)));
    framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(&test_config::test_broken_config, tester)));
    framework::master_test_suite().add(
        BOOST_TEST_CASE(boost::bind(&test_config::test_config_with_missing_module, tester)));
    framework::master_test_suite().add(
        BOOST_TEST_CASE(boost::bind(&test_config::test_config_with_broken_manifest, tester)));
    framework::master_test_suite().add(
        BOOST_TEST_CASE(boost::bind(&test_config::test_config_with_valid_manifest_missing_class, tester)));
    framework::master_test_suite().add(
        BOOST_TEST_CASE(boost::bind(&test_config::test_config_with_valid_manifest_valid_class, tester)));
    // framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(&test_config::test_dist_config, tester)));
    return 0;
}
