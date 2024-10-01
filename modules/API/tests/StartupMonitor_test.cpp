// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <gtest/gtest.h>

#include "StartupMonitor.hpp"

#include <thread>

namespace {
using namespace module;

struct StartupMonitorTest : public StartupMonitor {
    [[nodiscard]] constexpr bool startup_complete() const {
        return managers_ready;
    }
    [[nodiscard]] constexpr std::uint8_t total() const {
        return n_managers;
    }
    [[nodiscard]] inline std::uint8_t startup_count() const {
        return (ready_set) ? ready_set->size() : 0;
    }
};

TEST(StartupMonitor, init) {
    StartupMonitorTest startup;
    EXPECT_FALSE(startup.startup_complete());
    EXPECT_EQ(startup.startup_count(), 0);
    EXPECT_EQ(startup.total(), 0);

    bool woken{false};
    std::thread thread([&startup, &woken]() {
        startup.wait_ready();
        woken = true;
    });

    EXPECT_FALSE(woken);

    EXPECT_TRUE(startup.set_total(1));
    EXPECT_EQ(startup.total(), 1);
    EXPECT_FALSE(woken);
    EXPECT_TRUE(startup.notify_ready("manager1"));
    // EXPECT_EQ(startup.startup_count(), 1); will be 0 because startup is complete
    thread.join();
    EXPECT_TRUE(woken);
    EXPECT_TRUE(startup.startup_complete());
    EXPECT_EQ(startup.total(), 0);
}

TEST(StartupMonitor, zero) {
    StartupMonitorTest startup;
    EXPECT_FALSE(startup.startup_complete());
    EXPECT_EQ(startup.startup_count(), 0);
    EXPECT_EQ(startup.total(), 0);

    bool woken{false};
    std::thread thread([&startup, &woken]() {
        startup.wait_ready();
        woken = true;
    });

    EXPECT_FALSE(woken);

    EXPECT_TRUE(startup.set_total(0));
    EXPECT_EQ(startup.total(), 0);
    EXPECT_EQ(startup.startup_count(), 0);
    thread.join();
    EXPECT_TRUE(woken);
    EXPECT_TRUE(startup.startup_complete());
    EXPECT_EQ(startup.total(), 0);
}

TEST(StartupMonitor, invalidSequence) {
    StartupMonitorTest startup;
    EXPECT_FALSE(startup.startup_complete());
    EXPECT_FALSE(startup.notify_ready("manager1")); // total not set yet
    EXPECT_TRUE(startup.set_total(1));
    EXPECT_EQ(startup.startup_count(), 0);
    EXPECT_EQ(startup.total(), 1);

    bool woken{false};
    std::thread thread([&startup, &woken]() {
        startup.wait_ready();
        woken = true;
    });

    EXPECT_FALSE(startup.set_total(2)); // total already set
    EXPECT_EQ(startup.total(), 1);      // didn't change
    EXPECT_TRUE(startup.notify_ready("manager2"));
    // EXPECT_EQ(startup.startup_count(), 1); will be 0 because startup is complete
    thread.join();
    EXPECT_TRUE(woken);
    EXPECT_TRUE(startup.startup_complete());
    EXPECT_EQ(startup.total(), 0);
}

TEST(StartupMonitor, duplicateReady) {
    StartupMonitorTest startup;
    EXPECT_FALSE(startup.startup_complete());
    EXPECT_TRUE(startup.set_total(2));
    EXPECT_EQ(startup.startup_count(), 0);
    EXPECT_EQ(startup.total(), 2);

    bool woken{false};
    std::thread thread([&startup, &woken]() {
        startup.wait_ready();
        woken = true;
    });

    EXPECT_TRUE(startup.notify_ready("manager1"));
    EXPECT_EQ(startup.startup_count(), 1);
    EXPECT_TRUE(startup.notify_ready("manager1")); // duplicate
    EXPECT_EQ(startup.startup_count(), 1);
    EXPECT_FALSE(startup.startup_complete());
    EXPECT_TRUE(startup.notify_ready("manager2"));
    // EXPECT_EQ(startup.startup_count(), 2); will be 0 because startup is complete
    EXPECT_TRUE(startup.startup_complete());

    thread.join();
    EXPECT_TRUE(woken);
    EXPECT_TRUE(startup.startup_complete());
    EXPECT_EQ(startup.total(), 0);
}

} // namespace
