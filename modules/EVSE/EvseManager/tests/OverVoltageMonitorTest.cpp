// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>

#include "over_voltage/OverVoltageMonitor.hpp"

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

using namespace module;
using namespace std::chrono_literals;

class OverVoltageMonitorTest : public ::testing::Test {
protected:
    struct CallbackState {
        std::mutex mtx;
        std::condition_variable cv;
        bool called{false};
        OverVoltageMonitor::FaultType type;
        std::string reason;
    };

    static OverVoltageMonitor make_monitor(CallbackState& state, std::chrono::milliseconds duration) {
        return OverVoltageMonitor(
            [&state](OverVoltageMonitor::FaultType type, const std::string& reason) {
                std::lock_guard<std::mutex> lock(state.mtx);
                state.called = true;
                state.type = type;
                state.reason = reason;
                state.cv.notify_all();
            },
            duration);
    }

    static bool wait_for_callback(CallbackState& state, std::chrono::milliseconds timeout = 500ms) {
        std::unique_lock<std::mutex> lock(state.mtx);
        return state.cv.wait_for(lock, timeout, [&state] { return state.called; });
    }
};

TEST_F(OverVoltageMonitorTest, no_fault_below_limits) {
    CallbackState state;
    auto monitor = make_monitor(state, 100ms);

    monitor.set_limits(450.0, 420.0);
    monitor.start_monitor();

    monitor.update_voltage(400.0);
    monitor.update_voltage(410.0);

    EXPECT_FALSE(wait_for_callback(state));
}

TEST_F(OverVoltageMonitorTest, emergency_fault_triggers_immediately) {
    CallbackState state;
    auto monitor = make_monitor(state, 200ms);

    monitor.set_limits(450.0, 420.0);
    monitor.start_monitor();

    monitor.update_voltage(460.0);

    ASSERT_TRUE(wait_for_callback(state));
    EXPECT_EQ(state.type, OverVoltageMonitor::FaultType::Emergency);
}

TEST_F(OverVoltageMonitorTest, error_fault_triggers_after_duration) {
    CallbackState state;
    auto monitor = make_monitor(state, 100ms);

    monitor.set_limits(450.0, 420.0);
    monitor.start_monitor();

    monitor.update_voltage(430.0);

    ASSERT_TRUE(wait_for_callback(state, 300ms));
    EXPECT_EQ(state.type, OverVoltageMonitor::FaultType::Error);
}

TEST_F(OverVoltageMonitorTest, voltage_drop_cancels_error_timer) {
    CallbackState state;
    auto monitor = make_monitor(state, 150ms);

    monitor.set_limits(450.0, 420.0);
    monitor.start_monitor();

    monitor.update_voltage(430.0); // above error limit
    std::this_thread::sleep_for(50ms);
    monitor.update_voltage(410.0); // below error limit

    EXPECT_FALSE(wait_for_callback(state, 300ms));
}

TEST_F(OverVoltageMonitorTest, zero_duration_triggers_immediately) {
    CallbackState state;
    auto monitor = make_monitor(state, 0ms);

    monitor.set_limits(450.0, 420.0);
    monitor.start_monitor();

    monitor.update_voltage(425.0);

    ASSERT_TRUE(wait_for_callback(state));
    EXPECT_EQ(state.type, OverVoltageMonitor::FaultType::Error);
}

TEST_F(OverVoltageMonitorTest, fault_is_latched) {
    CallbackState state;
    auto monitor = make_monitor(state, 50ms);

    monitor.set_limits(450.0, 420.0);
    monitor.start_monitor();

    monitor.update_voltage(430.0);
    ASSERT_TRUE(wait_for_callback(state));

    {
        std::lock_guard<std::mutex> lock(state.mtx);
        state.called = false;
    }

    // Further voltage updates should not retrigger
    monitor.update_voltage(460.0);
    EXPECT_FALSE(wait_for_callback(state, 200ms));
}

TEST_F(OverVoltageMonitorTest, stop_monitor_suppresses_fault) {
    CallbackState state;
    auto monitor = make_monitor(state, 100ms);

    monitor.set_limits(450.0, 420.0);
    monitor.start_monitor();

    monitor.update_voltage(430.0);
    monitor.stop_monitor();

    EXPECT_FALSE(wait_for_callback(state, 300ms));
}

TEST_F(OverVoltageMonitorTest, reset_clears_latched_fault_and_allows_retrigger) {
    CallbackState state;
    auto monitor = make_monitor(state, 50ms);

    monitor.set_limits(450.0, 420.0);
    monitor.start_monitor();

    // First fault
    monitor.update_voltage(430.0);
    ASSERT_TRUE(wait_for_callback(state, 300ms));
    EXPECT_EQ(state.type, OverVoltageMonitor::FaultType::Error);

    // Clear callback state
    {
        std::lock_guard<std::mutex> lock(state.mtx);
        state.called = false;
    }

    // Reset should clear latch and timers
    monitor.reset();

    // Must restart monitoring explicitly
    monitor.start_monitor();

    // Second fault should be possible again
    monitor.update_voltage(430.0);
    ASSERT_TRUE(wait_for_callback(state, 300ms));
    EXPECT_EQ(state.type, OverVoltageMonitor::FaultType::Error);
}

TEST_F(OverVoltageMonitorTest, multiple_voltage_updates_update_timer_snapshot) {
    CallbackState state;
    auto monitor = make_monitor(state, 100ms);

    monitor.set_limits(500.0, 420.0);
    monitor.start_monitor();

    // First update arms the timer
    monitor.update_voltage(430.0);

    // Subsequent updates while timer is active
    monitor.update_voltage(435.0);
    monitor.update_voltage(432.0);
    monitor.update_voltage(440.0); // highest value

    ASSERT_TRUE(wait_for_callback(state, 300ms));
    EXPECT_EQ(state.type, OverVoltageMonitor::FaultType::Error);
}