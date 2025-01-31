// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "JsonDefinedEnergyManagerTest.hpp"
#include "EnergyManagerConfigJson.hpp"

#include <fstream>

namespace module {

JsonDefinedEnergyManagerTest::JsonDefinedEnergyManagerTest() = default;

JsonDefinedEnergyManagerTest::JsonDefinedEnergyManagerTest(const std::filesystem::path& path) {
    load_test(path);
}

void JsonDefinedEnergyManagerTest::TestBody() {
    run_test(start_time);
}

void JsonDefinedEnergyManagerTest::load_test(const std::filesystem::path& path) {
    std::ifstream f(path.c_str());
    json data = json::parse(f);

    if (data.contains("basefile")) {
        // Load base file first
        std::filesystem::path basefile = std::filesystem::path(path).parent_path() / std::string(data.at("basefile"));
        std::ifstream bf(basefile.c_str());
        json databf = json::parse(bf);

        // Apply patches
        data = databf.patch(data.at("patches"));
    }
    this->request = data.at("request");
    for (auto limit : data.at("expected_result")) {
        types::energy::EnforcedLimits e;
        from_json(limit, e);
        this->expected_result.push_back(e);
    }

    // Recreate the EnergyManagerImpl with the config from the test
    this->config = data.at("config");
    this->impl.reset(
        new EnergyManagerImpl(this->config, [](const std::vector<types::energy::EnforcedLimits>& limits) { return; }));

    this->comment = path;
    this->start_time = Everest::Date::from_rfc3339(data.at("start_time"));
}

void JsonDefinedEnergyManagerTest::run_test(date::utc_clock::time_point _start_time) {
    const auto enforced_limits = this->impl->run_optimizer(request, _start_time);

    json diff = json::diff(json(expected_result), json(enforced_limits));
    ASSERT_EQ(diff.size(), 0) << "Diff to expected output:" << std::endl
                              << diff.dump(2) << std::endl
                              << "----------------------------------------" << std::endl
                              << "Comment: " << std::endl
                              << comment << std::endl
                              << "----------------------------------------" << std::endl
                              << "Full Request: " << std::endl
                              << request << "----------------------------------------" << std::endl
                              << "Full Enforced Limits: " << std::endl
                              << json(enforced_limits).dump(4) << "----------------------------------------"
                              << std::endl;
}

// Example to modify the test after loading
// TEST_F(JsonDefinedEnergyManagerTest, json_based_test_01) {
//     load_test(std::string(JSON_TESTS_LOCATION) + "/1_0_two_evse_load_balancing.json");

//     // Do here any modifications to the test
//     this->request;
//     this->expected_result;

//     run_test();
// }

TEST_F(JsonDefinedEnergyManagerTest, phase_switching_1ph3ph) {
    load_test(std::string(JSON_TESTS_LOCATION) + "/5_ac_phase_switching.json");

    // this->impl->config.switch_3ph1ph_while_charging_mode = "Both";
    //  3ph initially
    run_test(start_time);

    // Next run should be single phase
    // this->expected_result[0].limits_root_side.value().ac_max_current_A = 18;
    // this->expected_result[0].limits_root_side.value().ac_max_phase_count = 1;
    // this->expected_result[0].limits_root_side.value().total_power_W = 4140;
    // this->expected_result[0].schedule.value()[2].limits_to_root.ac_max_current_A = 8.69565200805664;
    // this->expected_result[0].schedule.value()[2].limits_to_root.total_power_W = 2000.0;
    // this->expected_result[0].schedule.value()[2].limits_to_root.ac_max_phase_count = 1;
    // this->expected_result[0].valid_until = "2024-12-17T14:00:11.000Z";
    // run_test(start_time + std::chrono::seconds(1 * 3600 - 1));
}

} // namespace module
