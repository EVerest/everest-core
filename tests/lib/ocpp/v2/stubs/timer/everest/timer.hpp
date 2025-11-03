// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

///
/// @file timer.hpp
/// @brief Stub for the timer, without threads
///
/// This file is a stub for the timer. It just replaces the class, stores the callback and stores which methods are
/// called. The callback is not called itself, since it is stored, it can be called in a test for example. This makes
/// it easier to test classes that use the timer. Currently, with this implementation, only one timer per class under
/// test is possible, or at least only one callback can be stored at a time. If there is more needed, the timer_stub
/// file needs to be changed.
/// The callback is stored in timer_stub and can be requested from there.
///
/// @warning Only include this file if it can not conflict with the library!!!

#ifndef EVEREST_TIMER_HPP
#define EVEREST_TIMER_HPP

#include <boost/asio.hpp>
#include <chrono>
#include <date/date.h>
#include <date/tz.h>
#include <functional>

#include "../timer_stub.hpp"

namespace Everest {
// template <typename TimerClock = date::steady_clock> class Timer {
template <typename TimerClock = date::utc_clock> class Timer {

public:
    explicit Timer() {
    }

    explicit Timer(const std::function<void()>& callback) {
        timer_stub_set_callback(callback);
    }

    explicit Timer(boost::asio::io_context* /*io_context*/) {
    }

    explicit Timer(boost::asio::io_context* /*io_context*/, const std::function<void()>& callback) {
        timer_stub_set_callback(callback);
    }

    virtual ~Timer() {
    }

    template <class Clock, class Duration = typename Clock::duration>
    void at(const std::function<void()>& callback, const std::chrono::time_point<Clock, Duration>& time_point) {
        timer_stub_at_called(1);
        timer_stub_set_callback(callback);
    }

    template <class Clock, class Duration = typename Clock::duration>
    void at(const std::chrono::time_point<Clock, Duration>& time_point) {
        timer_stub_at_called(1);
    }

    template <class Rep, class Period>
    void interval(const std::function<void()>& callback, const std::chrono::duration<Rep, Period>& interval) {
        timer_stub_interval_called(1);
        timer_stub_set_callback(callback);
    }

    template <class Rep, class Period> void interval(const std::chrono::duration<Rep, Period>& interval) {
        timer_stub_interval_called(1);
    }

    template <class Rep, class Period>
    void timeout(const std::function<void()>& callback, const std::chrono::duration<Rep, Period>& interval) {
        timer_stub_set_callback(callback);
        timer_stub_timeout_called(1);
    }

    template <class Rep, class Period> void timeout(const std::chrono::duration<Rep, Period>& interval) {
        timer_stub_timeout_called(1);
    }

    void stop() {
        timer_stub_stop_called(1);
    }
};

using SteadyTimer = Timer<date::utc_clock>;
using SystemTimer = Timer<date::utc_clock>;
} // namespace Everest

#endif // EVEREST_TIMER_HPP
