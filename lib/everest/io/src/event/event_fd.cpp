// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <everest/io/event/event_fd.hpp>
#include <optional>
#include <stdexcept>
#include <sys/eventfd.h>

namespace everest::lib::io::event {

event_fd_base::event_fd_base(unsigned int initval, int flags) : m_fd(::eventfd(initval, flags)) {
    if (m_fd == -1) {
        throw std::runtime_error("failed to create an eventfd");
    }
}

event_fd_base::operator int() const {
    return get_raw_fd();
}

int event_fd_base::get_raw_fd() const {
    return m_fd;
}

bool event_fd_base::valid() const {
    return m_fd != unique_fd::NO_DESCRIPTOR_SENTINEL;
}

std::optional<uint64_t> event_fd_base::read() {
    eventfd_t eventfd_buffer{0};
    if (eventfd_read(m_fd, &eventfd_buffer) == 0) {
        return eventfd_buffer;
    }
    return std::nullopt;
}

bool event_fd_base::write(std::uint64_t data) {
    return eventfd_write(m_fd, data) == 0;
}

bool event_fd_base::notify() {
    return write(1);
}

event_fd::event_fd() : event_fd_base(0, 0) {
}

semaphore_fd::semaphore_fd() : event_fd_base(0, EFD_SEMAPHORE) {
}

} // namespace everest::lib::io::event
