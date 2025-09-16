// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once

#include <optional>
#include <queue>

namespace everest::lib::io::queue {

/**
 * Simplified interface for a queue based
 * on <a href="https://en.cppreference.com/w/cpp/container/queue">std::queue</a>
 * @tparam T Datatype held by the queue
 */
template <class T> class simple_queue {
public:
    /**
     * @var reference
     * @brief Inherited type definition from STL
     */
    using reference = typename std::queue<T>::reference;

    /**
     * @var const_reference
     * @brief Inherited type definition from STL
     */
    using const_reference = typename std::queue<T>::const_reference;

    /**
     * @var value_type
     * @brief Inherited type definition from STL
     */
    using value_type = typename std::queue<T>::value_type;

    /**
     * @var size_type
     * @brief Inherited type definition from STL
     */
    using size_type = typename std::queue<T>::size_type;

    /**
     * @brief Maps to std::queue::front()
     */
    const_reference front() const {
        return m_queue.front();
    }
    /**
     * @brief Maps to std::queue::back()
     */
    const_reference back() const {
        return m_queue.back();
    }

    /**
     * @brief Maps to std::queue::push(const value_type&)
     */
    void push(const value_type& value) {
        m_queue.push(value);
    }
    /**
     * @brief Maps to std::queue::push(value_type&&)
     */
    void push(value_type&& value) {
        m_queue.push(std::forward<value_type>(value));
    }

    /**
     * @brief Get the front element of the queue if available.
     * @return Maybe the front element
     */
    std::optional<value_type> pop() {
        if (m_queue.empty()) {
            return std::nullopt;
        }
        auto tmp = front();
        m_queue.pop();
        return tmp;
    }

    /**
     * @brief Maps to std::queue::empty()
     */
    bool empty() const {
        return m_queue.empty();
    }

    /**
     * @brief Maps to std::queue::size()
     */
    size_type size() const {
        return m_queue.size();
    }

private:
    std::queue<T> m_queue;
};

} // namespace everest::lib::io::queue
