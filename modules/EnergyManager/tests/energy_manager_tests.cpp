// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "JsonDefinedEnergyManagerTest.hpp"

namespace module {

// Register all json tests in the JSON_TESTS_LOCATION directory
void register_json_tests() {
    const std::filesystem::path json_tests{std::string(JSON_TESTS_LOCATION)};

    for (auto const& test_file : std::filesystem::directory_iterator{json_tests}) {
        if (test_file.is_regular_file()) {
            ::testing::RegisterTest("JsonDefinedEnergyManagerTest", test_file.path().stem().string().c_str(), nullptr,
                                    nullptr, __FILE__, __LINE__, [=]() -> JsonDefinedEnergyManagerTest* {
                                        return new JsonDefinedEnergyManagerTest(test_file.path());
                                    });
        }
    }
}

} // namespace module

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    // Add the JSON tests programmatically
    module::register_json_tests();

    return RUN_ALL_TESTS();
}
