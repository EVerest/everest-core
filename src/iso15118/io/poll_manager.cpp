// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/io/poll_manager.hpp>

#include <type_traits>
#include <vector>

#include <sys/eventfd.h>
#include <sys/poll.h>
#include <unistd.h>

#include <iso15118/detail/helper.hpp>

namespace iso15118::io {

static PollSet create_poll_set(const std::map<int, PollCallback>& map, int event_fd) {
    const auto total_size = map.size() + 1; // including event_fd
    decltype(PollSet::fds) fds(total_size);
    decltype(PollSet::callbacks) callbacks(total_size);

    auto index = 0;
    for (auto it = map.begin(); it != map.end(); ++it, ++index) {
        fds[index].fd = it->first;
        fds[index].events = POLLIN;
        callbacks[index] = &it->second;
    }

    fds[index].fd = event_fd;
    fds[index].events = POLLIN;

    return {std::move(fds), std::move(callbacks)};
}

PollManager::PollManager() {
    event_fd = eventfd(0, 0);
    if (event_fd == -1) {
        log_and_throw("Failed to create eventfd");
    }
}

void PollManager::register_fd(int fd, PollCallback& poll_callback) {
    registered_fds.emplace(fd, poll_callback);
    poll_set = create_poll_set(registered_fds, event_fd);
}

void PollManager::unregister_fd(int fd) {
    registered_fds.erase(fd);
    poll_set = create_poll_set(registered_fds, event_fd);
}

void PollManager::poll(int timeout_ms) {

    auto& pollfds = poll_set.fds;

    const auto ret = ::poll(pollfds.data(), pollfds.size(), timeout_ms);

    if (ret == -1) {
        log_and_throw("Poll failed\n");
    }

    if (ret == 0) {
        // timeout
        return;
    }

    // first check for event_fd
    if (pollfds[pollfds.size() - 1].revents & POLLIN) {
        eventfd_t tmp;
        eventfd_read(event_fd, &tmp);

        // just break;
        return;
    }

    // check fds
    for (auto i = 0; i < pollfds.size() - 1; ++i) {
        if (pollfds[i].revents & POLLIN) {
            (*poll_set.callbacks[i])();
        }
    }
}

void PollManager::abort() {
    eventfd_write(event_fd, 1);
}

} // namespace iso15118::io
