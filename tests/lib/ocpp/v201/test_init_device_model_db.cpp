// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2024 Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>

#include <ocpp/v201/device_model_storage_sqlite.hpp>

#define private public
// database_exists must be set with this test, but std::filesystem::exists is hard to stub, so this way we can set it
// in the test.
#include <ocpp/v201/init_device_model_db.hpp>
#undef private

#include <lib/ocpp/common/database_testing_utils.hpp>

namespace ocpp::v201 {

class InitDeviceModelDbTest : public DatabaseTestingUtils {
protected:
    const std::string DATABASE_PATH = "file::memory:?cache=shared";
    const std::string MIGRATION_FILES_PATH = "./resources/v201/device_model_migration_files";
    const std::string SCHEMAS_PATH = "./resources/config/v201/component_schemas";
    const std::string SCHEMAS_PATH_CHANGED = "./resources/config/v201/changed/component_schemas";
    const std::string CONFIG_PATH = "./resources/config/v201/config.json";
    const std::string CONFIG_PATH_CHANGED = "./resources/config/v201/changed/config.json";
    const std::string CONFIG_PATH_WRONG_ATTRIBUTE = "./resources/config/v201/config_wrong_attribute.json";
    const std::string CONFIG_PATH_NOT_EXISTING_ATTRIBUTE = "./resources/config/v201/config_not_existing_attribute.json";
    const std::string CONFIG_PATH_WRONG_COMPONENT = "./resources/config/v201/config_wrong_component.json";
    const std::string CONFIG_PATH_WRONG_COMPONENT_CONNECTORID =
        "./resources/config/v201/config_wrong_component_connectorid.json";
    const std::string CONFIG_PATH_WRONG_VARIABLE = "./resources/config/v201/config_wrong_variable.json";

public:
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
                         const std::string& variable_name, const std::optional<std::string>& variable_instance);

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
                             const std::string& value);

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
     * a given schema and check some variables and attributes to see if they exist. It will then initialize again, but
     * with a changed schema, which should update the database (in some cases).
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
    EXPECT_NO_THROW(db.initialize_database(std::filesystem::path(SCHEMAS_PATH), true));

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
    EXPECT_TRUE(variable_exists("UnitTestCtrlr", std::nullopt, 2, 3, "UnitTestPropertyAName", std::nullopt));
    EXPECT_TRUE(variable_exists("UnitTestCtrlr", std::nullopt, 2, 3, "UnitTestPropertyBName", std::nullopt));
    EXPECT_TRUE(variable_exists("UnitTestCtrlr", std::nullopt, 2, 3, "UnitTestPropertyCName", std::nullopt));

    // So now we have made some changes and added a new EVSE and changed the connector. The database should be changed
    // accordingly.
    InitDeviceModelDb db2 = InitDeviceModelDb(DATABASE_PATH, MIGRATION_FILES_PATH);
    // This time, the database does exist (again: std::filesystem::exists, which is automatically used, will not work
    // here because we use an in memory database, so we set the member ourselves).
    db2.database_exists = true;
    EXPECT_NO_THROW(db2.initialize_database(SCHEMAS_PATH_CHANGED, false));

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

    // Characteristics changed from not EVSE or Connector (should not change in db)
    EXPECT_TRUE(characteristics_exists("UnitTestCtrlr", std::nullopt, 2, 3, "UnitTestPropertyAName", std::nullopt,
                                       DataEnum::boolean, std::nullopt, std::nullopt, true, std::nullopt,
                                       std::nullopt));
    EXPECT_FALSE(characteristics_exists("UnitTestCtrlr", std::nullopt, 2, 3, "UnitTestPropertyAName", std::nullopt,
                                        DataEnum::string, std::nullopt, std::nullopt, true, std::nullopt,
                                        std::nullopt));

    // No change, because this is not a Connector or EVSE
    EXPECT_TRUE(variable_exists("UnitTestCtrlr", std::nullopt, 2, 3, "UnitTestPropertyBName", std::nullopt));
    EXPECT_TRUE(variable_exists("UnitTestCtrlr", std::nullopt, 2, 3, "UnitTestPropertyCName", std::nullopt));
}

TEST_F(InitDeviceModelDbTest, insert_values) {
    /* This test will test if the config and default values are correctly set. We test this twice: first an initial
     * config, then a changed configuration.
     */

    InitDeviceModelDb db(DATABASE_PATH, MIGRATION_FILES_PATH);

    // Database should not exist yet. But since it does a filesystem check and we have an in memory database, we
    // explicitly set the variable here.
    db.database_exists = false;
    // First create the database.
    EXPECT_NO_THROW(db.initialize_database(SCHEMAS_PATH, true));
    // Then insert the config and default values.
    EXPECT_TRUE(db.insert_config_and_default_values(SCHEMAS_PATH, CONFIG_PATH));

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

    // Default value
    EXPECT_TRUE(attribute_has_value("Connector", std::nullopt, 1, 1, "Available", std::nullopt, AttributeEnum::Actual,
                                    "false"));
    EXPECT_TRUE(attribute_has_value("EVSE", std::nullopt, 1, std::nullopt, "AvailabilityState", std::nullopt,
                                    AttributeEnum::Actual, "Unavailable"));
    EXPECT_TRUE(attribute_has_value("EVSE", std::nullopt, 2, std::nullopt, "SupplyPhases", std::nullopt,
                                    AttributeEnum::Actual, "0"));
    EXPECT_TRUE(
        attribute_has_value("Connector", std::nullopt, 1, 1, "ConnectorType", std::nullopt, AttributeEnum::Actual, ""));

    // Insert new config.
    // First set the source of an attribute to something else than 'default'
    set_attribute_source("Connector", std::nullopt, 2, 1, "ConnectorType", std::nullopt, AttributeEnum::Actual, "test");

    EXPECT_TRUE(db.insert_config_and_default_values(SCHEMAS_PATH, CONFIG_PATH_CHANGED));

    // Source was not 'default', connector type not changed.
    EXPECT_TRUE(attribute_has_value("Connector", std::nullopt, 2, 1, "ConnectorType", std::nullopt,
                                    AttributeEnum::Actual, "cChaoJi"));

    // Check changed values.
    EXPECT_TRUE(attribute_has_value("EVSE", std::nullopt, 2, std::nullopt, "Available", std::nullopt,
                                    AttributeEnum::Actual, "false"));
    EXPECT_TRUE(attribute_has_value("EVSE", std::nullopt, 1, std::nullopt, "SupplyPhases", std::nullopt,
                                    AttributeEnum::Actual, "2"));
    // // Variable does not exist so it could not set the value.
    // EXPECT_FALSE(variable_exists("EVSE", std::nullopt, 1, std::nullopt, "AvalableEVSEThingie", std::nullopt));
    // Variable was removed, so it will be set to the default value again.
    EXPECT_TRUE(
        attribute_has_value("EVSE", std::nullopt, 1, std::nullopt, "Power", std::nullopt, AttributeEnum::Actual, "0"));
    // Default value only applies to 'Actual', not 'MaxSet'
    EXPECT_TRUE(attribute_has_value("EVSE", std::nullopt, 1, std::nullopt, "Power", std::nullopt, AttributeEnum::MaxSet,
                                    "44000"));
    // // Component does not exist so it could not set anything.
    // EXPECT_FALSE(component_exists("UnitTestCtrlr", std::nullopt, 1, 5));
}

TEST_F(InitDeviceModelDbTest, wrong_migration_file_path) {
    InitDeviceModelDb db(DATABASE_PATH, "/tmp/thisdoesnotexisthopefully");
    // The migration script is not correct (there is none in the given folder), this should throw an exception.
    EXPECT_THROW(db.initialize_database(SCHEMAS_PATH, true), DatabaseMigrationException);
}

TEST_F(InitDeviceModelDbTest, wrong_schemas_path) {
    // Wrong schemas path while initializing database
    InitDeviceModelDb db(DATABASE_PATH, MIGRATION_FILES_PATH);
    EXPECT_THROW(db.initialize_database("/tmp/thisdoesnotexisthopefully", true), std::filesystem::filesystem_error);
}

TEST_F(InitDeviceModelDbTest, wrong_schemas_path2) {
    // Wrong schemas path to insert config and default values.
    InitDeviceModelDb db(DATABASE_PATH, MIGRATION_FILES_PATH);
    db.initialize_database(SCHEMAS_PATH, true);
    EXPECT_THROW(db.insert_config_and_default_values("/tmp/wrongschemaspath", CONFIG_PATH),
                 std::filesystem::filesystem_error);
}

TEST_F(InitDeviceModelDbTest, wrong_config_path) {
    // Wrong config file path
    InitDeviceModelDb db(DATABASE_PATH, MIGRATION_FILES_PATH);
    EXPECT_NO_THROW(db.initialize_database(SCHEMAS_PATH, true));
    EXPECT_THROW(db.insert_config_and_default_values(SCHEMAS_PATH, "/tmp/wrongconfigpath"),
                 nlohmann::detail::parse_error);
}

TEST_F(InitDeviceModelDbTest, no_initialization) {
    // Try to insert config and default values while the schemas are not inserted yet.
    InitDeviceModelDb db(DATABASE_PATH, MIGRATION_FILES_PATH);
    EXPECT_THROW(db.insert_config_and_default_values(SCHEMAS_PATH, CONFIG_PATH), InitDeviceModelDbError);
}

TEST_F(InitDeviceModelDbTest, config_wrong_attribute) {
    // Try to insert attribute that is not existing (not one of 'Actual', 'MinSet', 'MaxSet' or 'Target')
    InitDeviceModelDb db(DATABASE_PATH, MIGRATION_FILES_PATH);
    EXPECT_NO_THROW(db.initialize_database(SCHEMAS_PATH, true));
    EXPECT_THROW(db.insert_config_and_default_values(SCHEMAS_PATH, CONFIG_PATH_WRONG_ATTRIBUTE),
                 InitDeviceModelDbError);
}

TEST_F(InitDeviceModelDbTest, config_not_existing_attribute) {
    // Try to insert attribute that is not set in the component schema.
    InitDeviceModelDb db(DATABASE_PATH, MIGRATION_FILES_PATH);
    EXPECT_NO_THROW(db.initialize_database(SCHEMAS_PATH, true));
    EXPECT_THROW(db.insert_config_and_default_values(SCHEMAS_PATH, CONFIG_PATH_NOT_EXISTING_ATTRIBUTE),
                 InitDeviceModelDbError);
}

TEST_F(InitDeviceModelDbTest, config_wrong_component) {
    // Try to set value of attribute of component that does not exist in the component schema.
    InitDeviceModelDb db(DATABASE_PATH, MIGRATION_FILES_PATH);
    db.initialize_database(SCHEMAS_PATH, true);
    EXPECT_THROW(db.insert_config_and_default_values(SCHEMAS_PATH, CONFIG_PATH_WRONG_COMPONENT),
                 InitDeviceModelDbError);
}

TEST_F(InitDeviceModelDbTest, config_wrong_variable) {
    // Try to set value of attribute of variable that does not exist in the component schema.
    InitDeviceModelDb db(DATABASE_PATH, MIGRATION_FILES_PATH);
    db.initialize_database(SCHEMAS_PATH, true);
    EXPECT_THROW(db.insert_config_and_default_values(SCHEMAS_PATH, CONFIG_PATH_WRONG_VARIABLE), InitDeviceModelDbError);
}

TEST_F(InitDeviceModelDbTest, config_wrong_component_connectorid) {
    // Try to set attribute value of component with a not existing connector id in the component schema.
    InitDeviceModelDb db(DATABASE_PATH, MIGRATION_FILES_PATH);
    db.initialize_database(SCHEMAS_PATH, true);
    EXPECT_THROW(db.insert_config_and_default_values(SCHEMAS_PATH, CONFIG_PATH_WRONG_COMPONENT_CONNECTORID),
                 InitDeviceModelDbError);
}

TEST_F(InitDeviceModelDbTest, default_device_model_config) {
    // Test if the default device model config is correct and will create a valid database with valid values.
    const static std::string MIGRATION_FILES_PATH_DEFAULT = "./resources/v201/device_model_migration_files";
    const static std::string SCHEMAS_PATH_DEFAULT = "./resources/example_config/v201/component_schemas";
    const static std::string CONFIG_PATH_DEFAULT = "./resources/example_config/v201/config.json";
    InitDeviceModelDb db(DATABASE_PATH, MIGRATION_FILES_PATH_DEFAULT);
    EXPECT_NO_THROW(db.initialize_database(SCHEMAS_PATH_DEFAULT, true));
    EXPECT_NO_THROW(db.insert_config_and_default_values(SCHEMAS_PATH_DEFAULT, CONFIG_PATH_DEFAULT));
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
    std::unique_ptr<common::SQLiteStatementInterface> statement =
        this->database->new_statement(select_component_statement);

    statement->bind_text("@component_name", component_name, ocpp::common::SQLiteString::Transient);
    if (component_instance.has_value()) {
        statement->bind_text("@component_instance", component_instance.value(), ocpp::common::SQLiteString::Transient);
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
    std::unique_ptr<common::SQLiteStatementInterface> statement =
        this->database->new_statement(select_attribute_statement);

    statement->bind_text("@component_name", component_name, ocpp::common::SQLiteString::Transient);
    if (component_instance.has_value()) {
        statement->bind_text("@component_instance", component_instance.value(), ocpp::common::SQLiteString::Transient);
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

    statement->bind_text("@variable_name", variable_name, ocpp::common::SQLiteString::Transient);

    if (variable_instance.has_value()) {
        statement->bind_text("@variable_instance", variable_instance.value(), ocpp::common::SQLiteString::Transient);
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
                                            const std::optional<std::string>& variable_instance) {
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
                                                         "AND v.INSTANCE IS @variable_instance";

    std::unique_ptr<common::SQLiteStatementInterface> statement =
        this->database->new_statement(select_variable_statement);

    statement->bind_text("@component_name", component_name, ocpp::common::SQLiteString::Transient);
    if (component_instance.has_value()) {
        statement->bind_text("@component_instance", component_instance.value(), ocpp::common::SQLiteString::Transient);
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

    statement->bind_text("@variable_name", variable_name, ocpp::common::SQLiteString::Transient);

    if (variable_instance.has_value()) {
        statement->bind_text("@variable_instance", variable_instance.value(), ocpp::common::SQLiteString::Transient);
    } else {
        statement->bind_null("@variable_instance");
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
    std::unique_ptr<common::SQLiteStatementInterface> statement =
        this->database->new_statement(select_characteristics_statement);

    statement->bind_text("@component_name", component_name, ocpp::common::SQLiteString::Transient);
    if (component_instance.has_value()) {
        statement->bind_text("@component_instance", component_instance.value(), ocpp::common::SQLiteString::Transient);
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

    statement->bind_text("@variable_name", variable_name, ocpp::common::SQLiteString::Transient);

    if (variable_instance.has_value()) {
        statement->bind_text("@variable_instance", variable_instance.value(), ocpp::common::SQLiteString::Transient);
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

bool InitDeviceModelDbTest::attribute_has_value(const std::string& component_name,
                                                const std::optional<std::string>& component_instance,
                                                const std::optional<int>& component_evse_id,
                                                const std::optional<int>& component_connector_id,
                                                const std::string& variable_name,
                                                const std::optional<std::string>& variable_instance,
                                                const AttributeEnum& type, const std::string& value) {
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
    std::unique_ptr<common::SQLiteStatementInterface> statement =
        this->database->new_statement(select_attribute_statement);

    statement->bind_text("@component_name", component_name, ocpp::common::SQLiteString::Transient);
    if (component_instance.has_value()) {
        statement->bind_text("@component_instance", component_instance.value(), ocpp::common::SQLiteString::Transient);
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

    statement->bind_text("@variable_name", variable_name, ocpp::common::SQLiteString::Transient);

    if (variable_instance.has_value()) {
        statement->bind_text("@variable_instance", variable_instance.value(), ocpp::common::SQLiteString::Transient);
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
    return value == statement->column_text(0);
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

    std::unique_ptr<common::SQLiteStatementInterface> insert_variable_attribute_statement =
        this->database->new_statement(statement);

    insert_variable_attribute_statement->bind_text("@source", source, SQLiteString::Transient);
    insert_variable_attribute_statement->bind_text("@component_name", component_name,
                                                   ocpp::common::SQLiteString::Transient);
    if (component_instance.has_value()) {
        insert_variable_attribute_statement->bind_text("@component_instance", component_instance.value(),
                                                       ocpp::common::SQLiteString::Transient);
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

    insert_variable_attribute_statement->bind_text("@variable_name", variable_name,
                                                   ocpp::common::SQLiteString::Transient);

    if (variable_instance.has_value()) {
        insert_variable_attribute_statement->bind_text("@variable_instance", variable_instance.value(),
                                                       ocpp::common::SQLiteString::Transient);
    } else {
        insert_variable_attribute_statement->bind_null("@variable_instance");
    }

    insert_variable_attribute_statement->bind_int("@type_id", static_cast<int>(type));

    if (insert_variable_attribute_statement->step() != SQLITE_DONE) {
        EVLOG_error << "Could not set source of variable " << variable_name << " (component: " << component_name
                    << ") attribute " << static_cast<int>(type) << ": " << this->database->get_error_message();
    }
}

// TODO tests with non existing paths

} // namespace ocpp::v201
