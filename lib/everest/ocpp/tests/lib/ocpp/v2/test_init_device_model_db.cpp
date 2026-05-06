// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include <ocpp/v2/device_model_storage_sqlite.hpp>

#define private public
// database_exists must be set with this test, but std::filesystem::exists is hard to stub, so this way we can set it
// in the test.
#include <ocpp/v2/init_device_model_db.hpp>
#include <ocpp/v2/ocpp16_component_config_patcher.hpp>
#undef private

#include <lib/ocpp/common/database_testing_utils.hpp>

#include "ocpp16_test_config.hpp"

using namespace everest::db;
using namespace everest::db::sqlite;

namespace ocpp::v2 {

class InitDeviceModelDbTest : public DatabaseTestingUtils {
protected:
    const std::string DATABASE_PATH = "file::memory:?cache=shared";
    const std::string MIGRATION_FILES_PATH = "./resources/v2/device_model_migration_files";
    const std::string CONFIGS_PATH = "./resources/config/v2/component_config";
    const std::string CONFIGS_PATH_CHANGED = "./resources/config/v2/changed/component_config";
    const std::string CONFIGS_PATH_REQUIRED_NO_VALUE = "./resources/config/v2/wrong/component_config_required_no_value";
    const std::string CONFIGS_PATH_WRONG_VALUE_TYPE = "./resources/config/v2/wrong/component_config_wrong_value_type";
    const std::string STANDARD_CONFIGS_PATH = "./resources/example_config/v2/component_config";

public:
    enum class ValueCompareMode {
        Exact,
        JsonSemantic
    };

    InitDeviceModelDbTest() {
    }

    ///
    /// \brief Check if tables from the vector exist in the database.
    /// \param tables   List of tables to check.
    /// \param exist    True if they should exist, false if they should not.
    /// \return True when all tables exist while they should or when no tables exist whily they shouldn't.
    ///
    bool check_all_tables_exist(const std::vector<std::string>& tables, const bool exist);

    ///
    /// \brief Check if a given component exists in the database.
    ///
    /// If an optional parameter is std::nullopt, it will be compared with 'NULL' in the database.
    ///
    /// \param component_name           Name of the component
    /// \param component_instance       Instance of the component (optional)
    /// \param component_evse_id        EVSE id  (optional)
    /// \param component_connector_id   Connector id (optional)
    /// \return
    ///
    bool component_exists(const std::string& component_name, const std::optional<std::string>& component_instance,
                          const std::optional<int>& component_evse_id,
                          const std::optional<int>& component_connector_id);

    ///
    /// \brief Check if a given attribute exists in the databse.
    ///
    /// If an optional parameter is std::nullopt, it will be compared with 'NULL' in the database.
    ///
    /// \param component_name           Component
    /// \param component_instance       Component instance (optional)
    /// \param component_evse_id        EVSE id of the component (optional)
    /// \param component_connector_id   Connector id of the component (optional)
    /// \param variable_name            Variable name
    /// \param variable_instance        Instance of the variable (optional)
    /// \param mutability               Attribute mutability (optional)
    /// \param type                     Attribute type
    /// \return True if the attribute exists.
    ///
    bool attribute_exists(const std::string& component_name, const std::optional<std::string>& component_instance,
                          const std::optional<int>& component_evse_id, const std::optional<int>& component_connector_id,
                          const std::string& variable_name, const std::optional<std::string>& variable_instance,
                          const std::optional<MutabilityEnum>& mutability, const AttributeEnum& type);

    ///
    /// \brief Check if a given variable exists in the database.
    ///
    /// If an optional parameter is std::nullopt, it will be compared with 'NULL' in the database.
    ///
    /// \param component_name           Component name
    /// \param component_instance       Component instance (optional)
    /// \param component_evse_id        EVSE id of the compnent (optional)
    /// \param component_connector_id   Connector id of the component (optional)
    /// \param variable_name            Variable name
    /// \param variable_instance        Instance of the variable (optional)
    /// \return True if the variable exists.
    ///
    bool variable_exists(const std::string& component_name, const std::optional<std::string>& component_instance,
                         const std::optional<int>& component_evse_id, const std::optional<int>& component_connector_id,
                         const std::string& variable_name, const std::optional<std::string>& variable_instance,
                         const std::optional<std::string> source = std::nullopt);

    ///
    /// \brief Check if variable characteristics exists in the database.
    ///
    /// If an optional parameter is std::nullopt, it will be compared with 'NULL' in the database.
    ///
    /// \param component_name           Component name
    /// \param component_instance       Component instance (optional)
    /// \param component_evse_id        EVSE id of the component (optional)
    /// \param component_connector_id   Connector id of the component (optional)
    /// \param variable_name            Variable name
    /// \param variable_instance        Variable instance (optional)
    /// \param data_type                Dharacteristic data type
    /// \param max_limit                Characteristic max limit (optional)
    /// \param min_limit                Characteristic min limit (optional)
    /// \param supports_monitoring      Characteristic supports monitoring
    /// \param unit                     Characteristic unit (optional)
    /// \param values_list              Characteristic values list (optional)
    /// \return True if the characteristic exists in the database
    ///
    bool characteristics_exists(const std::string& component_name, const std::optional<std::string>& component_instance,
                                const std::optional<int>& component_evse_id,
                                const std::optional<int>& component_connector_id, const std::string& variable_name,
                                const std::optional<std::string>& variable_instance, const DataEnum data_type,
                                const std::optional<double>& max_limit, const std::optional<double>& min_limit,
                                const bool supports_monitoring, const std::optional<std::string>& unit,
                                const std::optional<std::string>& values_list);

    ///
    /// \brief Check if variable attribute has given value.
    /// \param component_name           Component name
    /// \param component_instance       Component instance
    /// \param component_evse_id        EVSE id of the component
    /// \param component_connector_id   Connector id of the component
    /// \param variable_name            Variable name
    /// \param variable_instance        Variable instance
    /// \param type                     Attribute type
    /// \param value                    Value to check.
    /// \return True if the attribute has the given value.
    ///
    bool attribute_has_value(const std::string& component_name, const std::optional<std::string>& component_instance,
                             const std::optional<int>& component_evse_id,
                             const std::optional<int>& component_connector_id, const std::string& variable_name,
                             const std::optional<std::string>& variable_instance, const AttributeEnum& type,
                             const std::string& value, const ValueCompareMode compare_mode = ValueCompareMode::Exact);

    ///
    /// \brief set_attribute_source     Set the source of a given variable attribute.
    /// \param component_name           Component name
    /// \param component_instance       Component instance
    /// \param component_evse_id        EVSE id of the component
    /// \param component_connector_id   Connector id of the component
    /// \param variable_name            Variable name
    /// \param variable_instance        Variable instance
    /// \param type                     Attribute type
    /// \param source                   Source to set
    ///
    void set_attribute_source(const std::string& component_name, const std::optional<std::string>& component_instance,
                              const std::optional<int>& component_evse_id,
                              const std::optional<int>& component_connector_id, const std::string& variable_name,
                              const std::optional<std::string>& variable_instance, const AttributeEnum& type,
                              const std::string& source);
};

TEST_F(InitDeviceModelDbTest, init_db) {
    /* This test tests if the initialization of the database is running successfully. It will first initialize with
     * a given component config and check some variables and attributes to see if they exist. It will then initialize
     * again, but with a changed component config, which should update the database (in some cases).
     * Because we test with an in memory database, and we never know what the order is of the tests, we test both cases
     * in one test.
     *
     * What we do in this test:
     *
     * - Insert whole configuration
     * - add EVSE / Connector component
     * - remove EVSE / Connector component
     * - add EVSE / Connector Variable
     * - add non-EVSE / Connector Variable
     * - remove EVSE / Connector Variable
     * - change EVSE / Connector Variable
     * - add EVSE / Connector Variable Attribute
     * - remove EVSE / Connector Variable Attribute
     * - change EVSE / Connector Variable Attribute
     * - add EVSE / Connector Variable Characteristic
     * - remove EVSE / Connector Variable Characteristic
     * - change EVSE / Connector Variable Characteristic
     * - change attribute not part of EVSE / Connector -> should not change in database
     */

    // No tables should exist yet.
    EXPECT_TRUE(check_all_tables_exist({"COMPONENT", "VARIABLE", "DATATYPE", "MONITOR", "MUTABILITY", "SEVERITY",
                                        "VARIABLE_ATTRIBUTE", "VARIABLE_ATTRIBUTE_TYPE", "VARIABLE_CHARACTERISTICS",
                                        "VARIABLE_MONITORING"},
                                       false));

    InitDeviceModelDb db(DATABASE_PATH, MIGRATION_FILES_PATH);

    // Database should not exist yet. But since it does a filesystem check and we have an in memory database, we
    // explicitly set the variable here.
    db.database_exists = false;
    auto component_configs = get_all_component_configs(CONFIGS_PATH);
    ASSERT_NO_THROW(db.initialize_database(component_configs, true));

    // Tables should have been created now.
    EXPECT_TRUE(check_all_tables_exist({"COMPONENT", "VARIABLE", "DATATYPE", "MONITOR", "MUTABILITY", "SEVERITY",
                                        "VARIABLE_ATTRIBUTE", "VARIABLE_ATTRIBUTE_TYPE", "VARIABLE_CHARACTERISTICS",
                                        "VARIABLE_MONITORING"},
                                       true));

    EXPECT_TRUE(attribute_exists("EVSE", std::nullopt, 1, std::nullopt, "AllowReset", std::nullopt,
                                 MutabilityEnum::ReadOnly, AttributeEnum::Actual));
    EXPECT_FALSE(variable_exists("Connector", std::nullopt, 1, 1, "Enabled", std::nullopt));
    EXPECT_TRUE(variable_exists("EVSE", std::nullopt, 1, std::nullopt, "ISO15118EvseId", std::nullopt));
    EXPECT_TRUE(characteristics_exists("EVSE", std::nullopt, 1, std::nullopt, "ISO15118EvseId", std::nullopt,
                                       DataEnum::string, std::nullopt, std::nullopt, true, std::nullopt, std::nullopt));
    EXPECT_TRUE(attribute_exists("EVSE", std::nullopt, 1, std::nullopt, "ISO15118EvseId", std::nullopt,
                                 MutabilityEnum::ReadWrite, AttributeEnum::Actual));
    EXPECT_TRUE(attribute_exists("Connector", std::nullopt, 1, 1, "ChargeProtocol", std::nullopt, std::nullopt,
                                 AttributeEnum::Actual));
    EXPECT_TRUE(attribute_exists("Connector", std::nullopt, 1, 1, "SupplyPhases", std::nullopt,
                                 MutabilityEnum::ReadOnly, AttributeEnum::Actual));
    EXPECT_TRUE(characteristics_exists("Connector", std::nullopt, 1, 1, "ConnectorType", std::nullopt, DataEnum::string,
                                       std::nullopt, std::nullopt, true, std::nullopt, std::nullopt));
    EXPECT_TRUE(component_exists("UnitTestCtrlr", std::nullopt, 2, 3));
    EXPECT_FALSE(component_exists("EVSE", std::nullopt, 3, std::nullopt));
    EXPECT_TRUE(component_exists("Connector", std::nullopt, 2, 1));
    EXPECT_TRUE(variable_exists("Connector", std::nullopt, 2, 1, "AvailabilityState", std::nullopt));
    EXPECT_TRUE(variable_exists("Connector", std::nullopt, 2, 1, "Available", std::nullopt));
    EXPECT_TRUE(variable_exists("Connector", std::nullopt, 2, 1, "ChargeProtocol", std::nullopt));
    EXPECT_TRUE(variable_exists("Connector", std::nullopt, 2, 1, "ConnectorType", std::nullopt));
    EXPECT_TRUE(variable_exists("Connector", std::nullopt, 2, 1, "SupplyPhases", std::nullopt));
    EXPECT_TRUE(attribute_exists("Connector", std::nullopt, 2, 1, "ChargeProtocol", std::nullopt, std::nullopt,
                                 AttributeEnum::Actual));
    EXPECT_TRUE(attribute_exists("Connector", std::nullopt, 2, 1, "ConnectorType", std::nullopt,
                                 MutabilityEnum::ReadOnly, AttributeEnum::Actual));
    EXPECT_TRUE(characteristics_exists("Connector", std::nullopt, 2, 1, "Available", std::nullopt, DataEnum::boolean,
                                       std::nullopt, std::nullopt, true, std::nullopt, std::nullopt));
    EXPECT_TRUE(characteristics_exists("Connector", std::nullopt, 2, 1, "AvailabilityState", std::nullopt,
                                       DataEnum::OptionList, std::nullopt, std::nullopt, true, std::nullopt,
                                       "Available,Occupied,Reserved,Unavailable,Faulted"));

    EXPECT_FALSE(component_exists("Connector", std::nullopt, 2, 2));
    EXPECT_TRUE(characteristics_exists("EVSE", std::nullopt, 1, std::nullopt, "Power", std::nullopt, DataEnum::decimal,
                                       22000.0, std::nullopt, true, "W", std::nullopt));
    EXPECT_TRUE(characteristics_exists("EVSE", std::nullopt, 2, std::nullopt, "AvailabilityState", std::nullopt,
                                       DataEnum::OptionList, std::nullopt, std::nullopt, true, std::nullopt,
                                       "Available,Occupied,Reserved,Unavailable,Faulted"));
    EXPECT_TRUE(attribute_exists("EVSE", std::nullopt, 2, std::nullopt, "Power", std::nullopt, MutabilityEnum::ReadOnly,
                                 AttributeEnum::Actual));
    EXPECT_TRUE(attribute_exists("EVSE", std::nullopt, 2, std::nullopt, "Power", std::nullopt, MutabilityEnum::ReadOnly,
                                 AttributeEnum::MaxSet));
    EXPECT_FALSE(attribute_exists("EVSE", std::nullopt, 1, std::nullopt, "Power", std::nullopt,
                                  MutabilityEnum::ReadWrite, AttributeEnum::Target));
    EXPECT_TRUE(characteristics_exists("UnitTestCtrlr", std::nullopt, 2, 3, "UnitTestPropertyAName", std::nullopt,
                                       DataEnum::boolean, std::nullopt, std::nullopt, true, std::nullopt,
                                       std::nullopt));
    EXPECT_TRUE(variable_exists("UnitTestCtrlr", std::nullopt, 2, 3, "UnitTestPropertyAName", std::nullopt, "OCPP"));
    EXPECT_TRUE(variable_exists("UnitTestCtrlr", std::nullopt, 2, 3, "UnitTestPropertyBName", std::nullopt));
    EXPECT_TRUE(variable_exists("UnitTestCtrlr", std::nullopt, 2, 3, "UnitTestPropertyCName", std::nullopt));

    // Test some values.
    EXPECT_TRUE(attribute_has_value("UnitTestCtrlr", std::nullopt, 2, 3, "UnitTestPropertyBName", std::nullopt,
                                    AttributeEnum::Actual, "test_value"));
    // Test some not default values.
    EXPECT_TRUE(attribute_has_value("EVSE", std::nullopt, 1, std::nullopt, "Available", std::nullopt,
                                    AttributeEnum::Actual, "false"));
    EXPECT_TRUE(attribute_has_value("EVSE", std::nullopt, 1, std::nullopt, "Power", std::nullopt, AttributeEnum::MaxSet,
                                    "44000"));
    EXPECT_TRUE(attribute_has_value("EVSE", std::nullopt, 1, std::nullopt, "Power", std::nullopt, AttributeEnum::Actual,
                                    "2000"));
    EXPECT_TRUE(attribute_has_value("EVSE", std::nullopt, 1, std::nullopt, "SupplyPhases", std::nullopt,
                                    AttributeEnum::Actual, "6"));
    EXPECT_TRUE(
        attribute_has_value("Connector", std::nullopt, 2, 1, "Available", std::nullopt, AttributeEnum::Actual, "true"));
    EXPECT_TRUE(attribute_has_value("Connector", std::nullopt, 2, 1, "ConnectorType", std::nullopt,
                                    AttributeEnum::Actual, "cChaoJi"));
    EXPECT_TRUE(attribute_has_value("EVSE", std::nullopt, 1, std::nullopt, "Available", std::nullopt,
                                    AttributeEnum::Actual, "false"));
    EXPECT_TRUE(attribute_has_value("EVSE", std::nullopt, 2, std::nullopt, "Available", std::nullopt,
                                    AttributeEnum::Actual, "true"));
    EXPECT_TRUE(attribute_has_value("EVSE", std::nullopt, 2, std::nullopt, "Power", std::nullopt, AttributeEnum::MaxSet,
                                    "22000"));
    EXPECT_TRUE(attribute_has_value("EVSE", std::nullopt, 2, std::nullopt, "AvailabilityState", std::nullopt,
                                    AttributeEnum::Actual, "Faulted"));
    EXPECT_TRUE(attribute_has_value("Connector", std::nullopt, 1, 1, "ConnectorType", std::nullopt,
                                    AttributeEnum::Actual, "cGBT"));

    // Default value
    EXPECT_TRUE(attribute_has_value("Connector", std::nullopt, 1, 1, "Available", std::nullopt, AttributeEnum::Actual,
                                    "false"));
    EXPECT_TRUE(attribute_has_value("EVSE", std::nullopt, 1, std::nullopt, "AvailabilityState", std::nullopt,
                                    AttributeEnum::Actual, "Unavailable"));
    EXPECT_TRUE(attribute_has_value("EVSE", std::nullopt, 2, std::nullopt, "SupplyPhases", std::nullopt,
                                    AttributeEnum::Actual, "0"));

    // So now we have made some changes and added a new EVSE and changed the connector. The database should be changed
    // accordingly.

    // First set the source of an attribute to something else than 'default'
    set_attribute_source("Connector", std::nullopt, 1, 1, "Available", std::nullopt, AttributeEnum::Actual, "test");

    InitDeviceModelDb db2 = InitDeviceModelDb(DATABASE_PATH, MIGRATION_FILES_PATH);
    // This time, the database does exist (again: std::filesystem::exists, which is automatically used, will not work
    // here because we use an in memory database, so we set the member ourselves).
    db2.database_exists = true;
    component_configs = get_all_component_configs(CONFIGS_PATH_CHANGED);
    ASSERT_NO_THROW(db2.initialize_database(component_configs, false));

    // So now some records should have been changed !
    EXPECT_TRUE(attribute_exists("EVSE", std::nullopt, 1, std::nullopt, "AllowReset", std::nullopt,
                                 MutabilityEnum::ReadOnly, AttributeEnum::Actual));
    // Added variable
    EXPECT_TRUE(variable_exists("Connector", std::nullopt, 1, 1, "Enabled", std::nullopt));

    // Removed variable (also removed characteristics and attribute)
    EXPECT_FALSE(variable_exists("EVSE", std::nullopt, 1, std::nullopt, "ISO15118EvseId", std::nullopt));
    EXPECT_FALSE(characteristics_exists("EVSE", std::nullopt, 1, std::nullopt, "ISO15118EvseId", std::nullopt,
                                        DataEnum::string, std::nullopt, std::nullopt, true, std::nullopt,
                                        std::nullopt));
    EXPECT_FALSE(attribute_exists("EVSE", std::nullopt, 1, std::nullopt, "ISO15118EvseId", std::nullopt,
                                  MutabilityEnum::ReadWrite, AttributeEnum::Actual));

    // Changed mutability
    EXPECT_FALSE(attribute_exists("Connector", std::nullopt, 1, 1, "ChargeProtocol", std::nullopt, std::nullopt,
                                  AttributeEnum::Actual));
    EXPECT_TRUE(attribute_exists("Connector", std::nullopt, 1, 1, "ChargeProtocol", std::nullopt,
                                 MutabilityEnum::WriteOnly, AttributeEnum::Actual));

    // Changed attribute type
    EXPECT_FALSE(attribute_exists("Connector", std::nullopt, 1, 1, "SupplyPhases", std::nullopt,
                                  MutabilityEnum::ReadOnly, AttributeEnum::Actual));
    EXPECT_TRUE(attribute_exists("Connector", std::nullopt, 1, 1, "SupplyPhases", std::nullopt,
                                 MutabilityEnum::ReadOnly, AttributeEnum::Target));

    // Changed datatype
    EXPECT_FALSE(characteristics_exists("Connector", std::nullopt, 1, 1, "ConnectorType", std::nullopt,
                                        DataEnum::string, std::nullopt, std::nullopt, true, std::nullopt,
                                        std::nullopt));
    EXPECT_TRUE(characteristics_exists("Connector", std::nullopt, 1, 1, "ConnectorType", std::nullopt,
                                       DataEnum::integer, std::nullopt, std::nullopt, true, std::nullopt,
                                       std::nullopt));

    // Changed unit
    EXPECT_FALSE(characteristics_exists("EVSE", std::nullopt, 1, std::nullopt, "Power", std::nullopt, DataEnum::decimal,
                                        22000.0, std::nullopt, true, "W", std::nullopt));
    EXPECT_TRUE(characteristics_exists("EVSE", std::nullopt, 1, std::nullopt, "Power", std::nullopt, DataEnum::decimal,
                                       22000.0, std::nullopt, true, "kW", std::nullopt));

    // No change
    EXPECT_TRUE(component_exists("UnitTestCtrlr", std::nullopt, 2, 3));

    // Component added
    EXPECT_TRUE(component_exists("EVSE", std::nullopt, 3, std::nullopt));
    EXPECT_TRUE(component_exists("Connector", std::nullopt, 2, 2));
    // Component removed, variables, characteristics and attributes should also have been removed.
    EXPECT_FALSE(component_exists("Connector", std::nullopt, 2, 1));
    EXPECT_FALSE(variable_exists("Connector", std::nullopt, 2, 1, "AvailabilityState", std::nullopt));
    EXPECT_FALSE(variable_exists("Connector", std::nullopt, 2, 1, "Available", std::nullopt));
    EXPECT_FALSE(variable_exists("Connector", std::nullopt, 2, 1, "ChargeProtocol", std::nullopt));
    EXPECT_FALSE(variable_exists("Connector", std::nullopt, 2, 1, "ConnectorType", std::nullopt));
    EXPECT_FALSE(variable_exists("Connector", std::nullopt, 2, 1, "SupplyPhases", std::nullopt));
    EXPECT_FALSE(attribute_exists("Connector", std::nullopt, 2, 1, "ChargeProtocol", std::nullopt, std::nullopt,
                                  AttributeEnum::Actual));
    EXPECT_FALSE(attribute_exists("Connector", std::nullopt, 2, 1, "ConnectorType", std::nullopt,
                                  MutabilityEnum::ReadOnly, AttributeEnum::Actual));
    EXPECT_FALSE(characteristics_exists("Connector", std::nullopt, 2, 1, "Available", std::nullopt, DataEnum::boolean,
                                        std::nullopt, std::nullopt, true, std::nullopt, std::nullopt));
    EXPECT_FALSE(characteristics_exists("Connector", std::nullopt, 2, 1, "AvailabilityState", std::nullopt,
                                        DataEnum::OptionList, std::nullopt, std::nullopt, true, std::nullopt,
                                        "Available,Occupied,Reserved,Unavailable,Faulted"));

    // Changed supports monitoring and datatype
    EXPECT_FALSE(characteristics_exists("EVSE", std::nullopt, 2, std::nullopt, "AvailabilityState", std::nullopt,
                                        DataEnum::OptionList, std::nullopt, std::nullopt, true, std::nullopt,
                                        "Available,Occupied,Reserved,Unavailable,Faulted"));
    EXPECT_TRUE(characteristics_exists("EVSE", std::nullopt, 2, std::nullopt, "AvailabilityState", std::nullopt,
                                       DataEnum::string, std::nullopt, std::nullopt, false, std::nullopt,
                                       "Available,Occupied,Reserved,Unavailable,Faulted"));

    // One attribute removed, the other not
    EXPECT_FALSE(attribute_exists("EVSE", std::nullopt, 2, std::nullopt, "Power", std::nullopt,
                                  MutabilityEnum::ReadOnly, AttributeEnum::Actual));
    EXPECT_TRUE(attribute_exists("EVSE", std::nullopt, 2, std::nullopt, "Power", std::nullopt, MutabilityEnum::ReadOnly,
                                 AttributeEnum::MaxSet));

    // Add attribute
    EXPECT_TRUE(attribute_exists("EVSE", std::nullopt, 1, std::nullopt, "Power", std::nullopt,
                                 MutabilityEnum::ReadWrite, AttributeEnum::Target));

    // Characteristics changed from not EVSE or Connector
    EXPECT_FALSE(characteristics_exists("UnitTestCtrlr", std::nullopt, 2, 3, "UnitTestPropertyAName", std::nullopt,
                                        DataEnum::boolean, std::nullopt, std::nullopt, true, std::nullopt,
                                        std::nullopt));
    EXPECT_TRUE(characteristics_exists("UnitTestCtrlr", std::nullopt, 2, 3, "UnitTestPropertyAName", std::nullopt,
                                       DataEnum::string, std::nullopt, std::nullopt, true, std::nullopt, std::nullopt));

    // Removed UnitTestPropertyCName
    EXPECT_TRUE(variable_exists("UnitTestCtrlr", std::nullopt, 2, 3, "UnitTestPropertyBName", std::nullopt));
    EXPECT_FALSE(variable_exists("UnitTestCtrlr", std::nullopt, 2, 3, "UnitTestPropertyCName", std::nullopt));

    // Source was not 'default', available not changed.
    EXPECT_TRUE(attribute_has_value("Connector", std::nullopt, 1, 1, "Available", std::nullopt, AttributeEnum::Actual,
                                    "false"));

    // Check changed values.
    EXPECT_TRUE(attribute_has_value("EVSE", std::nullopt, 2, std::nullopt, "Available", std::nullopt,
                                    AttributeEnum::Actual, "false"));
    EXPECT_TRUE(attribute_has_value("EVSE", std::nullopt, 1, std::nullopt, "SupplyPhases", std::nullopt,
                                    AttributeEnum::Actual, "2"));

    // Variable was removed, so it will be set to the default value again.
    EXPECT_TRUE(
        attribute_has_value("EVSE", std::nullopt, 1, std::nullopt, "Power", std::nullopt, AttributeEnum::Actual, "0"));
    // Default value only applies to 'Actual', not 'MaxSet'
    EXPECT_TRUE(attribute_has_value("EVSE", std::nullopt, 1, std::nullopt, "Power", std::nullopt, AttributeEnum::MaxSet,
                                    "44000"));
    // // Component does not exist so it could not set anything.
    EXPECT_FALSE(component_exists("UnitTestCtrlr", std::nullopt, 1, 5));
}

TEST_F(InitDeviceModelDbTest, wrong_migration_file_path) {
    InitDeviceModelDb db(DATABASE_PATH, "/tmp/thisdoesnotexisthopefully");
    // The migration script is not correct (there is none in the given folder), this should throw an exception.
    const auto component_configs = get_all_component_configs(CONFIGS_PATH);
    EXPECT_THROW(db.initialize_database(component_configs, true), MigrationException);
}

TEST_F(InitDeviceModelDbTest, wrong_config_path) {
    // Wrong component config path while initializing database
    InitDeviceModelDb db(DATABASE_PATH, MIGRATION_FILES_PATH);
    EXPECT_THROW(get_all_component_configs("/tmp/thisdoesnotexisthopefully"), std::filesystem::filesystem_error);
}

TEST_F(InitDeviceModelDbTest, default_device_model_config) {
    // Test if the default device model config is correct and will create a valid database with valid values.
    const static std::string MIGRATION_FILES_PATH_DEFAULT = "./resources/v2/device_model_migration_files";
    const static std::string CONFIG_PATH_DEFAULT = "./resources/example_config/v2/component_config";
    InitDeviceModelDb db(DATABASE_PATH, MIGRATION_FILES_PATH_DEFAULT);
    const auto component_configs = get_all_component_configs(CONFIG_PATH_DEFAULT);
    EXPECT_NO_THROW(db.initialize_database(component_configs, true));
}

TEST_F(InitDeviceModelDbTest, wrong_type) {
    // Test if initializing fails when there is a missing required value.
    InitDeviceModelDb db(DATABASE_PATH, MIGRATION_FILES_PATH);
    try {
        const auto component_configs = get_all_component_configs(CONFIGS_PATH_WRONG_VALUE_TYPE);
        db.initialize_database(component_configs, true);
        FAIL() << "Expected InitDeviceModelDbError, but no exception was thrown";
    } catch (const InitDeviceModelDbError& exception) {
        EXPECT_EQ(std::string(exception.what()),
                  "Check integrity failed:\n"
                  "- Component UnitTestCtrlr, evse 2, connector 3\n"
                  "  - Variable UnitTestPropertyAName, errors:\n"
                  "    - Attribute 'Actual' value (42) has wrong type, type should have been: boolean.\n"
                  "  - Variable UnitTestPropertyBName, errors:\n"
                  "    - Default value (test) has wrong type, type should have been decimal.\n"
                  "    - Attribute 'Actual' value (test_value) has wrong type, type should have been: decimal.\n"
                  "  - Variable UnitTestPropertyCName, errors:\n"
                  "    - Default value (true) has wrong type, type should have been integer.\n");
    } catch (const std::exception& exception) {
        FAIL() << "Expected InitDeviceModelDbError, but exception of type " << typeid(exception).name()
               << " was thrown";
    }
}

// Helper functions

bool InitDeviceModelDbTest::check_all_tables_exist(const std::vector<std::string>& tables, const bool exist) {
    for (const std::string& table : tables) {
        if (DoesTableExist(table) != exist) {
            EVLOG_info << "Table " << table << " does" << (exist ? " not" : "") << " exist";
            return false;
        }
    }

    return true;
}

bool InitDeviceModelDbTest::component_exists(const std::string& component_name,
                                             const std::optional<std::string>& component_instance,
                                             const std::optional<int>& component_evse_id,
                                             const std::optional<int>& component_connector_id) {
    static const std::string select_component_statement = "SELECT ID "
                                                          "FROM COMPONENT "
                                                          "WHERE NAME=@component_name "
                                                          "AND INSTANCE IS @component_instance "
                                                          "AND EVSE_ID IS @evse_id "
                                                          "AND CONNECTOR_ID IS @connector_id";
    std::unique_ptr<StatementInterface> statement = this->database->new_statement(select_component_statement);

    statement->bind_text("@component_name", component_name, SQLiteString::Transient);
    if (component_instance.has_value()) {
        statement->bind_text("@component_instance", component_instance.value(), SQLiteString::Transient);
    } else {
        statement->bind_null("@component_instance");
    }

    if (component_evse_id.has_value()) {
        statement->bind_int("@evse_id", component_evse_id.value());
    } else {
        statement->bind_null("@evse_id");
    }

    if (component_connector_id.has_value()) {
        statement->bind_int("@connector_id", component_connector_id.value());
    } else {
        statement->bind_null("@connector_id");
    }

    if (statement->step() == SQLITE_ERROR) {
        return false;
    }

    return statement->get_number_of_rows() == 1;
}

bool InitDeviceModelDbTest::attribute_exists(
    const std::string& component_name, const std::optional<std::string>& component_instance,
    const std::optional<int>& component_evse_id, const std::optional<int>& component_connector_id,
    const std::string& variable_name, const std::optional<std::string>& variable_instance,
    const std::optional<MutabilityEnum>& mutability, const AttributeEnum& type) {
    static const std::string select_attribute_statement = "SELECT ID "
                                                          "FROM VARIABLE_ATTRIBUTE va "
                                                          "WHERE va.VARIABLE_ID=("
                                                          "SELECT v.ID "
                                                          "FROM VARIABLE v "
                                                          "JOIN COMPONENT c ON c.ID=v.COMPONENT_ID "
                                                          "WHERE c.NAME=@component_name "
                                                          "AND c.INSTANCE IS @component_instance "
                                                          "AND c.EVSE_ID IS @evse_id "
                                                          "AND c.CONNECTOR_ID IS @connector_id "
                                                          "AND v.NAME=@variable_name "
                                                          "AND v.INSTANCE IS @variable_instance) "
                                                          "AND va.TYPE_ID=@type_id "
                                                          "AND va.MUTABILITY_ID IS @mutability_id";
    std::unique_ptr<StatementInterface> statement = this->database->new_statement(select_attribute_statement);

    statement->bind_text("@component_name", component_name, SQLiteString::Transient);
    if (component_instance.has_value()) {
        statement->bind_text("@component_instance", component_instance.value(), SQLiteString::Transient);
    } else {
        statement->bind_null("@component_instance");
    }

    if (component_evse_id.has_value()) {
        statement->bind_int("@evse_id", component_evse_id.value());
    } else {
        statement->bind_null("@evse_id");
    }

    if (component_connector_id.has_value()) {
        statement->bind_int("@connector_id", component_connector_id.value());
    } else {
        statement->bind_null("@connector_id");
    }

    statement->bind_text("@variable_name", variable_name, SQLiteString::Transient);

    if (variable_instance.has_value()) {
        statement->bind_text("@variable_instance", variable_instance.value(), SQLiteString::Transient);
    } else {
        statement->bind_null("@variable_instance");
    }

    statement->bind_int("@type_id", static_cast<int>(type));

    if (mutability.has_value()) {
        statement->bind_int("@mutability_id", static_cast<int>(mutability.value()));
    } else {
        statement->bind_null("@mutability_id");
    }

    if (statement->step() == SQLITE_ERROR) {
        return false;
    }

    return statement->get_number_of_rows() == 1;
}

bool InitDeviceModelDbTest::variable_exists(const std::string& component_name,
                                            const std::optional<std::string>& component_instance,
                                            const std::optional<int>& component_evse_id,
                                            const std::optional<int>& component_connector_id,
                                            const std::string& variable_name,
                                            const std::optional<std::string>& variable_instance,
                                            const std::optional<std::string> source) {
    static const std::string select_variable_statement = "SELECT ID "
                                                         "FROM VARIABLE v "
                                                         "WHERE v.COMPONENT_ID=("
                                                         "SELECT c.ID "
                                                         "FROM COMPONENT c "
                                                         "WHERE c.NAME=@component_name "
                                                         "AND c.INSTANCE IS @component_instance "
                                                         "AND c.EVSE_ID IS @evse_id "
                                                         "AND c.CONNECTOR_ID IS @connector_id) "
                                                         "AND v.NAME=@variable_name "
                                                         "AND v.INSTANCE IS @variable_instance "
                                                         "AND v.source IS @variable_source";

    std::unique_ptr<StatementInterface> statement = this->database->new_statement(select_variable_statement);

    statement->bind_text("@component_name", component_name, SQLiteString::Transient);
    if (component_instance.has_value()) {
        statement->bind_text("@component_instance", component_instance.value(), SQLiteString::Transient);
    } else {
        statement->bind_null("@component_instance");
    }

    if (component_evse_id.has_value()) {
        statement->bind_int("@evse_id", component_evse_id.value());
    } else {
        statement->bind_null("@evse_id");
    }

    if (component_connector_id.has_value()) {
        statement->bind_int("@connector_id", component_connector_id.value());
    } else {
        statement->bind_null("@connector_id");
    }

    statement->bind_text("@variable_name", variable_name, SQLiteString::Transient);

    if (variable_instance.has_value()) {
        statement->bind_text("@variable_instance", variable_instance.value(), SQLiteString::Transient);
    } else {
        statement->bind_null("@variable_instance");
    }

    if (source.has_value()) {
        statement->bind_text("@variable_source", source.value(), SQLiteString::Transient);
    } else {
        statement->bind_null("@variable_source");
    }

    if (statement->step() == SQLITE_ERROR) {
        return false;
    }

    return statement->get_number_of_rows() == 1;
}

bool InitDeviceModelDbTest::characteristics_exists(
    const std::string& component_name, const std::optional<std::string>& component_instance,
    const std::optional<int>& component_evse_id, const std::optional<int>& component_connector_id,
    const std::string& variable_name, const std::optional<std::string>& variable_instance, const DataEnum data_type,
    const std::optional<double>& max_limit, const std::optional<double>& min_limit, const bool supports_monitoring,
    const std::optional<std::string>& unit, const std::optional<std::string>& values_list) {
    static const std::string select_characteristics_statement = "SELECT ID "
                                                                "FROM VARIABLE_CHARACTERISTICS vc "
                                                                "WHERE vc.VARIABLE_ID=("
                                                                "SELECT v.ID "
                                                                "FROM VARIABLE v "
                                                                "JOIN COMPONENT c ON c.ID = v.COMPONENT_ID "
                                                                "WHERE c.NAME=@component_name "
                                                                "AND c.INSTANCE IS @component_instance "
                                                                "AND c.EVSE_ID IS @evse_id "
                                                                "AND c.CONNECTOR_ID IS @connector_id "
                                                                "AND v.NAME=@variable_name "
                                                                "AND v.INSTANCE IS @variable_instance) "
                                                                "AND vc.DATATYPE_ID=@datatype_id "
                                                                "AND vc.MAX_LIMIT IS @max_limit "
                                                                "AND vc.MIN_LIMIT IS @min_limit "
                                                                "AND vc.SUPPORTS_MONITORING IS @supports_monitoring "
                                                                "AND vc.UNIT IS @unit "
                                                                "AND vc.VALUES_LIST IS @values_list";
    std::unique_ptr<StatementInterface> statement = this->database->new_statement(select_characteristics_statement);

    statement->bind_text("@component_name", component_name, SQLiteString::Transient);
    if (component_instance.has_value()) {
        statement->bind_text("@component_instance", component_instance.value(), SQLiteString::Transient);
    } else {
        statement->bind_null("@component_instance");
    }

    if (component_evse_id.has_value()) {
        statement->bind_int("@evse_id", component_evse_id.value());
    } else {
        statement->bind_null("@evse_id");
    }

    if (component_connector_id.has_value()) {
        statement->bind_int("@connector_id", component_connector_id.value());
    } else {
        statement->bind_null("@connector_id");
    }

    statement->bind_text("@variable_name", variable_name, SQLiteString::Transient);

    if (variable_instance.has_value()) {
        statement->bind_text("@variable_instance", variable_instance.value(), SQLiteString::Transient);
    } else {
        statement->bind_null("@variable_instance");
    }

    statement->bind_int("@datatype_id", static_cast<int>(data_type));

    if (max_limit.has_value()) {
        statement->bind_double("@max_limit", static_cast<double>(max_limit.value()));
    } else {
        statement->bind_null("@max_limit");
    }

    if (min_limit.has_value()) {
        statement->bind_double("@min_limit", static_cast<double>(min_limit.value()));
    } else {
        statement->bind_null("@min_limit");
    }

    statement->bind_int("@supports_monitoring", (supports_monitoring ? 1 : 0));

    if (unit.has_value()) {
        statement->bind_text("@unit", unit.value(), SQLiteString::Transient);
    } else {
        statement->bind_null("@unit");
    }

    if (values_list.has_value()) {
        statement->bind_text("@values_list", values_list.value(), SQLiteString::Transient);
    } else {
        statement->bind_null("@values_list");
    }

    if (statement->step() == SQLITE_ERROR) {
        return false;
    }

    return statement->get_number_of_rows() == 1;
}

bool InitDeviceModelDbTest::attribute_has_value(
    const std::string& component_name, const std::optional<std::string>& component_instance,
    const std::optional<int>& component_evse_id, const std::optional<int>& component_connector_id,
    const std::string& variable_name, const std::optional<std::string>& variable_instance, const AttributeEnum& type,
    const std::string& value, const ValueCompareMode compare_mode) {
    const std::string select_attribute_statement = "SELECT VALUE "
                                                   "FROM VARIABLE_ATTRIBUTE va "
                                                   "WHERE va.VARIABLE_ID=("
                                                   "SELECT v.ID "
                                                   "FROM VARIABLE v "
                                                   "JOIN COMPONENT c ON c.ID=v.COMPONENT_ID "
                                                   "WHERE c.NAME=@component_name "
                                                   "AND c.INSTANCE IS @component_instance "
                                                   "AND c.EVSE_ID IS @evse_id "
                                                   "AND c.CONNECTOR_ID IS @connector_id "
                                                   "AND v.NAME=@variable_name "
                                                   "AND v.INSTANCE IS @variable_instance) "
                                                   "AND va.TYPE_ID=@type_id";
    std::unique_ptr<StatementInterface> statement = this->database->new_statement(select_attribute_statement);

    statement->bind_text("@component_name", component_name, SQLiteString::Transient);
    if (component_instance.has_value()) {
        statement->bind_text("@component_instance", component_instance.value(), SQLiteString::Transient);
    } else {
        statement->bind_null("@component_instance");
    }

    if (component_evse_id.has_value()) {
        statement->bind_int("@evse_id", component_evse_id.value());
    } else {
        statement->bind_null("@evse_id");
    }

    if (component_connector_id.has_value()) {
        statement->bind_int("@connector_id", component_connector_id.value());
    } else {
        statement->bind_null("@connector_id");
    }

    statement->bind_text("@variable_name", variable_name, SQLiteString::Transient);

    if (variable_instance.has_value()) {
        statement->bind_text("@variable_instance", variable_instance.value(), SQLiteString::Transient);
    } else {
        statement->bind_null("@variable_instance");
    }

    statement->bind_int("@type_id", static_cast<int>(type));

    int step = statement->step();

    if (step == SQLITE_ERROR) {
        return false;
    }

    if (statement->column_type(0) == SQLITE_NULL) {
        return false;
    }

    const std::string actual_value = statement->column_text(0);

    if (value == actual_value) {
        return true;
    }

    if (compare_mode == ValueCompareMode::Exact) {
        EVLOG_info << "Actual value is: " << actual_value << ", expected value is: " << value;
        return false;
    }

    if (compare_mode == ValueCompareMode::JsonSemantic) {
        try {
            const auto expected_json = nlohmann::json::parse(value);
            const auto actual_json = nlohmann::json::parse(actual_value);

            const auto result = expected_json == actual_json;
            if (!result) {
                EVLOG_info << "Actual JSON value is: " << actual_json.dump()
                           << ", expected JSON value is: " << expected_json.dump();
            }
            return result;
        } catch (const nlohmann::json::parse_error&) {
            return false;
        }
    }
    return false;
}

void InitDeviceModelDbTest::set_attribute_source(const std::string& component_name,
                                                 const std::optional<std::string>& component_instance,
                                                 const std::optional<int>& component_evse_id,
                                                 const std::optional<int>& component_connector_id,
                                                 const std::string& variable_name,
                                                 const std::optional<std::string>& variable_instance,
                                                 const AttributeEnum& type, const std::string& source) {
    static const std::string statement = "UPDATE VARIABLE_ATTRIBUTE "
                                         "SET VALUE_SOURCE=@source "
                                         "WHERE VARIABLE_ID = ("
                                         "SELECT VARIABLE.ID "
                                         "FROM VARIABLE "
                                         "JOIN COMPONENT ON COMPONENT.ID = VARIABLE.COMPONENT_ID "
                                         "WHERE COMPONENT.NAME = @component_name "
                                         "AND COMPONENT.INSTANCE IS @component_instance "
                                         "AND COMPONENT.EVSE_ID IS @evse_id "
                                         "AND COMPONENT.CONNECTOR_ID IS @connector_id "
                                         "AND VARIABLE.NAME = @variable_name "
                                         "AND VARIABLE.INSTANCE IS @variable_instance) "
                                         "AND TYPE_ID = @type_id";

    std::unique_ptr<StatementInterface> insert_variable_attribute_statement = this->database->new_statement(statement);

    insert_variable_attribute_statement->bind_text("@source", source, SQLiteString::Transient);
    insert_variable_attribute_statement->bind_text("@component_name", component_name, SQLiteString::Transient);
    if (component_instance.has_value()) {
        insert_variable_attribute_statement->bind_text("@component_instance", component_instance.value(),
                                                       SQLiteString::Transient);
    } else {
        insert_variable_attribute_statement->bind_null("@component_instance");
    }

    if (component_evse_id.has_value()) {
        insert_variable_attribute_statement->bind_int("@evse_id", component_evse_id.value());
    } else {
        insert_variable_attribute_statement->bind_null("@evse_id");
    }

    if (component_connector_id.has_value()) {
        insert_variable_attribute_statement->bind_int("@connector_id", component_connector_id.value());
    } else {
        insert_variable_attribute_statement->bind_null("@connector_id");
    }

    insert_variable_attribute_statement->bind_text("@variable_name", variable_name, SQLiteString::Transient);

    if (variable_instance.has_value()) {
        insert_variable_attribute_statement->bind_text("@variable_instance", variable_instance.value(),
                                                       SQLiteString::Transient);
    } else {
        insert_variable_attribute_statement->bind_null("@variable_instance");
    }

    insert_variable_attribute_statement->bind_int("@type_id", static_cast<int>(type));

    if (insert_variable_attribute_statement->step() != SQLITE_DONE) {
        EVLOG_error << "Could not set source of variable " << variable_name << " (component: " << component_name
                    << ") attribute " << static_cast<int>(type) << ": " << this->database->get_error_message();
    }
}

// Verifies that a full OCPP 1.6 config is patched into mapped variable attributes and maxLimit characteristics.
TEST_F(InitDeviceModelDbTest, patch_with_ocpp16_full_config_maps_values) {
    const auto ocpp16_config = std::make_unique<ocpp::v16::ChargePointConfiguration>(
        ocpp::tests::make_ocpp16_test_config_full(), CONFIG_DIR_V16, USER_CONFIG_FILE_LOCATION_V16);

    InitDeviceModelDb db(DATABASE_PATH, MIGRATION_FILES_PATH);
    db.database_exists = false;

    // Get initial component configs
    auto component_configs = get_all_component_configs(STANDARD_CONFIGS_PATH);
    patch_component_config_with_ocpp16(component_configs, *ocpp16_config);

    db.initialize_database(component_configs, true);

    // ============================================================================
    // Core Profile
    // ============================================================================
    EXPECT_TRUE(attribute_has_value("AuthCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "OfflineTxForUnknownIdEnabled", std::nullopt, AttributeEnum::Actual, "false"));
    EXPECT_TRUE(attribute_has_value("AuthCacheCtrlr", std::nullopt, std::nullopt, std::nullopt, "Enabled", std::nullopt,
                                    AttributeEnum::Actual, "false"));
    EXPECT_TRUE(attribute_has_value("AuthCtrlr", std::nullopt, std::nullopt, std::nullopt, "AuthorizeRemoteStart",
                                    std::nullopt, AttributeEnum::Actual, "false"));
    EXPECT_TRUE(attribute_has_value("AlignedDataCtrlr", std::nullopt, std::nullopt, std::nullopt, "Interval",
                                    std::nullopt, AttributeEnum::Actual, "700"));
    EXPECT_TRUE(attribute_has_value("TxCtrlr", std::nullopt, std::nullopt, std::nullopt, "EVConnectionTimeOut",
                                    std::nullopt, AttributeEnum::Actual, "123"));
    EXPECT_TRUE(attribute_has_value("OCPPCommCtrlr", std::nullopt, std::nullopt, std::nullopt, "HeartbeatInterval",
                                    std::nullopt, AttributeEnum::Actual, "905"));
    EXPECT_TRUE(attribute_has_value("AuthCtrlr", std::nullopt, std::nullopt, std::nullopt, "LocalAuthorizeOffline",
                                    std::nullopt, AttributeEnum::Actual, "false"));
    EXPECT_TRUE(attribute_has_value("AuthCtrlr", std::nullopt, std::nullopt, std::nullopt, "LocalPreAuthorize",
                                    std::nullopt, AttributeEnum::Actual, "false"));
    EXPECT_TRUE(attribute_has_value("TxCtrlr", std::nullopt, std::nullopt, std::nullopt, "MaxEnergyOnInvalidId",
                                    std::nullopt, AttributeEnum::Actual, "503"));
    EXPECT_TRUE(attribute_has_value("AlignedDataCtrlr", std::nullopt, std::nullopt, std::nullopt, "Measurands",
                                    std::nullopt, AttributeEnum::Actual, "Energy.Active.Import.Register,Frequency"));
    EXPECT_TRUE(attribute_has_value("SampledDataCtrlr", std::nullopt, std::nullopt, std::nullopt, "TxUpdatedMeasurands",
                                    std::nullopt, AttributeEnum::Actual,
                                    "Energy.Active.Import.Register,Power.Active.Import"));
    EXPECT_TRUE(attribute_has_value("SampledDataCtrlr", std::nullopt, std::nullopt, std::nullopt, "TxUpdatedInterval",
                                    std::nullopt, AttributeEnum::Actual, "301"));
    EXPECT_TRUE(attribute_has_value("OCPPCommCtrlr", std::nullopt, std::nullopt, std::nullopt, "ResetRetries",
                                    std::nullopt, AttributeEnum::Actual, "2"));
    EXPECT_TRUE(attribute_has_value("TxCtrlr", std::nullopt, std::nullopt, std::nullopt, "StopTxOnInvalidId",
                                    std::nullopt, AttributeEnum::Actual, "false"));
    EXPECT_TRUE(attribute_has_value("AlignedDataCtrlr", std::nullopt, std::nullopt, std::nullopt, "TxEndedMeasurands",
                                    std::nullopt, AttributeEnum::Actual, "Energy.Active.Import.Register"));
    EXPECT_TRUE(attribute_has_value("SampledDataCtrlr", std::nullopt, std::nullopt, std::nullopt, "TxEndedMeasurands",
                                    std::nullopt, AttributeEnum::Actual, "Energy.Active.Import.Register"));
    EXPECT_TRUE(attribute_has_value("OCPPCommCtrlr", std::nullopt, std::nullopt, std::nullopt, "MessageAttempts",
                                    "TransactionEvent", AttributeEnum::Actual, "2"));
    EXPECT_TRUE(attribute_has_value("OCPPCommCtrlr", std::nullopt, std::nullopt, std::nullopt, "MessageAttemptInterval",
                                    "TransactionEvent", AttributeEnum::Actual, "11"));
    EXPECT_TRUE(attribute_has_value("OCPPCommCtrlr", std::nullopt, std::nullopt, std::nullopt, "WebSocketPingInterval",
                                    std::nullopt, AttributeEnum::Actual, "11"));

    // ============================================================================
    // Local Auth List Management
    // ============================================================================
    EXPECT_TRUE(attribute_has_value("LocalAuthListCtrlr", std::nullopt, std::nullopt, std::nullopt, "Enabled",
                                    std::nullopt, AttributeEnum::Actual, "false"));
    EXPECT_TRUE(attribute_has_value("LocalAuthListCtrlr", std::nullopt, std::nullopt, std::nullopt, "ItemsPerMessage",
                                    std::nullopt, AttributeEnum::Actual, "512"));

    // ============================================================================
    // Smart Charging
    // ============================================================================
    EXPECT_TRUE(attribute_has_value("SmartChargingCtrlr", std::nullopt, std::nullopt, std::nullopt, "ProfileStackLevel",
                                    std::nullopt, AttributeEnum::Actual, "1001"));
    EXPECT_TRUE(attribute_has_value("SmartChargingCtrlr", std::nullopt, std::nullopt, std::nullopt, "RateUnit",
                                    std::nullopt, AttributeEnum::Actual, "Current,Power"));
    EXPECT_TRUE(attribute_has_value("SmartChargingCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "PeriodsPerSchedule", std::nullopt, AttributeEnum::Actual, "1001"));
    EXPECT_TRUE(attribute_has_value("SmartChargingCtrlr", std::nullopt, std::nullopt, std::nullopt, "Phases3to1",
                                    std::nullopt, AttributeEnum::Actual, "false"));

    // ============================================================================
    // Firmware Management
    // ============================================================================
    EXPECT_TRUE(attribute_has_value("OCPPCommCtrlr", std::nullopt, std::nullopt, std::nullopt, "FileTransferProtocols",
                                    std::nullopt, AttributeEnum::Actual, "FTP,FTPS"));

    // ============================================================================
    // Internal - Device Information
    // ============================================================================
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt, "ChargePointId",
                                    std::nullopt, AttributeEnum::Actual, "cp002"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt, "ChargeBoxSerialNumber",
                                    std::nullopt, AttributeEnum::Actual, "cp002"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt, "ChargePointModel",
                                    std::nullopt, AttributeEnum::Actual, "Model"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "ChargePointSerialNumber", std::nullopt, AttributeEnum::Actual, "serial"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt, "ChargePointVendor",
                                    std::nullopt, AttributeEnum::Actual, "EVerest"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt, "FirmwareVersion",
                                    std::nullopt, AttributeEnum::Actual, "0.2"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt, "ICCID", std::nullopt,
                                    AttributeEnum::Actual, "8914800000000000000"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt, "IMSI", std::nullopt,
                                    AttributeEnum::Actual, "262 012345678901"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt, "MeterSerialNumber",
                                    std::nullopt, AttributeEnum::Actual, "meter-serial"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt, "MeterType",
                                    std::nullopt, AttributeEnum::Actual, "DC"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt, "NumberOfConnectors",
                                    std::nullopt, AttributeEnum::Actual, "3"));

    // ============================================================================
    // Internal - Security & Communication
    // ============================================================================

    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt, "SupportedCiphers12",
                                    std::nullopt, AttributeEnum::Actual,
                                    "ECDHE-ECDSA-AES128-GCM-SHA256:AES256-GCM-SHA384"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt, "SupportedCiphers13",
                                    std::nullopt, AttributeEnum::Actual,
                                    "TLS_AES_256_GCM_SHA384:TLS_AES_128_GCM_SHA256:TLS_CHACHA20_POLY1305_SHA256"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt, "UseTPM", std::nullopt,
                                    AttributeEnum::Actual, "true"));
    EXPECT_TRUE(attribute_has_value("OCPPCommCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "RetryBackOffRandomRange", std::nullopt, AttributeEnum::Actual, "12"));
    EXPECT_TRUE(attribute_has_value("OCPPCommCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "RetryBackOffRepeatTimes", std::nullopt, AttributeEnum::Actual, "4"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "AuthorizeConnectorZeroOnConnectorOne", std::nullopt, AttributeEnum::Actual,
                                    "false"));

    // ============================================================================
    // Internal - Logging
    // ============================================================================
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt, "LogMessages",
                                    std::nullopt, AttributeEnum::Actual, "false"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt, "LogMessagesFormat",
                                    std::nullopt, AttributeEnum::Actual, "log,html,session_logging,security"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt, "LogRotation",
                                    std::nullopt, AttributeEnum::Actual, "true"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt, "LogRotationDateSuffix",
                                    std::nullopt, AttributeEnum::Actual, "true"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "LogRotationMaximumFileSize", std::nullopt, AttributeEnum::Actual, "1"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "LogRotationMaximumFileCount", std::nullopt, AttributeEnum::Actual, "2"));

    // ============================================================================
    // Internal - Charging Profile Configuration
    // ============================================================================
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "SupportedChargingProfilePurposeTypes", std::nullopt, AttributeEnum::Actual,
                                    "ChargePointMaxProfile,TxProfile"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "MaxCompositeScheduleDuration", std::nullopt, AttributeEnum::Actual, "31536000"));

    // ============================================================================
    // Internal - WebSocket Configuration
    // ============================================================================
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt, "WebsocketPingPayload",
                                    std::nullopt, AttributeEnum::Actual, "Hello from EVerest!"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt, "WebsocketPongTimeout",
                                    std::nullopt, AttributeEnum::Actual, "6"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "UseSslDefaultVerifyPaths", std::nullopt, AttributeEnum::Actual, "false"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt, "VerifyCsmsCommonName",
                                    std::nullopt, AttributeEnum::Actual, "false"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "VerifyCsmsAllowWildcards", std::nullopt, AttributeEnum::Actual, "false"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt, "OcspRequestInterval",
                                    std::nullopt, AttributeEnum::Actual, "604800"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt, "IFace", std::nullopt,
                                    AttributeEnum::Actual, "eth0"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "UseTPMSeccLeafCertificate", std::nullopt, AttributeEnum::Actual, "true"));
    EXPECT_TRUE(attribute_has_value("OCPPCommCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "RetryBackOffWaitMinimum", std::nullopt, AttributeEnum::Actual, "4"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt, "LogMessagesRaw",
                                    std::nullopt, AttributeEnum::Actual, "false"));
    EXPECT_TRUE(attribute_has_value("SmartChargingCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "IgnoredProfilePurposesOffline", std::nullopt, AttributeEnum::Actual,
                                    "ChargePointMaxProfile,TxProfile"));
    EXPECT_TRUE(attribute_has_value("SmartChargingCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "CompositeScheduleDefaultLimitAmps", std::nullopt, AttributeEnum::Actual, "92"));
    EXPECT_TRUE(attribute_has_value("SmartChargingCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "CompositeScheduleDefaultLimitWatts", std::nullopt, AttributeEnum::Actual, "42"));
    EXPECT_TRUE(attribute_has_value("SmartChargingCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "CompositeScheduleDefaultNumberPhases", std::nullopt, AttributeEnum::Actual, "2"));
    EXPECT_TRUE(attribute_has_value("SmartChargingCtrlr", std::nullopt, std::nullopt, std::nullopt, "SupplyVoltage",
                                    std::nullopt, AttributeEnum::Actual, "220"));

    // ============================================================================
    // Internal - ISO 15118 Configuration
    // ============================================================================
    EXPECT_TRUE(attribute_has_value("ISO15118Ctrlr", std::nullopt, std::nullopt, std::nullopt, "SeccId", std::nullopt,
                                    AttributeEnum::Actual, "DEPNX100002"));
    EXPECT_TRUE(attribute_has_value("ISO15118Ctrlr", std::nullopt, std::nullopt, std::nullopt, "CountryName",
                                    std::nullopt, AttributeEnum::Actual, "EN"));
    EXPECT_TRUE(attribute_has_value("ISO15118Ctrlr", std::nullopt, std::nullopt, std::nullopt, "OrganizationName",
                                    std::nullopt, AttributeEnum::Actual, "Pionix"));

    // ============================================================================
    // Internal - Message Queue Configuration
    // ============================================================================
    EXPECT_TRUE(attribute_has_value("OCPPCommCtrlr", std::nullopt, std::nullopt, std::nullopt, "QueueAllMessages",
                                    std::nullopt, AttributeEnum::Actual, "false"));
    EXPECT_TRUE(attribute_has_value("OCPPCommCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "MessageTypesDiscardForQueueing", std::nullopt, AttributeEnum::Actual,
                                    "Heartbeat,StatusNotification"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "MessageQueueSizeThreshold", std::nullopt, AttributeEnum::Actual, "6000"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt, "MaxMessageSize",
                                    std::nullopt, AttributeEnum::Actual, "65000"));

    // ============================================================================
    // Internal - TLS Keylog (Debug)
    // ============================================================================
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt, "TLSKeylogFile",
                                    std::nullopt, AttributeEnum::Actual, "/tmp/ocpp_tls_keylog.txt"));
    EXPECT_TRUE(attribute_has_value("InternalCtrlr", std::nullopt, std::nullopt, std::nullopt, "EnableTLSKeylog",
                                    std::nullopt, AttributeEnum::Actual, "true"));

    // ============================================================================
    // Security Profile
    // ============================================================================
    EXPECT_TRUE(attribute_has_value("SecurityCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "AdditionalRootCertificateCheck", std::nullopt, AttributeEnum::Actual, "true"));
    // BasicAuthPassword is not changed by migration: AuthorizationKey now maps to OCPP16LegacyCtrlr
    EXPECT_TRUE(attribute_has_value("SecurityCtrlr", std::nullopt, std::nullopt, std::nullopt, "BasicAuthPassword",
                                    std::nullopt, AttributeEnum::Actual, "DEADBEEFDEADBEEF"));
    EXPECT_TRUE(attribute_has_value("SecurityCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "MaxCertificateChainSize", std::nullopt, AttributeEnum::Actual, "6000"));
    EXPECT_TRUE(attribute_has_value("SecurityCtrlr", std::nullopt, std::nullopt, std::nullopt, "OrganizationName",
                                    std::nullopt, AttributeEnum::Actual, "EVerest"));
    // SecurityProfile now maps to OCPP16LegacyCtrlr, so this retains the default value from the component config
    EXPECT_TRUE(attribute_has_value("SecurityCtrlr", std::nullopt, std::nullopt, std::nullopt, "SecurityProfile",
                                    std::nullopt, AttributeEnum::Actual, "1"));
    EXPECT_TRUE(attribute_has_value("SecurityCtrlr", std::nullopt, std::nullopt, std::nullopt, "CertSigningWaitMinimum",
                                    std::nullopt, AttributeEnum::Actual, "15"));
    EXPECT_TRUE(attribute_has_value("SecurityCtrlr", std::nullopt, std::nullopt, std::nullopt, "CertSigningRepeatTimes",
                                    std::nullopt, AttributeEnum::Actual, "3"));

    // ============================================================================
    // PnC (Plug and Charge)
    // ============================================================================
    EXPECT_TRUE(attribute_has_value("ISO15118Ctrlr", std::nullopt, std::nullopt, std::nullopt, "PnCEnabled",
                                    std::nullopt, AttributeEnum::Actual, "false"));
    EXPECT_TRUE(attribute_has_value("ISO15118Ctrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "CentralContractValidationAllowed", std::nullopt, AttributeEnum::Actual, "false"));
    EXPECT_TRUE(attribute_has_value("ISO15118Ctrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "ContractValidationOffline", std::nullopt, AttributeEnum::Actual, "false"));

    // ============================================================================
    // Cost and Price
    // ============================================================================
    EXPECT_TRUE(attribute_has_value("TariffCostCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "NumberOfDecimalsForCostValues", std::nullopt, AttributeEnum::Actual, "5"));
    EXPECT_TRUE(attribute_has_value("ClockCtrlr", std::nullopt, std::nullopt, std::nullopt, "TimeOffset", std::nullopt,
                                    AttributeEnum::Actual, "02:00"));
    EXPECT_TRUE(attribute_has_value("ClockCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "NextTimeOffsetTransitionDateTime", std::nullopt, AttributeEnum::Actual,
                                    "2024-01-01T00:00:00"));
    EXPECT_TRUE(attribute_has_value("ClockCtrlr", std::nullopt, std::nullopt, std::nullopt, "TimeOffset",
                                    "NextTransition", AttributeEnum::Actual, "01:00"));

    // ============================================================================
    // Keys that do not exist in OCPP2.x
    // ============================================================================

    EXPECT_TRUE(attribute_has_value("OCPP16LegacyCtrlr", std::nullopt, std::nullopt, std::nullopt, "BlinkRepeat",
                                    std::nullopt, AttributeEnum::Actual, "10"));
    EXPECT_TRUE(attribute_has_value("OCPP16LegacyCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "ConnectorPhaseRotationMaxLength", std::nullopt, AttributeEnum::Actual, "101"));
    EXPECT_TRUE(attribute_has_value("OCPP16LegacyCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "GetConfigurationMaxKeys", std::nullopt, AttributeEnum::Actual, "512"));
    EXPECT_TRUE(attribute_has_value("OCPP16LegacyCtrlr", std::nullopt, std::nullopt, std::nullopt, "LightIntensity",
                                    std::nullopt, AttributeEnum::Actual, "12"));
    EXPECT_TRUE(attribute_has_value("OCPP16LegacyCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "MinimumStatusDuration", std::nullopt, AttributeEnum::Actual, "2"));
    EXPECT_TRUE(attribute_has_value("OCPP16LegacyCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "StopTransactionOnEVSideDisconnect", std::nullopt, AttributeEnum::Actual, "false"));
    EXPECT_TRUE(
        attribute_has_value("OCPP16LegacyCtrlr", std::nullopt, std::nullopt, std::nullopt, "SupportedFeatureProfiles",
                            std::nullopt, AttributeEnum::Actual,
                            "Core,FirmwareManagement,RemoteTrigger,Reservation,LocalAuthListManagement,SmartCharging"));
    EXPECT_TRUE(attribute_has_value("OCPP16LegacyCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "SupportedFeatureProfilesMaxLength", std::nullopt, AttributeEnum::Actual, "512"));
    EXPECT_TRUE(attribute_has_value("OCPP16LegacyCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "UnlockConnectorOnEVSideDisconnect", std::nullopt, AttributeEnum::Actual, "false"));
    EXPECT_TRUE(attribute_has_value("OCPP16LegacyCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "ReserveConnectorZeroSupported", std::nullopt, AttributeEnum::Actual, "true"));
    EXPECT_TRUE(attribute_has_value("OCPP16LegacyCtrlr", std::nullopt, std::nullopt, std::nullopt, "HostName",
                                    std::nullopt, AttributeEnum::Actual, "cp002.local"));
    EXPECT_TRUE(attribute_has_value("OCPP16LegacyCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "AllowChargingProfileWithoutStartSchedule", std::nullopt, AttributeEnum::Actual,
                                    "true"));
    EXPECT_TRUE(attribute_has_value("OCPP16LegacyCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "WaitForStopTransactionsOnResetTimeout", std::nullopt, AttributeEnum::Actual,
                                    "30"));
    EXPECT_TRUE(attribute_has_value("OCPP16LegacyCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "StopTransactionIfUnlockNotSupported", std::nullopt, AttributeEnum::Actual,
                                    "false"));
    EXPECT_TRUE(attribute_has_value("OCPP16LegacyCtrlr", std::nullopt, std::nullopt, std::nullopt, "MeterPublicKeys",
                                    std::nullopt, AttributeEnum::Actual, "PUBLIC_KEY_1,PUBLIC_KEY_2"));
    EXPECT_TRUE(attribute_has_value("OCPP16LegacyCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "DisableSecurityEventNotifications", std::nullopt, AttributeEnum::Actual, "true"));
    EXPECT_TRUE(attribute_has_value("OCPP16LegacyCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "ISO15118CertificateManagementEnabled", std::nullopt, AttributeEnum::Actual,
                                    "false"));
    EXPECT_TRUE(attribute_has_value("OCPP16LegacyCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "CustomDisplayCostAndPrice", std::nullopt, AttributeEnum::Actual, "false"));

    EXPECT_TRUE(attribute_has_value("OCPP16LegacyCtrlr", std::nullopt, std::nullopt, std::nullopt, "DefaultPrice",
                                    std::nullopt, AttributeEnum::Actual,
                                    "{\"priceText\":\"This is the price\",\"priceTextOffline\":\"Show this price text "
                                    "when offline!\",\"chargingPrice\":{\"kWhPrice\":3.14,\"hourPrice\":0.42}}",
                                    ValueCompareMode::JsonSemantic));
    EXPECT_TRUE(attribute_has_value(
        "OCPP16LegacyCtrlr", std::nullopt, std::nullopt, std::nullopt, "DefaultPriceText", std::nullopt,
        AttributeEnum::Actual,
        "{\"priceTexts\":[{\"priceText\":\"This is the price\",\"priceTextOffline\":\"Show this price text when "
        "offline!\",\"language\":\"en\"},{\"priceText\":\"Dit is de prijs\",\"priceTextOffline\":\"Laat dit zien "
        "wanneer de charging station offline is!\",\"language\":\"nl\"},{\"priceText\":\"Dette er "
        "prisen\",\"priceTextOffline\":\"Vis denne pristeksten når du er frakoblet\",\"language\":\"nb_NO\"}]}",
        ValueCompareMode::JsonSemantic));
    EXPECT_TRUE(attribute_has_value("OCPP16LegacyCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "CustomIdleFeeAfterStop", std::nullopt, AttributeEnum::Actual, "false"));
    EXPECT_TRUE(attribute_has_value("OCPP16LegacyCtrlr", std::nullopt, std::nullopt, std::nullopt, "SupportedLanguages",
                                    std::nullopt, AttributeEnum::Actual, "en, nl, de, nb_NO"));
    EXPECT_TRUE(attribute_has_value("OCPP16LegacyCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "CustomMultiLanguageMessages", std::nullopt, AttributeEnum::Actual, "true"));
    EXPECT_TRUE(attribute_has_value("OCPP16LegacyCtrlr", std::nullopt, std::nullopt, std::nullopt, "Language",
                                    std::nullopt, AttributeEnum::Actual, "en"));
    EXPECT_TRUE(attribute_has_value("OCPP16LegacyCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                    "WaitForSetUserPriceTimeout", std::nullopt, AttributeEnum::Actual, "5000"));

    // ============================================================================
    // OCPP 1.6 keys with problematic OCPP 2.x mappings (MAPPING_MISC_ADDITIONAL)
    // AuthorizationKey is handled via special case (WriteOnly, not in get_all_key_value)
    // CentralSystemURI and SecurityProfile go through the generic loop
    // ============================================================================
    EXPECT_TRUE(attribute_has_value("OCPP16LegacyCtrlr", std::nullopt, std::nullopt, std::nullopt, "AuthorizationKey",
                                    std::nullopt, AttributeEnum::Actual, "SECRETSECRETSECRET"));
    EXPECT_TRUE(attribute_has_value("OCPP16LegacyCtrlr", std::nullopt, std::nullopt, std::nullopt, "CentralSystemURI",
                                    std::nullopt, AttributeEnum::Actual,
                                    "127.0.0.1:8181/steve/websocket/CentralSystemService/"));
    EXPECT_TRUE(attribute_has_value("OCPP16LegacyCtrlr", std::nullopt, std::nullopt, std::nullopt, "SecurityProfile",
                                    std::nullopt, AttributeEnum::Actual, "1"));

    const auto measurands_values_list = "Energy.Active.Import.Register,Energy.Active.Export.Register,Power.Active."
                                        "Import,Voltage,Current.Import,Frequency,Current.Offered,Power.Offered,SoC";

    EXPECT_TRUE(characteristics_exists("AlignedDataCtrlr", std::nullopt, std::nullopt, std::nullopt, "Measurands",
                                       std::nullopt, DataEnum::MemberList, 611, std::nullopt, true, std::nullopt,
                                       measurands_values_list));
    EXPECT_TRUE(characteristics_exists("SampledDataCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                       "TxUpdatedMeasurands", std::nullopt, DataEnum::MemberList, 622, std::nullopt,
                                       true, std::nullopt, measurands_values_list));
    EXPECT_TRUE(characteristics_exists("AlignedDataCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                       "TxEndedMeasurands", std::nullopt, DataEnum::MemberList, 633, std::nullopt, true,
                                       std::nullopt, measurands_values_list));
    EXPECT_TRUE(characteristics_exists("SampledDataCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                       "TxEndedMeasurands", std::nullopt, DataEnum::MemberList, 644, std::nullopt, true,
                                       std::nullopt, measurands_values_list));
    EXPECT_TRUE(characteristics_exists("SampledDataCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                       "TxStartedMeasurands", std::nullopt, DataEnum::MemberList, std::nullopt,
                                       std::nullopt, true, std::nullopt, measurands_values_list));
    EXPECT_TRUE(characteristics_exists("LocalAuthListCtrlr", std::nullopt, std::nullopt, std::nullopt, "Entries",
                                       std::nullopt, DataEnum::integer, 512, std::nullopt, true, std::nullopt,
                                       std::nullopt));
    EXPECT_TRUE(characteristics_exists("SecurityCtrlr", std::nullopt, std::nullopt, std::nullopt, "CertificateEntries",
                                       std::nullopt, DataEnum::integer, 700, std::nullopt, true, std::nullopt,
                                       std::nullopt));
    EXPECT_TRUE(characteristics_exists("SmartChargingCtrlr", std::nullopt, std::nullopt, std::nullopt, "Entries",
                                       "ChargingProfiles", DataEnum::integer, 1001, std::nullopt, true, std::nullopt,
                                       std::nullopt));
}

} // namespace ocpp::v2
