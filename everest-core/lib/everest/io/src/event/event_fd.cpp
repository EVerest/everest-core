// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <everest/io/event/event_fd.hpp>
#include <optional>
#include <stdexcept>
#include <sys/eventfd.h>

namespace everest::lib::io::event {

event_fd::event_fd() : m_fd(::eventfd(0, 0)) {
    if (m_fd == -1) {
        throw std::runtime_error("failed to create an eventfd");
    }
}

event_fd::operator int() const {
    return get_raw_fd();
}

int event_fd::get_raw_fd() const {
    return m_fd;
}

bool event_fd::valid() const {
    return m_fd != unique_fd::NO_DESCRIPTOR_SENTINEL;
}

std::optional<uint64_t> event_fd::read() {
    eventfd_t eventfd_buffer{0};
    if (eventfd_read(m_fd, &eventfd_buffer) == 0) {
        return eventfd_buffer;
    }
    return std::nullopt;
}

bool event_fd::write(std::uint64_t data) {
    return eventfd_write(m_fd, data) == 0;
}

bool event_fd::notify() {
    return write(1);
}

} // namespace everest::lib::io::event
