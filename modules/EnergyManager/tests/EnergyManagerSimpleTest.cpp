// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>
#include <iostream>

#include "EnergyManagerImpl.hpp"
#include "utils.hpp"

namespace module {

static const std::string BASE_PATH = std::string(ENERGY_FLOW_REQUESTS_LOCATION) + "/";

class EnergyManagerSimpleTests : public ::testing::Test {

protected:
    std::unique_ptr<EnergyManagerImpl> impl;

    void SetUp() override {
        EnergyManagerConfig config{
            230.0, // nominal_ac_voltage
            1,     // update_interval
            60,    // schedule_interval_duration
            1,     // schedule_total_duration
            0.5,   // slice_ampere
            500,   // slice_watt
            false, // debug
        };
        this->impl = std::make_unique<EnergyManagerImpl>(
            config, [](const std::vector<types::energy::EnforcedLimits>& limits) { return; });
    }
};

TEST_F(EnergyManagerSimpleTests, example_01) {

    const std::string example_file = BASE_PATH + "example.json";
    const auto e = get_energy_flow_request_from_file(example_file);

    const auto start_time = Everest::Date::from_rfc3339("2024-12-17T13:00:00.000Z");
    const auto enforced_limits = this->impl->run_optimizer(e, start_time);

    ASSERT_EQ(enforced_limits.size(), 2);
    ASSERT_TRUE(enforced_limits[0].limits_root_side.has_value());
    ASSERT_TRUE(enforced_limits[0].limits_root_side.value().ac_max_current_A.has_value());
    ASSERT_EQ(enforced_limits[0].limits_root_side.value().ac_max_current_A.value(), 32);
}

} // namespace module
