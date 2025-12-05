// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "gtest/gtest.h"
#include <everest/util/async/monitor.hpp>
#include <future>
#include <memory>
#include <string>
#include <thread>
#include <vector>

using namespace everest::lib::util;

struct SharedData {
    int value = 0;
    std::string name = "initial";
    // Unique ID to track object identity across moves/swaps
    long long id = std::chrono::duration_cast<std::chrono::nanoseconds>(
                       std::chrono::high_resolution_clock::now().time_since_epoch())
                       .count();
};

// --- Test Fixtures for Noexcept Checks ---
// 1. Type that is NOT nothrow-swappable (due to non-noexcept move constructor)
struct ThrowingMover {
    int* ptr;

    // Move Constructor: NOT noexcept (This is the key difference)
    ThrowingMover(ThrowingMover&& other) noexcept(false) : ptr(std::exchange(other.ptr, nullptr)) {
        if (!ptr)
            throw std::runtime_error("simulated throw on move");
    }

    // Move Assignment: NOT noexcept
    ThrowingMover& operator=(ThrowingMover&& other) noexcept(false) {
        if (this != &other) {
            std::swap(ptr, other.ptr);
        }
        return *this;
    }

    ThrowingMover() : ptr(new int(42)) {
    }
    ~ThrowingMover() {
        delete ptr;
    }
    ThrowingMover(const ThrowingMover&) = delete;
    ThrowingMover& operator=(const ThrowingMover&) = delete;

    // Define custom swap for ADL (must use the same non-noexcept status)
    friend void swap(ThrowingMover& lhs, ThrowingMover& rhs) noexcept(false) {
        std::swap(lhs.ptr, rhs.ptr);
    }
};

// 2. Type that IS nothrow-swappable
struct NoThrowMover {
    int* ptr;

    // Move Constructor: IS noexcept
    NoThrowMover(NoThrowMover&& other) noexcept : ptr(std::exchange(other.ptr, nullptr)) {
    }

    // Move Assignment: IS noexcept
    NoThrowMover& operator=(NoThrowMover&& other) noexcept {
        if (this != &other) {
            std::swap(ptr, other.ptr);
        }
        return *this;
    }

    NoThrowMover() : ptr(new int(42)) {
    }
    ~NoThrowMover() {
        delete ptr;
    }
    NoThrowMover(const NoThrowMover&) = delete;
    NoThrowMover& operator=(const NoThrowMover&) = delete;

    // Define custom swap for ADL (must be noexcept)
    friend void swap(NoThrowMover& lhs, NoThrowMover& rhs) noexcept {
        std::swap(lhs.ptr, rhs.ptr);
    }
};

// --- The Static Assert Tests ---
// We use basic static_asserts to verify the compiler's calculated noexcept status.

namespace NoexceptTests {
using namespace everest::lib::util;
template <typename T> using monitor = everest::lib::util::monitor<T>;
using M_NT = monitor<NoThrowMover>; // Monitor protecting the safe type
using M_T = monitor<ThrowingMover>; // Monitor protecting the unsafe type

// --- 1. Test against NoThrowMover (T is noexcept swappable) ---
// All move and swap operations on the monitor should be noexcept(true).

static_assert(std::is_nothrow_swappable_v<NoThrowMover>,
              "Prerequisite 1 failed: NoThrowMover must be noexcept swappable.");

// Move Constructor: Should be noexcept(true)
static_assert(std::is_nothrow_move_constructible_v<M_NT>, "NT Test 1 failed: Move Constructor must be noexcept.");

// Member Swap: Should be noexcept(true)
static_assert(noexcept(std::declval<M_NT>().swap(std::declval<M_NT&>())),
              "NT Test 2 failed: Member Swap must be noexcept.");

// Move Assignment: Should be noexcept(true)
static_assert(std::is_nothrow_move_assignable_v<M_NT>, "NT Test 3 failed: Move Assignment must be noexcept.");

// --- 2. Test against ThrowingMover (T is NOT noexcept swappable) ---
// All move and swap operations on the monitor should be noexcept(false).

static_assert(!std::is_nothrow_swappable_v<ThrowingMover>,
              "Prerequisite 2 failed: ThrowingMover must NOT be noexcept swappable.");

// Move Constructor: Should be noexcept(false) (allows exceptions)
static_assert(!std::is_nothrow_move_constructible_v<M_T>, "T Test 1 failed: Move Constructor must NOT be noexcept.");

// Member Swap: Should be noexcept(false) (allows exceptions)
static_assert(!noexcept(std::declval<M_T>().swap(std::declval<M_T&>())),
              "T Test 2 failed: Member Swap must NOT be noexcept.");

// Move Assignment: Should be noexcept(false) (allows exceptions)
static_assert(!std::is_nothrow_move_assignable_v<M_T>, "T Test 3 failed: Move Assignment must NOT be noexcept.");
} // namespace NoexceptTests

using namespace everest::lib::util;

class MonitorTest : public ::testing::Test {
protected:
    monitor<SharedData> simple_monitor_;
    monitor<std::unique_ptr<SharedData>> ptr_monitor_;
    // A timed mutex enabled monitor::handle(timeout)
    monitor<SharedData, std::timed_mutex> timed_mtx_monitor_;

    // Time constants for tests
    const std::chrono::milliseconds BLOCK_TIME = std::chrono::milliseconds(200);
    const std::chrono::milliseconds SHORT_WAIT = std::chrono::milliseconds(10);
    const std::chrono::milliseconds LONG_WAIT = std::chrono::milliseconds(300);
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
        std::this_thread::sleep_for(BLOCK_TIME);

        // Lock released when handle goes out of scope 🔓
    });

    // Main thread waits until the blocker explicitly confirms it is holding the lock
    blocker_locked_future.get();

    // Test 1: Try to acquire the lock with a short timeout (Expected to FAIL)
    auto handle_opt = timed_mtx_monitor_.handle(SHORT_WAIT);
    EXPECT_FALSE(handle_opt.has_value());

    // Test 2: Try to acquire the lock with a long timeout (Expected to SUCCEED eventually)
    // The total wait time will be slightly longer than BLOCK_TIME (200ms).
    auto start_success = std::chrono::steady_clock::now();
    auto handle_long_opt = timed_mtx_monitor_.handle(LONG_WAIT);

    auto duration_success = std::chrono::steady_clock::now() - start_success;

    EXPECT_TRUE(handle_long_opt.has_value());
    // FIX 2: Explicitly compare the count() to ensure stable comparison and output
    EXPECT_GE(duration_success.count(), BLOCK_TIME.count());

    blocker.join();
}

TEST_F(MonitorTest, TimedMutexLockAcquisition) {
    // This test ensures the complex timing logic for acquisition is sound.

    // Synchronization barrier: Blocker signals it has acquired the lock
    std::promise<void> blocker_locked_promise;
    std::future<void> blocker_locked_future = blocker_locked_promise.get_future();

    // THREAD A: The Blocker (Holds the lock on timed_mtx_monitor_)
    std::thread blocker([&] {
        // 1. Acquire lock
        auto handle = timed_mtx_monitor_.handle();
        blocker_locked_promise.set_value(); // Signal: Lock is now held

        // 2. Hold the lock for the required duration
        std::this_thread::sleep_for(BLOCK_TIME); // 200ms

        // Lock released when handle goes out of scope
    });

    // 3. Main thread waits until the lock is actively held
    blocker_locked_future.get();

    // --- Test 1: Fail Case (Wait is shorter than remaining lock time) ---
    auto fail_handle = timed_mtx_monitor_.handle(SHORT_WAIT);
    EXPECT_FALSE(fail_handle.has_value());

    // --- Test 2: Success Case (Wait is longer than remaining lock time) ---
    auto start_success_timing = std::chrono::steady_clock::now();

    // Acquire the lock with a sufficient timeout (300ms)
    auto success_handle = timed_mtx_monitor_.handle(LONG_WAIT);

    auto duration_success = std::chrono::steady_clock::now() - start_success_timing;

    // Must succeed acquisition
    EXPECT_TRUE(success_handle.has_value());

    // FIX 3: Explicitly compare the count() to ensure stable comparison and output
    EXPECT_GE(duration_success.count(), BLOCK_TIME.count());

    blocker.join();
}

TEST_F(MonitorTest, ConditionVariableAtomicity) {
    bool notification_sent = false;
    // Use the SharedData member to track state
    simple_monitor_.handle()->value = 0;

    // THREAD A: The Waiter
    std::thread waiter([&] {
        auto handle = simple_monitor_.handle();

        // Waiter signals that it is holding the lock and about to enter the wait state
        // (This is implicitly tested by the notifier having to wait for the lock)

        // Predicate check: Ensure 'notification_sent' is only true IF the lock is reacquired
        handle.wait([&] {
            // The predicate will be checked spuriously, but the critical check is on wake
            return notification_sent;
        });

        // After waking up, the lock is held. Verify the resource state.
        // This checks that the state modification (value=1) happened while the lock was released.
        EXPECT_EQ(1, handle->value);
    });

    // Give the waiter time to acquire the lock and block on the CV (Crucial setup time)
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // THREAD B: The Notifier (This thread modifies the state and notifies)
    {
        // Must acquire the lock. This proves the waiter released it atomically inside wait().
        auto handle = simple_monitor_.handle();

        // 1. Modify the resource while holding the lock
        handle->value = 1;

        // 2. Set the wait condition *after* modification
        notification_sent = true;

        // Lock is released here, which allows the waiter to potentially reacquire it.
    }

    simple_monitor_.notify_one();

    waiter.join();

    // Final check that the state is 1, confirming the waiter successfully completed its EXPECT.
    EXPECT_EQ(1, simple_monitor_.handle()->value);
}

TEST_F(MonitorTest, ThreadSafeMoveOperations) {
    // Setup: Monitor m1 (Source) starts with data, Monitor m2 (Destination) starts empty.
    monitor<SharedData> m1;
    m1.handle()->value = 10;
    auto m1_initial_id = m1.handle()->id; // Track resource identity

    // Create m2 with different data
    monitor<SharedData> m2;
    m2.handle()->value = 99;
    auto m2_initial_id = m2.handle()->id;

    std::promise<void> blocker_locked_promise;
    std::future<void> blocker_locked_future = blocker_locked_promise.get_future();

    // THREAD A: The Blocker (Holds the lock on m1 to force the move operation to wait)
    std::thread blocker([&] {
        auto handle = m1.handle(); // Lock m1
        blocker_locked_promise.set_value();
        std::this_thread::sleep_for(BLOCK_TIME);
    });

    // Main thread waits until m1 is locked by the blocker
    blocker_locked_future.get();

    // --- Move Assignment Test: m2 = std::move(m1) ---

    // Because the move assignment operator calls monitor::swap(m2, m1), and swap locks both,
    // it must wait for m1's lock (held by blocker thread) to be released.
    auto start_move = std::chrono::steady_clock::now();
    m2 = std::move(m1); // Should block here until blocker releases m1's lock
    auto duration_move = std::chrono::steady_clock::now() - start_move;

    // Verify the move blocked until the blocker thread finished (duration > hold time)
    EXPECT_GE(duration_move, BLOCK_TIME);

    // Verify data transfer (m2 now has m1's initial data)
    EXPECT_EQ(10, m2.handle()->value);
    EXPECT_EQ(m1_initial_id, m2.handle()->id); // m2 now owns m1's resource

    // Verify source state (m1 now has m2's initial data)
    // The move assignment resulted in a SWAP.
    EXPECT_EQ(99, m1.handle()->value);
    EXPECT_EQ(m2_initial_id, m1.handle()->id); // m1 now owns m2's original resource

    blocker.join();
}
