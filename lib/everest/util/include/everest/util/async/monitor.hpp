// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/**
 * @file monitor.hpp
 * @brief Provides a generic RAII Monitor pattern implementation for thread-safe access to a shared resource.
 *
 * The Monitor pattern bundles shared data with a synchronization mechanism (mutex and condition variable)
 * to ensure only one thread can access the data at any given time, and provides tools for thread
 * coordination (waiting and signaling).
 */

#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <type_traits>
#include <utility>

namespace everest::lib::util {

template <typename T, typename = void> struct has_arrow_operator : std::false_type {};

template <typename T>
struct has_arrow_operator<T, std::void_t<decltype(std::declval<T>().operator->())>> : std::true_type {};

template <typename T> inline constexpr bool has_arrow_operator_v = has_arrow_operator<T>::value;

/**
 * @brief The RAII guard that provides locked access to the shared data T.
 * * This object is non-copyable but movable. Its existence guarantees that the
 * underlying mutex in the parent monitor object is held. When this handle
 * goes out of scope, the lock is automatically released.
 *
 * @tparam T The type of the protected resource.
 * @tparam MTX The mutex type used for locking (e.g., std::mutex, std::recursive_mutex).
 */
template <class T, class MTX> class monitor_handle {
public:
    /**
     * @brief Constructs the monitor handle and takes ownership of the acquired lock.
     * @param obj Reference to the protected resource within the monitor.
     * @param mtx R-value reference to the acquired unique lock, ownership is moved.
     * @param cv Reference to the condition variable in the monitor.
     */
    monitor_handle(T& obj, std::unique_lock<MTX>&& mtx, std::condition_variable& cv) :
        m_obj(obj), m_lock(std::move(mtx)), m_cv(cv) {
    }

    // Monitor handles should not be copied, as that would duplicate a unique lock.
    monitor_handle(monitor_handle<T, MTX> const& other) = delete;
    monitor_handle<T, MTX>& operator=(monitor_handle<T, MTX> const& rhs) = delete;

    // Defaulted move operations allow the handle to be moved (e.g., returned from monitor::handle).
    /** @brief generated default move constructor*/
    monitor_handle(monitor_handle<T, MTX>&& other) = default;
    /** @brief generated default move assignment*/
    monitor_handle<T, MTX>& operator=(monitor_handle<T, MTX>&& rhs) = default;

    /**
     * @brief Destructor. Automatically releases the lock held by m_lock.
     */
    ~monitor_handle() = default;

    /**
     * @brief Overloads the dereference operator to allow reference access to the protected object.
     * * This provides direct reference access to the guarded object T (or the wrapper T, e.g., std::unique_ptr<...>&).
     * The lock is held during the access.
     *
     * @return Reference to the protected object T.
     */
    T& operator*() {
        return m_obj;
    }

    /**
     * @brief Overloads the arrow operator to provide unified pointer-like access to the protected object.
     * * This implementation uses C++17's `if constexpr` to support three primary access patterns:
     * 1. **Chaining (Returns T&):** Used when T is a pointer-like wrapper (e.g., std::unique_ptr<T>) or a raw pointer
     * (T*). This allows the compiler's built-in chaining mechanism to continue indirection until the final object is
     * reached.
     * 2. **Direct Pointer Access (Returns T*):** Used when T is the final object type (e.g., a simple struct or class).
     * This terminates the chain immediately with a pointer to the object.
     * * @note This method holds the mutex lock for the duration of the access.
     *
     * @return `decltype(auto)` returns T& for chaining/pointers, or T* for direct access.
     */
    decltype(auto) operator->() {
        if constexpr (has_arrow_operator_v<T> || std::is_pointer_v<T>) {
            return m_obj;
        } else {
            return &m_obj;
        }
    }

    /**
     * @brief Blocks the thread until the provided predicate returns true.
     * @details This function atomically releases the lock (allowing other threads to acquire it)
     * and waits for a notification on the condition variable. When woken, it re-acquires
     * the lock and re-checks the predicate.
     * * @note This method is only available when MTX is std::mutex.
     * @tparam Predicate The callable type (e.g., lambda) that returns a boolean.
     * @param pred The condition that must become true to stop waiting.
     */
    template <class Predicate, class U = MTX, std::enable_if_t<std::is_same_v<U, std::mutex>>* = nullptr>
    void wait(Predicate&& pred) {
        m_cv.wait(m_lock, std::forward<Predicate>(pred));
    }

    /**
     * @brief Blocks the thread until the predicate returns true or the timeout expires.
     * @note This method is only available when MTX is std::mutex.
     * @tparam Rep The type representing the duration count (e.g., int, long).
     * @tparam Period The type representing the duration period (e.g., std::milli, std::ratio<1>).
     * @tparam Predicate The callable type (e.g., lambda) that returns a boolean.
     * @param pred The condition that must become true to stop waiting.
     * @param timeout The maximum time to wait for the condition to become true.
     * @return true if the predicate became true, false if the timeout expired.
     */
    template <class Rep, class Period, class Predicate, class U = MTX,
              std::enable_if_t<std::is_same_v<U, std::mutex>>* = nullptr>
    bool wait_for(Predicate&& pred, std::chrono::duration<Rep, Period> timeout) {
        return m_cv.wait_for(m_lock, timeout, std::forward<Predicate>(pred));
    }

    /**
     * @brief Blocks the thread until the predicate returns true or the absolute time point is reached.
     * * If the predicate is false, the lock is atomically released, and the thread sleeps until
     * a notification is received or abs_time is reached. When woken, the lock is re-acquired
     * and the predicate is re-checked.
     * @note This method is only available when MTX is std::mutex.
     * @tparam Clock The clock type used for the time point (e.g., std::system_clock, std::steady_clock).
     * @tparam Duration The duration type used for the time point.
     * @tparam Predicate The callable type (e.g., lambda) that returns a boolean.
     * @param abs_time The absolute time point at which the wait will cease, regardless of predicate state.
     * @param pred The condition that must become true to stop waiting.
     * @return true if the predicate became true, false if the absolute time was reached.
     */
    template <class Clock, class Duration, class Predicate, class U = MTX,
              std::enable_if_t<std::is_same_v<U, std::mutex>>* = nullptr>
    bool wait_until(std::chrono::time_point<Clock, Duration> const& abs_time, Predicate&& pred) {
        return m_cv.wait_until(m_lock, abs_time, std::forward<Predicate>(pred));
    }

private:
    T& m_obj;                      ///< Reference to the protected resource.
    std::unique_lock<MTX> m_lock;  ///< The unique lock, holding the mutex during the handle's lifetime.
    std::condition_variable& m_cv; ///< Reference to the monitor's condition variable.
};

/**
 * @brief A generic monitor class that manages RAII access to its resource T.
 * * Provides thread-safe data encapsulation using a mutex and thread coordination
 * via a condition variable. Access to the internal resource T is only possible
 * by obtaining a monitor_handle.
 *
 * @tparam T The type of the resource being protected.
 * @tparam MTX The mutex type to use, defaults to std::mutex.
 */
template <class T, class MTX = std::mutex> class monitor {
public:
    monitor() = default;
    /**
     * @brief Constructs the internal object T using move construction.
     */
    explicit monitor(T&& obj) : m_obj(std::move(obj)) {
    }

    /**
     * @brief Constructs the internal object T using perfect forwarding.
     * @tparam ArgsT Types of arguments used to construct T.
     * @param args Arguments passed to the constructor of T.
     */
    template <class... ArgsT> explicit monitor(ArgsT&&... args) : m_obj(std::forward<ArgsT>(args)...) {
    }

    ~monitor() = default;

    // Monitors should not be copied.
    monitor(monitor<T, MTX> const& other) = delete;
    monitor<T, MTX>& operator=(monitor<T, MTX> const& rhs) = delete;

    /**
     * @brief Thread-safe move constructor. Locks the source mutex before swapping.
     */
    monitor(monitor<T, MTX>&& other) {
        // Lock the source monitor's mutex before moving its data to ensure thread safety
        std::unique_lock lock(other.m_mtx);
        std::swap(m_obj, other.m_obj);
        // Note: m_mtx and m_cv are not swapped; they remain tied to the current object.
    }

    /**
     * @brief Thread-safe move assignment operator. Locks the source mutex before swapping.
     * @return Reference to the current object.
     */
    monitor<T, MTX>& operator=(monitor<T, MTX>&& rhs) {
        // Lock the source monitor's mutex before moving its data to ensure thread safety
        std::unique_lock lock(rhs.m_mtx);
        std::swap(m_obj, rhs.m_obj);
        return *this;
    }

    /**
     * @brief Blocks indefinitely to acquire the lock and return a handle.
     * @return monitor_handle<T, MTX> with ownership of the acquired lock.
     */
    monitor_handle<T, MTX> handle() {
        std::unique_lock lock(m_mtx);
        return monitor_handle<T, MTX>(m_obj, std::move(lock), m_cv);
    }

    /**
     * @brief Attempts to acquire the lock within the specified timeout duration.
     * @note This method is only available when MTX is std::timed_mutex.
     * @tparam Rep The type representing the duration count.
     * @tparam Period The type representing the duration period.
     * @param timeout The maximum time to wait for the lock.
     * @return An optional handle: contains the handle if the lock was acquired, std::nullopt otherwise.
     */
    template <class Rep, class Period, class U = MTX, std::enable_if_t<std::is_same_v<U, std::timed_mutex>>* = nullptr>
    std::optional<monitor_handle<T, MTX>> handle(std::chrono::duration<Rep, Period> timeout) {
        auto deadline = std::chrono::steady_clock::now() + timeout;

        std::unique_lock lock(m_mtx, std::defer_lock);
        if (not lock.try_lock_until(deadline)) {
            return std::nullopt;
        }

        return monitor_handle<T, MTX>(m_obj, std::move(lock), m_cv);
    }

    /**
     * @brief Wakes up one thread currently waiting on the monitor's condition variable.
     * @note This method is only available when MTX is std::mutex.
     */
    template <class U = MTX, std::enable_if_t<std::is_same_v<U, std::mutex>>* = nullptr> void notify_one() {
        m_cv.notify_one();
    }

    /**
     * @brief Wakes up all threads currently waiting on the monitor's condition variable.
     * @note This method is only available when MTX is std::mutex.
     */
    template <class U = MTX, std::enable_if_t<std::is_same_v<U, std::mutex>>* = nullptr> void notify_all() {
        m_cv.notify_all();
    }

private:
    MTX m_mtx;                    ///< The mutex protecting the resource T.
    T m_obj;                      ///< The protected resource.
    std::condition_variable m_cv; ///< The condition variable for thread coordination.
};

} // namespace everest::lib::util
