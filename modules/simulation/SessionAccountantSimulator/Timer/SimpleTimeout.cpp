/*
 * Licensor: Pionix GmbH, 2024
 * License: BaseCamp - License Version 1.0
 *
 * Licensed under the terms and conditions of the BaseCamp License contained in the "LICENSE" file, also available
 * under: https://pionix.com/pionix-license-terms
 * You may not use this file/code except in compliance with said License.
 */

#include "SimpleTimeout.hpp"

namespace Timeout {

/*
 * ################## Base class #######################
 */
void TimeoutBase::set_callback(const cb_signature& callback) {
    m_callback = callback;
}

void TimeoutBase::set_duration(std::chrono::seconds duration) {
    m_duration = duration;
    reset();
}

/*
 * ################## Mock class #######################
 */
void MockTimeout::start() {
    m_is_running.store(true);
}

void MockTimeout::reset() {
}

void MockTimeout::cancel() {
    m_is_valid.store(false);
    m_is_cancelled.store(true);
}

void MockTimeout::elapse_time() {
    if (m_is_running.load() && !m_is_cancelled.load() && m_callback) {
        m_callback();
    }
}

/*
 * ################## Real class #######################
 */
void SimpleTimeout::start() {
    if (!m_is_running) {
        m_waiting_thread = std::thread([this]() { runner(); });
    }
}

void SimpleTimeout::runner() {
    if (m_is_cancelled) {
        return;
    }
    std::chrono::time_point<std::chrono::steady_clock> until_time = std::chrono::steady_clock::now() + m_duration;

    while (std::chrono::steady_clock::now() < until_time) {
        std::unique_lock<std::mutex> lock(m_mtx);
        if (m_cv.wait_until(lock, until_time, [this]() { return m_is_reset || m_is_cancelled; })) {
            if (m_is_cancelled) {
                return;
            }
            if (m_is_reset) {
                until_time = std::chrono::steady_clock::now() + m_duration;
                m_is_reset.store(false);
            }
        }
    }
    if (m_callback) {
        m_callback();
    }
}

void SimpleTimeout::reset() {
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_is_reset.store(true);
    }
    m_cv.notify_one();
}

void SimpleTimeout::cancel() {
    m_is_valid.store(false);
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_is_cancelled.store(true);
    }
    m_cv.notify_one();

    if (m_waiting_thread.joinable()) {
        m_waiting_thread.join();
    }
}

SimpleTimeout::~SimpleTimeout() {
    cancel();
}

} // namespace Timeout
