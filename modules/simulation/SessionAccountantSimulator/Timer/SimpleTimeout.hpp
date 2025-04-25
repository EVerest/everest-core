/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <thread>

namespace Timeout {
using cb_signature = std::function<void(void)>;

class TimeoutBase {
public:
    explicit TimeoutBase(std::chrono::seconds duration) : m_duration(duration) {
    }
    virtual ~TimeoutBase() noexcept = default;

    void set_callback(const cb_signature& callback);

    virtual void start() = 0;
    virtual void cancel() = 0;

    virtual void set_duration(std::chrono::seconds duration);
    virtual void reset() = 0;

    bool is_valid() {
        return m_is_valid;
    }

protected:
    cb_signature m_callback;
    std::chrono::seconds m_duration;
    std::atomic_bool m_is_valid{true};
};

class MockTimeout : public TimeoutBase {
public:
    explicit MockTimeout(std::chrono::seconds duration) : TimeoutBase(duration) {
    }

    void start() override;
    void cancel() override;

    void reset() override;

    // Interface to let time elapse manually for testing
    void elapse_time();

private:
    std::atomic_bool m_is_running{false};
    std::atomic_bool m_is_cancelled{false};
};

class SimpleTimeout : public TimeoutBase {
public:
    explicit SimpleTimeout(std::chrono::seconds duration) : TimeoutBase(duration) {
    }

    ~SimpleTimeout() override;

    void start() override;
    void cancel() override;

    void reset() override;

private:
    void runner();

    std::atomic_bool m_is_running{false};
    std::atomic_bool m_is_cancelled{false};
    std::atomic_bool m_is_reset{false};
    std::condition_variable m_cv;
    std::mutex m_mtx;
    std::thread m_waiting_thread;
};

} // namespace Timeout
