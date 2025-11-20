// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "gtest/gtest.h"
#include <everest/util/async/async_wrapper.hpp>
#include <exception>
#include <future>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

struct Counter {
    Counter(int v) : value(v) {
    }
    int value = 0;
    std::thread::id worker_id;

    // Mutator
    void add(int v) {
        value += v;
        worker_id = std::this_thread::get_id();
    }
    // Accessor
    int get() const {
        return value;
    }
    // Thrower
    int throw_if_equal(int v) {
        if (value == v) {
            throw std::runtime_error("User-defined fatal error.");
        }
        return value;
    }
};

using namespace everest::lib::util;

// --- GTEST FIXTURE ---
namespace everest::lib::util::testing_interface {
class AsyncWrapperTest : public ::testing::Test {
public:
    template <class T> std::shared_ptr<std::promise<void>> const& get_global_promise_for_test(T& wrapper) const {
        return wrapper.m_global_promise;
    }

    template <class T> auto& get_queue_for_test(T& wrapper) {
        return wrapper.m_queue;
    }
};
} // namespace everest::lib::util::testing_interface

using namespace everest::lib::util::testing_interface;

template <class T> class TestQueue : public thread_safe_queue<T> {
public:
    using ThisT = thread_safe_queue<T>;
    void push(T item) {
        ThisT::push(item);
    }

    T pop() {
        if (m_throw_on_next_pop) {
            throw std::runtime_error("oh no");
        }

        auto tmp = ThisT::pop();

        return tmp;
    }

    void force_throw_on_next_pop() {
        m_throw_on_next_pop = true;
        push([]() {});
    }

private:
    bool m_throw_on_next_pop{false};
};

template <typename T>
using async_guarded_testqueue = async_wrapper_impl<T, GlobalFailurePolicy, WaitToFinishPolicy, TestQueue>;

// Test 1: Basic functionality and thread serialization (Background/WaitToFinish)
TEST_F(AsyncWrapperTest, CoreFunctionality) {
    async_wrapper_wait<Counter> wrapper(0);

    // 1. Check asynchronous execution and result retrieval
    auto fut1 = wrapper([](Counter& c) {
        c.add(5);
        return c.get();
    });
    auto fut2 = wrapper([](Counter& c) {
        c.add(10);
        return c.get();
    });

    EXPECT_EQ(fut1.get(), 5);
    EXPECT_EQ(fut2.get(), 15);

    // 2. Check side effect on resource
    auto fut3 = wrapper([](Counter& c) { return c.get(); });
    EXPECT_EQ(fut3.get(), 15);

    // 3. Check 'run' (fire-and-forget)
    wrapper.run([](Counter& c) { c.add(5); });

    auto fut4 = wrapper([](Counter& c) { return c.get(); });
    EXPECT_EQ(fut4.get(), 20);

    // 4. Check thread ID
    std::thread::id main_thread_id = std::this_thread::get_id();
    auto fut_id = wrapper([](Counter& c) { return c.worker_id; });

    EXPECT_NE(fut_id.get(), main_thread_id);
}

// Test 2: LocalFailurePolicy (Background) - User Exception is Isolated
TEST_F(AsyncWrapperTest, LocalPolicy_UserExceptionIsContained) {
    async_wrapper_wait<Counter> wrapper(5);

    auto fut_a = wrapper([](Counter& c) { return c.throw_if_equal(5); });
    auto fut_b = wrapper([](Counter& c) {
        c.add(10);
        return c.get();
    });

    fut_a.wait();
    fut_b.wait();

    ASSERT_THROW(fut_a.get(), std::runtime_error);

    int received_value = 0;
    EXPECT_NO_THROW(received_value = fut_b.get());
    EXPECT_EQ(received_value, 15);

    auto fut_c = wrapper([](Counter& c) { return c.get(); });
    fut_c.wait();
    EXPECT_EQ(fut_c.get(), 15);
}

// Test 3: GlobalFailurePolicy (Guarded) - User Exception Shuts Down Instance
TEST_F(AsyncWrapperTest, GlobalPolicy_UserExceptionCausesShutdown) {
    async_wrapper_guarded_wait<Counter> wrapper(5);

    auto fut_a = wrapper([](Counter& c) { return c.throw_if_equal(5); });
    auto fut_b = wrapper([](Counter& c) {
        c.add(10);
        return c.get();
    });

    fut_a.wait();
    fut_b.wait();

    ASSERT_THROW(fut_a.get(), std::runtime_error);
    ASSERT_THROW(fut_b.get(), std::runtime_error);

    auto fut_c = wrapper([](Counter& c) { return c.get(); });
    fut_c.wait();
    ASSERT_THROW(fut_c.get(), std::runtime_error);
}

// Test 4: GlobalFailureSignal_BlocksNewTasks (Tests signal effect)
TEST_F(AsyncWrapperTest, GlobalFailureSignal_BlocksNewTasks) {
    async_wrapper_guarded_wait<Counter> wrapper(0);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // 1. Manually set the Global Promise (simulating infrastructure or user failure)
    try {
        throw std::runtime_error("Simulated Global Signal Set.");
    } catch (...) {
        get_global_promise_for_test(wrapper)->set_exception(std::current_exception());
    }

    // 2. Submit a new task (Task B)
    auto fut_b = wrapper([](Counter& c) {
        c.add(50);
        return c.get();
    });

    fut_b.wait();

    // 3. Task B must immediately fail because the infrastructure flag is set
    ASSERT_THROW(fut_b.get(), std::runtime_error);
}

// Test 5: Destructor Behavior - WaitToFinishPolicy vs FastQuitPolicy
TEST_F(AsyncWrapperTest, DestructorShutdownPolicies) {
    // Setup 1: Test WaitToFinishPolicy (Guaranteed execution of queued task)
    int wait_result = 0;
    {
        async_wrapper_guarded_wait<Counter> wrapper(0);
        auto fut = wrapper([&wait_result](Counter& c) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            wait_result = 1;
            return c.get();
        });
        // Destructor runs here (WaitToFinishPolicy::shutdown), MUST wait 50ms.
    }
    // Result confirms the destructor waited for the task to finish.
    EXPECT_EQ(wait_result, 1);

    // Setup 2: Test FastQuitPolicy (Drops queued task, joins quickly)
    int fast_result = 0;
    {
        async_wrapper_guarded_fast<Counter> wrapper(0);

        // Push a task that runs briefly. If the task starts, the destructor must wait.
        // We rely on the race condition being won by the destructor for EXPECT_EQ(0) to pass.
        wrapper.run([&fast_result](Counter& c) {
            std::this_thread::sleep_for(std::chrono::microseconds(100)); // Very fast sleep
            fast_result = 2; // Should not reach here if the task is aborted while queued
        });

        // Destructor runs here (FastQuitPolicy::shutdown), should join quickly.
    }
    // If fast_result == 0, the task was aborted while queued.
    // If fast_result == 2, the task started and the destructor waited for it to finish.
    EXPECT_EQ(fast_result, 0);
}

// Test 6: Verify Worker's internal catch block works correctly
TEST_F(AsyncWrapperTest, InfrastructureFailure_WorkerSetsSignalAndShutsDown) {
    async_guarded_testqueue<Counter> wrapper(0); // Use the specialized TestQueue type

    // 1. Ensure worker thread is blocked on pop()
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // 2. Force the queue to throw an exception, waking up the worker thread
    get_queue_for_test(wrapper).force_throw_on_next_pop();

    // 3. Give the worker time to execute the catch block and shut down.
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // 4. Submit a new task (Task A). This hits the Synchronous Gatekeeper check.
    auto fut_a = wrapper([](Counter& c) {
        c.add(99);
        return c.get();
    });

    fut_a.wait();

    // 5. Task A must fail with the infrastructure exception
    bool active_runtime_error = false;
    std::string message = "";
    try {
        fut_a.get();
    } catch (const std::runtime_error& e) {
        message = e.what();
        active_runtime_error = true;
    }
    EXPECT_TRUE(active_runtime_error);

    EXPECT_EQ(message, "Async worker infrastructure failure.");
}

// Test 7: Run method must not block the main thread (optimized fire-and-forget)
TEST_F(AsyncWrapperTest, RunMethodDoesNotBlock) {
    // We use the WaitToFinish policy so we know the task will complete before the test ends.
    async_wrapper_wait<Counter> wrapper(0);
    const int SLOW_TASK_MS = 50;

    auto slow_task = [SLOW_TASK_MS](Counter& c) {
        // Task takes significant time to execute
        std::this_thread::sleep_for(std::chrono::milliseconds(SLOW_TASK_MS));
        c.add(1);
    };

    auto start_time = std::chrono::steady_clock::now();

    // Submit the slow task using the optimized run() method
    wrapper.run(slow_task);

    auto end_time = std::chrono::steady_clock::now();

    // The main thread should not have blocked for the duration of the task.
    // The elapsed time should be much less than the task execution time (10ms tolerance).
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    // 1. Assertion on timing: Proves non-blocking submission
    EXPECT_LT(elapsed_ms, 10);

    // 2. Assert task eventually ran (rely on WaitToFinishPolicy destructor)
    auto fut_check = wrapper([](Counter& c) { return c.get(); });
    EXPECT_EQ(fut_check.get(), 1);
}
