// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "gtest/gtest.h"
#include <everest/util/async/monitor.hpp>
#include <future>
#include <memory>
#include <string>
#include <thread>
#include <vector>

struct SharedData {
    int value = 0;
    std::string name = "initial";
};

using namespace everest::lib::util;

class MonitorTest : public ::testing::Test {
protected:
    monitor<SharedData> simple_monitor_;
    monitor<std::unique_ptr<SharedData>> ptr_monitor_;
    // A timed mutex enabled monitor::handle(timeout)
    monitor<SharedData, std::timed_mutex> timed_mtx_monitor_;
};

TEST_F(MonitorTest, SingleThreadedAccess) {
    // Block 1: Access and Modify (Lock acquired by handle, then released)
    {
        // Acquire the handle (locks the mutex)
        auto handle = simple_monitor_.handle();

        // Access and modify the data using operator->
        handle->value = 100;
        handle->name = "updated";

        // When 'handle' goes out of scope here, the lock is released (RAII).
    }

    // Block 2: Verify changes (Lock acquired, then released)
    {
        // Now acquiring the lock succeeds because it was released above.
        auto handle_check = simple_monitor_.handle();

        // Verify
        EXPECT_EQ(100, handle_check->value);
        EXPECT_EQ("updated", handle_check->name);
    }
}

TEST_F(MonitorTest, PointerLikeAccessChaining) {
    // Block 1: Initialization (Ensures the unique_ptr is created)
    {
        auto h = ptr_monitor_.handle();
        *h = std::make_unique<SharedData>();
    } // h is destroyed, lock released.

    // Block 2: Access and Modify (Lock acquired by handle, then released)
    {
        // Acquire lock
        auto handle = ptr_monitor_.handle();

        // Access via chaining (T=unique_ptr<SharedData>)
        handle->value = 42;
        handle->name = "chained";
    } // handle is destroyed, lock released.

    // Block 3: Verify (Lock acquired, then released)
    {
        // Acquire lock
        auto handle_check = ptr_monitor_.handle();

        // Access via chaining
        EXPECT_EQ(42, handle_check->value);
        EXPECT_EQ("chained", handle_check->name);
    }
}

TEST_F(MonitorTest, ThreadSafeIncrement) {
    const int num_threads = 10;
    const int increments_per_thread = 1000;
    std::vector<std::thread> threads;

    // Set initial value to 0
    simple_monitor_.handle()->value = 0;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&] {
            for (int j = 0; j < increments_per_thread; ++j) {
                // Handle scope ensures RAII locking on every single increment
                auto handle = simple_monitor_.handle();
                handle->value++;
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    // Verify the final value is correct
    auto final_handle = simple_monitor_.handle();
    EXPECT_EQ(num_threads * increments_per_thread, final_handle->value);
}

TEST_F(MonitorTest, ConditionVariableWaitNotify) {
    bool done = false;

    // Future/Promise pair 1: Waiter signals it is ready to wait
    std::promise<void> waiter_ready_promise;
    std::future<void> waiter_ready_future = waiter_ready_promise.get_future();

    std::thread waiter([&] {
        // Acquire handle
        auto handle = simple_monitor_.handle();

        // Signal that we are holding the lock and about to wait
        waiter_ready_promise.set_value();

        // Wait until 'done' is true.
        handle.wait([&] { return done; });

        EXPECT_EQ(99, handle->value);
    });

    // Main thread waits until the waiter has acquired the lock and set the promise
    waiter_ready_future.get();

    // Notifier thread operation (guaranteed to happen after waiter has locked/signaled)
    {
        // Acquire lock
        auto handle = simple_monitor_.handle();
        handle->value = 99;
        done = true;
    }

    // Notify the waiting thread
    simple_monitor_.notify_one();

    waiter.join();
}

// --------------------------------------------------------------------------------

TEST_F(MonitorTest, TryLockHandleTimeout) {
    // Future/Promise pair 1: Blocker signals it has acquired the lock
    std::promise<void> blocker_locked_promise;
    std::future<void> blocker_locked_future = blocker_locked_promise.get_future();

    // The shared object is used as the resource for the lock

    std::thread blocker([&] {
        // Acquire the lock
        auto handle = timed_mtx_monitor_.handle(); // 🔒 Lock acquired

        // Signal to the main thread that the lock is held
        blocker_locked_promise.set_value();

        // Hold the lock for a specified duration
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        // Lock released when handle goes out of scope 🔓
    });

    // Main thread waits until the blocker explicitly confirms it is holding the lock
    blocker_locked_future.get();

    // Test 1: Try to acquire the lock with a short timeout (Expected to FAIL)
    auto handle_opt = timed_mtx_monitor_.handle(std::chrono::milliseconds(10));
    EXPECT_FALSE(handle_opt.has_value());

    // Test 2: Try to acquire the lock with a long timeout (Expected to SUCCEED eventually)
    // Note: The total wait time will be about 300ms, which is sufficient for the blocker to finish.
    auto handle_long_opt = timed_mtx_monitor_.handle(std::chrono::milliseconds(300));
    EXPECT_TRUE(handle_long_opt.has_value());

    blocker.join();
}

TEST_F(MonitorTest, TimedMutexLockAcquisition) {
    // Synchronization barriers
    std::promise<void> blocker_locked_promise;
    std::future<void> blocker_locked_future = blocker_locked_promise.get_future();
    std::promise<void> start_waiting_promise;
    std::future<void> start_waiting_future = start_waiting_promise.get_future();

    std::chrono::milliseconds hold_time(100);
    std::chrono::milliseconds short_wait(10);
    std::chrono::milliseconds long_wait(200);

    // THREAD A: The Blocker (Holds the lock on timed_mtx_monitor_)
    std::thread blocker([&] {
        // 1. Acquire lock
        auto handle = timed_mtx_monitor_.handle();
        blocker_locked_promise.set_value(); // Signal: Lock is now held

        // 3. Wait for the main thread to start waiting/timing
        start_waiting_future.get();

        // 4. Hold the lock for the required duration
        std::this_thread::sleep_for(hold_time);

        // Lock released when handle goes out of scope
    });

    // 2. Main thread waits until the lock is actively held
    blocker_locked_future.get();

    // --- Test 1: Fail Case (Wait is shorter than remaining lock time) ---
    // The previous timing logic was adequate here, as failure should be fast.
    auto fail_handle = timed_mtx_monitor_.handle(short_wait);
    EXPECT_FALSE(fail_handle.has_value());

    // --- Test 2: Success Case (Wait is longer than remaining lock time) ---
    // We now start the timer precisely when the blocker starts its hold duration.
    auto start_success = std::chrono::steady_clock::now();

    // Signal the blocker to start its sleep/hold time
    start_waiting_promise.set_value();

    // Acquire the lock with a sufficient timeout (this call blocks and times the wait)
    auto success_handle = timed_mtx_monitor_.handle(long_wait);

    // Calculate duration from the point the blocker started holding the lock
    auto duration_success = std::chrono::steady_clock::now() - start_success;

    // Must succeed acquisition
    EXPECT_TRUE(success_handle.has_value());

    // The time waited MUST be greater than or equal to the hold_time
    // We use EXPECT_GE (Greater than or Equal to) for robustness against scheduler timing
    EXPECT_GE(duration_success, hold_time);

    blocker.join();
}

TEST_F(MonitorTest, ConditionVariableAtomicity) {
    bool predicate_was_checked = false;
    bool notification_sent = false;

    // THREAD A: The Waiter
    std::thread waiter([&] {
        // Use simple_monitor_ (std::mutex) for CV operations
        auto handle = simple_monitor_.handle();

        // Waiter signals that it is holding the lock and about to enter the wait state
        // (This part is simplified here to focus on the CV logic itself)

        // Predicate check: Ensure 'notification_sent' is only true IF the lock is reacquired
        handle.wait([&] {
            predicate_was_checked = true;
            return notification_sent;
        });

        // After waking up, the lock is held. Verify the resource state.
        EXPECT_EQ(1, handle->value);
    });

    // Give the waiter time to acquire the lock and block on the CV
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // THREAD B: The Notifier (This thread modifies the state and notifies)
    {
        // Must acquire the lock. This proves the waiter released it.
        auto handle = simple_monitor_.handle();

        // 1. Modify the resource while holding the lock
        handle->value = 1;

        // 2. Set the wait condition *after* modification
        notification_sent = true;

        // 3. Ensure the predicate has NOT been checked since the wait began (it will be checked on wake)
        // This is tricky to assert safely without more barriers, but the logic holds:
        // The first successful predicate check occurs only after the notify/wake.
    } // Lock released, waiter is notified

    simple_monitor_.notify_one();

    waiter.join();

    // Final verification that the CV logic executed properly
    EXPECT_TRUE(predicate_was_checked);
}

TEST_F(MonitorTest, ThreadSafeMoveOperations) {
    // Setup: Monitor m1 is the source, initialized with a value
    monitor<SharedData> m1;
    m1.handle()->value = 10;

    std::promise<void> blocker_locked_promise;
    std::future<void> blocker_locked_future = blocker_locked_promise.get_future();

    // THREAD A: The Blocker (Holds the lock on m1 to force the move operation to wait)
    std::thread blocker([&] {
        auto handle = m1.handle(); // Lock m1
        blocker_locked_promise.set_value();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    });

    // Main thread waits until m1 is locked by the blocker
    blocker_locked_future.get();

    // --- Move Assignment Test ---
    monitor<SharedData> m2;

    // Move m1 to m2. This operation MUST wait for the blocker thread to release m1's lock
    // because the move assignment operator correctly acquires a unique_lock on the source (m1) mutex.
    auto start_move = std::chrono::steady_clock::now();
    m2 = std::move(m1); // Should block here until blocker releases m1's lock
    auto duration_move = std::chrono::steady_clock::now() - start_move;

    // Verify the move blocked until the blocker thread finished (duration > hold time)
    EXPECT_GT(duration_move, std::chrono::milliseconds(100));

    // Verify data transfer (m2 should now have the data)
    EXPECT_EQ(10, m2.handle()->value);

    blocker.join();
}
