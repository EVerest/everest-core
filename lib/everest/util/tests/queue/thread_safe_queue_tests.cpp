// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "gtest/gtest.h"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <everest/util/queue/thread_safe_queue.hpp>
#include <memory> // For std::unique_ptr
#include <numeric>
#include <optional>
#include <set>
#include <thread>
#include <type_traits> // For std::is_same_v
#include <vector>

// Note: Add includes for your simple_queue and thread_safe_queue here
// #include "simple_queue.h"
// #include "thread_safe_queue.h"

using namespace std::chrono_literals;
using namespace everest::lib::util;

// =================================================================
// 1. Test Fixture Setup
// =================================================================

// Helper to initialize TypeParam correctly in typed tests
template <typename T> T initialize_value(int id) {
    if constexpr (std::is_same_v<T, int>) {
        return id;
    } else if constexpr (std::is_same_v<T, std::string>) {
        return "Value_" + std::to_string(id);
    } else {
        // Fallback for other non-tested types
        return T{};
    }
}

// Test Fixture
template <typename T> class ThreadSafeQueueTest : public ::testing::Test {
protected:
    thread_safe_queue<T> queue;
};

// Typed Test Suite for int and std::string
using QueueTypes = ::testing::Types<int, std::string>;
TYPED_TEST_SUITE(ThreadSafeQueueTest, QueueTypes);

// Define a test suite specifically for concurrency checks (using int)
using ThreadSafeQueueIntTest = ThreadSafeQueueTest<int>;

// =================================================================
// 2. Basic Functionality Tests (Single Thread)
// =================================================================

TYPED_TEST(ThreadSafeQueueTest, PushAndPopSimple) {
    TypeParam expected_value = initialize_value<TypeParam>(42);

    this->queue.push(expected_value);

    // Test the non-blocking pop
    std::optional<TypeParam> result = this->queue.try_pop();

    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), expected_value);
    ASSERT_FALSE(this->queue.try_pop().has_value());
}

TYPED_TEST(ThreadSafeQueueTest, MultiplePushAndPopOrder) {
    const int count = 5;
    for (int i = 0; i < count; ++i) {
        this->queue.push(initialize_value<TypeParam>(i));
    }

    for (int i = 0; i < count; ++i) {
        TypeParam expected_value = initialize_value<TypeParam>(i);
        std::optional<TypeParam> result = this->queue.try_pop();

        ASSERT_TRUE(result.has_value());
        ASSERT_EQ(result.value(), expected_value);
    }
    ASSERT_FALSE(this->queue.try_pop().has_value());
}

// =================================================================
// 3. Time-Based Functionality Tests
// =================================================================

TYPED_TEST(ThreadSafeQueueTest, TryPopWithTimeout_Timeout) {
    auto start = std::chrono::steady_clock::now();

    // Try to pop with a short timeout
    std::optional<TypeParam> result = this->queue.try_pop(10ms);

    auto end = std::chrono::steady_clock::now();

    ASSERT_FALSE(result.has_value());

    auto elapsed = end - start;
    ASSERT_GE(elapsed, 9ms);
    ASSERT_LE(elapsed, 50ms);
}

TYPED_TEST(ThreadSafeQueueTest, TryPopWithTimeout_ImmediateSuccess) {
    TypeParam value = initialize_value<TypeParam>(101);
    this->queue.push(value);

    auto start = std::chrono::steady_clock::now();
    std::optional<TypeParam> result = this->queue.try_pop(10s);
    auto end = std::chrono::steady_clock::now();

    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), value);

    ASSERT_LT(end - start, 5ms);
}

// =================================================================
// 4. Synchronization and Blocking Tests
// =================================================================

TEST_F(ThreadSafeQueueIntTest, BlockingPopUnblocksOnPush) {
    const int expected_value = 123;
    std::atomic<int> result = 0;

    std::thread consumer([this, &result] {
        // Blocking pop() call
        result = this->queue.pop();
    });

    std::this_thread::sleep_for(100ms);

    this->queue.push(expected_value);

    consumer.join();
    ASSERT_EQ(result.load(), expected_value);
}

TEST_F(ThreadSafeQueueIntTest, MultipleWaitersUnblockedSequentially) {
    const int num_waiters = 5;
    std::vector<std::thread> consumers;
    std::atomic<int> pops_received = 0;

    for (int i = 0; i < num_waiters; ++i) {
        consumers.emplace_back([this, &pops_received] {
            this->queue.pop();
            pops_received++;
        });
    }

    std::this_thread::sleep_for(100ms);

    // Push exactly the number of waiters—only one waiter should be released per push
    for (int i = 0; i < num_waiters; ++i) {
        this->queue.push(i);
    }

    for (auto& t : consumers) {
        t.join();
    }

    ASSERT_EQ(pops_received.load(), num_waiters);
}

// =================================================================
// 5. Stress and Race Condition Tests
// =================================================================

TEST_F(ThreadSafeQueueIntTest, ConcurrentPushConsistency) {
    const int num_producers = 10;
    const int items_per_producer = 1000;
    const int total_items = num_producers * items_per_producer;

    std::vector<std::thread> producers;
    std::set<int> expected_values;

    for (int i = 0; i < num_producers; ++i) {
        producers.emplace_back([this, i, items_per_producer] {
            int start_value = i * items_per_producer;
            for (int j = 0; j < items_per_producer; ++j) {
                this->queue.push(start_value + j);
            }
        });
        int start_value = i * items_per_producer;
        for (int j = 0; j < items_per_producer; ++j) {
            expected_values.insert(start_value + j);
        }
    }

    for (auto& t : producers) {
        t.join();
    }

    // Drain the queue and check for consistency
    std::set<int> retrieved_values;
    for (int i = 0; i < total_items; ++i) {
        auto val = this->queue.pop();
        retrieved_values.insert(val);
    }

    ASSERT_EQ(retrieved_values.size(), total_items);
    ASSERT_EQ(retrieved_values, expected_values);
}

TEST_F(ThreadSafeQueueIntTest, ConcurrentPopNoDuplicate) {
    const int total_items = 10000;
    const int num_consumers = 10;

    // Producer pushes all items
    for (int i = 0; i < total_items; ++i) {
        this->queue.push(i);
    }

    // Consumers pop concurrently
    std::vector<std::thread> consumers;
    std::mutex result_mtx;
    std::set<int> retrieved_values;
    std::atomic<int> pop_count = 0;

    for (int i = 0; i < num_consumers; ++i) {
        consumers.emplace_back([this, &result_mtx, &retrieved_values, &pop_count, total_items] {
            while (pop_count.load() < total_items) {
                // Use try_pop so threads don't block indefinitely waiting for a push
                // that won't come until the other threads finish.
                if (auto val = this->queue.try_pop(); val.has_value()) {
                    std::lock_guard lock(result_mtx);
                    retrieved_values.insert(val.value());
                    pop_count++;
                }
                std::this_thread::yield();
            }
        });
    }

    for (auto& t : consumers) {
        t.join();
    }

    // Check consistency
    ASSERT_EQ(pop_count.load(), total_items) << "Total pops do not match total pushed items.";
    ASSERT_EQ(retrieved_values.size(), total_items) << "Duplicate items were retrieved.";
}

// =================================================================
// 6. Move-Only Type Compatibility Test (Verifying the push/pop fix)
// =================================================================

// Test fixture for std::unique_ptr<int> (a move-only type)
class ThreadSafeQueueMoveOnlyTest : public ::testing::Test {
protected:
    thread_safe_queue<std::unique_ptr<int>> queue;
};

TEST_F(ThreadSafeQueueMoveOnlyTest, HandlesConcurrentMoveOnlyTypes) {
    const int total_items = 1000;
    const int num_threads = 5;

    std::vector<std::thread> threads;
    std::atomic<int> pop_count = 0;

    // Producer/Consumer set for unique ownership verification
    std::set<int> retrieved_values;
    std::mutex result_mtx;

    // Start 5 threads: 3 producers, 2 consumers
    for (int i = 0; i < num_threads; ++i) {
        if (i < 3) { // Producers
            threads.emplace_back([this, i, total_items] {
                int start_value = i * total_items;
                for (int j = 0; j < total_items; ++j) {
                    // Requires thread_safe_queue::push(T&&)
                    this->queue.push(std::make_unique<int>(start_value + j));
                }
            });
        } else { // Consumers
            threads.emplace_back([this, &pop_count, &result_mtx, &retrieved_values, total_items] {
                int pops = 0;
                while (pops < total_items * 3 / 2) { // Try to pop 1500 times
                    // Requires thread_safe_queue::try_pop()
                    if (auto opt_ptr = this->queue.try_pop(); opt_ptr.has_value()) {
                        std::lock_guard lock(result_mtx);
                        retrieved_values.insert(*opt_ptr.value());
                        pop_count++;
                        pops++;
                    }
                    std::this_thread::yield();
                }
            });
        }
    }

    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    // Drain any remaining items in the main thread (should be few/none)
    while (auto opt_ptr = this->queue.try_pop()) {
        std::lock_guard lock(result_mtx);
        retrieved_values.insert(*opt_ptr.value());
        pop_count++;
    }

    const int total_expected = total_items * 3; // 3 producers * 1000 items

    ASSERT_EQ(pop_count.load(), total_expected) << "Total items popped does not match total pushed.";
    ASSERT_EQ(retrieved_values.size(), total_expected)
        << "Duplicate pointers/values were retrieved, indicating a race condition failure or a failed move.";
}
