// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "JsonDefinedEnergyManagerTest.hpp"

static const std::string source1 = "SOURCE1";
static const std::string source2 = "SOURCE2";

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

TEST(FreeFunctionTests, GetWattFreqTable) {
    std::vector<types::energy::FrequencyWattPoint> table{{49., -7000}, {50., 0}, {51., 7000}};

    EXPECT_EQ(module::get_watt_from_freq_table(table, 48.), -7000.);
    EXPECT_EQ(module::get_watt_from_freq_table(table, 49.), -7000.);
    EXPECT_EQ(module::get_watt_from_freq_table(table, 49.5), -3500.);
    EXPECT_EQ(module::get_watt_from_freq_table(table, 50.), 0.);
    EXPECT_EQ(module::get_watt_from_freq_table(table, 50.5), 3500.);
    EXPECT_EQ(module::get_watt_from_freq_table(table, 51.), 7000.);
    EXPECT_EQ(module::get_watt_from_freq_table(table, 52.), 7000.);
}

TEST(FreeFunctionTests, ApplyLimitIfSmaller) {
    std::optional<types::energy::NumberWithSource> base;
    types::energy::NumberWithSource result;
    const std::string source = "SOURCE";

    // Base has no value yet
    module::apply_limit_if_smaller(base, 20., source);
    EXPECT_TRUE(base.has_value());
    EXPECT_EQ(base.value().value, 20.);
    EXPECT_EQ(base.value().source, source);

    // Now base has a value, test if a bigger limit does not apply
    module::apply_limit_if_smaller(base, 21., source);
    EXPECT_TRUE(base.has_value());
    EXPECT_EQ(base.value().value, 20.);
    EXPECT_EQ(base.value().source, source);

    // Now base has a value, test if a smaller limit does apply
    module::apply_limit_if_smaller(base, 19., source);
    EXPECT_TRUE(base.has_value());
    EXPECT_EQ(base.value().value, 19.);
    EXPECT_EQ(base.value().source, source);
}

TEST(FreeFunctionTests, ApplySetpoints) {
    module::ScheduleReq imp;
    module::ScheduleReq::value_type v;

    // At first it is mostly empty
    v.timestamp = "2024-12-17T13:08:46.479Z";
    imp.push_back(v);
    // Add another entry with an ampere limit
    v.timestamp = "2024-12-17T13:08:47.479Z";
    v.limits_to_root.ac_max_current_A = {13.0, source1};
    imp.push_back(v);
    // Add another entry with an additional watt limit
    v.timestamp = "2024-12-17T13:08:48.479Z";
    v.limits_to_root.total_power_W = {2200.0, source2};
    imp.push_back(v);

    module::ScheduleReq exp{imp};

    module::ScheduleReq imp_orig{imp};
    module::ScheduleReq exp_orig{imp};

    module::ScheduleSetpoints setpoints;
    module::ScheduleSetpoints::value_type s;
    // At first no setpoint is actually set
    s.timestamp = "2024-12-17T13:08:46.479Z";
    setpoints.push_back(s);
    s.timestamp = "2024-12-17T13:08:47.479Z";
    setpoints.push_back(s);
    s.timestamp = "2024-12-17T13:08:48.479Z";
    setpoints.push_back(s);

    module::apply_setpoints(imp, exp, setpoints, {});

    EXPECT_FALSE(imp[0].limits_to_root.ac_max_current_A.has_value());
    EXPECT_FALSE(imp[0].limits_to_root.total_power_W.has_value());

    EXPECT_TRUE(imp[1].limits_to_root.ac_max_current_A.has_value());
    EXPECT_FALSE(imp[1].limits_to_root.total_power_W.has_value());
    EXPECT_EQ(imp[1].limits_to_root.ac_max_current_A.value().value, 13.0);
    EXPECT_EQ(imp[1].limits_to_root.ac_max_current_A.value().source, source1);

    EXPECT_TRUE(imp[2].limits_to_root.ac_max_current_A.has_value());
    EXPECT_TRUE(imp[2].limits_to_root.total_power_W.has_value());
    EXPECT_EQ(imp[2].limits_to_root.ac_max_current_A.value().value, 13.0);
    EXPECT_EQ(imp[2].limits_to_root.ac_max_current_A.value().source, source1);
    EXPECT_EQ(imp[2].limits_to_root.total_power_W.value().value, 2200.);
    EXPECT_EQ(imp[2].limits_to_root.total_power_W.value().source, source2);

    setpoints.clear();
    types::energy::SetpointType sp;
    sp.ac_current_A = 8.0;
    sp.source = "SOURCESETPOINT";
    s.setpoint = sp;
    s.timestamp = "2024-12-17T13:08:46.479Z";
    setpoints.push_back(s);
    s.timestamp = "2024-12-17T13:08:47.479Z";
    setpoints.push_back(s);
    s.timestamp = "2024-12-17T13:08:48.479Z";
    setpoints.push_back(s);

    module::apply_setpoints(imp, exp, setpoints, {});

    EXPECT_TRUE(imp[0].limits_to_root.ac_max_current_A.has_value());
    EXPECT_EQ(imp[0].limits_to_root.ac_max_current_A.value().value, 8.0);
    EXPECT_EQ(imp[0].limits_to_root.ac_max_current_A.value().source, "SOURCESETPOINT");
    EXPECT_FALSE(imp[0].limits_to_root.total_power_W.has_value());

    EXPECT_TRUE(imp[1].limits_to_root.ac_max_current_A.has_value());
    EXPECT_EQ(imp[1].limits_to_root.ac_max_current_A.value().value, 8.0);
    EXPECT_EQ(imp[1].limits_to_root.ac_max_current_A.value().source, "SOURCESETPOINT");
    EXPECT_FALSE(imp[1].limits_to_root.total_power_W.has_value());

    EXPECT_TRUE(imp[2].limits_to_root.ac_max_current_A.has_value());
    EXPECT_EQ(imp[2].limits_to_root.ac_max_current_A.value().value, 8.0);
    EXPECT_EQ(imp[2].limits_to_root.ac_max_current_A.value().source, "SOURCESETPOINT");
    EXPECT_TRUE(imp[2].limits_to_root.total_power_W.has_value());
    EXPECT_EQ(imp[2].limits_to_root.total_power_W.value().value, 2200);
    EXPECT_EQ(imp[2].limits_to_root.total_power_W.value().source, source2);

    EXPECT_TRUE(exp[0].limits_to_root.ac_max_current_A.has_value());
    EXPECT_EQ(exp[0].limits_to_root.ac_max_current_A.value().value, 0.0);
    EXPECT_EQ(exp[0].limits_to_root.ac_max_current_A.value().source, "SOURCESETPOINT");
    EXPECT_FALSE(exp[0].limits_to_root.total_power_W.has_value());

    EXPECT_TRUE(exp[1].limits_to_root.ac_max_current_A.has_value());
    EXPECT_EQ(exp[1].limits_to_root.ac_max_current_A.value().value, 0.0);
    EXPECT_EQ(exp[1].limits_to_root.ac_max_current_A.value().source, "SOURCESETPOINT");
    EXPECT_FALSE(exp[1].limits_to_root.total_power_W.has_value());

    EXPECT_TRUE(exp[2].limits_to_root.ac_max_current_A.has_value());
    EXPECT_EQ(exp[2].limits_to_root.ac_max_current_A.value().value, 0.0);
    EXPECT_EQ(exp[2].limits_to_root.ac_max_current_A.value().source, "SOURCESETPOINT");
    EXPECT_TRUE(exp[2].limits_to_root.total_power_W.has_value());
    EXPECT_EQ(exp[2].limits_to_root.total_power_W.value().value, 2200);
    EXPECT_EQ(exp[2].limits_to_root.total_power_W.value().source, source2);

    imp = imp_orig;
    exp = exp_orig;
    setpoints.clear();
    sp.ac_current_A = 14.0;
    sp.source = "SOURCESETPOINT";
    s.setpoint = sp;
    s.timestamp = "2024-12-17T13:08:46.479Z";
    setpoints.push_back(s);
    s.timestamp = "2024-12-17T13:08:47.479Z";
    setpoints.push_back(s);
    s.timestamp = "2024-12-17T13:08:48.479Z";
    setpoints.push_back(s);

    module::apply_setpoints(imp, exp, setpoints, {});

    EXPECT_TRUE(imp[0].limits_to_root.ac_max_current_A.has_value());
    EXPECT_EQ(imp[0].limits_to_root.ac_max_current_A.value().value, 14.0);
    EXPECT_EQ(imp[0].limits_to_root.ac_max_current_A.value().source, "SOURCESETPOINT");
    EXPECT_FALSE(imp[0].limits_to_root.total_power_W.has_value());

    EXPECT_TRUE(imp[1].limits_to_root.ac_max_current_A.has_value());
    EXPECT_EQ(imp[1].limits_to_root.ac_max_current_A.value().value, 13.0);
    EXPECT_EQ(imp[1].limits_to_root.ac_max_current_A.value().source, source1);
    EXPECT_FALSE(imp[1].limits_to_root.total_power_W.has_value());

    EXPECT_TRUE(imp[2].limits_to_root.ac_max_current_A.has_value());
    EXPECT_EQ(imp[2].limits_to_root.ac_max_current_A.value().value, 13.0);
    EXPECT_EQ(imp[2].limits_to_root.ac_max_current_A.value().source, source1);
    EXPECT_TRUE(imp[2].limits_to_root.total_power_W.has_value());
    EXPECT_EQ(imp[2].limits_to_root.total_power_W.value().value, 2200);
    EXPECT_EQ(imp[2].limits_to_root.total_power_W.value().source, source2);

    EXPECT_TRUE(exp[0].limits_to_root.ac_max_current_A.has_value());
    EXPECT_EQ(exp[0].limits_to_root.ac_max_current_A.value().value, 0.0);
    EXPECT_EQ(exp[0].limits_to_root.ac_max_current_A.value().source, "SOURCESETPOINT");
    EXPECT_FALSE(exp[0].limits_to_root.total_power_W.has_value());

    EXPECT_TRUE(exp[1].limits_to_root.ac_max_current_A.has_value());
    EXPECT_EQ(exp[1].limits_to_root.ac_max_current_A.value().value, 0.0);
    EXPECT_EQ(exp[1].limits_to_root.ac_max_current_A.value().source, "SOURCESETPOINT");
    EXPECT_FALSE(exp[1].limits_to_root.total_power_W.has_value());

    EXPECT_TRUE(exp[2].limits_to_root.ac_max_current_A.has_value());
    EXPECT_EQ(exp[2].limits_to_root.ac_max_current_A.value().value, 0.0);
    EXPECT_EQ(exp[2].limits_to_root.ac_max_current_A.value().source, "SOURCESETPOINT");
    EXPECT_TRUE(exp[2].limits_to_root.total_power_W.has_value());
    EXPECT_EQ(exp[2].limits_to_root.total_power_W.value().value, 2200);
    EXPECT_EQ(exp[2].limits_to_root.total_power_W.value().source, source2);
}
