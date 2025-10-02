// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <everest/io/event/unique_fd.hpp>
#include <unistd.h>
#include <utility>

namespace everest::lib::io::event {

namespace {
void close_descriptor_if_valid(int fd) {
    if (fd != unique_fd::NO_DESCRIPTOR_SENTINEL) {
        // NOTE (aw): according to the close(2) man page, close might return an error but it should not be retried
        ::close(fd);
    }
}
} // namespace

unique_fd::unique_fd(int fd) : m_fd(fd){};

unique_fd::operator int() const {
    return m_fd;
}

bool unique_fd::is_fd() const {
    return m_fd != NO_DESCRIPTOR_SENTINEL;
}

int unique_fd::release() {
    return std::exchange(m_fd, NO_DESCRIPTOR_SENTINEL);
}

void unique_fd::close() {
    close_descriptor_if_valid(m_fd);
    std::exchange(m_fd, NO_DESCRIPTOR_SENTINEL);
}

unique_fd::unique_fd(unique_fd&& other) : m_fd(std::exchange(other.m_fd, NO_DESCRIPTOR_SENTINEL)) {
}

unique_fd& unique_fd::operator=(unique_fd&& other) {
    if (this != &other) {
        close_descriptor_if_valid(m_fd);
        m_fd = std::exchange(other.m_fd, NO_DESCRIPTOR_SENTINEL);
    }

    return *this;
}

unique_fd::~unique_fd() {
    close_descriptor_if_valid(m_fd);
}
} // namespace everest::lib::io::event
