// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <ctime>
#include <everest/io/event/timer_fd.hpp>
#include <stdexcept>
#include <sys/timerfd.h>
#include <unistd.h>

namespace everest::lib::io::event {

timer_fd::timer_fd() : m_fd(::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK)) {
    if (m_fd == -1) {
        throw std::runtime_error("failed to create an timerfd");
    }
}

timer_fd::operator int() const {
    return get_raw_fd();
}

int timer_fd::get_raw_fd() const {
    return m_fd;
}

bool timer_fd::valid() const {
    return m_fd != unique_fd::NO_DESCRIPTOR_SENTINEL;
}

int timer_fd::read() {
    uint64_t buffer;
    return ::read(m_fd, &buffer, sizeof(buffer));
}

bool timer_fd::set_timeout_ms(long long to) {
    return set_timeout_ns(1000 * 1000 * to);
}

bool timer_fd::set_timeout_us(long long to) {
    return set_timeout_ns(1000 * to);
}

bool timer_fd::set_timeout_ns(long long to) {
    struct itimerspec timer;
    auto sec = to / 1000000000;
    auto nano = to % 1000000000;

    timer.it_interval.tv_nsec = nano;
    timer.it_interval.tv_sec = sec;
    timer.it_value.tv_nsec = nano;
    timer.it_value.tv_sec = sec;

    return timerfd_settime(m_fd, 0, &timer, NULL) == 0;
}

} // namespace everest::lib::io::event
