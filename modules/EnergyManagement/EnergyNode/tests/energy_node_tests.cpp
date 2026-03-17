// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <generated/types/energy.hpp>

#include <gtest/gtest.h>

// Include the utility header
#include "../energy_grid/energy_schedule_utils.hpp"

// Helper function to create test schedule entries
static types::energy::ScheduleReqEntry create_test_entry(double total_power = 0.0,
                                                         std::optional<int> root_phase_count = std::nullopt,
                                                         std::optional<int> leaves_phase_count = std::nullopt) {
    types::energy::ScheduleReqEntry entry;
    entry.timestamp = "2024-01-01T00:00:00Z"; // Required field

    if (total_power > 0) {
        entry.limits_to_root.total_power_W =
            types::energy::NumberWithSource{static_cast<float>(total_power), "test_source"};
        entry.limits_to_leaves.total_power_W =
            types::energy::NumberWithSource{static_cast<float>(total_power), "test_source"};
    }

    if (root_phase_count.has_value()) {
        entry.limits_to_root.ac_max_phase_count =
            types::energy::IntegerWithSource{root_phase_count.value(), "test_source"};
    }

    if (leaves_phase_count.has_value()) {
        entry.limits_to_leaves.ac_max_phase_count =
            types::energy::IntegerWithSource{leaves_phase_count.value(), "test_source"};
    }

    return entry;
}

class EnergyNodeTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    void TearDown() override {
    }

    void process_schedule_with_limits(std::vector<types::energy::ScheduleReqEntry>& schedule,
                                      double fuse_limit_A = 32.0, int phase_count = 3, double nominal_voltage_V = 230.0,
                                      bool enhance_with_current_limits = false) {
        // Call the actual implementation function
        module::energy_grid::process_schedule_with_limits(schedule, "test_source", fuse_limit_A, phase_count,
                                                          nominal_voltage_V, enhance_with_current_limits);
    }
};

/// Test enhancement feature disabled by default
TEST_F(EnergyNodeTest, TestEnhancementDisabledByDefault) {
    auto entry = create_test_entry(7000.0); // 7kW total power
    std::vector<types::energy::ScheduleReqEntry> schedule = {entry};

    process_schedule_with_limits(schedule);

    // Enhancement should not be applied when disabled (no current limits from power calculation)
    EXPECT_FALSE(schedule[0].limits_to_leaves.ac_max_current_A.has_value());

    // But fuse limits should still be applied to limits_to_root
    EXPECT_TRUE(schedule[0].limits_to_root.ac_max_current_A.has_value());
    EXPECT_EQ(schedule[0].limits_to_root.ac_max_current_A->value, 32.0f);
}

/// Test enhancement feature when enabled
TEST_F(EnergyNodeTest, TestEnhancementEnabled) {
    auto entry = create_test_entry(7000.0); // 7kW total power
    std::vector<types::energy::ScheduleReqEntry> schedule = {entry};

    process_schedule_with_limits(schedule, 32.0, 3, 230.0, true);

    // Calculate expected current: 7000W / (230V * 3 phases) = 10.25A
    float expected_current = 7000.0f / (230.0f * 3.0f);

    EXPECT_TRUE(schedule[0].limits_to_root.ac_max_current_A.has_value());
    EXPECT_NEAR(schedule[0].limits_to_root.ac_max_current_A->value, expected_current, 0.01f);

    // limits_to_leaves should not be modified (matching fuse limit behavior)
    EXPECT_FALSE(schedule[0].limits_to_leaves.ac_max_current_A.has_value());
}

/// Test phase count priority: schedule entry over module config
TEST_F(EnergyNodeTest, TestPhaseCountPriority) {
    // Create entry with 1 phase specified
    auto entry = create_test_entry(7000.0, 1, 1); // 1 phase in schedule entry
    std::vector<types::energy::ScheduleReqEntry> schedule = {entry};

    process_schedule_with_limits(schedule, 32.0, 3, 230.0, true);

    // Should use 1 phase from schedule entry, not 3 from module config
    // 7000W / (230V * 1 phase) = 30.43A
    float expected_current = 7000.0f / (230.0f * 1.0f);

    EXPECT_TRUE(schedule[0].limits_to_root.ac_max_current_A.has_value());
    EXPECT_NEAR(schedule[0].limits_to_root.ac_max_current_A->value, expected_current, 0.01f);
}

/// Test fallback to module config when schedule has no phase count
TEST_F(EnergyNodeTest, TestPhaseCountFallback) {
    // Create entry without phase count
    auto entry = create_test_entry(7000.0); // No phase count specified
    std::vector<types::energy::ScheduleReqEntry> schedule = {entry};

    process_schedule_with_limits(schedule, 32.0, 2, 230.0, true);

    // Should use 2 phases from module config
    // 7000W / (230V * 2 phases) = 15.22A
    float expected_current = 7000.0f / (230.0f * 2.0f);

    EXPECT_TRUE(schedule[0].limits_to_root.ac_max_current_A.has_value());
    EXPECT_NEAR(schedule[0].limits_to_root.ac_max_current_A->value, expected_current, 0.01f);
}

/// Test default to 1 phase when no phase count available
TEST_F(EnergyNodeTest, TestPhaseCountDefault) {
    // Create entry without phase count
    auto entry = create_test_entry(7000.0); // No phase count specified
    std::vector<types::energy::ScheduleReqEntry> schedule = {entry};

    process_schedule_with_limits(schedule, 32.0, 0, 230.0, true);

    // Should default to 1 phase
    // 7000W / (230V * 1 phase) = 30.43A
    float expected_current = 7000.0f / (230.0f * 1.0f);

    EXPECT_TRUE(schedule[0].limits_to_root.ac_max_current_A.has_value());
    EXPECT_NEAR(schedule[0].limits_to_root.ac_max_current_A->value, expected_current, 0.01f);
}

/// Test that existing current limits are not overwritten
TEST_F(EnergyNodeTest, TestPreserveExistingCurrentLimits) {
    auto entry = create_test_entry(7000.0); // 7kW total power
    entry.limits_to_root.ac_max_current_A =
        types::energy::NumberWithSource{25.0f, "existing_source"}; // Pre-existing current limit
    entry.limits_to_leaves.ac_max_current_A = types::energy::NumberWithSource{20.0f, "existing_source"};

    std::vector<types::energy::ScheduleReqEntry> schedule = {entry};

    process_schedule_with_limits(schedule, 32.0, 3, 230.0, true);

    // Existing current limits should be preserved
    EXPECT_TRUE(schedule[0].limits_to_root.ac_max_current_A.has_value());
    EXPECT_EQ(schedule[0].limits_to_root.ac_max_current_A->value, 25.0f);

    EXPECT_TRUE(schedule[0].limits_to_leaves.ac_max_current_A.has_value());
    EXPECT_EQ(schedule[0].limits_to_leaves.ac_max_current_A->value, 20.0f);
}

/// Test fuse limit still applies when enhancement is enabled
TEST_F(EnergyNodeTest, TestFuseLimitWithEnhancement) {
    auto entry = create_test_entry(10000.0); // 10kW would calculate to ~14.5A with 3 phases
    std::vector<types::energy::ScheduleReqEntry> schedule = {entry};

    process_schedule_with_limits(schedule, 10.0, 3, 230.0, true);

    // Calculated current would be ~14.5A, but fuse limit of 10A should be applied
    EXPECT_TRUE(schedule[0].limits_to_root.ac_max_current_A.has_value());
    EXPECT_EQ(schedule[0].limits_to_root.ac_max_current_A->value, 10.0f);
}

/// Test multiple schedule entries
TEST_F(EnergyNodeTest, TestMultipleEntries) {
    auto entry1 = create_test_entry(3000.0, 1); // 3kW, 1 phase
    auto entry2 = create_test_entry(6000.0, 2); // 6kW, 2 phases
    auto entry3 = create_test_entry(9000.0);    // 9kW, use module config (3 phases)

    std::vector<types::energy::ScheduleReqEntry> schedule = {entry1, entry2, entry3};

    process_schedule_with_limits(schedule, 32.0, 3, 230.0, true);

    // Entry 1: 3000W / (230V * 1) = 13.04A
    EXPECT_TRUE(schedule[0].limits_to_root.ac_max_current_A.has_value());
    EXPECT_NEAR(schedule[0].limits_to_root.ac_max_current_A->value, 3000.0f / 230.0f, 0.01f);

    // Entry 2: 6000W / (230V * 2) = 13.04A
    EXPECT_TRUE(schedule[1].limits_to_root.ac_max_current_A.has_value());
    EXPECT_NEAR(schedule[1].limits_to_root.ac_max_current_A->value, 6000.0f / (230.0f * 2.0f), 0.01f);

    // Entry 3: 9000W / (230V * 3) = 13.04A
    EXPECT_TRUE(schedule[2].limits_to_root.ac_max_current_A.has_value());
    EXPECT_NEAR(schedule[2].limits_to_root.ac_max_current_A->value, 9000.0f / (230.0f * 3.0f), 0.01f);
}

/// Test configurable voltage - 120V (US standard)
TEST_F(EnergyNodeTest, TestConfigurableVoltage120V) {
    auto entry = create_test_entry(7000.0); // 7kW total power
    std::vector<types::energy::ScheduleReqEntry> schedule = {entry};

    process_schedule_with_limits(schedule, 32.0, 3, 120.0, true);

    // Calculate expected current: 7000W / (120V * 3 phases) = 19.44A
    float expected_current = 7000.0f / (120.0f * 3.0f);

    EXPECT_TRUE(schedule[0].limits_to_root.ac_max_current_A.has_value());
    EXPECT_NEAR(schedule[0].limits_to_root.ac_max_current_A->value, expected_current, 0.01f);
}

/// Test configurable voltage - 400V (3-phase industrial)
TEST_F(EnergyNodeTest, TestConfigurableVoltage400V) {
    auto entry = create_test_entry(22000.0); // 22kW total power
    std::vector<types::energy::ScheduleReqEntry> schedule = {entry};

    process_schedule_with_limits(schedule, 32.0, 3, 400.0, true);

    // Calculate expected current: 22000W / (400V * 3 phases) = 18.33A
    float expected_current = 22000.0f / (400.0f * 3.0f);

    EXPECT_TRUE(schedule[0].limits_to_root.ac_max_current_A.has_value());
    EXPECT_NEAR(schedule[0].limits_to_root.ac_max_current_A->value, expected_current, 0.01f);
}

/// Test configurable voltage - 240V (US split-phase)
TEST_F(EnergyNodeTest, TestConfigurableVoltage240V) {
    auto entry = create_test_entry(19200.0); // 19.2kW total power
    std::vector<types::energy::ScheduleReqEntry> schedule = {entry};

    process_schedule_with_limits(schedule, 40.0, 2, 240.0, true);

    // Calculate expected current: 19200W / (240V * 2 phases) = 40A
    float expected_current = 19200.0f / (240.0f * 2.0f);

    EXPECT_TRUE(schedule[0].limits_to_root.ac_max_current_A.has_value());
    EXPECT_EQ(schedule[0].limits_to_root.ac_max_current_A->value, expected_current);
}
