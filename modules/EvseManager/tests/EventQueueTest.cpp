// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <EventQueue.hpp>
#include <gtest/gtest.h>

#include <condition_variable>
#include <mutex>
#include <thread>

namespace {

enum class ErrorHandlingEvents : std::uint8_t {
    prevent_charging,
    prevent_charging_welded,
    all_errors_cleared
};

TEST(EventQueue, init) {
    module::EventQueue<ErrorHandlingEvents> queue;
    auto events = queue.get_events();
    EXPECT_EQ(events.size(), 0);
}

TEST(EventQueue, one) {
    module::EventQueue<ErrorHandlingEvents> queue;
    auto events = queue.get_events();
    EXPECT_EQ(events.size(), 0);

    queue.push(ErrorHandlingEvents::prevent_charging);
    events = queue.get_events();
    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0], ErrorHandlingEvents::prevent_charging);

    events = queue.get_events();
    EXPECT_EQ(events.size(), 0);
}

TEST(EventQueue, two) {
    module::EventQueue<ErrorHandlingEvents> queue;
    auto events = queue.get_events();
    EXPECT_EQ(events.size(), 0);

    queue.push(ErrorHandlingEvents::prevent_charging);
    queue.push(ErrorHandlingEvents::prevent_charging_welded);
    events = queue.get_events();
    ASSERT_EQ(events.size(), 2);
    EXPECT_EQ(events[0], ErrorHandlingEvents::prevent_charging);
    EXPECT_EQ(events[1], ErrorHandlingEvents::prevent_charging_welded);

    events = queue.get_events();
    EXPECT_EQ(events.size(), 0);
}

TEST(EventQueue, wait) {
    module::EventQueue<ErrorHandlingEvents> queue;
    auto events = queue.get_events();
    EXPECT_EQ(events.size(), 0);

    std::size_t count = 0;
    std::condition_variable cv;
    std::mutex mux;
    bool ready{false};

    std::thread wait_thread([&cv, &count, &queue, &ready, &mux]() {
        {
            std::lock_guard<std::mutex> lock(mux);
            ready = true;
        }
        cv.notify_one();
        auto events = queue.wait();
        count = events.size();
        {
            std::lock_guard<std::mutex> lock(mux);
            ready = false;
        }
        cv.notify_one();
    });

    std::unique_lock<std::mutex> ul(mux);
    cv.wait(ul, [&ready] { return ready; });
    ASSERT_EQ(count, 0U);

    queue.push(ErrorHandlingEvents::prevent_charging);

    cv.wait(ul, [&ready] { return !ready; });
    ASSERT_EQ(count, 1U);

    events = queue.get_events();
    EXPECT_EQ(events.size(), 0);

    wait_thread.join();
}

} // namespace
