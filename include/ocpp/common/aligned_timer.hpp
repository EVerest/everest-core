// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#pragma once

#include <everest/logging.hpp>
#include <everest/timer.hpp>
#include <ocpp/common/types.hpp>

namespace ocpp {

class ClockAlignedTimer : private Everest::Timer<std::chrono::system_clock> {
public:
    using system_time_point = std::chrono::time_point<std::chrono::system_clock>;

private:
    system_time_point start_point;
    std::chrono::seconds call_interval;

    std::function<void()> callback;

    system_time_point get_next_timepoint() {
        using namespace std::chrono_literals;

        auto now = std::chrono::system_clock::now();
        auto diff = now - this->start_point;
        auto time_to_next = 0s;
        // Only calculate next time if it is positve. Otherwise we would end up before the start point
        if (diff > 0s) {
            time_to_next = ((std::chrono::duration_cast<std::chrono::seconds>(diff) / this->call_interval) + 1) *
                           this->call_interval;
            ;
        }
        auto next_time = this->start_point + time_to_next;
        EVLOG_debug << "Clock aligned interval every " << this->call_interval.count() << " seconds, starting at "
                    << ocpp::DateTime(date::utc_clock::from_sys(this->start_point))
                    << ". Next one at: " << ocpp::DateTime(date::utc_clock::from_sys(next_time));
        EVLOG_debug << "This amounts to "
                    << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::hours(24)) / this->call_interval
                    << " samples per day";
        return next_time;
    }

    system_time_point call_next() {
        if (this->callback == nullptr) {
            return system_time_point{};
        }

        auto wrapper = [this]() {
            this->callback();
            this->at(this->get_next_timepoint());
        };

        auto next_timepoint = this->get_next_timepoint();
        this->at(wrapper, next_timepoint);
        return next_timepoint;
    }

public:
    ClockAlignedTimer() = default;

    explicit ClockAlignedTimer(boost::asio::io_context* io_context) : Timer(io_context) {
    }

    explicit ClockAlignedTimer(boost::asio::io_context* io_context, const std::function<void()>& callback) :
        Timer(io_context), callback(callback) {
    }

    template <class Rep, class Period>
    system_time_point interval_starting_from(const std::function<void()>& callback,
                                             const std::chrono::duration<Rep, Period> interval,
                                             system_time_point start_point) {
        this->callback = callback;
        return this->interval_starting_from(interval, start_point);
    }

    template <class Rep, class Period>
    system_time_point interval_starting_from(const std::chrono::duration<Rep, Period> interval,
                                             system_time_point start_point) {
        this->start_point = start_point;
        this->call_interval = interval;

        return this->call_next();
    }

    template <class Rep, class Period>
    system_time_point set_interval(const std::chrono::duration<Rep, Period>& interval) {
        this->call_interval = interval;
        return this->call_next();
    }

    using Everest::Timer<std::chrono::system_clock>::stop;
};

} // namespace ocpp
