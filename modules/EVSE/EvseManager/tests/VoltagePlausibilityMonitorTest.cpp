// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>

#include "voltage_plausibility/VoltagePlausibilityMonitor.hpp"

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>

using namespace module;
using namespace std::chrono_literals;

class VoltagePlausibilityMonitorTest : public ::testing::Test {
protected:
    struct CallbackState {
        std::mutex mtx;
        std::condition_variable cv;
        bool called{false};
        std::string reason;
    };

    static VoltagePlausibilityMonitor make_monitor(CallbackState& state, double max_spread_threshold_V,
                                                   std::chrono::milliseconds fault_duration) {
        return VoltagePlausibilityMonitor(
            [&state](const std::string& reason) {
                std::lock_guard<std::mutex> lock(state.mtx);
                state.called = true;
                state.reason = reason;
                state.cv.notify_all();
            },
            max_spread_threshold_V, fault_duration);
    }

    static bool wait_for_callback(CallbackState& state, std::chrono::milliseconds timeout = 500ms) {
        std::unique_lock<std::mutex> lock(state.mtx);
        return state.cv.wait_for(lock, timeout, [&state] { return state.called; });
    }

    static void clear_callback_state(CallbackState& state) {
        std::lock_guard<std::mutex> lock(state.mtx);
        state.called = false;
        state.reason.clear();
    }

    static void set_two_sources(VoltagePlausibilityMonitor& monitor, double power_supply_V, double powermeter_V) {
        monitor.update_power_supply_voltage(power_supply_V);
        monitor.update_powermeter_voltage(powermeter_V);
    }
};

TEST_F(VoltagePlausibilityMonitorTest, no_fault_below_threshold) {
    CallbackState state;
    auto monitor = make_monitor(state, /*max_spread_threshold_V=*/10.0, /*fault_duration=*/50ms);

    monitor.start_monitor();

    set_two_sources(monitor, 400.0, 405.0); // spread 5 V <= 10 V

    EXPECT_FALSE(wait_for_callback(state, 200ms));

    monitor.stop_monitor();
}

TEST_F(VoltagePlausibilityMonitorTest, requires_at_least_two_sources) {
    CallbackState state;
    auto monitor = make_monitor(state, /*max_spread_threshold_V=*/1.0, /*fault_duration=*/50ms);

    monitor.start_monitor();

    monitor.update_power_supply_voltage(400.0); // only one source available

    EXPECT_FALSE(wait_for_callback(state, 200ms));

    monitor.stop_monitor();
}

TEST_F(VoltagePlausibilityMonitorTest, zero_duration_triggers_immediately) {
    CallbackState state;
    auto monitor = make_monitor(state, /*max_spread_threshold_V=*/5.0, /*fault_duration=*/0ms);

    monitor.start_monitor();

    set_two_sources(monitor, 400.0, 420.0); // spread 20 V > 5 V

    ASSERT_TRUE(wait_for_callback(state, 200ms));
    EXPECT_NE(state.reason.find("fault immediately"), std::string::npos);
}

TEST_F(VoltagePlausibilityMonitorTest, spread_above_threshold_triggers_after_duration) {
    CallbackState state;
    auto monitor = make_monitor(state, /*max_spread_threshold_V=*/5.0, /*fault_duration=*/50ms);

    monitor.start_monitor();

    set_two_sources(monitor, 400.0, 420.0); // arms timer

    ASSERT_TRUE(wait_for_callback(state, 300ms));
    EXPECT_NE(state.reason.find("for at least"), std::string::npos);
    EXPECT_NE(state.reason.find("exceeded threshold"), std::string::npos);
}

TEST_F(VoltagePlausibilityMonitorTest, returning_within_threshold_cancels_timer) {
    CallbackState state;
    auto monitor = make_monitor(state, /*max_spread_threshold_V=*/5.0, /*fault_duration=*/120ms);

    monitor.start_monitor();

    set_two_sources(monitor, 400.0, 420.0); // spread 20 V -> arm timer

    std::this_thread::sleep_for(40ms);

    // Bring spread back within the threshold before the timer expires
    monitor.update_powermeter_voltage(402.0); // spread now 2 V -> cancel timer

    EXPECT_FALSE(wait_for_callback(state, 250ms));
}

TEST_F(VoltagePlausibilityMonitorTest, fault_is_latched_and_not_retriggered) {
    CallbackState state;
    auto monitor = make_monitor(state, /*max_spread_threshold_V=*/1.0, /*fault_duration=*/0ms);

    monitor.start_monitor();

    set_two_sources(monitor, 400.0, 405.0); // immediate fault

    ASSERT_TRUE(wait_for_callback(state, 200ms));

    clear_callback_state(state);

    // After a fault the monitor stops and latches the fault; further updates must not retrigger.
    set_two_sources(monitor, 410.0, 430.0);
    EXPECT_FALSE(wait_for_callback(state, 150ms));
}

TEST_F(VoltagePlausibilityMonitorTest, stop_monitor_suppresses_fault) {
    CallbackState state;
    auto monitor = make_monitor(state, /*max_spread_threshold_V=*/5.0, /*fault_duration=*/80ms);

    monitor.start_monitor();

    set_two_sources(monitor, 400.0, 420.0); // arm timer
    monitor.stop_monitor();                 // cancels timer

    EXPECT_FALSE(wait_for_callback(state, 250ms));
}

TEST_F(VoltagePlausibilityMonitorTest, reset_clears_samples_so_old_values_are_not_used) {
    CallbackState state;
    auto monitor = make_monitor(state, /*max_spread_threshold_V=*/5.0, /*fault_duration=*/80ms);

    monitor.start_monitor();

    // Prime both sources with a large spread, then stop and reset.
    set_two_sources(monitor, 400.0, 420.0);
    monitor.stop_monitor();
    monitor.reset();

    // Restart monitoring. Provide only a single new source sample.
    // If reset() didn't clear the old powermeter sample timestamp, we'd still have 2 sources and might arm/fault.
    monitor.start_monitor();
    monitor.update_power_supply_voltage(400.0);

    EXPECT_FALSE(wait_for_callback(state, 250ms));
}
