// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>
#include <thread>

#include "temperature_monitor.hpp"

using module::main::TemperatureMonitor;

namespace {

TemperatureMonitor make_monitor(double warning_level_C, double error_level_C, double hysteresis_K,
                                int min_time_as_valid_ms) {
    return TemperatureMonitor(TemperatureMonitor::Config{warning_level_C, error_level_C, hysteresis_K,
                                                         std::chrono::milliseconds(min_time_as_valid_ms)});
}

} // namespace

TEST(TemperatureMonitorTest, NoEventsBelowThresholds) {
    auto monitor = make_monitor(/*warning*/ 50.0, /*error*/ 60.0, /*hyst*/ 3.0, /*min_time_ms*/ 0);

    auto events = monitor.update(40.0, 41.0);

    EXPECT_FALSE(events.warning_raised);
    EXPECT_FALSE(events.warning_cleared);
    EXPECT_FALSE(events.error_raised);
    EXPECT_FALSE(events.error_cleared);
}

TEST(TemperatureMonitorTest, WarningRaisedImmediatelyWhenAboveWarningAndZeroMinTime) {
    auto monitor = make_monitor(/*warning*/ 50.0, /*error*/ 60.0, /*hyst*/ 3.0, /*min_time_ms*/ 0);

    auto events = monitor.update(51.0, 49.0); // max = 51 > 50

    EXPECT_TRUE(events.warning_raised);
    EXPECT_FALSE(events.warning_cleared);
    EXPECT_FALSE(events.error_raised);
    EXPECT_FALSE(events.error_cleared);
}

TEST(TemperatureMonitorTest, ErrorRaisedImmediatelyWhenAboveErrorAndZeroMinTime) {
    auto monitor = make_monitor(/*warning*/ 50.0, /*error*/ 60.0, /*hyst*/ 3.0, /*min_time_ms*/ 0);

    auto events = monitor.update(61.0, 59.0); // max = 61 > 60 (also > 50 warning threshold)

    // When temperature exceeds error threshold, it also exceeds warning threshold,
    // so both warning and error should be raised
    EXPECT_TRUE(events.warning_raised);
    EXPECT_FALSE(events.warning_cleared);
    EXPECT_TRUE(events.error_raised);
    EXPECT_FALSE(events.error_cleared);
}

TEST(TemperatureMonitorTest, WarningClearsWithHysteresis) {
    auto monitor = make_monitor(/*warning*/ 50.0, /*error*/ 60.0, /*hyst*/ 3.0, /*min_time_ms*/ 0);

    // Raise warning
    auto events = monitor.update(52.0, 51.0);
    EXPECT_TRUE(events.warning_raised);
    EXPECT_FALSE(events.warning_cleared);

    // Still above warning - hysteresis threshold, no clear
    events = monitor.update(49.0, 48.5); // max = 49, threshold - hyst = 47
    EXPECT_FALSE(events.warning_raised);
    EXPECT_FALSE(events.warning_cleared);

    // Drop below warning - hysteresis threshold, should clear
    events = monitor.update(46.0, 45.0); // max = 46 < 47
    EXPECT_FALSE(events.warning_raised);
    EXPECT_TRUE(events.warning_cleared);
}

TEST(TemperatureMonitorTest, ErrorClearsWithHysteresis) {
    auto monitor = make_monitor(/*warning*/ 50.0, /*error*/ 60.0, /*hyst*/ 5.0, /*min_time_ms*/ 0);

    // Raise error
    auto events = monitor.update(61.0, 62.0);
    EXPECT_TRUE(events.error_raised);
    EXPECT_FALSE(events.error_cleared);

    // Still above error - hysteresis threshold, no clear
    events = monitor.update(56.0, 55.0); // error threshold - hyst = 55
    EXPECT_FALSE(events.error_raised);
    EXPECT_FALSE(events.error_cleared);

    // Drop below error - hysteresis threshold, should clear
    events = monitor.update(54.0, 53.0); // max = 54 < 55
    EXPECT_FALSE(events.error_raised);
    EXPECT_TRUE(events.error_cleared);
}

TEST(TemperatureMonitorTest, WarningNotRaisedBeforeMinTimeElapses) {
    constexpr int min_time_ms = 100;
    auto monitor = make_monitor(/*warning*/ 50.0, /*error*/ 60.0, /*hyst*/ 3.0, min_time_ms);

    // First update: temperature exceeds threshold, but min_time hasn't elapsed
    auto events = monitor.update(51.0, 50.5); // max = 51 > 50
    EXPECT_FALSE(events.warning_raised);
    EXPECT_FALSE(events.warning_cleared);

    // Wait less than min_time - still no warning
    std::this_thread::sleep_for(std::chrono::milliseconds(min_time_ms / 2));
    events = monitor.update(51.0, 50.5);
    EXPECT_FALSE(events.warning_raised);
    EXPECT_FALSE(events.warning_cleared);
}

TEST(TemperatureMonitorTest, WarningRaisedAfterMinTimeElapses) {
    constexpr int min_time_ms = 100;
    auto monitor = make_monitor(/*warning*/ 50.0, /*error*/ 60.0, /*hyst*/ 3.0, min_time_ms);

    // First update: temperature exceeds threshold, starts timer
    auto events = monitor.update(51.0, 50.5); // max = 51 > 50
    EXPECT_FALSE(events.warning_raised);

    // Wait for min_time to elapse
    std::this_thread::sleep_for(std::chrono::milliseconds(min_time_ms + 10)); // Add small buffer for test timing

    // Next update after min_time elapsed should raise warning
    events = monitor.update(51.0, 50.5);
    EXPECT_TRUE(events.warning_raised);
    EXPECT_FALSE(events.warning_cleared);
}

TEST(TemperatureMonitorTest, TimerResetsWhenTemperatureDropsBelowThreshold) {
    constexpr int min_time_ms = 100;
    auto monitor = make_monitor(/*warning*/ 50.0, /*error*/ 60.0, /*hyst*/ 3.0, min_time_ms);

    // Start timing: temperature exceeds threshold
    auto events = monitor.update(51.0, 50.5); // max = 51 > 50
    EXPECT_FALSE(events.warning_raised);

    // Wait part of the min_time
    std::this_thread::sleep_for(std::chrono::milliseconds(min_time_ms / 2));

    // Temperature drops below threshold - timer should reset
    events = monitor.update(49.0, 48.0); // max = 49 < 50
    EXPECT_FALSE(events.warning_raised);
    EXPECT_FALSE(events.warning_cleared);

    // Wait again for min_time - but timer was reset, so we need to start over
    std::this_thread::sleep_for(std::chrono::milliseconds(min_time_ms / 2));
    events = monitor.update(49.0, 48.0);
    EXPECT_FALSE(events.warning_raised); // Still below threshold

    // Now exceed threshold again and wait full min_time
    events = monitor.update(51.0, 50.5);
    EXPECT_FALSE(events.warning_raised);
    std::this_thread::sleep_for(std::chrono::milliseconds(min_time_ms + 10));
    events = monitor.update(51.0, 50.5);
    EXPECT_TRUE(events.warning_raised); // Now should be raised
}

TEST(TemperatureMonitorTest, ErrorNotRaisedBeforeMinTimeElapses) {
    constexpr int min_time_ms = 100;
    auto monitor = make_monitor(/*warning*/ 50.0, /*error*/ 60.0, /*hyst*/ 3.0, min_time_ms);

    // Temperature exceeds error threshold
    auto events = monitor.update(61.0, 60.5); // max = 61 > 60
    EXPECT_FALSE(events.error_raised);
    EXPECT_FALSE(events.warning_raised); // Also above warning, but min_time applies

    // Wait less than min_time
    std::this_thread::sleep_for(std::chrono::milliseconds(min_time_ms / 2));
    events = monitor.update(61.0, 60.5);
    EXPECT_FALSE(events.error_raised);
    EXPECT_FALSE(events.warning_raised);
}

TEST(TemperatureMonitorTest, ErrorRaisedAfterMinTimeElapses) {
    constexpr int min_time_ms = 100;
    auto monitor = make_monitor(/*warning*/ 50.0, /*error*/ 60.0, /*hyst*/ 3.0, min_time_ms);

    // Start timing: temperature exceeds error threshold
    auto events = monitor.update(61.0, 60.5); // max = 61 > 60
    EXPECT_FALSE(events.error_raised);
    EXPECT_FALSE(events.warning_raised);

    // Wait for min_time to elapse
    std::this_thread::sleep_for(std::chrono::milliseconds(min_time_ms + 10));

    // Next update after min_time elapsed should raise both warning and error
    events = monitor.update(61.0, 60.5);
    EXPECT_TRUE(events.error_raised);
    EXPECT_TRUE(events.warning_raised); // Also above warning threshold
    EXPECT_FALSE(events.error_cleared);
    EXPECT_FALSE(events.warning_cleared);
}

TEST(TemperatureMonitorTest, MultipleUpdatesAccumulateTimeCorrectly) {
    constexpr int min_time_ms = 100;
    auto monitor = make_monitor(/*warning*/ 50.0, /*error*/ 60.0, /*hyst*/ 3.0, min_time_ms);

    // First update starts timer
    auto events = monitor.update(51.0, 50.5);
    EXPECT_FALSE(events.warning_raised);

    // Multiple updates while above threshold should accumulate time
    std::this_thread::sleep_for(std::chrono::milliseconds(min_time_ms / 3));
    events = monitor.update(51.0, 50.5);
    EXPECT_FALSE(events.warning_raised);

    std::this_thread::sleep_for(std::chrono::milliseconds(min_time_ms / 3));
    events = monitor.update(51.0, 50.5);
    EXPECT_FALSE(events.warning_raised);

    // Final update after total time >= min_time should raise warning
    std::this_thread::sleep_for(std::chrono::milliseconds(min_time_ms / 3 + 10));
    events = monitor.update(51.0, 50.5);
    EXPECT_TRUE(events.warning_raised);
}
