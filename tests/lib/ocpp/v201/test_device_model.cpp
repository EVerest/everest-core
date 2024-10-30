// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>
#include <ocpp/v201/ctrlr_component_variables.hpp>
#include <ocpp/v201/device_model.hpp>
#include <ocpp/v201/device_model_storage_sqlite.hpp>

namespace ocpp {
namespace v201 {

class DeviceModelTest : public ::testing::Test {
protected:
    const std::string DEVICE_MODEL_DATABASE = "./resources/unittest_device_model.db";
    std::unique_ptr<DeviceModel> dm;
    const RequiredComponentVariable cv = ControllerComponentVariables::AlignedDataInterval;

    void SetUp() override {
        dm =
            std::make_unique<DeviceModel>(std::move(std::make_unique<DeviceModelStorageSqlite>(DEVICE_MODEL_DATABASE)));
    }

    void TearDown() override {
        // reset the value to default
        dm->set_value(cv.component, cv.variable.value(), ocpp::v201::AttributeEnum::Actual, "10", "test");
    }
};

/// \brief Test if value of 0 is allowed for ControllerComponentVariables::AlignedDataInterval although the minLimit is
/// set to 5
TEST_F(DeviceModelTest, test_allow_zero) {
    // default value is 10
    auto r = dm->get_value<int>(cv, ocpp::v201::AttributeEnum::Actual);
    ASSERT_EQ(r, 10);

    // try to set to value of 2, which is not allowed because minLimit of
    auto sv_result = dm->set_value(cv.component, cv.variable.value(), ocpp::v201::AttributeEnum::Actual, "2", "test");
    ASSERT_EQ(sv_result, SetVariableStatusEnum::Rejected);

    // try to set to 0, which is allowed because 0 is an exception
    sv_result = dm->set_value(cv.component, cv.variable.value(), ocpp::v201::AttributeEnum::Actual, "0", "test");
    ASSERT_EQ(sv_result, SetVariableStatusEnum::Accepted);

    r = dm->get_value<int>(cv, ocpp::v201::AttributeEnum::Actual);
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
    evse.id = 2;
    evse.connectorId = 3;

    Component component1;
    component1.name = "UnitTestCtrlr";
    component1.evse = evse;

    Component component2;
    component2.name = "AlignedDataCtrlr";

    Variable variable_comp1;
    variable_comp1.name = "UnitTestPropertyAName";
    Variable variable_comp2;
    variable_comp2.name = "Interval";

    std::vector<ComponentVariable> components = {
        {component1, std::nullopt, variable_comp1},
        {component2, std::nullopt, variable_comp2},
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
    // Interval does not support monitoring
    ASSERT_EQ(results[1].status, SetMonitoringStatusEnum::Rejected);
}

TEST_F(DeviceModelTest, test_get_monitors) {
    std::vector<MonitoringCriterionEnum> criteria = {
        MonitoringCriterionEnum::DeltaMonitoring,
        MonitoringCriterionEnum::PeriodicMonitoring,
        MonitoringCriterionEnum::ThresholdMonitoring,
    };

    EVSE evse;
    evse.id = 2;
    evse.connectorId = 3;

    Component component1;
    component1.name = "UnitTestCtrlr";
    component1.evse = evse;

    Component component2;
    component2.name = "AlignedDataCtrlr";

    Variable variable_comp1;
    variable_comp1.name = "UnitTestPropertyAName";
    Variable variable_comp2;
    variable_comp2.name = "Interval";

    std::vector<ComponentVariable> components = {
        {component1, std::nullopt, variable_comp1},
        {component2, std::nullopt, variable_comp2},
    };

    auto results = dm->get_monitors(criteria, components);
    ASSERT_EQ(results.size(), 1);

    ASSERT_EQ(results[0].variableMonitoring.size(), 1);
    auto monitor1 = results[0].variableMonitoring[0];

    // Valued used above
    SetMonitoringData req_one;
    req_one.value = 0.0;
    req_one.type = MonitorEnum::PeriodicClockAligned;
    req_one.severity = 7;
    req_one.component = component1;
    req_one.variable = variable_comp1;

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
    evse.id = 2;
    evse.connectorId = 3;

    Component component1;
    component1.name = "UnitTestCtrlr";
    component1.evse = evse;
    Component component2;
    component2.name = "AlignedDataCtrlr";

    Variable variable_comp1;
    variable_comp1.name = "UnitTestPropertyAName";
    Variable variable_comp2;
    variable_comp2.name = "Interval";

    std::vector<ComponentVariable> components = {
        {component1, std::nullopt, variable_comp1},
        {component2, std::nullopt, variable_comp2},
    };

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

    std::vector<SetMonitoringData> requests;
    requests.push_back(hardwired_one);
    requests.push_back(hardwired_two);

    auto set_result = dm->set_monitors(requests, VariableMonitorType::HardWiredMonitor);
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

} // namespace v201
} // namespace ocpp
