// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once

#include "simple_queue.hpp"
#include <chrono>
#include <condition_variable>
#include <mutex>

namespace everest::lib::io::queue {

/**
 * A thread safe queue implemented on top of \ref queue::simple_queue. <br>
 * The common resource \ref simple_queue is guarded by a mutex in every member function. A caller blocking on \p pop
 * or \p try_pop will be unblocked new data made available via \p push
 * @tparam T Datatype held by the queue
 */
template <class T> class thread_safe_queue {
public:
    /**
     * @var value_type
     * @brief Inherited type definition.
     */
    using value_type = typename simple_queue<T>::value_type;

    /**
     * @brief Push new data into the queue
     * @param[in] value data
     */
    void push(const value_type& value) {
        std::unique_lock lock(m_mtx);
        m_queue.push(value);
        lock.unlock();
        m_cv.notify_one();
    }
    /**
     * @brief Push new data into the queue
     * @param[in] value data
     */
    void push(value_type&& value) {
        std::unique_lock lock(m_mtx);
        m_queue.push(std::forward<value_type>(value));
        lock.unlock();
        m_cv.notify_one();
    }

    /**
     * @brief Try to get an element from the queue.
     * @details Returns immediately.
     * @return An element from the queue, if one is available. \p std::nullopt otherwise
     */
    std::optional<value_type> try_pop() {
        return pop_impl(0);
    }

    /**
     * @brief Try to get an element from the queue.
     * @details Returns as soon as data is availble or after timeout.
     * @param[in] timeout as <a href="https://en.cppreference.com/w/cpp/chrono/duration">std::chrono::duration</a>.
     * Smallest unit acceptable is milliseconds.
     * @return An element from the queue, if one is available. \p std::nullopt otherwise
     */
    template <class Rep, class Period> std::optional<value_type> try_pop(std::chrono::duration<Rep, Period> timeout) {
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(timeout);
        return pop_impl(ms.count());
    }

    /**
     * @brief Get an element from the queue
     * @details Only returns, when data is available
     * @return An element from the queue.
     */
    value_type pop() {
        return pop_impl(-1).value();
    }

private:
    std::optional<value_type> pop_impl(int timeout_ms) {
        std::unique_lock lock(m_mtx);
        m_cv.wait_for(lock, std::chrono::milliseconds(timeout_ms), [this]() { return not m_queue.empty(); });
        return m_queue.pop();
    }

    simple_queue<T> m_queue;
    std::mutex m_mtx;
    std::condition_variable m_cv;
};

} // namespace everest::lib::io::queue
