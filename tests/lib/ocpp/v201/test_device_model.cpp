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
        dm->set_value(cv.component, cv.variable.value(), ocpp::v201::AttributeEnum::Actual, "10");
    }
};

/// \brief Test if value of 0 is allowed for ControllerComponentVariables::AlignedDataInterval although the minLimit is
/// set to 5
TEST_F(DeviceModelTest, test_allow_zero) {
    // default value is 10
    auto r = dm->get_value<int>(cv, ocpp::v201::AttributeEnum::Actual);
    ASSERT_EQ(r, 10);

    // try to set to value of 2, which is not allowed because minLimit of
    auto sv_result = dm->set_value(cv.component, cv.variable.value(), ocpp::v201::AttributeEnum::Actual, "2");
    ASSERT_EQ(sv_result, SetVariableStatusEnum::Rejected);

    // try to set to 0, which is allowed because 0 is an exception
    sv_result = dm->set_value(cv.component, cv.variable.value(), ocpp::v201::AttributeEnum::Actual, "0");
    ASSERT_EQ(sv_result, SetVariableStatusEnum::Accepted);

    r = dm->get_value<int>(cv, ocpp::v201::AttributeEnum::Actual);
    ASSERT_EQ(r, 0);
}

} // namespace v201
} // namespace ocpp
