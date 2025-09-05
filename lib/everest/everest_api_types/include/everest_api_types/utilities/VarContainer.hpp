// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef VARCONTAINER_HPP
#define VARCONTAINER_HPP

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <optional>

namespace everest::lib::API {

/* @brief VarContainer provides thread safe synchronisation and caching for the wrapped type.
 *
 * @details
 * It is desinged for two distinct use cases:
 * - Single producer / single consumer pattern with queue size of one
 * - Asynchronious production of a cached value, which can be read without delay.
 *
 * @tpatem T Type to be wrapped. Requirements: default constructable, assignable
 */
template <class T> class VarContainer {
public:
    /**
     * @brief Producer interface.
     * @details Sets the value for wrapped type @p T. Set flags for
     *  - new available data,
     *  - data cached to
     * the consumer side.
     * @param[in] value New Value
     */
    void set(const T& value) {
        {
            std::scoped_lock lock(data_mutex);
            data = value;
            unread_data = true;
            cached_data = true;
        }
        condvar.notify_one();
    }

    /**
     * @brief Clear wrapper status.
     * @details Clears all flags. After calling this function cached data is no longer available. Call @ref "set" to
     * make data available again.
     */
    void clear() {
        std::scoped_lock lock(data_mutex);
        unread_data = false;
        cached_data = false;
    };

    /**
     * @brief Waits for a new value.
     * @details Waits until @p timeout for a new value to be set. Returns as soon as new data is available.
     * @param[in] d If new data is available, it is written to @p d. In case of a timeout @p d is unchanged.
     * @param[in] timeout Maximum time to for a new data.
     * @return Returns @a true if new data was written to @p d. @a otherwise.
     */
    template <class Rep, class Periode> bool wait_for(T& d, const std::chrono::duration<Rep, Periode>& timeout) {
        std::unique_lock<std::mutex> lock(data_mutex);
        auto result = cond_wait_impl(lock, timeout);
        if (result) {
            d = data;
        }
        return result;
    }

    /**
     * @brief Waits for a new value.
     * @details Waits until @p timeout for a new value to be set. Returns as soon as new data is available.
     * @param[in] timeout Maximum time to for a new data.
     * @return The @p std::optional<T> holds the new data. It is empty in case of timeout.
     */
    template <class Rep, class Periode> std::optional<T> wait_for(const std::chrono::duration<Rep, Periode>& timeout) {
        std::unique_lock<std::mutex> lock(data_mutex);
        if (cond_wait_impl(lock, timeout)) {
            return data;
        }
        return {};
    }

    /**
     * @brief Access cached data or wait for a new value.
     * @details If available return cached data immediately. If no cached data is available, it waits until @p timeout
     * for a new value to be set.
     * @param[in] timeout Maximum time to for a new data.
     * @return The @p std::optional<T> holds the new data. It is empty in case of timeout.
     */
    template <class Rep, class Periode>
    std::optional<T> cached_or_wait_for(const std::chrono::duration<Rep, Periode>& timeout) {
        std::unique_lock<std::mutex> lock(data_mutex);
        if (cached_data) {
            return data;
        } else if (cond_wait_impl(lock, timeout)) {
            return data;
        }
        return {};
    }

private:
    template <class Rep, class Periode>
    bool cond_wait_impl(std::unique_lock<std::mutex>& lock, const std::chrono::duration<Rep, Periode>& timeout) {
        if (condvar.wait_for(lock, timeout, [this] { return unread_data; })) {
            // wait_for did NOT timeout
            unread_data = false;
            return true;
        }
        return false;
    }

    T data;
    std::condition_variable condvar;
    std::mutex data_mutex;
    bool unread_data{false};
    bool cached_data{false};
};
} // namespace everest::lib::API
#endif
