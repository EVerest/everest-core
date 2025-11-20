// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/**
 * @file Wrapping of a resource and serializing the access to the resource
 * @brief The content is based on the conference talk 'Concurrency-and-Parallelism'
 * held by Herb Sutter at C-and-Beyond-2012.
 * The original pattern has been extend with exhaustive handling of corner cases and errors.
 * Policies allow for the adaptation to common use cases.
 */

#pragma once

#include <atomic>
#include <everest/util/queue/thread_safe_queue.hpp>
#include <exception>
#include <functional>
#include <future>
#include <memory>
#include <thread>
#include <type_traits>

namespace everest::lib::util::testing_interface {
//  Forward declaration of the test fixture used as a friend
class AsyncWrapperTest;
} // namespace everest::lib::util::testing_interface

namespace everest::lib::util {

// --- EXCEPTION POLICIES (Policy 1) ---

/**
 * @brief Policy that triggers a global shutdown if a user-supplied task throws an exception.
 * @details Used for guarded resources where an exception implies resource corruption.
 */
struct GlobalFailurePolicy {
    /**
     * @brief Sets the global promise with the given exception pointer, permanently failing the executor.
     */
    static void handle_user_exception(std::shared_ptr<std::promise<void>> const& global_promise_ptr,
                                      std::exception_ptr current_exception_ptr) {
        try {
            global_promise_ptr->set_exception(current_exception_ptr);
        } catch (const std::future_error&) {
            // Ignore: promise was already satisfied by an earlier thread/task
        }
    }
};

/**
 * @brief Policy that contains a user-supplied exception locally (does not cause global failure).
 * @details Used for background tasks where exceptions do not corrupt the resource state.
 */
struct LocalFailurePolicy {
    /**
     * @brief Handles the user exception by doing nothing to the global promise.
     */
    static void handle_user_exception([[maybe_unused]] std::shared_ptr<std::promise<void>> const& global_promise_ptr,
                                      [[maybe_unused]] std::exception_ptr current_exception_ptr) {
    }
};

// --- SHUTDOWN POLICIES (Policy 2) ---

/**
 * @brief Destructor policy that waits for all queued tasks to finish execution before joining the worker thread.
 */
struct WaitToFinishPolicy {
    /**
     * @brief Performs a graceful shutdown by pushing a sentinel task and joining.
     */
    template <typename QueueT, typename ThreadT>
    static void shutdown(QueueT& queue, ThreadT& worker, std::atomic_bool& done_flag) {
        // Push the final stop signal, which will be executed after all pending tasks.
        queue.push([&done_flag] { done_flag = true; });
        worker.join();
    }
};

/**
 * @brief Destructor policy that signals the worker to exit immediately, potentially dropping queued tasks.
 */
struct FastQuitPolicy {
    /**
     * @brief Performs an immediate shutdown by setting the flag and unblocking the thread.
     */
    template <typename QueueT, typename ThreadT>
    static void shutdown(QueueT& queue, ThreadT& worker, std::atomic_bool& done_flag) {
        // Signal termination immediately
        done_flag = true;
        // Send a dummy task to unblock the worker if it's currently blocking on pop()
        queue.push([] {});
        worker.join();
    }
};

/**
 * @brief A single-threaded asynchronous executor that serializes access to a resource T.
 * @details All operations on T are performed sequentially on a dedicated worker thread. It uses two policies
 * (ExceptionPolicy and ShutdownPolicy) and a QueueT template parameter to define its entire contract.
 * @tparam T The resource type being wrapped.
 * @tparam ExceptionPolicy Defines global failure behavior (e.g., GlobalFailurePolicy).
 * @tparam ShutdownPolicy Defines destructor behavior (e.g., WaitToFinishPolicy).
 * @tparam QueueT The underlying thread-safe queue implementation.
 */
template <typename T, typename ExceptionPolicy, typename ShutdownPolicy,
          template <class> typename QueueT = thread_safe_queue>
class async_wrapper_impl {
public:
    friend class everest::lib::util::testing_interface::AsyncWrapperTest; ///< Allows GTest fixture to access
                                                                          ///< protected/private members for testing.

    using ThisT = async_wrapper_impl<T, ExceptionPolicy, ShutdownPolicy, QueueT>;

    /**
     * @brief Constructs the wrapper, initializing the resource and starting the worker thread.
     */
    template <class... ArgsT>
    explicit async_wrapper_impl(ArgsT&&... args) :
        m_resource(std::forward<ArgsT>(args)...), m_global_promise(std::make_shared<std::promise<void>>()) {

        m_global_future = m_global_promise->get_future();

        m_worker = std::thread([this] {
            while (not m_done) {
                try {
                    m_queue.pop()(); // Execute the task lambda
                } catch (const std::exception& e) {

                    // Critical infrastructure failure handling
                    try {
                        m_global_promise->set_exception(
                            std::make_exception_ptr(std::runtime_error("Async worker infrastructure failure.")));
                    } catch (const std::future_error&) {
                        // Ignore: Promise was already set by a concurrent thread/user task
                    }
                    m_done = true; // Signal thread termination
                } catch (...) {
                    m_done = true; // Handle non-standard exceptions
                }
            }
        });
    }

    // Rule of Five members
    async_wrapper_impl(ThisT&& other) noexcept = default;
    ThisT& operator=(ThisT&&) noexcept = default;
    async_wrapper_impl(ThisT const& other) = delete;

    /**
     * @brief Destructor that shuts down the worker thread according to the ShutdownPolicy.
     */
    ~async_wrapper_impl() {
        ShutdownPolicy::shutdown(m_queue, m_worker, m_done);
    }

private:
    template <typename Fut, typename F> void promise_set_value(std::promise<Fut>& prom, F& f, T& t) const {
        prom.set_value(f(t));
    }

    template <typename F> void promise_set_value(std::promise<void>& prom, F& f, T& t) const {
        f(t);
        prom.set_value();
    }

    bool is_global_failure_signaled() const {
        return m_global_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    }

    template <typename F, typename R> void enqueue_task(F&& f, std::shared_ptr<std::promise<R>> prom) const {
        auto global_promise_ptr = m_global_promise;

        // --- SYNCHRONOUS FAILURE CHECK (Gatekeeper, executed on calling thread) ---
        if (is_global_failure_signaled()) {
            try {
                m_global_future.get();
            } catch (const std::exception&) {
                prom->set_exception(std::current_exception());
            }
            return;
        }

        m_queue.push([f = std::forward<F>(f), prom, global_promise_ptr, this] {
            // --- ASYNCHRONOUS FAILURE CHECK (Mandatory Final Gatekeeper) ---
            if (is_global_failure_signaled()) {
                try {
                    m_global_future.get();
                } catch (const std::exception&) {
                    prom->set_exception(std::current_exception());
                }
                return;
            }

            try {
                promise_set_value(*prom, f, m_resource);
            } catch (...) {
                prom->set_exception(std::current_exception());

                ExceptionPolicy::handle_user_exception(global_promise_ptr, std::current_exception());
            }
        });
    }

    // --- ENQUEUE FOR FIRE-AND-FORGET TASKS FOR (No Promise, No Wait) ---
    template <typename F> void enqueue_task(F&& f) const {
        if (is_global_failure_signaled()) {
            return;
        }

        auto global_promise_ptr = m_global_promise;
        m_queue.push([f = std::forward<F>(f), global_promise_ptr, this] {
            if (is_global_failure_signaled()) {
                return;
            }
            try {
                f(m_resource);
            } catch (...) {
                ExceptionPolicy::handle_user_exception(global_promise_ptr, std::current_exception());
            }
        });
    }

    mutable T m_resource;
    mutable QueueT<std::function<void()>> m_queue;
    std::thread m_worker;
    std::atomic_bool m_done{false};
    std::shared_ptr<std::promise<void>> m_global_promise;
    std::shared_future<void> m_global_future;

public:
    /**
     * @brief Submits a function object to the worker thread and returns a future for the result.
     * @details The function is guaranteed to be executed sequentially with respect to other tasks.
     * It receives the managed resource as it's only parameter.
     * @tparam F Function object callable with T&.
     * @return std::future<R> where R is the return type of F. The future will contain an exception
     * if the task fails or if a global failure signal has been set.
     */
    template <typename F> auto operator()(F f) const {
        using ReturnT = decltype(f(m_resource));
        auto prom = std::make_shared<std::promise<ReturnT>>();
        auto fut = prom->get_future();

        enqueue_task(std::forward<F>(f), prom);

        return fut;
    }

    /**
     * @brief Submits a function object to the worker thread without returning a future (fire-and-forget).
     * @details This uses an optimized path that avoids creating a std::promise, relying only on the
     * ExceptionPolicy for failure handling.
     * @tparam F Function object callable with T&.
     */
    template <typename F> void run(F f) const {
        enqueue_task<F>(std::forward<F>(f));
    }
};

// --- TYPEDEFS ---

/** @brief Base alias for resources where a task failure causes GLOBAL corruption (Guarded). */
template <typename T, typename ShutdownPolicy>
using async_wrapper_guarded = async_wrapper_impl<T, GlobalFailurePolicy, ShutdownPolicy>;

/** @brief Intermediate base alias for resources where a task failure is LOCALIZED (Background). */
template <typename T, typename ShutdownPolicy>
using async_wrapper_local = async_wrapper_impl<T, LocalFailurePolicy, ShutdownPolicy>;

// Final Usage Types (Combine Exception Policy and Shutdown Policy)

/** * @brief Primary default wrapper: Local Failure (Background) with Fast Quit.
 * @tparam T The resource type.
 */
template <typename T> using async_wrapper = async_wrapper_local<T, FastQuitPolicy>;

/** @brief Guarded resource with graceful (wait-to-finish) shutdown. */
template <typename T> using async_wrapper_guarded_wait = async_wrapper_guarded<T, WaitToFinishPolicy>;
/** @brief Local resource with graceful (wait-to-finish) shutdown. */
template <typename T> using async_wrapper_wait = async_wrapper_local<T, WaitToFinishPolicy>;
/** @brief Guarded resource with fast (potentially lossy) shutdown. */
template <typename T> using async_wrapper_guarded_fast = async_wrapper_guarded<T, FastQuitPolicy>;
/** @brief Local resource with fast (potentially lossy) shutdown. */
template <typename T> using async_wrapper_fast = async_wrapper_local<T, FastQuitPolicy>;

} // namespace everest::lib::util
