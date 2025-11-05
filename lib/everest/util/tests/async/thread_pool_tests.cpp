// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "gtest/gtest.h"
#include <atomic>
#include <chrono>
#include <everest/util/async/thread_pool.hpp>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <thread>
#include <vector>

using namespace everest::lib::util;

// --- Test Fixture ---
// A fixture allows us to set up and tear down the thread pool easily.
class ThreadPoolTest : public ::testing::Test {
protected:
    // Pool size chosen small to encourage contention
    const unsigned int POOL_SIZE = 4;

    // The thread_pool object will be initialized here and automatically
    // destroyed (and joined) when the test ends.
    thread_pool* pool;

    void SetUp() override {
        pool = new thread_pool(POOL_SIZE);
    }

    void TearDown() override {
        delete pool;
        pool = nullptr;
    }
};

// --- Helper Functions for Binding ---
// Example function to test argument passing
int add(int a, int b) {
    return a + b;
}

// Example function to test void return
void do_nothing() {
    // This is run by a worker thread
}

// --- Test Cases ---

// 1. Basic Correctness Tests
// -------------------------

TEST_F(ThreadPoolTest, Test_Immediate_Execution_And_Contention) {
    std::atomic<int> counter{0};
    const int num_tasks = 1000;
    std::vector<std::future<void>> futures;

    // Submit many more tasks than threads to test queue contention
    for (int i = 0; i < num_tasks; ++i) {
        // Use a lambda with no arguments (uses the specialized operator() if available)
        futures.push_back((*pool)([&counter]() { counter++; }));
    }

    // Wait for all tasks to complete
    for (auto& f : futures) {
        f.get();
    }

    // Check if all tasks ran correctly
    ASSERT_EQ(counter.load(), num_tasks);
}

TEST_F(ThreadPoolTest, Test_Future_Return_Value_And_Arguments) {
    // Test passing arguments to a simple function
    std::future<int> f1 = (*pool)(add, 10, 20);

    // Test a lambda with a return value and local arguments
    int multiplier = 5;
    std::future<double> f2 = (*pool)([multiplier](double val) { return val * multiplier; }, 10.0);

    ASSERT_EQ(f1.get(), 30);
    ASSERT_DOUBLE_EQ(f2.get(), 50.0);
}

TEST_F(ThreadPoolTest, Test_Run_No_Future) {
    // run() shouldn't block, so we use a flag to check execution
    std::atomic<bool> executed{false};

    // This should submit and return immediately
    pool->run([&executed]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        executed.store(true);
    });

    // We must wait for the task to finish to assert correctness
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ASSERT_TRUE(executed.load());
}

// 2. Exception and Error Handling Tests
// ------------------------------------

TEST_F(ThreadPoolTest, Test_Task_Exception_Transfer) {
    // Submit a task that throws an exception
    auto throwing_task = []() -> int {
        throw std::runtime_error("Task failed intentionally");
        return 42;
    };

    std::future<int> f = (*pool)(throwing_task);

    // future::get() should re-throw the exception from the worker thread
    ASSERT_THROW(f.get(), std::runtime_error);
}

// 3. Critical Shutdown Tests
// -------------------------

TEST_F(ThreadPoolTest, Test_Clean_Destruction_Blocked_Workers) {
    const int num_threads = POOL_SIZE;
    std::vector<std::future<void>> futures;
    std::atomic<int> started_count{0};

    // Submit exactly POOL_SIZE tasks that sleep for a long time
    for (int i = 0; i < num_threads; ++i) {
        futures.push_back((*pool)([&started_count]() {
            started_count++;
            // Block the thread, forcing the destructor to wait
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }));
    }

    // Wait briefly to ensure all threads have started their tasks
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // The destruction of the 'pool' fixture happens automatically in TearDown.
    // TearDown calls 'delete pool', which calls '~thread_pool()'.
    // If TearDown completes without crashing, the test passes.

    ASSERT_EQ(started_count.load(), num_threads) << "Not all threads started blocking tasks.";
}

TEST_F(ThreadPoolTest, Test_Clean_Destruction_Full_Queue) {
    const int num_tasks_to_queue = POOL_SIZE * 5; // Overwhelm the pool
    std::vector<std::future<void>> futures;

    // Submit many tasks, including long-running ones
    for (int i = 0; i < num_tasks_to_queue; ++i) {
        futures.push_back((*pool)([]() {
            // Mix of fast and slow tasks
            if (rand() % 10 == 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }));
    }

    // Do NOT call f.get() here. Let the pool be destroyed immediately,
    // simulating a sudden program exit.
    // The destruction in TearDown must complete without a deadlock.

    SUCCEED(); // If TearDown completes, the shutdown was clean.
}

TEST_F(ThreadPoolTest, Test_Void_Return) {
    std::future<void> f = (*pool)(do_nothing);

    // future::get() must be called to ensure the task finished without exception
    // It returns void, but checks for exceptions set on the promise.
    f.get();

    SUCCEED();
}

TEST_F(ThreadPoolTest, Test_Parallel_Execution_Proved) {
    // A single task duration that is long enough to measure accurately.
    const auto task_duration = std::chrono::milliseconds(100);

    // Number of tasks equals the number of threads in the pool
    const unsigned int num_tasks = POOL_SIZE;

    // Calculate the expected sequential time (N tasks * T duration)
    const auto expected_sequential_duration = task_duration * num_tasks;

    // The expected parallel time should be slightly more than one task's duration
    // We use a large tolerance factor (e.g., 2.5x the single task duration)
    // to account for thread creation, scheduling, and I/O overhead.
    const auto expected_parallel_limit = task_duration * 2.5;

    std::vector<std::future<void>> futures;

    auto start_time = std::chrono::steady_clock::now();

    // 1. Submit N blocking tasks (one for each thread)
    for (unsigned int i = 0; i < num_tasks; ++i) {
        futures.push_back((*pool)([task_duration]() { std::this_thread::sleep_for(task_duration); }));
    }

    // 2. Wait for all tasks to complete
    for (auto& f : futures) {
        f.get();
    }

    auto end_time = std::chrono::steady_clock::now();
    auto actual_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    // 3. Assert parallelism

    // Log results for manual inspection if the test fails
    // std::cout << "\n[ PARALLELISM TEST ]" << std::endl;
    // std::cout << "  Pool Size: " << POOL_SIZE << " threads" << std::endl;
    // std::cout << "  Single Task Time: " << task_duration.count() << "ms" << std::endl;
    // std::cout << "  Sequential Expected: " << expected_sequential_duration.count() << "ms" << std::endl;
    // std::cout << "  Parallel Limit: " << expected_parallel_limit.count() << "ms" << std::endl;
    // std::cout << "  Actual Time Taken: " << actual_duration.count() << "ms" << std::endl;

    // CRITICAL ASSERTION: The actual time must be much less than the sequential time.
    // Use EXPECT_LT (less than) against the safe parallel limit.
    EXPECT_LT(actual_duration.count(), expected_parallel_limit.count())
        << "The total time taken (" << actual_duration.count() << "ms) suggests tasks ran sequentially."
        << "Expected time less than " << expected_parallel_limit.count() << "ms for parallel execution.";
}
