// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef JSON_DEFINED_ENERGY_MANAGER_TEST_HPP
#define JSON_DEFINED_ENERGY_MANAGER_TEST_HPP

#include <chrono>
#include <date/date.h>
#include <date/tz.h>
#include <filesystem>
#include <gtest/gtest.h>
#include <memory>

#include <generated/types/energy.hpp>

#include "EnergyManagerImpl.hpp"

namespace module {

// This test runs a test from a single json file
// and asserts that the result matches the expected result defined in the same file
class JsonDefinedEnergyManagerTest : public ::testing::Test {
public:
    JsonDefinedEnergyManagerTest();
    explicit JsonDefinedEnergyManagerTest(const std::filesystem::path& path);
    void TestBody() override;

protected:
    void load_test(const std::filesystem::path& path);
    void run_test(date::utc_clock::time_point _start_time);

    std::unique_ptr<EnergyManagerImpl> impl;
    date::utc_clock::time_point start_time;
    EnergyManagerConfig config;
    types::energy::EnergyFlowRequest request;
    std::vector<types::energy::EnforcedLimits> expected_result;

private:
    std::string comment;
};

} // namespace module

#endif // JSON_DEFINED_ENERGY_MANAGER_TEST_HPP
