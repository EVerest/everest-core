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
    run_test(start_times);
}

void JsonDefinedEnergyManagerTest::load_test(const std::filesystem::path& path) {
    std::ifstream f(path.c_str());
    json data;
    try {
        data = json::parse(f);
    } catch (...) {
        EVLOG_error << "Cannot parse JSON file " << path;
    }

    if (data.contains("basefile")) {
        // Load base file first
        std::filesystem::path basefile = std::filesystem::path(path).parent_path() / std::string(data.at("basefile"));
        std::ifstream bf(basefile.c_str());
        json databf = json::parse(bf);

        // Apply patches
        data = databf.patch(data.at("patches"));
    }
    this->request = data.at("request");

    for (auto results : data.at("expected_results")) {
        std::vector<types::energy::EnforcedLimits> l;
        for (auto limit : results) {
            types::energy::EnforcedLimits e;
            from_json(limit, e);
            l.push_back(e);
        }
        this->expected_results.push_back(l);
    }

    // Recreate the EnergyManagerImpl with the config from the test
    this->config = data.at("config");
    this->impl.reset(
        new EnergyManagerImpl(this->config, [](const std::vector<types::energy::EnforcedLimits>& limits) { return; }));

    this->comment = path;
    for (auto start_time : data.at("start_times")) {
        this->start_times.push_back(Everest::Date::from_rfc3339(start_time));
    }
}

void JsonDefinedEnergyManagerTest::run_test(std::vector<date::utc_clock::time_point> _start_times) {

    assert(_start_times.size() == expected_results.size());

    for (int i = 0; i < _start_times.size(); i++) {

        const auto enforced_limits = this->impl->run_optimizer(request, _start_times[i]);

        json diff = json::diff(json(expected_results[i]), json(enforced_limits));
        ASSERT_EQ(diff.size(), 0) << "At start time " << _start_times[i] << ": Diff to expected output:" << std::endl
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
}

// Example to modify the test after loading
// TEST_F(JsonDefinedEnergyManagerTest, json_based_test_01) {
//     load_test(std::string(JSON_TESTS_LOCATION) + "/1_0_two_evse_load_balancing.json");

//     // Do here any modifications to the test
//     this->request;
//     this->expected_result;

//     run_test();
// }

} // namespace module
