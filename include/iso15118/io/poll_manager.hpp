// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <functional>
#include <map>
#include <vector>

#include <poll.h>
namespace iso15118::io {

using PollCallback = const std::function<void()>;
struct PollSet {
    std::vector<struct pollfd> fds;
    std::vector<PollCallback*> callbacks;
};

class PollManager {
public:
    PollManager();

    void register_fd(int fd, PollCallback& poll_callback);
    void unregister_fd(int fd);

    void poll(int timeout_ms);
    void abort();

private:
    std::map<int, PollCallback> registered_fds;

    PollSet poll_set;

    int event_fd{-1};
};

} // namespace iso15118::io
