// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>

#include <device_model_test_helper.hpp>

#include <ocpp/v2/ctrlr_component_variables.hpp>
#include <ocpp/v2/device_model.hpp>
#include <ocpp/v2/device_model_storage_sqlite.hpp>

namespace ocpp {
namespace v2 {

class DeviceModelTest : public ::testing::Test {
protected:
    DeviceModelTestHelper device_model_test_helper;
    DeviceModel* dm;
    const RequiredComponentVariable cv = ControllerComponentVariables::AlignedDataInterval;
    const std::map<int32_t, int32_t> evse_connector_structure{{1, 1}, {2, 1}};

    DeviceModelTest() : device_model_test_helper(), dm(device_model_test_helper.get_device_model()) {
    }
};

/// \brief Test if value can be changed. And test if setting a value of 0 is allowed for a value that is allowed to be
/// 0.
TEST_F(DeviceModelTest, test_set_value_and_allow_zero) {
    // default value is 900
    auto r = dm->get_value<int>(cv, ocpp::v2::AttributeEnum::Actual);
    ASSERT_EQ(r, 900);

    // try to set to 2, which is allowed because there is no minLimit
    auto sv_result = dm->set_value(cv.component, cv.variable.value(), ocpp::v2::AttributeEnum::Actual, "2", "test");
    ASSERT_EQ(sv_result, SetVariableStatusEnum::Accepted);

    // try to set to 0, which is allowed because 0 is an exception
    sv_result = dm->set_value(cv.component, cv.variable.value(), ocpp::v2::AttributeEnum::Actual, "0", "test");
    ASSERT_EQ(sv_result, SetVariableStatusEnum::Accepted);

    r = dm->get_value<int>(cv, ocpp::v2::AttributeEnum::Actual);
    ASSERT_EQ(r, 0);
}

/// \brief Test if value of 0 is allowed for ControllerComponentVariables::AlignedDataInterval although the minLimit is
/// set to 5
TEST_F(DeviceModelTest, test_min_limit_and_allow_zero) {
    // Set a min limit to 'Interval' of 'AlignedDataCtrlr'.
    VariableCharacteristics c;
    c.minLimit = 10;
    c.dataType = DataEnum::integer;
    c.supportsMonitoring = true;
    c.unit = "s";

    device_model_test_helper.update_variable_characteristics(c, "AlignedDataCtrlr", std::nullopt, std::nullopt,
                                                             std::nullopt, "Interval", std::nullopt);

    dm = device_model_test_helper.get_device_model();

    // default value is 900
    auto r = dm->get_value<int>(cv, ocpp::v2::AttributeEnum::Actual);
    ASSERT_EQ(r, 900);

    // try to set to 2, which is not allowed because minLimit of 10
    auto sv_result = dm->set_value(cv.component, cv.variable.value(), ocpp::v2::AttributeEnum::Actual, "2", "test");
    ASSERT_EQ(sv_result, SetVariableStatusEnum::Rejected);

    // try to set to 0, which is allowed because 0 is an exception
    sv_result = dm->set_value(cv.component, cv.variable.value(), ocpp::v2::AttributeEnum::Actual, "0", "test");
    ASSERT_EQ(sv_result, SetVariableStatusEnum::Accepted);

    r = dm->get_value<int>(cv, ocpp::v2::AttributeEnum::Actual);
    ASSERT_EQ(r, 0);
}

TEST_F(DeviceModelTest, test_component_as_key_in_map) {
    std::map<Component, int32_t> components_to_ints;

    Component base_comp;
    base_comp.name = "Foo";
    components_to_ints[base_comp] = 1;

    Component different_instance_comp;
    different_instance_comp.name = "Foo";
    different_instance_comp.instance = "bar";

    Component different_evse_comp;
    different_evse_comp.name = "Foo";
    EVSE evse0;
    evse0.id = 0;
    different_evse_comp.evse = evse0;

    Component different_evse_and_instance_comp;
    different_evse_and_instance_comp.name = "Foo";
    different_evse_and_instance_comp.evse = evse0;
    different_evse_and_instance_comp.instance = "bar";

    Component comp_with_custom_data;
    comp_with_custom_data.name = "Foo";
    comp_with_custom_data.customData = json::object({{"vendorId", "Baz"}});

    Component different_name_comp;
    different_name_comp.name = "Bar";

    EXPECT_EQ(components_to_ints.find(base_comp)->second, 1);
    EXPECT_EQ(components_to_ints.find(different_instance_comp), components_to_ints.end());
    EXPECT_EQ(components_to_ints.find(different_evse_comp), components_to_ints.end());
    EXPECT_EQ(components_to_ints.find(different_evse_and_instance_comp), components_to_ints.end());
    EXPECT_EQ(components_to_ints.find(comp_with_custom_data)->second, 1);
    EXPECT_EQ(components_to_ints.find(different_name_comp), components_to_ints.end());
}

TEST_F(DeviceModelTest, test_set_monitors) {
    std::vector<SetMonitoringData> requests;

    EVSE evse;
    evse.id = 1;
    evse.connectorId = 1;

    Component component1;
    component1.name = "Connector";
    component1.evse = evse;

    Component component2;
    component2.name = "AlignedDataCtrlr";

    Variable variable_comp1;
    variable_comp1.name = "SupplyPhases";
    Variable variable_comp2;
    variable_comp2.name = "Interval";

    std::vector<ComponentVariable> components = {
        {component1, variable_comp1},
        {component2, variable_comp2},
    };

    // Clear all existing monitors for a clean test state
    auto existing_monitors = dm->get_monitors({}, components);
    for (auto& result : existing_monitors) {
        std::vector<int32_t> ids;
        for (auto& monitor : result.variableMonitoring) {
            ids.push_back(monitor.id);
        }
        dm->clear_monitors(ids, true);
    }

    SetMonitoringData req_one;
    req_one.value = 0.0;
    req_one.type = MonitorEnum::PeriodicClockAligned;
    req_one.severity = 7;
    req_one.component = component1;
    req_one.variable = variable_comp1;
    SetMonitoringData req_two;
    req_two.value = 4.579;
    req_two.type = MonitorEnum::UpperThreshold;
    req_two.severity = 3;
    req_two.component = component2;
    req_two.variable = variable_comp2;

    requests.push_back(req_one);
    requests.push_back(req_two);

    auto results = dm->set_monitors(requests);
    ASSERT_EQ(results.size(), requests.size());

    ASSERT_EQ(results[0].status, SetMonitoringStatusEnum::Accepted);
    // Interval is not a float but an integer.
    ASSERT_EQ(results[1].status, SetMonitoringStatusEnum::Rejected);
}

TEST_F(DeviceModelTest, test_get_monitors) {
    // Set 'Interval' to not support monitoring.
    VariableCharacteristics c;
    c.minLimit = 10;
    c.dataType = DataEnum::integer;
    c.supportsMonitoring = false;
    c.unit = "s";

    device_model_test_helper.update_variable_characteristics(c, "AlignedDataCtrlr", std::nullopt, std::nullopt,
                                                             std::nullopt, "Interval", std::nullopt);

    dm = device_model_test_helper.get_device_model();

    std::vector<MonitoringCriterionEnum> criteria = {
        MonitoringCriterionEnum::DeltaMonitoring,
        MonitoringCriterionEnum::PeriodicMonitoring,
        MonitoringCriterionEnum::ThresholdMonitoring,
    };

    EVSE evse;
    evse.id = 1;
    evse.connectorId = 1;

    Component component1;
    component1.name = "Connector";
    component1.evse = evse;

    Component component2;
    component2.name = "AlignedDataCtrlr";

    Variable variable_comp1;
    variable_comp1.name = "SupplyPhases";
    Variable variable_comp2;
    variable_comp2.name = "Interval";

    std::vector<ComponentVariable> components = {
        {component1, variable_comp1},
        {component2, variable_comp2},
    };

    SetMonitoringData req_one;
    req_one.value = 0.0;
    req_one.type = MonitorEnum::PeriodicClockAligned;
    req_one.severity = 7;
    req_one.component = component1;
    req_one.variable = variable_comp1;
    SetMonitoringData req_two;
    req_two.value = 4.579;
    req_two.type = MonitorEnum::UpperThreshold;
    req_two.severity = 3;
    req_two.component = component2;
    req_two.variable = variable_comp2;

    std::vector<SetMonitoringData> requests;
    requests.push_back(req_one);
    requests.push_back(req_two);

    auto results_set = dm->set_monitors(requests);
    ASSERT_EQ(results_set.size(), requests.size());

    auto results_get = dm->get_monitors(criteria, components);
    ASSERT_EQ(results_get.size(), 1);

    ASSERT_EQ(results_get[0].variableMonitoring.size(), 1);
    auto monitor1 = results_get[0].variableMonitoring[0];

    ASSERT_EQ(monitor1.severity, 7);
    ASSERT_EQ(monitor1.type, MonitorEnum::PeriodicClockAligned);
}

TEST_F(DeviceModelTest, test_clear_monitors) {
    std::vector<MonitoringCriterionEnum> criteria = {
        MonitoringCriterionEnum::DeltaMonitoring,
        MonitoringCriterionEnum::PeriodicMonitoring,
        MonitoringCriterionEnum::ThresholdMonitoring,
    };

    EVSE evse;
    evse.id = 1;
    evse.connectorId = 1;

    Component component1;
    component1.name = "Connector";
    component1.evse = evse;
    Component component2;
    component2.name = "AlignedDataCtrlr";

    Variable variable_comp1;
    variable_comp1.name = "SupplyPhases";
    Variable variable_comp2;
    variable_comp2.name = "Interval";

    std::vector<ComponentVariable> components = {
        {component1, variable_comp1},
        {component2, variable_comp2},
    };

    SetMonitoringData req_one;
    req_one.value = 0.0;
    req_one.type = MonitorEnum::PeriodicClockAligned;
    req_one.severity = 7;
    req_one.component = component1;
    req_one.variable = variable_comp1;
    SetMonitoringData req_two;
    req_two.value = 4.579;
    req_two.type = MonitorEnum::UpperThreshold;
    req_two.severity = 3;
    req_two.component = component2;
    req_two.variable = variable_comp2;

    std::vector<SetMonitoringData> requests;
    requests.push_back(req_one);
    requests.push_back(req_two);

    auto results_set = dm->set_monitors(requests);

    // Insert some monitors that are hard-wired
    SetMonitoringData hardwired_one;
    hardwired_one.value = 0.0;
    hardwired_one.type = MonitorEnum::PeriodicClockAligned;
    hardwired_one.severity = 5;
    hardwired_one.component = component1;
    hardwired_one.variable = variable_comp1;
    SetMonitoringData hardwired_two;
    hardwired_two.value = 8.579;
    hardwired_two.type = MonitorEnum::UpperThreshold;
    hardwired_two.severity = 2;
    hardwired_two.component = component2;
    hardwired_two.variable = variable_comp2;

    std::vector<SetMonitoringData> requests2;
    requests2.push_back(hardwired_one);
    requests2.push_back(hardwired_two);

    auto set_result = dm->set_monitors(requests2, VariableMonitorType::HardWiredMonitor);
    std::vector<int> hardwired_monitor_ids;

    ASSERT_EQ(set_result.size(), 2);
    ASSERT_EQ(set_result[0].status, SetMonitoringStatusEnum::Accepted);
    ASSERT_EQ(set_result[1].status, SetMonitoringStatusEnum::Rejected);

    for (auto& res : set_result) {
        if (res.status == SetMonitoringStatusEnum::Accepted) {
            hardwired_monitor_ids.push_back(res.id.value());
        }
    }

    auto current_results = dm->get_monitors(criteria, components);

    // Delete all found IDs
    std::vector<int> to_delete;
    for (auto& result : current_results) {
        for (auto& monitor : result.variableMonitoring) {
            to_delete.push_back(monitor.id);
        }
    }

    auto clear_result = dm->clear_custom_monitors();
    ASSERT_EQ(clear_result, 1); // 1 custom should be deleted, since 1 is rejected

    auto results = dm->get_monitors(criteria, components);
    ASSERT_EQ(results.size(), 1);

    for (auto& result : results) {
        // Each have 1 hardwired monitor
        ASSERT_EQ(result.variableMonitoring.size(), 1);
    }

    // All must be rejected
    auto res_clear = dm->clear_monitors(hardwired_monitor_ids);

    for (auto& result : res_clear) {
        ASSERT_EQ(result.status, ClearMonitoringStatusEnum::Rejected);
    }

    // Clear all for next test iteration
    dm->clear_monitors(hardwired_monitor_ids, true);
}

/// \brief Tests check_integrity does not raise error for valid database
TEST_F(DeviceModelTest, test_check_integrity_valid) {
    EXPECT_NO_THROW(dm->check_integrity(evse_connector_structure));
}

/// \brief Tests check_integrity raises exception for invalid database
TEST_F(DeviceModelTest, test_check_integrity_invalid_missing_required_variable) {
    // Remove a required variable from the databsae.
    device_model_test_helper.remove_variable_from_db("DisplayMessageCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                                     "DisplayMessages", std::nullopt);
    dm = device_model_test_helper.get_device_model();
    // Set the display message ctrlr to available.
    dm->set_value(ControllerComponentVariables::DisplayMessageCtrlrAvailable.component,
                  ControllerComponentVariables::DisplayMessageCtrlrAvailable.variable.value(), AttributeEnum::Actual,
                  "true", "test", true);
    // This should throw an exception as the display message ctrlr is available, but there is a required variable
    // missing.
    EXPECT_THROW(dm->check_integrity(evse_connector_structure), DeviceModelError);

    // Set display message ctrlr to unavailable.
    dm->set_value(ControllerComponentVariables::DisplayMessageCtrlrAvailable.component,
                  ControllerComponentVariables::DisplayMessageCtrlrAvailable.variable.value(), AttributeEnum::Actual,
                  "false", "test", true);

    // This should now pass.
    EXPECT_NO_THROW(dm->check_integrity(evse_connector_structure));

    // If we completely remove the variable.
    device_model_test_helper.remove_variable_from_db("DisplayMessageCtrlr", std::nullopt, std::nullopt, std::nullopt,
                                                     "Available", std::nullopt);
    dm = device_model_test_helper.get_device_model();
    // Then it should pass as well.
    EXPECT_NO_THROW(dm->check_integrity(evse_connector_structure));
}

TEST_F(DeviceModelTest, test_check_integrity_invalid_missing_required_variable_value) {
    // Remove a required variable value from the database
    device_model_test_helper.set_variable_attribute_value_null("SecurityCtrlr", std::nullopt, std::nullopt,
                                                               std::nullopt, "OrganizationName", std::nullopt,
                                                               AttributeEnum::Actual);
    // This should throw an exception.
    EXPECT_THROW(dm->check_integrity(evse_connector_structure), DeviceModelError);
}

TEST_F(DeviceModelTest, test_check_integrity_no_supported_versions_found) {
    // There are no known supported ocpp versions.
    dm->set_value(ControllerComponentVariables::SupportedOcppVersions.component,
                  ControllerComponentVariables::SupportedOcppVersions.variable.value(), AttributeEnum::Actual, "",
                  "test", true);

    // This should throw an exception.
    EXPECT_THROW(dm->check_integrity(evse_connector_structure), DeviceModelError);
}

TEST_F(DeviceModelTest, test_check_integrity_wrong_connector_structure) {
    std::map<int32_t, int32_t> evse_connector_structure{{1, 3}, {2, 1}, {3, 1}};
    EXPECT_THROW(dm->check_integrity(evse_connector_structure), DeviceModelError);

    evse_connector_structure = std::map<int32_t, int32_t>{{1, 3}, {2, 1}};
    EXPECT_THROW(dm->check_integrity(evse_connector_structure), DeviceModelError);

    evse_connector_structure = std::map<int32_t, int32_t>{{1, 1}};
    EXPECT_THROW(dm->check_integrity(evse_connector_structure), DeviceModelError);
}

TEST_F(DeviceModelTest, test_check_integrity_missing_required_evse_variable) {
    // Set required EVSE variable value to NULL.
    device_model_test_helper.set_variable_attribute_value_null("EVSE", std::nullopt, 1, std::nullopt, "SupplyPhases",
                                                               std::nullopt, AttributeEnum::Actual);
    dm = device_model_test_helper.get_device_model();
    // This should throw an exception.
    EXPECT_THROW(dm->check_integrity(evse_connector_structure), DeviceModelError);

    // Now remove the complete variable.
    device_model_test_helper.remove_variable_from_db("EVSE", std::nullopt, 1, std::nullopt, "SupplyPhases",
                                                     std::nullopt);
    dm = device_model_test_helper.get_device_model();
    // This should also throw an exception.
    EXPECT_THROW(dm->check_integrity(evse_connector_structure), DeviceModelError);
}

TEST_F(DeviceModelTest, test_check_integrity_missing_required_connector_variable) {
    // Set required EVSE variable value to NULL.
    device_model_test_helper.set_variable_attribute_value_null("Connector", std::nullopt, 1, 1, "ConnectorType",
                                                               std::nullopt, AttributeEnum::Actual);
    dm = device_model_test_helper.get_device_model();
    // This should throw an exception.
    EXPECT_THROW(dm->check_integrity(evse_connector_structure), DeviceModelError);

    // Now remove the complete variable.
    device_model_test_helper.remove_variable_from_db("Connector", std::nullopt, 1, 1, "ConnectorType", std::nullopt);
    dm = device_model_test_helper.get_device_model();
    // This should also throw an exception.
    EXPECT_THROW(dm->check_integrity(evse_connector_structure), DeviceModelError);
}

TEST_F(DeviceModelTest, test_check_integrity_missing_required_evse_power) {
    // Remove maxLimit of 'Power' variable of EVSE.
    VariableCharacteristics c;
    c.maxLimit = std::nullopt;
    c.dataType = DataEnum::decimal;
    c.supportsMonitoring = true;
    c.unit = "W";
    device_model_test_helper.update_variable_characteristics(c, "EVSE", std::nullopt, 1, std::nullopt, "Power",
                                                             std::nullopt);
    dm = device_model_test_helper.get_device_model();
    // This should throw an exception.
    EXPECT_THROW(dm->check_integrity(evse_connector_structure), DeviceModelError);

    // Now remove the complete variable.
    device_model_test_helper.remove_variable_from_db("EVSE", std::nullopt, 1, std::nullopt, "Power", std::nullopt);
    dm = device_model_test_helper.get_device_model();
    // This should also throw an exception.
    EXPECT_THROW(dm->check_integrity(evse_connector_structure), DeviceModelError);
}

} // namespace v2
} // namespace ocpp
